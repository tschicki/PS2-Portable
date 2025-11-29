----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Patrick Haid
-- 
-- Create Date: 11.04.2025 13:32:41
-- Design Name: PS2 Video Processor
-- Module Name: line_fifo_improved - Behavioral
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
 
entity line_fifo_improved is
  generic (
    RAM_WIDTH : integer := 24;
    RAM_DEPTH : integer := 801;
    OUTREG    : boolean := true
    );
  port (
   --deinterlacer side
   i_rst_n : in std_logic;
   i_clk : in std_logic; 
   i_we : in std_logic;
   i_data : in std_logic_vector(RAM_WIDTH-1 downto 0);
   i_addr : in std_logic_vector((logb2(RAM_DEPTH)-1) downto 0);
   o_fifo_full : out std_logic;
   i_sync : in std_logic;
   
   --scaler side
   i_re : in std_logic;
   o_data_a : out std_logic_vector(RAM_WIDTH-1 downto 0);             --date to out line buffer
   o_data_b : out std_logic_vector(RAM_WIDTH-1 downto 0);
   o_data_c : out std_logic_vector(RAM_WIDTH-1 downto 0);             --date to out line buffer
   o_data_d : out std_logic_vector(RAM_WIDTH-1 downto 0);
   
   i_addr_low : in std_logic_vector((logb2(RAM_DEPTH)-1) downto 0);
   i_addr_high : in std_logic_vector((logb2(RAM_DEPTH)-1) downto 0);
   tail_ptr_inc : in unsigned(1 downto 0);                            --increment tail pointer                      
   o_fifo_empty : out std_logic
  );
end line_fifo_improved;
 
architecture rtl of line_fifo_improved is
 
component linebuffer_dual_port_read is
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
end component;

type state_type is (state_init, state_check, state_active, state_end, state_skip_line, state_correct, state_wait);
signal a_state, b_state : state_type := state_init;

signal a_ram_sel : std_logic_vector(5 downto 0) := "000000";
signal fifo_full_r : std_logic := '0';
signal we_falling, we_falling_1 : std_logic := '0';
signal head_pointer, head_pointer_corrected, tail_pointer, fill_count, tail_pointer_temp : unsigned(3 downto 0) := "0000";

signal fifo_empty_r : std_logic := '1';
signal re_falling, re_falling_1 : std_logic := '0';

signal data_l_0, data_h_0, data_l_1, data_h_1: std_logic_vector(RAM_WIDTH-1 downto 0) := (others=>'0');
signal data_l_2, data_h_2, data_l_3, data_h_3: std_logic_vector(RAM_WIDTH-1 downto 0) := (others=>'0');
signal data_l_4, data_h_4, data_l_5, data_h_5: std_logic_vector(RAM_WIDTH-1 downto 0) := (others=>'0');

signal addra_0, addra_1, addra_2, addra_3, addra_4, addra_5 : std_logic_vector((logb2(RAM_DEPTH)-1) downto 0);

begin

line_0:linebuffer_dual_port_read
generic map (RAM_WIDTH => RAM_WIDTH, RAM_DEPTH => RAM_DEPTH, OUTREG => OUTREG)
port map(
   i_rst_n =>i_rst_n,
   i_clk =>i_clk,
   i_we =>a_ram_sel(0),
   i_data =>i_data,
   i_addr =>i_addr,
   i_re =>i_re,
   i_addr_low =>i_addr_low,
   i_addr_high =>i_addr_high,
   o_data_low =>data_l_0,
   o_data_high =>data_h_0
   );
   
line_1:linebuffer_dual_port_read
generic map (RAM_WIDTH => RAM_WIDTH, RAM_DEPTH => RAM_DEPTH, OUTREG => OUTREG)
port map(
   i_rst_n =>i_rst_n,
   i_clk =>i_clk,
   i_we =>a_ram_sel(1),
   i_data =>i_data,
   i_addr =>i_addr,
   i_re =>i_re,
   i_addr_low =>i_addr_low,
   i_addr_high =>i_addr_high,
   o_data_low =>data_l_1,
   o_data_high =>data_h_1
   );
   
line_2:linebuffer_dual_port_read
generic map (RAM_WIDTH => RAM_WIDTH, RAM_DEPTH => RAM_DEPTH, OUTREG => OUTREG)
port map(
   i_rst_n =>i_rst_n,
   i_clk =>i_clk,
   i_we =>a_ram_sel(2),
   i_data =>i_data,
   i_addr =>i_addr,
   i_re =>i_re,
   i_addr_low =>i_addr_low,
   i_addr_high =>i_addr_high,
   o_data_low =>data_l_2,
   o_data_high =>data_h_2
   );
   
line_3:linebuffer_dual_port_read
generic map (RAM_WIDTH => RAM_WIDTH, RAM_DEPTH => RAM_DEPTH, OUTREG => OUTREG)
port map(
   i_rst_n =>i_rst_n,
   i_clk =>i_clk,
   i_we =>a_ram_sel(3),
   i_data =>i_data,
   i_addr =>i_addr,
   i_re =>i_re,
   i_addr_low =>i_addr_low,
   i_addr_high =>i_addr_high,
   o_data_low =>data_l_3,
   o_data_high =>data_h_3
   );  
   
line_4:linebuffer_dual_port_read
generic map (RAM_WIDTH => RAM_WIDTH, RAM_DEPTH => RAM_DEPTH, OUTREG => OUTREG)
port map(
   i_rst_n =>i_rst_n,
   i_clk =>i_clk,
   i_we =>a_ram_sel(4),
   i_data =>i_data,
   i_addr =>i_addr,
   i_re =>i_re,
   i_addr_low =>i_addr_low,
   i_addr_high =>i_addr_high,
   o_data_low =>data_l_4,
   o_data_high =>data_h_4
   );
   
line_5:linebuffer_dual_port_read
generic map (RAM_WIDTH => RAM_WIDTH, RAM_DEPTH => RAM_DEPTH, OUTREG => OUTREG)
port map(
   i_rst_n =>i_rst_n,
   i_clk =>i_clk,
   i_we =>a_ram_sel(5),
   i_data =>i_data,
   i_addr =>i_addr,
   i_re =>i_re,
   i_addr_low =>i_addr_low,
   i_addr_high =>i_addr_high,
   o_data_low =>data_l_5,
   o_data_high =>data_h_5
   );

edges:process(i_clk)
begin

if(rising_edge(i_clk)) then

    we_falling_1 <= i_we;

    if(i_we = '0' and we_falling_1 = '1') then
        we_falling <= '1';
    else
        we_falling <= '0';
    end if;
    
    re_falling_1 <= i_re;

    if(i_re = '0' and re_falling_1 = '1') then
        re_falling <= '1';
    else
        re_falling <= '0';
    end if;  
    
end if;
end process;

write_fsm:process(i_clk, i_rst_n, i_sync)
begin

if(i_rst_n = '0' or i_sync = '1') then
   a_state <= state_init;
   head_pointer <= "0000";
   head_pointer_corrected <= "0000";
   fifo_full_r <= '0';
else
    if(rising_edge(i_clk)) then
        --calculate the fill count based on current pointers, this happens outside of the state machines, but the state machines will have to wait for the updated fill count
        if(tail_pointer > head_pointer) then
            fill_count <= head_pointer_corrected - tail_pointer;
        else
            fill_count <= head_pointer - tail_pointer;
        end if;
        
        case(a_state) is
            
            when state_init =>
                fifo_full_r <= '0';
                a_state <= state_check;
        
            when state_check =>
                if(fill_count = 5) then
                    fifo_full_r <= '1'; --fifo is full
                else
                    fifo_full_r <= '0'; --fifo has space 
                end if;
                
                --while writing we force the fifo full flag so after the transfer we have time to analyze the fill count before the next write
                if(i_we = '1' and fifo_full_r = '0') then
                    a_state <= state_active;
                end if;
                
            when state_active =>
                fifo_full_r <= '1';     --if a write is ongoing we block the fifo full flag
            
                if(we_falling = '1') then
                    if(head_pointer < 5) then
                        head_pointer <= head_pointer + 1;
                    else
                        head_pointer <= "0000";
                    end if;
                    a_state <= state_correct;
                end if;
            
            when state_correct => --get us some time to calculate the corrected head pointer, if needed
                head_pointer_corrected <= head_pointer + 6;
                a_state <= state_wait;
            
            when state_wait =>  --a delay cycle for the fill count to be calculated
                a_state <= state_check;
            
            when others => null;
        
        end case;

    end if;
end if;
end process;
--set the WE for the currently selected ram
a_ram_sel <= "000001" when (head_pointer = 0 and i_we = '1') else
             "000010" when (head_pointer = 1 and i_we = '1') else
             "000100" when (head_pointer = 2 and i_we = '1') else
             "001000" when (head_pointer = 3 and i_we = '1') else
             "010000" when (head_pointer = 4 and i_we = '1') else
             "100000" when (head_pointer = 5 and i_we = '1') else
             "000000";
                       
o_fifo_full <= fifo_full_r;

read_fsm:process(i_clk, i_rst_n, i_sync)
begin

if(i_rst_n = '0' or i_sync = '1') then
   b_state <= state_init;
   tail_pointer <= "0000";
   tail_pointer_temp <= "0000";
   fifo_empty_r <= '1'; 
else
    if(rising_edge(i_clk)) then

        case(b_state) is
    
        when state_init =>
            --ready is set to low after every line
            fifo_empty_r <= '1';
            b_state <= state_check;
            
        when state_check =>
            --before ready is driven high, check that there is at least 1 free line in the buffer to write
            if(fill_count < 2) then
                fifo_empty_r <= '1'; -- fifo is empty
            else
                fifo_empty_r <= '0'; --fifo has entries
            end if;
            
            if(tail_ptr_inc /= "00") then
                tail_pointer_temp <= tail_pointer + tail_ptr_inc;   --if not zero, increment the temp tail pointer by tail_ptr_inc
            end if;
            
            if(tail_pointer_temp > 5) then
                tail_pointer <= tail_pointer_temp - 6;  --the sum was higher than the fifo count, so correct it
            else
                tail_pointer <= tail_pointer_temp;  --the sum was below the fifo count, no action necessary
            end if;
           
            --when reading we force the fifo empty flag to have enough time after the transfer for updating the fill count
            if(i_re = '1' and fifo_empty_r = '0') then
                b_state <= state_active;
            end if;
            
        when state_active =>
            fifo_empty_r <= '1';
            --read line
            if(re_falling = '1') then
                b_state <= state_check;
            end if;
            
        when state_wait =>
            b_state <= state_check;
        
        when others => null;
        end case;
              
    end if;
end if;
end process;

--muxes to select which ram appears on the output, beware that the RE condition was removed (so the output never goes back to zero             
o_data_a <= data_l_0 when (tail_pointer = 0) else
            data_l_1 when (tail_pointer = 1) else
            data_l_2 when (tail_pointer = 2) else
            data_l_3 when (tail_pointer = 3) else
            data_l_4 when (tail_pointer = 4) else
            data_l_5 when (tail_pointer = 5) else
            (others => '0');
 
o_data_b <= data_h_0 when (tail_pointer = 0) else
            data_h_1 when (tail_pointer = 1) else
            data_h_2 when (tail_pointer = 2) else
            data_h_3 when (tail_pointer = 3) else
            data_h_4 when (tail_pointer = 4) else
            data_h_5 when (tail_pointer = 5) else
            (others => '0');           

o_data_c <= data_l_1 when (tail_pointer = 0) else
            data_l_2 when (tail_pointer = 1) else
            data_l_3 when (tail_pointer = 2) else
            data_l_4 when (tail_pointer = 3) else
            data_l_5 when (tail_pointer = 4) else
            data_l_0 when (tail_pointer = 5) else
            (others => '0');   

o_data_d <= data_h_1 when (tail_pointer = 0) else
            data_h_2 when (tail_pointer = 1) else
            data_h_3 when (tail_pointer = 2) else
            data_h_4 when (tail_pointer = 3) else
            data_h_5 when (tail_pointer = 4) else
            data_h_0 when (tail_pointer = 5) else
            (others => '0');
      
o_fifo_empty <= fifo_empty_r;


end rtl;
