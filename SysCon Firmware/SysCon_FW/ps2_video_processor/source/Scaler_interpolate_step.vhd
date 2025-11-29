----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Patrick Haid
-- 
-- Create Date: 02.04.2025 07:14:26
-- Design Name: PS2 Video Processor
-- Module Name: color_interpolate - Behavioral
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

entity color_interpolate is
Port ( 
    i_clk : in std_logic;
    i_rst_n : in std_logic;
    
    i_color_A : in unsigned(7 downto 0);
    i_color_B : in unsigned(7 downto 0);
    i_color_C : in unsigned(7 downto 0);
    i_color_D : in unsigned(7 downto 0);
    
    i_weight_A : in unsigned(17 downto 0);
    i_weight_B : in unsigned(17 downto 0);
    i_weight_C : in unsigned(17 downto 0);
    i_weight_D : in unsigned(17 downto 0);
    
    o_color : out unsigned(7 downto 0)
);
end color_interpolate;

architecture Behavioral of color_interpolate is

component unsigned_reg_mult is
generic(
    WIDTH : integer := 18
    );
port(
   clk : in std_logic;
   a : in unsigned(WIDTH-1 downto 0);
   b : in unsigned(WIDTH-1 downto 0);
   o : out unsigned(2*WIDTH-1 downto 0)
);
end component;

signal A_new, B_new, C_new, D_new : unsigned (35 downto 0) := (others=>'0');
signal AB, CD, color_out_r : unsigned (35 downto 0) := (others=>'0');


begin

new_A:unsigned_reg_mult port map(
    clk => i_clk,
    a => "0000000000" & i_color_A,
    b => i_weight_A,
    o => A_new);
  
new_B:unsigned_reg_mult port map(
    clk => i_clk,
    a => "0000000000" & i_color_B,
    b => i_weight_B,
    o => B_new);

new_C:unsigned_reg_mult port map(
    clk => i_clk,
    a => "0000000000" & i_color_C,
    b => i_weight_C,
    o => C_new);
    
new_D:unsigned_reg_mult port map(
    clk => i_clk,
    a => "0000000000" & i_color_D,
    b => i_weight_D,
    o => D_new);  


o_color <= color_out_r(22 downto 15);

process(i_clk, i_rst_n)
begin

if(i_rst_n = '0') then

else
    if(rising_edge(i_clk)) then  
        --STAGE 7
        --A_new_r = A_r * weight_A
        --B_new_r = B_r * weight_B
        --C_new_r = C_r * weight_C
        --D_new_r = D_r * weight_D
        
        --STAGE 8
        --AB_r = A_new_r + B_new_r
        --CD_r = C_new_r + D_new_r
        --nuke fractional part and convert to 8 bit integer
        AB <= A_new + B_new;
        CD <= C_new + D_new;
        
        --STAGE 9
        --output_pixel_r = AB_r + CD_r
        color_out_r <= AB + CD;
    end if;
end if;
end process;

end Behavioral;
