----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Patrick Haid
-- 
-- Create Date: 13.01.2025 21:35:41
-- Design Name: PS2 Video Processor
-- Module Name: Input Linebuffer - Behavioral
-- Project Name: Portable PS2
-- Target Devices: Trion T20
-- Tool Versions: Efinity
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------

--TODO
--replace the a_ready with something else to switch the state machine
    --maybe instead of the vsync use a pulse generated in
    --could also be triggered from state change of input address MSB
    --could be triggered by falling edge on active -> most promising solution
--add another wait state, as it seems an invalid line is output every start of a frame

library ieee;
use ieee.std_logic_1164.all;
use IEEE.NUMERIC_STD.ALL;
 
library work;
use work.scaler_types.all;
use work.ram_pkg.all;
 
entity ip_line_buffer is
  port (
  a_i_clk :                 in std_logic;                         --pixel clock of clock domain a
  a_i_we :                  in std_logic;                          --write enable input
  a_i_data :                in std_logic_vector(31 downto 0);    --video data in
  a_i_addr  :               in  std_logic_vector(9 downto 0);
  a_i_video_cfg :           in video_config_t;                  --video config
  a_i_line_active :         in std_logic;
  a_i_vsync :               in std_logic;
  
  b_i_rst_n :               in std_logic;                           --reset of readout state machine                   
  b_i_clk :                 in std_logic;                          --pixel clock of clock domain a
  b_i_re :                  in std_logic;                           --read enable input
  b_o_data :                out std_logic_vector(31 downto 0);    --video data out; in 32 byte blocks
  b_i_addr :                in std_logic_vector(9 downto 0);
  b_o_video_cfg :           out video_config_t;
  b_o_write_req :           out std_logic;                  --high when pixels are to be transmitted
  b_vsync :                 out std_logic                        --single pulse falling edge of vsync
  );
end ip_line_buffer;
 
architecture rtl of ip_line_buffer is
 
component efx_simple_dual_port_2_clk_ram is
  generic (
    RAM_WIDTH : integer := 32;
    RAM_DEPTH : integer := 2048;
    OUTREG    : boolean := true
    );
  port (
    addra  : in  std_logic_vector((logb2(RAM_DEPTH)-1) downto 0);
    addrb  : in  std_logic_vector((logb2(RAM_DEPTH)-1) downto 0);
    dina   : in  std_logic_vector(RAM_WIDTH-1 downto 0);
    clka   : in  std_logic;
    clkb   : in  std_logic;
    wea    : in  std_logic;
    enb    : in  std_logic;
    rstb   : in  std_logic;
    regceb : in  std_logic;
    doutb  : out std_logic_vector(RAM_WIDTH-1 downto 0));
end component efx_simple_dual_port_2_clk_ram;



--CLK A side definitions
signal a_bsel_1, a_bsel_2 : std_logic := '0';
signal a_addr_ram : std_logic_vector(10 downto 0) := (others => '0');

--CLK B side definitions
type state_type is (state_init, state_idle, state_config, state_read);
signal b_state : state_type := state_init;
signal b_data_ram : std_logic_vector(31 downto 0) := (others => '0');
signal b_addr_ram : std_logic_vector(10 downto 0) := (others => '0');
signal b_bsel : std_logic := '0';
signal b_max_pixels : integer range 0 to 5000 := 0;
signal b_write_req_r : std_logic := '0';
signal b_line_active_falling : std_logic := '0';
signal b_vsync_r : std_logic := '0';
signal b_video_config_r : video_config_t := (h_active_pixels => 0,
                                             h_compressed_pixels => 0,
                                             v_active_pixels => 0,
                                             odd_even_flag => '0',
                                             interlaced_flag => "000",
                                             ntpal_flag => '0',
                                             deinterlacer_debug => '0',
                                             motion_threshold => 0,
                                             x_scaling_factor => (others=>'0'),
                                             y_scaling_factor => (others=>'0'),
                                             x_active_out => (others=>'0'),
                                             y_active_out => (others=>'0')
                                             );

--CDC definitions
signal line_active_cdc1, line_active_cdc2, line_active_cdc3 : std_logic := '0';
signal vsync_cdc1, vsync_cdc2, vsync_cdc3 : std_logic := '0';


begin
 
ram: efx_simple_dual_port_2_clk_ram
  generic map (RAM_WIDTH => 32, RAM_DEPTH => 2048, OUTREG => true)
  port map (
    addra  => a_addr_ram,
    addrb  => b_addr_ram,
    dina   => a_i_data,
    clka   => a_i_clk,
    clkb   => b_i_clk,
    wea    => a_i_we,
    enb    => b_i_re,
    rstb   => b_i_rst_n,
    regceb => b_i_re,
    doutb  => b_data_ram);



process(a_i_clk)
begin
    if(rising_edge(a_i_clk)) then
        --CDC B -> A
        a_bsel_1 <= not b_bsel;
        a_bsel_2 <= a_bsel_1;
    end if;

end process;

a_addr_ram <= a_bsel_2 & a_i_addr;

process(b_i_clk)
begin
    if(rising_edge(b_i_clk)) then
            --CDC A -> B
            line_active_cdc1 <= a_i_line_active;
            line_active_cdc2 <= line_active_cdc1;
            line_active_cdc3 <= line_active_cdc2;

            vsync_cdc1 <= a_i_vsync;
            vsync_cdc2 <= vsync_cdc1;
            vsync_cdc3 <= vsync_cdc2;
            --on rising edge of data ready -> trigger start of state machine
            if(line_active_cdc2 = '0' and line_active_cdc3 = '1') then
                b_line_active_falling <= '1';
            else
                b_line_active_falling <= '0';
            end if;
            --on falling edge of VSYNC -> send a single positive pulse out for frame sync
            if(vsync_cdc2 = '0' and vsync_cdc3 = '1') then
                b_vsync_r <= '1';
                b_video_config_r <= a_i_video_cfg;
            else
                b_vsync_r <= '0';
            end if;

    end if;

end process;
b_vsync <= b_vsync_r;

process(b_i_clk, b_i_rst_n)
begin

        if(b_i_rst_n = '0') then
            b_write_req_r <= '0';
            b_state <= state_init;
            b_max_pixels <= 0;
            b_bsel <= '0';
            
        else
            if(rising_edge(b_i_clk)) then

                case(b_state) is
                    when state_init =>
                        --wait until data ready goes high
                        if(b_line_active_falling = '1') then
                            b_state <= state_config;
                        end if;

                    when state_config =>
                        --store the config of this field
                        --b_ctrl_r <= a_i_ctrl;
                        b_max_pixels <= a_i_video_cfg.h_compressed_pixels;
                        b_bsel <= not b_bsel;
                        
                        b_write_req_r <= '1';
                        b_state <= state_idle;

                    when state_idle =>
                        --when read access -> shift out pixel data and increment pixel counter
                        --when pixel counter = max_pixels, all lines were transmitted, jump back to state_init
                        if(to_integer(unsigned(b_i_addr)) = b_max_pixels) then  
                            b_state <= state_init;
                            b_write_req_r <= '0';
                        end if;

                    when others =>
                        b_state <= state_init;
                end case;

            end if;
        end if;
end process; 

b_addr_ram <= b_bsel & b_i_addr;
b_o_video_cfg <= b_video_config_r;
b_o_write_req <= b_write_req_r;

b_o_data <= b_data_ram when (b_i_re = '1') else
          (others=>'0');
end architecture;

