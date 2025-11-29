----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Patrick Haid
-- 
-- Create Date: 13.01.2025 21:35:41
-- Design Name: PS2 Video Processor
-- Module Name: LCD Video Output Module
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
--clkin is 100MHz
--DEN is latched on falling clk
    --latched together with the first bit of data
--data is latched on falling clk
    --64CLK after falling edge of hsync
--vsync is latched on falling clk, negative polarity
--hsync is latched on falling clk, negative polarity

entity video_out_de is
Port( 
   --interface with out line buffer
   i_rst_n : in std_logic;
   i_clk : in std_logic;                                 --framebuffer side pixel clock
   o_re : out std_logic;                                --read request from the output line buffer
   i_data : in std_logic_vector(23 downto 0);             --date to out line buffer
   o_addr : out std_logic_vector(9 downto 0);
   i_ntpal : in std_logic;            --control word relevant for video out
   i_ready : in std_logic;                             --high as long as there is data to be read from the framebuffer; will go low when one whole frame was read
   o_end : out std_logic;
   
   --video output signals
   o_data : out STD_LOGIC_VECTOR (23 downto 0);
   o_de : out STD_LOGIC;
   o_lcd_en : out std_logic
   );
end video_out_de;

architecture Behavioral of video_out_de is

type state_type is (state_init, state_idle_pal, state_idle_ntsc);
signal state : state_type := state_init;

signal pixel_counter, read_counter, line_counter : integer range 0 to 1023 := 0;
signal v_active, h_active : std_logic := '0';

signal read_en, read_counter_enable : std_logic := '0';
signal frame_end : std_logic := '0';
signal lcd_en_out : std_logic := '1';

begin

counters:process(i_clk, i_rst_n)
begin

if(i_rst_n = '0') then
    pixel_counter <= 0;
    line_counter <= 0;
    read_counter <= 0;
    read_en <= '0';
    read_counter_enable <= '0';
    v_active <= '0';
    h_active <= '0';
    frame_end <= '0';
    lcd_en_out <= '0';
    state <= state_init;
else

    if(rising_edge(i_clk)) then

        case(state) is
        when state_init =>
            --stay in init until the last line of the frame was transmitted
            pixel_counter <= 0;
            line_counter <= 0;
            v_active <= '0';
            h_active <= '0';
            --frame_end <= '0';
            if(i_ready = '1') then
                
                frame_end <= '0';
                lcd_en_out <= '1';
                
                if(i_ntpal = C_PAL) then
                    state <= state_idle_pal;
                else
                    state <= state_idle_ntsc;
                end if;
                
            else
                state <= state_init;
            end if;
    
        when state_idle_pal =>
        
            if(read_counter_enable = '1') then
                read_counter <= read_counter + 1;
            end if;

            pixel_counter <= pixel_counter + 1;
            
            case(pixel_counter) is
                when 80 =>
                    if(v_active = '1') then
                        read_en <= '1';
                    else
                        read_en <= '0';
                    end if;
                    
                when 88 =>
                    if(v_active = '1') then
                        read_counter_enable <= '1';
                    else
                        read_counter_enable <= '0';
                    end if;
                
                when 90 to 889 =>
                    h_active <= '1';
                
                when 890 =>
                    h_active <= '0';
                    read_counter <= 1;
                    read_en <= '0';
                    read_counter_enable <= '0';
                    pixel_counter <= 0;
                
                    line_counter <= line_counter + 1;

                    case(line_counter) is
                        when 7 to 486 =>
                            --beginning of valid data
                            v_active <= '1';         
                        when 487 =>
                            v_active <= '0';
                            frame_end <= '1';
                            state <= state_init;
                        when others =>
                            v_active <= '0';
                            frame_end <= '0';
                    end case;
                when others =>
                    h_active <= '0';
            end case;  
            
        when state_idle_ntsc =>
        
            if(read_counter_enable = '1') then
                read_counter <= read_counter + 1;
            end if;

            pixel_counter <= pixel_counter + 1;
            
            case(pixel_counter) is
                when 10 =>
                    if(v_active = '1') then
                        read_en <= '1';
                    else
                        read_en <= '0';
                    end if;
                    
                when 18 =>
                    if(v_active = '1') then
                        read_counter_enable <= '1';
                    else
                        read_counter_enable <= '0';
                    end if;
                
                when 20 to 819 =>
                    h_active <= '1';
                
                when 820 =>
                    h_active <= '0';
                    read_counter <= 1;
                    read_en <= '0';
                    read_counter_enable <= '0';
                    pixel_counter <= 0;
                
                    line_counter <= line_counter + 1;

                    case(line_counter) is
                        when 7 to 486 =>
                            --beginning of valid data
                            v_active <= '1';         
                        when 487 =>
                            v_active <= '0';
                            frame_end <= '1';
                            state <= state_init;
                        when others =>
                            v_active <= '0';
                            frame_end <= '0';
                    end case;
                when others =>
                    h_active <= '0';
            end case; 
            
            
             
        when others => null;
        end case;
    end if;
end if;    

end process;

o_data <= i_data when (v_active = '1' and h_active = '1') else
             (others=>'0'); 

o_de <= '1' when (v_active = '1' and h_active = '1') else
      '0';
      
o_addr <= std_logic_vector(to_unsigned(read_counter, 10));
o_lcd_en <= lcd_en_out;
o_end <= frame_end;
o_re <= read_en;

end Behavioral;

