----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Patrick Haid
-- 
-- Create Date: 13.01.2025 21:35:41
-- Design Name: PS2 Video Processor
-- Module Name: Global Reset Module
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

entity reset_module is
Port ( 
    --PLL clock input 
    PLL_CLKOUT_0 :      in std_logic;  --clkin is 166MHz
    PLL_CLKOUT_1 :      in std_logic;  --clkin is 166MHz with phaseshift
    PLL_CLKOUT_2 :      in std_logic;  --clkin for video out 66408300Hz
    --PLL locked flag input
    locked:             in std_logic;
    --reset request input
    i_rst_req_n  :      in std_logic;
    --reset outputs; async assert, sync release
    o_rst_sync0  :      out std_logic; --reset for PLL CLK 0 domain (most internal logic)
    o_rst_sync1  :      out std_logic; --reset for PLL CLK 1 domain (shifted CLK0, only applicable for SDRAM controller)
    o_rst_sync2  :      out std_logic  --reset for PLL CLK 2 domain (video output clock)
);
end reset_module;

architecture Behavioral of reset_module is

--resets--------------------------------------------------------
signal rst_ctr_0, rst_ctr_1 : integer range 0 to 5000 := 0;
signal rst_sync_0, rst_sync_1, rst_sync_2, rst_sync_2a, rst_sync_2b, rst_sync_2c : std_logic := '0';

begin

o_rst_sync0 <= rst_sync_0;
o_rst_sync1 <= rst_sync_1;
o_rst_sync2 <= rst_sync_2;

reset_counter:process (PLL_CLKOUT_0, PLL_CLKOUT_1, PLL_CLKOUT_2, i_rst_req_n, locked)
begin
 if (i_rst_req_n = '0' or locked = '0') then
    rst_sync_0 <= '0';
    rst_sync_1 <= '0';
    rst_sync_2 <= '0';
    rst_ctr_0 <= 0;
    rst_ctr_1 <= 0;
 else 
    if rising_edge(PLL_CLKOUT_0) then
        if (rst_ctr_0 < 4400) then
            rst_ctr_0 <= rst_ctr_0 + 1;
        else
            rst_sync_0 <= '1';
        end if;
    end if;
    
    if rising_edge(PLL_CLKOUT_1) then
        if (rst_ctr_1 < 4400) then
            rst_ctr_1 <= rst_ctr_1 + 1;
        else
            rst_sync_1 <= '1';
        end if;
    end if;
    
    if rising_edge(PLL_CLKOUT_2) then
        rst_sync_2a <= rst_sync_0;
        rst_sync_2b <= rst_sync_2a;
        rst_sync_2c <= rst_sync_2b;
        
        if(rst_sync_2c = '0' and rst_sync_2b = '1') then
            rst_sync_2 <= '1';
        end if;
        
    end if;
end if;
end process; 
end Behavioral;
