----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 26.11.2025 20:11:05
-- Design Name: PS2 Video Processor
-- Module Name: PS2 Video MAGH Detector Module
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
use work.ram_pkg.all;

entity MAGH_detector is
    Port ( 
        i_rst_n :           in std_logic;                         --reset input from PS2
        i_clk :             in std_logic;                         --PS2 pixel clock input
        i_data :            in STD_LOGIC_VECTOR (23 downto 0);    --parallel RGB video from PS2
        
        i_h_active :        in std_logic;                         --horizontal active
        i_v_active :        in std_logic;                         --vertical active
        o_magh_det :        out std_logic_vector (3 downto 0)     --detected MAGH value
        
    );
end MAGH_detector;

architecture Behavioral of MAGH_detector is

type state_type is (state_wait_for_active,
                    state_wait_for_data,
                    state_wait_for_pixel_boundary,
                    state_magh_detection,
                    state_magh_evaluate);
                        
signal state : state_type := state_wait_for_active;

signal compare_counter : unsigned(12 downto 0) := (others => '0');
signal compare_counter_line, compare_counter_frame : unsigned(12 downto 0) := (others => '0');
signal data_last : std_logic_vector (23 downto 0) := (others => '0');

signal o_magh_det_r : std_logic_vector(3 downto 0) := "0000";

begin

magh:process(i_clk, i_rst_n)
    begin
    
    if(i_rst_n = '0') then
        compare_counter <= (others => '0');
        compare_counter_line <= "0000000001010";     --needs to be reset to the max value we can expect!
        compare_counter_frame <= "0000000001010";    --needs to be reset to the max value we can expect!
        state <= state_wait_for_active;
        o_magh_det_r <= "0000";
    else
        if(rising_edge(i_clk)) then

            if(i_v_active = '1') then
                data_last <= i_data;
            
                case(state) is
                    when state_wait_for_active =>
                        if(i_h_active = '1') then
                            state <= state_wait_for_data;
                        end if;
                    when state_wait_for_data =>
                        state <= state_wait_for_pixel_boundary;
                        
                    when state_wait_for_pixel_boundary =>
                        --just to make sure we don't take possibly cut-off pixels at the beginning of a line into account
                        if(i_data /= data_last) then
                            state <= state_magh_detection;
                        end if;
                        
                    when state_magh_detection => 
                        if(i_data = data_last) then
                            --if data matches we count up
                            compare_counter <= compare_counter + 1;
                        else
                            --if it doesn't match it means we crossed a pixel boundary, so store the count and reset the counter - only if the new count is lower that the previous one!
                            compare_counter <= (others => '0');
                            if(compare_counter < compare_counter_line) then
                                compare_counter_line <= compare_counter;
                            end if;
                        end if;
                        
                        if(i_h_active = '0') then
                            state <= state_magh_evaluate;
                        end if;
                        
                    when state_magh_evaluate =>
                        --if the result is plausible (<10) and the smallest yet, we store it
                        if(compare_counter_line < compare_counter_frame) then
                            --this is the lowest result yet
                            compare_counter_frame <= compare_counter_line;
                        end if;
                        
                        compare_counter <= (others => '0');
                        compare_counter_line <= "0000000001010";
                        state <= state_wait_for_active;
                        
                    when others =>
                        null;   
                    
                end case;
            else
                if(compare_counter_frame < 10 and compare_counter_frame > 2) then
                    --if the end result is plausible we output it
                    o_magh_det_r <= std_logic_vector(compare_counter_frame(3 downto 0));
                else
                    null;
                end if;
                
                compare_counter_frame <= "0000000001010";
                compare_counter_line <= "0000000001010";
                state <= state_wait_for_active;
            
            end if;
        end if;
        
    end if;
end process;

o_magh_det <= o_magh_det_r;

end Behavioral;
