----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Patrick Haid
-- 
-- Create Date: 13.01.2025 21:35:41
-- Design Name: PS2 Video Processor
-- Module Name: Framebuffer Scheduler
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

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use ieee.std_logic_unsigned.all;

library work;
use work.scaler_types.all;

entity framebuffer_scheduler is
Port ( 
  --RESET and CLK
  i_rst_sync0 :             in std_logic;                 --reset syncronized with clk0
  i_rst_sync1 :             in std_logic;                 --reset syncronized with clk1
  i_clk0 :                  in std_logic;                 --150MHz clock
  i_clk1 :                  in std_logic;                 --150MHz clock, shifted
  --interface to input line buffer                   
  o_re :                    out std_logic;                           --read enable input
  i_data :                  in std_logic_vector(31 downto 0);    --video data out; in 32 byte blocks
  o_addr :                  out std_logic_vector(9 downto 0);
  i_video_cfg :             in video_config_t;                     --video config
  i_wr_req :                in std_logic;                  --high when pixels are to be transmitted
  i_vsync_pulse :           in std_logic;
  --interface to deinterlacer
  i_re_req :                in std_logic;
  o_data :                  out std_logic_vector(31 downto 0);     --video data
  i_addr :                  in std_logic_vector(21 downto 0);
  o_field_info :            out field_info_t;
  o_frame_sync :            out std_logic;
  o_valid :                 out std_logic;
  --interface to SDRAM
  sdram_address :           out std_logic_vector(11 downto 0);
  sdram_bank_sel :          out std_logic_vector(1 downto 0);
  sdram_cas :               out std_logic;
  sdram_cke :               out std_logic;
  sdram_cs :                out std_logic;
  sdram_dqm :               out std_logic_vector(3 downto 0);
  --wrapper ports for bidirectional buffer - only for Trion
  sdram_data_IN :           in std_logic_vector(31 downto 0);
  sdram_data_OUT :          out std_logic_vector(31 downto 0);
  sdram_data_OE :           out std_logic_vector(31 downto 0);
  sdram_ras :               out std_logic;
  sdram_we :                out std_logic
);
end framebuffer_scheduler;

architecture Behavioral of framebuffer_scheduler is

component sdram_controller is
Port ( 
  --interface to SDRAM
  sdram_address :           out std_logic_vector(11 downto 0);
  sdram_bank_sel :          out std_logic_vector(1 downto 0);
  sdram_cas :               out std_logic;
  sdram_cke :               out std_logic;
  sdram_cs :                out std_logic;
  sdram_dqm :               out std_logic_vector(3 downto 0);
  --wrapper ports for bidirectional buffer - only for Trion
  sdram_datin :             in std_logic_vector(31 downto 0);
  sdram_datout :            out std_logic_vector(31 downto 0);
  sdram_datoe :             out std_logic_vector(31 downto 0);
  sdram_ras :               out std_logic;
  sdram_we :                out std_logic;
  --user access port
  in_addr :                 in std_logic_vector(21 downto 0);
  in_data :                 in std_logic_vector(31 downto 0);
  out_data :                out std_logic_vector(31 downto 0);
  in_rst_0 :                in std_logic;                   --reset memory controller
  in_rst_1 :                in std_logic;                   --reset memory controller
  in_rw :                   in std_logic;                     --0 -> read; 1 -> write
  in_cke :                  in std_logic;                     --1 indicates a pending a read or write; needs to be reset by the controller when in_valid or out_valid turn high
  in_dqm:                   in std_logic_vector(3 downto 0);  --select which bytes are read/written
  --status outputs
  out_data_valid :          out std_logic;                    --high when read data is valid/ready; indicate to shift out the data with falling edge of clk
  busy :                    out std_logic;                    --high when the controller is busy executing a cmd
  --clock input
  in_clk :                  in std_logic;                     --clkin is 100MHz
  in_clk_dly:               in std_logic                      --delayed input clkin to read data from the sdram
);
end component;

----connections to SDRAM Controller----------------------------------
signal in_addr : std_logic_vector(21 downto 0) := (others=>'0');
signal in_data : std_logic_vector(31 downto 0) := (others=>'0');
signal in_rw : std_logic := '0';
signal in_cke : std_logic := '0';
signal in_dqm : std_logic_vector(3 downto 0) := "0000";
signal sdram_data_valid : std_logic;
signal sdram_busy : std_logic;
signal out_data : std_logic_vector(31 downto 0);
signal cmd_ctr : integer range 0 to 31 := 0;
signal sdram_datin : std_logic_vector(31 downto 0);
signal sdram_datout : std_logic_vector(31 downto 0);
signal sdram_datoe : std_logic_vector(31 downto 0);
--video input--------------------------------------------------------
signal rd_req_vid_in : std_logic;                  
--video out-----------------------------------------------------
signal out_read_start, out_read_en : std_logic := '0';
signal data_out_valid : std_logic := '0';
signal read_word : std_logic_vector(31 downto 0) := (others=>'0');
signal in_pxl_ctr, sdram_wr_pxl : integer range 0 to 1023 := 0;

signal field_info_r : field_info_t := (bank_odd_even =>'0',
                                        bank_count_A =>"000",
                                        bank_count_B =>"000",
                                        bank_count_C =>"000",
                                        bank_count_D =>"000"
                                        );

signal odd_even_flag : std_logic := '0';        

signal line_in_count : integer range 0 to 511 := 0;

type bank_array is array (7 downto 0) of std_logic;
signal bank_odd_even_write, bank_odd_even_read : std_logic := '0';
signal bank_count_write, bank_count_A, bank_count_B, bank_count_C, bank_count_D : integer range 0 to 8 := 0;

signal read_done_flag : std_logic := '0';

type state_type is (
    state_init,
    state_wait,
    state_write,
    state_read,
    state_idle,
    state_wait_busy,
    state_switch_bank
);
signal state, next_state, current_operation : state_type := state_init;

begin

tristate : for i in 0 to 31 generate
begin
sdram_datin(i) <= sdram_data_IN(i);
sdram_data_OUT(i) <= sdram_datout(i);
sdram_data_OE(i) <= sdram_datoe(i);
end generate tristate;

sdram: sdram_controller port map(  
    sdram_address => sdram_address,
    sdram_bank_sel => sdram_bank_sel,
    sdram_cas => sdram_cas,
    sdram_cke => sdram_cke,
    --sdram_clk,
    sdram_cs => sdram_cs,
    sdram_dqm => sdram_dqm,
    sdram_datin => sdram_datin,
    sdram_datout => sdram_datout,
    sdram_datoe => sdram_datoe,
    sdram_ras => sdram_ras,
    sdram_we => sdram_we,
                    
    in_addr => in_addr,
    in_data => in_data,
    out_data => out_data,
    in_rst_0 => i_rst_sync0,
    in_rst_1 => i_rst_sync1,
    in_rw => in_rw,
    in_cke => in_cke,
    in_dqm => in_dqm,
    out_data_valid => sdram_data_valid,
    busy => sdram_busy,
    in_clk => i_clk0,
    in_clk_dly => i_clk1
);

odd_even_flag <= i_video_cfg.odd_even_flag;

read:process(i_clk0, i_rst_sync0)
begin

if(i_rst_sync0 = '0') then
    data_out_valid <= '0';
else
    if(rising_edge(i_clk0)) then
        read_word <= out_data;
        if(sdram_data_valid = '1') then 
            data_out_valid <= '1';
        else
            data_out_valid <= '0';
        end if;
     end if;
end if;
end process;

banking:process(i_clk0, i_rst_sync0)
begin

if(i_rst_sync0 = '0') then

    --reset values for outputs to deinterlacer
    out_read_start <= '0';
    out_read_en <= '0';
    bank_count_A <= 0;
    bank_count_B <= 0;
    bank_count_C <= 0;
    bank_count_D <= 0;
    bank_odd_even_write <= '0';
    bank_odd_even_read <= '0';
    field_info_r.bank_odd_even <= '0';
    field_info_r.bank_count_A <= "000";
    field_info_r.bank_count_B <= "000";
    field_info_r.bank_count_C <= "000";
    field_info_r.bank_count_D <= "000";
else
    if(rising_edge(i_clk0)) then 
        --send synchronisation pulse for starting output state machines and update control word
        if(i_vsync_pulse = '1') then  
        
            field_info_r.bank_odd_even <= bank_odd_even_read;
            field_info_r.bank_count_A <= std_logic_vector(to_unsigned(bank_count_A, 3));
            field_info_r.bank_count_B <= std_logic_vector(to_unsigned(bank_count_B, 3));
            field_info_r.bank_count_C <= std_logic_vector(to_unsigned(bank_count_C, 3));
            field_info_r.bank_count_D <= std_logic_vector(to_unsigned(bank_count_D, 3));
            
            if(out_read_en = '1') then
                out_read_start <= '1';
            end if;
        else
            out_read_start <= '0';
        end if;
   
        --new version with 8 banks
        --handle bank counter and odd-even flags
        if(i_vsync_pulse = '1') then 
            if(bank_count_write < 7) then
                bank_count_write <= bank_count_write + 1;
            else
                bank_count_write <= 0;
            end if;
            
            bank_count_A <= bank_count_write;
            bank_count_B <= bank_count_A;
            bank_count_C <= bank_count_B;
            bank_count_D <= bank_count_C;
            
            bank_odd_even_write <= odd_even_flag;
            bank_odd_even_read <= bank_odd_even_write;
            
            out_read_en <= '1';         
        end if;
 
    end if;      
end if;

end process;

fsm:process(i_clk0, i_rst_sync0)
    begin
    
if(i_rst_sync0 = '0') then
    state <= state_init;
    cmd_ctr <= 0;
    --reset values for SDRAM interface
    in_addr <= (others=>'0');
    in_data <= (others=>'0');
    in_rw <= '0';
    in_cke <= '0';
    in_dqm <= "0000";
    rd_req_vid_in <= '0';
    in_pxl_ctr <= 0;
    line_in_count <= 0;
    read_done_flag <= '0';
else
    if(rising_edge(i_clk0)) then

        cmd_ctr <= cmd_ctr + 1;  
        
        if(i_vsync_pulse = '1') then
            line_in_count <= 0;
        end if;
       
        --reset input pixel counter
        if(i_wr_req = '0') then
           in_pxl_ctr <= 0;
        end if;
 
        case(state) is
            when state_init =>
                --initialize default values
                --wait until SDRAM is initialized
                --enter independent mode(PS2 is off -> only display OSD)
                if(sdram_busy = '0') then
                    state <= state_idle;
                    next_state <= state_write;
                end if;

            when state_idle =>
                
                current_operation <= state_idle;
            
                if(i_re_req = '0' and i_wr_req = '1' and sdram_busy = '0') then  --write 8 words  
                    state <= state_write;  
                    cmd_ctr <= 0;
                end if;
                    
                if(i_re_req = '1' and i_wr_req = '0' and sdram_busy = '0') then  --read 8 words
                    state <= state_read;
                    cmd_ctr <= 0;
                end if;
                    
                if(i_re_req = '1' and i_wr_req = '1' and sdram_busy = '0') then  --switch between read and write
                    state <= next_state;    
                    cmd_ctr <= 0; 
                end if;

            when state_write =>
            
                if(cmd_ctr <= 7) then
                    in_pxl_ctr <= in_pxl_ctr + 1;
                end if;
            
                case(cmd_ctr) is
                when 0 =>
                    rd_req_vid_in <= '1';
                    sdram_wr_pxl <= in_pxl_ctr; --sdram address is the initial pixel counter, before incrementing it
                when 3 to 10 =>
                    in_rw <= '1';
                    in_cke <= '1';
                    in_addr <= std_logic_vector(to_unsigned(bank_count_write, 3)) & std_logic_vector(to_unsigned(line_in_count, 9)) & std_logic_vector(to_unsigned(sdram_wr_pxl, 10));
                    in_data <= i_data;
                    in_dqm <= "0000";
                when 11 =>
                    in_rw <= '0';
                    in_cke <= '0';
                    in_addr <= (others=>'0');
                    in_data <= (others=>'0');
                    in_dqm <= "0000";
                    rd_req_vid_in <= '0';
                    -----
                    state <= state_wait_busy;
                    current_operation <= state_write;
                when others => null; 
                end case;
              
            when state_read =>
            
                if(i_re_req = '0') then
                    --the RD is only low for 2 clock cycles, so we need this IF to detect it later
                    read_done_flag <= '1';  
                end if;
            
                case(cmd_ctr) is
                    when 0 =>
                        in_rw <= '0';
                        in_cke <= '1';
                        in_addr <= i_addr;
                        in_data <= (others=>'0');
                        in_dqm <= "0000";
                    when 8 =>
                        in_rw <= '0';
                        in_cke <= '0';
                        in_addr <= (others=>'0');
                        in_data <= (others=>'0');
                        in_dqm <= "0000";
                        ------
                        state <= state_wait_busy;
                        current_operation <= state_read;
                    when others => null;              
                end case;
                            
            when state_wait_busy =>
                
                --reading
                if(sdram_busy = '0' and current_operation = state_read) then
                    if(i_re_req = '0') then
                        read_done_flag <= '0';
                        state <= state_idle;
                        next_state <= state_write;
                    else
                        state <= state_read;
                        cmd_ctr <= 0;
                    end if;
                end if;
                
                --writing
                if(sdram_busy = '0' and current_operation = state_write) then
                    if(i_wr_req = '0') then
                        state <= state_idle;
                        next_state <= state_read;
                        line_in_count <= line_in_count + 1;
                    else
                        state <= state_write;
                        cmd_ctr <= 0;
                    end if;
                end if;
                    
            when others => null;

            end case;
     end if;
end if;
end process;

o_re <= rd_req_vid_in;
o_addr <= std_logic_vector(to_unsigned(in_pxl_ctr, 10));

o_data <= read_word;
o_field_info <= field_info_r;
o_frame_sync <= out_read_start;
o_valid <= data_out_valid;

end Behavioral;
