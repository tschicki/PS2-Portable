----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Patrick Haid
-- 
-- Create Date: 28.02.2025 12:35:50
-- Design Name: PS2 Video Processor
-- Module Name: output_linebuffer - Behavioral
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

library work;
use work.scaler_types.all;
use work.ram_pkg.all;

entity output_linebuffer is
    Port ( 
        --150MHz side control
        a_clk : in std_logic;
        a_sync : in std_logic;
        a_video_cfg : in video_config_t;
        --video out side control
        b_rst_n : in std_logic;
        b_clk : in std_logic;
        
        --data input from scaler
        a_we : in std_logic;                               
        a_data : in std_logic_vector(23 downto 0);             
        a_addr : in std_logic_vector(9 downto 0); 
        a_frame_end : out std_logic;
        a_line_start : out std_logic;  
        --interface with video output
        b_re : in std_logic;                               
        b_data : out std_logic_vector(23 downto 0);             
        b_addr : in std_logic_vector(9 downto 0);   
        b_sync : out std_logic;                    
        b_frame_end : in std_logic;
        b_ntpal : out std_logic
    );
end output_linebuffer;

architecture Behavioral of output_linebuffer is

    component efx_simple_dual_port_2_clk_ram is
      generic (
        RAM_WIDTH : integer := 24;
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
    
    --clock domain crossing:
    signal bsel, bsel_b, cdc_bsel : std_logic := '0';
    signal cdc_end, a_end_r : std_logic := '0';
    signal cdc_re, a_re : std_logic := '0';
    signal cdc_sync, b_sync_r : std_logic := '0';
    signal b_ntpal_r, cdc_ntpal : std_logic := '0';
    --other control signals:
    signal re_rising : std_logic := '0';
    signal sync_extender : integer range 0 to 31 := 0;
    signal sync_long : std_logic := '0';
    --buffer depth of linebuffer, must be bigger than 2x horizontal resolution
    constant C_BUFFER_DEPTH : integer := 2048;

begin

    b_sync <= b_sync_r;
    b_ntpal <= b_ntpal_r; 
    a_frame_end <= a_end_r;  
    a_line_start <= re_rising;
    
    cdc_buffer: efx_simple_dual_port_2_clk_ram
      generic map (RAM_WIDTH => 24, RAM_DEPTH => C_BUFFER_DEPTH, OUTREG => true) 
      port map (
        addra  => bsel & a_addr,
        addrb  => bsel_b & b_addr,
        dina   => a_data,
        clka   => a_clk,
        clkb   => b_clk,
        wea    => a_we,
        enb    => b_re,
        rstb   => b_rst_n,
        regceb => b_re,
        doutb  => b_data);
    
    
     cdc:process(a_clk, b_clk)
     begin
     
         if(rising_edge(a_clk)) then
            cdc_end <= b_frame_end;
            a_end_r <= cdc_end;
            
            cdc_re <= b_re;
            a_re <= cdc_re;
        
            if(a_re = '0' and cdc_re = '1') then
                re_rising <= '1';
                bsel <= not bsel;
            else
                re_rising <= '0';  
            end if; 
            
            if(a_sync = '1') then
                sync_long <= '1';
                sync_extender <= 0;
            end if;
            
            if(sync_long = '1') then
                if(sync_extender < 30) then
                    sync_extender <= sync_extender + 1;
                else
                    sync_extender <= 0;
                    sync_long <= '0';  
                end if;
            end if;
         end if;
         
         if(rising_edge(b_clk)) then
            cdc_bsel <= not bsel;
            bsel_b <= cdc_bsel;
            
            cdc_ntpal <= a_video_cfg.ntpal_flag;
            b_ntpal_r <= cdc_ntpal;
            
            cdc_sync <= sync_long;
            b_sync_r <= cdc_sync;
         
         end if; 
     end process;
   
end Behavioral;
