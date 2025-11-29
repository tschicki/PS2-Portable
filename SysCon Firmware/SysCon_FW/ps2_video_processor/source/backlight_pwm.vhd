----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Patrick Haid
-- 
-- Create Date: 22.03.2025 23:29:43
-- Design Name: PS2 Video Processor
-- Module Name: backlight_pwm - Behavioral
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

entity backlight_pwm is
    Port ( 
        i_clk : in std_logic;
        i_rst_n : in std_logic;
        o_pwm : out std_logic;
        --control interface to configure all the modules
        i_data_config :     in std_logic_vector(7 downto 0);
        i_addr_config :     in std_logic_vector(15 downto 0);
        i_rw_config :       in std_logic;
        i_cs_config :       in std_logic
    );
end backlight_pwm;

architecture Behavioral of backlight_pwm is

--control interface
signal register_max_count : std_logic_vector(7 downto 0) := X"BB";
signal register_trigger_count : std_logic_vector(7 downto 0) := X"00";

type state_type_write is (state_wait, 
                          state_analyze,
                          state_access_regs
                          );
signal config_state : state_type_write := state_wait;

signal current_data : std_logic_vector(7 downto 0) := (others=>'0');
signal current_addr : std_logic_vector(15 downto 0) := (others=>'0');
signal rw_flag : std_logic := '0';

signal counter : integer range 0 to 4095 := 0;
signal pwm_r : std_logic := '1';
signal max_count : std_logic_vector(11 downto 0) := (others=>'0');
signal trigger_count : std_logic_vector(11 downto 0) := (others=>'0');

begin

count:process(i_clk)
begin

if(rising_edge(i_clk)) then

    if(counter < to_integer(unsigned(max_count))) then
        counter <= counter + 1;
        
        if(counter = to_integer(unsigned(trigger_count))) then
            pwm_r <= not pwm_r;  
        end if;
    else
        counter <= 0;
        pwm_r <= '1';
    end if;
    
    

end if;

end process;

o_pwm <= pwm_r;

config_write:process(i_clk)
begin
    if(rising_edge(i_clk)) then
    
        case(config_state) is
            when state_wait =>
                --wait for cs high
                if(i_cs_config = '1') then
                    current_data <= i_data_config;
                    current_addr <= i_addr_config;
                    rw_flag <= i_rw_config;
                    config_state <= state_analyze;
                end if;
                
            when state_analyze =>
                --check which ram is accessed
                if(current_addr(15 downto 12) = RANGE_BRIGHTNESS) then
                    config_state <= state_access_regs;
                else
                    config_state <= state_wait;
                end if;
 
            when state_access_regs =>
                if(rw_flag = '0') then
                    if(current_addr(0) = '0') then
                        register_max_count <= current_data;
                    else
                        register_trigger_count <= current_data;
                    end if;
                end if;
                config_state <= state_wait;   
            when others => null;
        end case;
        
    max_count <= register_max_count & "0000";
    trigger_count <= register_trigger_count & "0000";
    
    end if;

end process;

end Behavioral;
