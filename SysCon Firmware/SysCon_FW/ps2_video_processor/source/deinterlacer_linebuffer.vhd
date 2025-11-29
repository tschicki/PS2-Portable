----------------------------------------------------------------------------------
-- Company:
-- Engineer: Patrick Haid
-- 
-- Create Date: 13.01.2025 21:35:41
-- Design Name: PS2 Video Processor
-- Module Name: Motion Adaptive Deinterlacer Linebuffer
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


library ieee;
use ieee.std_logic_1164.all;
use IEEE.NUMERIC_STD.ALL;
 
library work;
use work.scaler_types.all;
use work.ram_pkg.all;

entity ma_linebuffer is
    Port ( 
        i_clk : in std_logic;
        i_rst_sync_0 : in std_logic;
        --write
        we : in std_logic_vector(5 downto 0);
        wr_addr : in std_logic_vector(9 downto 0);
        wr_data : in std_logic_vector(23 downto 0);
        --read
        re : in std_logic;
        rd_addr : in std_logic_vector(9 downto 0);
        rd_data_0 : out std_logic_vector(23 downto 0);
        rd_data_1 : out std_logic_vector(23 downto 0);
        rd_data_2 : out std_logic_vector(23 downto 0);
        rd_data_3 : out std_logic_vector(23 downto 0);
        rd_data_4 : out std_logic_vector(23 downto 0);
        rd_data_5 : out std_logic_vector(23 downto 0)
    );
end ma_linebuffer;

architecture Behavioral of ma_linebuffer is

component efx_simple_dual_port_2_clk_ram is
  generic (
    RAM_WIDTH : integer := 24;
    RAM_DEPTH : integer := 1024;
    OUTREG    : boolean := true
    );
  Port (
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

begin

ram0_c: efx_simple_dual_port_2_clk_ram
  generic map (RAM_WIDTH => 24, RAM_DEPTH => 1024, OUTREG => true) 
  port map (
    addra  => wr_addr,
    addrb  => rd_addr,
    dina   => wr_data,
    clka   => i_clk,
    clkb   => i_clk,
    wea    => we(0),
    enb    => re,
    rstb   => i_rst_sync_0,
    regceb => re,
    doutb  => rd_data_0);

ram1_d: efx_simple_dual_port_2_clk_ram
  generic map (RAM_WIDTH => 24, RAM_DEPTH => 1024, OUTREG => true)
  port map (
    addra  => wr_addr,
    addrb  => rd_addr,
    dina   => wr_data,
    clka   => i_clk,
    clkb   => i_clk,
    wea    => we(1),
    enb    => re,
    rstb   => i_rst_sync_0,
    regceb => re,
    doutb  => rd_data_1); 
    
ram2_b: efx_simple_dual_port_2_clk_ram
  generic map (RAM_WIDTH => 24, RAM_DEPTH => 1024, OUTREG => true)
  port map (
    addra  => wr_addr,
    addrb  => rd_addr,
    dina   => wr_data,
    clka   => i_clk,
    clkb   => i_clk,
    wea    => we(2),
    enb    => re,
    rstb   => i_rst_sync_0,
    regceb => re,
    doutb  => rd_data_2);

ram3_e: efx_simple_dual_port_2_clk_ram
  generic map (RAM_WIDTH => 24, RAM_DEPTH => 1024, OUTREG => true)
  port map (
    addra  => wr_addr,
    addrb  => rd_addr,
    dina   => wr_data,
    clka   => i_clk,
    clkb   => i_clk,
    wea    => we(3),
    enb    => re,
    rstb   => i_rst_sync_0,
    regceb => re,
    doutb  => rd_data_3);

ram4_a: efx_simple_dual_port_2_clk_ram
  generic map (RAM_WIDTH => 24, RAM_DEPTH => 1024, OUTREG => true)
  port map (
    addra  => wr_addr,
    addrb  => rd_addr,
    dina   => wr_data,
    clka   => i_clk,
    clkb   => i_clk,
    wea    => we(4),
    enb    => re,
    rstb   => i_rst_sync_0,
    regceb => re,
    doutb  => rd_data_4);      

ram5_f: efx_simple_dual_port_2_clk_ram
  generic map (RAM_WIDTH => 24, RAM_DEPTH => 1024, OUTREG => true)
  port map (
    addra  => wr_addr,
    addrb  => rd_addr,
    dina   => wr_data,
    clka   => i_clk,
    clkb   => i_clk,
    wea    => we(5),
    enb    => re,
    rstb   => i_rst_sync_0,
    regceb => re,
    doutb  => rd_data_5);   


end Behavioral;
