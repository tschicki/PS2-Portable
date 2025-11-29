----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Patrick Haid
-- 
-- Create Date: 30.03.2025 19:48:59
-- Design Name: PS2 Video Processor
-- Module Name: multilplier - Behavioral
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
use ieee.numeric_std.all;

entity signed_reg_mult is
generic(
    WIDTH : integer := 3
    );
port(
   clk : in std_logic;
   a : in signed(WIDTH-1 downto 0);
   b : in signed(WIDTH-1 downto 0);
   o : out signed(2*WIDTH-1 downto 0)
);
end signed_reg_mult;

architecture rtl of signed_reg_mult is
    signal a_reg : signed (WIDTH-1 downto 0);
    signal b_reg : signed (WIDTH-1 downto 0);
    signal output : signed (2*WIDTH-1 downto 0);
begin

output <= a_reg * b_reg;

process(clk)
begin
    if(rising_edge(clk)) then
       a_reg <= a;
	   b_reg <= b;
	   o <= output;
	end if;
	
end process;
end rtl;


library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity unsigned_reg_mult is
generic(
    WIDTH : integer := 3
    );
port(
   clk : in std_logic;
   a : in unsigned(WIDTH-1 downto 0);
   b : in unsigned(WIDTH-1 downto 0);
   o : out unsigned(2*WIDTH-1 downto 0)
);
end unsigned_reg_mult;

architecture rtl of unsigned_reg_mult is
    signal a_reg : unsigned (WIDTH-1 downto 0);
    signal b_reg : unsigned (WIDTH-1 downto 0);
    signal output : unsigned (2*WIDTH-1 downto 0);
begin

output <= a_reg * b_reg;

process(clk)
begin
    if(rising_edge(clk)) then
       a_reg <= a;
	   b_reg <= b;
	   o <= output;
	end if;
	
end process;

end rtl;
