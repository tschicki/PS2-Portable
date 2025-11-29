----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Patrick Haid
-- 
-- Create Date: 07.04.2025 21:08:04
-- Design Name: PS2 Video Processor
-- Module Name: bilinear_scaler_pipeline - Behavioral
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

library work;
use work.scaler_types.all;

entity bilinear_scaler_pipeline is
Port ( 
    --control inputs
    i_clk :                 in std_logic;
    i_rst_n :               in std_logic;
    --signals fed into the pipeline
    x_scaling_factor :      in unsigned (17 downto 0);
    y_scaling_factor :      in unsigned (17 downto 0);
    pixel_counter :         in unsigned (17 downto 0);
    line_counter :          in unsigned (17 downto 0);
    i_data_a :              in std_logic_vector (23 downto 0);
    i_data_b :              in std_logic_vector (23 downto 0);
    i_data_c :              in std_logic_vector (23 downto 0);
    i_data_d :              in std_logic_vector (23 downto 0);
    --signals returning from the pipeline
    addr_x_low :            out unsigned (9 downto 0);
    addr_x_high :           out unsigned (9 downto 0);
    addr_y_low :            out unsigned (9 downto 0);
    o_data :                out unsigned(23 downto 0)
    );
end bilinear_scaler_pipeline;

architecture Behavioral of bilinear_scaler_pipeline is

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

component color_interpolate is
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
end component;

--interfacing with multipliers
signal x_pixel_addr, y_pixel_addr : unsigned (35 downto 0) := (others=>'0');
signal x_l, x_h, y_l, y_l_last, x_l_1 : unsigned (9 downto 0) := (others=>'0');
signal x_weight, x_weight_1, y_weight, y_weight_1, x_weight_r, y_weight_r : unsigned (17 downto 0) := (others=>'0');
signal weight_A_mult, weight_B_mult, weight_C_mult, weight_D_mult : unsigned (35 downto 0) := (others=>'0');
signal weight_A, weight_B, weight_C, weight_D : unsigned (17 downto 0) := (others=>'0');
signal pixel_A, pixel_B, pixel_C, pixel_D : unsigned(23 downto 0) := (others=>'0');
signal out_r, out_g, out_b : unsigned(7 downto 0) := (others=>'0');

begin

x_address:unsigned_reg_mult port map(
    clk => i_clk,
    a => x_scaling_factor,
    b => pixel_counter,
    o => x_pixel_addr);

y_address:unsigned_reg_mult port map(
    clk => i_clk,
    a => y_scaling_factor,
    b => line_counter,
    o => y_pixel_addr);
    
a_weight:unsigned_reg_mult port map(
    clk => i_clk,
    a => x_weight_1,
    b => y_weight_1,
    o => weight_A_mult);

b_weight:unsigned_reg_mult port map(
    clk => i_clk,
    a => x_weight,
    b => y_weight_1,
    o => weight_B_mult);

c_weight:unsigned_reg_mult port map(
    clk => i_clk,
    a => y_weight,
    b => x_weight_1,
    o => weight_C_mult);

d_weight:unsigned_reg_mult port map(
    clk => i_clk,
    a => x_weight,
    b => y_weight,
    o => weight_D_mult);

color_r:color_interpolate port map( 
    i_clk => i_clk,
    i_rst_n => i_rst_n,
    
    i_color_A => pixel_A(7 downto 0),
    i_color_B => pixel_B(7 downto 0),
    i_color_C => pixel_C(7 downto 0),
    i_color_D => pixel_D(7 downto 0),
    
    i_weight_A => weight_A,
    i_weight_B => weight_B,
    i_weight_C => weight_C,
    i_weight_D => weight_D,
    
    o_color => out_r
); 
color_g:color_interpolate port map( 
    i_clk => i_clk,
    i_rst_n => i_rst_n,
    
    i_color_A => pixel_A(15 downto 8),
    i_color_B => pixel_B(15 downto 8),
    i_color_C => pixel_C(15 downto 8),
    i_color_D => pixel_D(15 downto 8),
    
    i_weight_A => weight_A,
    i_weight_B => weight_B,
    i_weight_C => weight_C,
    i_weight_D => weight_D,
    
    o_color => out_g
); 
color_b:color_interpolate port map( 
    i_clk => i_clk,
    i_rst_n => i_rst_n,
    
    i_color_A => pixel_A(23 downto 16),
    i_color_B => pixel_B(23 downto 16),
    i_color_C => pixel_C(23 downto 16),
    i_color_D => pixel_D(23 downto 16),
    
    i_weight_A => weight_A,
    i_weight_B => weight_B,
    i_weight_C => weight_C,
    i_weight_D => weight_D,
    
    o_color => out_b
); 

---------------------------------------------------------------------------------------------
--BILINEAR SCALER CALCULATION:
--scaling factors: 3 bit integer + 15 bit fractal
--weight: 3 bit integer + 15 bit fractal
--product of weights: 3 bit integer + 15 bit fractal

--all calculations are always positive -> use unsigned
    
--x_scaling_factor_r = (input_resolution_h - 1) / (output_resolution_h - 1)
--y_scaling_factor_r = (input_resolution_v - 1) / (output_resolution_v - 1)

 calculate:process(i_clk, i_rst_n)
 begin
 
 if(i_rst_n = '0') then
 
 else
    if(rising_edge(i_clk)) then
    ----------------------------
    --STAGE 1
    --output_pxl_counter must always start at 1 for the calculation to be correct
    --x_pixel_addr = (x_scaling_factor_r * output_pxl_counter)
    --y_pixel_addr = (y_scaling_factor_r * output_line_counter)
    
    --STAGE 2
    --nuking the fractional part -> converting to integer
    x_l_1 <= x_pixel_addr(24 downto 15);
    y_l <= y_pixel_addr(24 downto 15);
    
    --STAGE 3
    --adding 1 to geht the high address
    x_h <= x_l_1 + 1;
    x_l <= x_l_1;   --buffer for them to be synchronous
    
    -----------------------------------------------------------------
    --STAGE 4
    --nuking the integer part of the weight
    x_weight_r <= "000" & x_pixel_addr(14 downto 0);
    y_weight_r <= "000" & y_pixel_addr(14 downto 0);
    
    --STAGE 5
    x_weight_1 <= X"8000" - x_weight_r(17 downto 0);    -- 1 - x_weight_r
    y_weight_1 <= X"8000" - y_weight_r(17 downto 0);    -- 1 - y_weight_r
    --buffering for using later:
    x_weight <= x_weight_r;
    y_weight <= y_weight_r;
    
    --STAGE 6
    --converting the multiplication result back to original scale
    weight_A <= weight_A_mult(32 downto 15);    --weight_A = x_weight_1 * y_weight_1
    weight_B <= weight_B_mult(32 downto 15);    --weight_B = x_weight * y_weight_1
    weight_C <= weight_C_mult(32 downto 15);    --weight_C = y_weight * x_weight_1
    weight_D <= weight_D_mult(32 downto 15);    --weight_D = x_weight * y_weight
    
    --before this point we must probably determine which line is to load , see above line_inc
    pixel_A <= unsigned(i_data_a);
    pixel_B <= unsigned(i_data_b);
    pixel_C <= unsigned(i_data_c);
    pixel_D <= unsigned(i_data_d);
    
    --following calculations are handled in the color_interpolate modules:
    --STAGE 7
    --A_new = A * weight_A
    --B_new = B * weight_B
    --C_new = C * weight_C
    --D_new = D * weight_D
   
    --STAGE 8
    --AB = A_new + B_new
    --CD = C_new + D_new
    
    --STAGE 9
    --output_pixel = AB + CD
 
    end if;
 
 end if;
 end process;
 
 o_data <= out_b & out_g & out_r;
 addr_x_low <= x_l;
 addr_x_high <= x_h;
 addr_y_low <= y_l;

end Behavioral;
