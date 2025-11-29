----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Patrick Haid
-- 
-- Create Date: 11.04.2025 13:35:38
-- Design Name: PS2 Video Processor
-- Module Name: linebuffer_dual_port_read - Behavioral
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

entity linebuffer_dual_port_read is
  generic (
    RAM_WIDTH : integer := 24;
    RAM_DEPTH : integer := 800;
    OUTREG    : boolean := true
    );
  Port (
  --write port
   i_rst_n :        in std_logic;
   i_clk :          in std_logic; 
   i_we :           in std_logic;
   i_data :         in std_logic_vector(RAM_WIDTH-1 downto 0);
   i_addr :         in std_logic_vector((logb2(RAM_DEPTH)-1) downto 0);
   
   --read port
   i_re :           in std_logic;
   i_addr_low :     in std_logic_vector((logb2(RAM_DEPTH)-1) downto 0);
   i_addr_high :    in std_logic_vector((logb2(RAM_DEPTH)-1) downto 0);
   o_data_low :     out std_logic_vector(RAM_WIDTH-1 downto 0);             --date to out line buffer
   o_data_high :    out std_logic_vector(RAM_WIDTH-1 downto 0)
   );
end linebuffer_dual_port_read;

architecture Behavioral of linebuffer_dual_port_read is

component efx_simple_dual_port_2_clk_ram is
  generic (
    RAM_WIDTH : integer := 24;
    RAM_DEPTH : integer := 800;
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

begin

line_low: efx_simple_dual_port_2_clk_ram
  generic map (RAM_WIDTH => RAM_WIDTH, RAM_DEPTH => RAM_DEPTH, OUTREG => OUTREG)
  port map (
    addra  => i_addr,
    addrb  => i_addr_low,
    dina   => i_data,
    clka   => i_clk,
    clkb   => i_clk,
    wea    => i_we,
    enb    => i_re,
    rstb   => i_rst_n,
    regceb => i_re,
    doutb  => o_data_low);

line_high: efx_simple_dual_port_2_clk_ram
  generic map (RAM_WIDTH => RAM_WIDTH, RAM_DEPTH => RAM_DEPTH, OUTREG => OUTREG)
  port map (
    addra  => i_addr,
    addrb  => i_addr_high,
    dina   => i_data,
    clka   => i_clk,
    clkb   => i_clk,
    wea    => i_we,
    enb    => i_re,
    rstb   => i_rst_n,
    regceb => i_re,
    doutb  => o_data_high);


end Behavioral;
