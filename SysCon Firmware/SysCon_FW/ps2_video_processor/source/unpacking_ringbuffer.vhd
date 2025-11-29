----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Patrick Haid
-- 
-- Create Date: 13.01.2025 21:35:41
-- Design Name: PS2 Video Processor
-- Module Name: Pixel Data Unpacker
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
  
entity ring_buffer is
  port (
    clk :                   in std_logic;
    rst_n :                 in std_logic;
    -- Write port
    i_valid :               in std_logic;
    i_data :                in std_logic_vector(31 downto 0);
    -- Read port
    o_we_en :               out std_logic;
    o_data :                out std_logic_vector(23 downto 0);
    o_addr :                out std_logic_vector(9 downto 0)
  );
end ring_buffer;
  
architecture rtl of ring_buffer is

constant RAM_DEPTH : integer := 192; --gemeinsames vielfaches von 24 & 32, unbedingt!!!
constant HEAD_LIMIT : integer := RAM_DEPTH-32-1;
constant TAIL_LIMIT : integer := RAM_DEPTH-24-1;

  signal head_lo : integer range RAM_DEPTH-1 downto 0 := 0;
  signal head_hi : integer range RAM_DEPTH-1 downto 0 := 31;
  signal tail_lo : integer range RAM_DEPTH-1 downto 0 := 0;
  signal tail_hi : integer range RAM_DEPTH-1 downto 0 := 23;
  
  signal inbuffer : std_logic_vector(RAM_DEPTH-1 downto 0) := (others=>'0');
  
  signal empty_i : std_logic;
  signal we_e_r : std_logic;
  signal fill_count_i : integer range RAM_DEPTH-1 downto 0;
  
  signal pxl_counter : integer range 1023 downto 0 := 0;
  
begin
  
  -- Copy internal signals to output
  o_we_en <= we_e_r;
  o_addr <= std_logic_vector(to_unsigned(pxl_counter, 10));
  -- Set the flags
  empty_i <= '0' when fill_count_i >= 48 else '1';
  
  -- Update the head pointer in write
  PROC_HEAD : process(clk, rst_n)
  begin
    if rst_n = '0' then
        head_lo <= 0;
        head_hi <= 31; 
    else
        if rising_edge(clk) then
            if i_valid = '1' then
                if(head_hi <= HEAD_LIMIT) then
                    head_lo <= head_lo + 32;
                    head_hi <= head_lo + 63; 
                else
                    head_lo <= 0;
                    head_hi <= 31;
                end if;
            end if;
            
        end if;
    end if;
  end process;
  
  -- Update the tail pointer on read and pulse valid
  PROC_TAIL : process(clk, rst_n)
  begin
    if rst_n = '0' then
        tail_lo <= 0;
        tail_hi <= 23;
        we_e_r <= '0';
        pxl_counter <= 0;
    else
        if rising_edge(clk) then 

            if empty_i = '0' then
                if(tail_hi <= TAIL_LIMIT) then
                    tail_lo <= tail_lo + 24;
                    tail_hi <= tail_lo + 47;
                else
                    tail_lo <= 0;
                    tail_hi <= 23;
                end if;
          
                we_e_r <= '1';
                pxl_counter <= pxl_counter + 1;
            else
                we_e_r <= '0';   
            end if;
  
        end if;
    end if;
  end process;
  
  -- Write to and read from the RAM
  PROC_RAM : process(clk, rst_n)
  begin
    if rst_n = '0' then
        inbuffer <= (others=>'0');
        fill_count_i <= 0;
    else
        if rising_edge(clk) then
            inbuffer(head_hi downto head_lo) <= i_data;
            o_data <= inbuffer(tail_hi downto tail_lo);
            
            if head_lo < tail_lo then
                fill_count_i <= head_lo - tail_lo + RAM_DEPTH;
            else
                fill_count_i <= head_lo - tail_lo;
            end if;
            
        end if;
    end if;
  end process;
  
end architecture;
