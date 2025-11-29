----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Patrick Haid
-- 
-- Create Date: 13.01.2025 21:35:41
-- Design Name: PS2 Video Processor
-- Module Name: Motion Adaptive Deinterlacer
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
 
entity ma_deinterlacer_4field is
  port (
  --interface to framebuffer
   i_rst_n :        in std_logic;
   i_clk :          in std_logic; 
   o_re :           out std_logic;
   i_data :         in std_logic_vector(31 downto 0);     --video data
   a_addr :         out std_logic_vector(21 downto 0);
   i_video_cfg :    in video_config_t;                    --video config
   i_field_info :   in field_info_t;
   i_sync :         in std_logic;
   i_valid :        in std_logic;
   --interface to output buffer fifo
   o_we :           out std_logic;
   o_data :         out std_logic_vector(23 downto 0);
   o_addr :         out std_logic_vector(9 downto 0);
   i_ready :        in std_logic;
   i_end :          in std_logic
  );
end ma_deinterlacer_4field;
 
architecture rtl of ma_deinterlacer_4field is
 
component ma_linebuffer is
    Port ( 
    i_clk :         in std_logic;
    i_rst_sync_0 :  in std_logic;
    --write
    we :            in std_logic_vector(5 downto 0);
    wr_addr :       in std_logic_vector(9 downto 0);
    wr_data :       in std_logic_vector(23 downto 0);
    --read
    re :            in std_logic;
    rd_addr :       in std_logic_vector(9 downto 0);
    rd_data_0 :     out std_logic_vector(23 downto 0);
    rd_data_1 :     out std_logic_vector(23 downto 0);
    rd_data_2 :     out std_logic_vector(23 downto 0);
    rd_data_3 :     out std_logic_vector(23 downto 0);
    rd_data_4 :     out std_logic_vector(23 downto 0);
    rd_data_5 :     out std_logic_vector(23 downto 0)
    );
end component ma_linebuffer;

component ring_buffer is
  port(
    clk :               in std_logic;
    rst_n :             in std_logic;
    -- Write port
    i_valid :           in std_logic;
    i_data :            in std_logic_vector(31 downto 0);
    -- Read port
    o_we_en :           out std_logic;
    o_data :            out std_logic_vector(23 downto 0);
    o_addr :            out std_logic_vector(9 downto 0)
    );
end component;

--STATES-----
type prim_state_type is (state_init, state_read, state_even_first_line, state_odd_first_line, state_read_field_A, state_read_field_B, state_read_field_C, state_read_field_D, state_calculate, state_wait, state_odd_even_check, state_wait_for_sync, state_progressive_A, state_progressive_B, state_linedouble_A, state_linedouble_B);
type sec_state_type is (state_sec_idle, state_sec_output_blank, state_sec_output_field_a, state_sec_motion, state_sec_check_busy);
signal prim_state, prim_next_state, prim_state_queue : prim_state_type := state_init;
signal sec_state, sec_next_state, sec_state_queue : sec_state_type := state_sec_idle;

--data input side signals
signal addr_framebuffer : std_logic_vector(21 downto 0) := (others => '0');
signal out_line_ctr : integer range 0 to 511 := 0;
signal in_counter : integer range 0 to 1023 := 0;
signal read_en, i_valid_r1, i_valid_r2 : std_logic := '0';
signal we, we_q : std_logic_vector(5 downto 0) := (others => '0');
signal we_req : std_logic_vector(5 downto 0) := "000001";
signal we_en : std_logic := '0';
signal datin : std_logic_vector(23 downto 0) := (others => '0');
signal ram_line_wr_addr : std_logic_vector(9 downto 0) := (others => '0');
signal ringbuffer_rst_n, ringbuffer_rst_o : std_logic := '1';
--status input
signal bank_A, bank_B, bank_C, bank_D : std_logic_vector(2 downto 0) := (others => '0');
signal max_pixels, max_packed_pixels, max_lines : integer range 0 to 1023 := 0;
signal interlaced_flag : std_logic_vector(2 downto 0) := "000";
signal bank_last_odd_even : std_logic := '0';
--data output side signals
signal line_re_en, o_we_r : std_logic := '0';
signal data_0, data_1, data_2, data_3, data_4, data_5 : std_logic_vector(23 downto 0) := (others => '0');
signal line_rd_addr, line_wr_addr : integer range 0 to 1023 := 0;
signal o_data_r : std_logic_vector(23 downto 0) := (others => '0');
signal ram_rd_addr : std_logic_vector(9 downto 0) := (others => '0');
signal re_line_0, re_line_1, re_line_2, re_line_3, re_line_4, re_line_5 : std_logic_vector(5 downto 0) := "000001";
signal data_field_A1, data_field_C1, data_field_B, data_field_D, data_field_A2, data_field_C2 : std_logic_vector(23 downto 0) := (others => '0');

----Motion Detector signals--------------------------------
--RGB split calculation buffers
signal data_A1_r, data_A1_g, data_A1_b : signed(8 downto 0) := (others => '0');
signal data_A2_r, data_A2_g, data_A2_b : signed(8 downto 0) := (others => '0');
signal data_B_r, data_B_g, data_B_b : signed(8 downto 0) := (others => '0');
signal data_C1_r, data_C1_g, data_C1_b : signed(8 downto 0) := (others => '0');
signal data_C2_r, data_C2_g, data_C2_b : signed(8 downto 0) := (others => '0');
signal data_D_r, data_D_g, data_D_b : signed(8 downto 0) := (others => '0');
--calculations
signal sub_A1C1_r, sub_A1C1_g, sub_A1C1_b  : signed(23 downto 0) := (others => '0');
signal sub_BD_r, sub_BD_g, sub_BD_b  : signed(23 downto 0) := (others => '0');
signal sub_A2C2_r, sub_A2C2_g, sub_A2C2_b  : signed(23 downto 0) := (others => '0');
signal avg_A1A2_r, avg_A1A2_g, avg_A1A2_b : unsigned(8 downto 0) := (others => '0');
signal avg_data_reg : std_logic_vector(23 downto 0) := (others => '0');
--buffers to bridge clock cycles
signal data_B_reg_1, data_B_reg_2, data_B_reg_3 : std_logic_vector(23 downto 0) := (others => '0');
signal wr_start_a, wr_start_b, wr_start_c, wr_start_d, wr_start_e, wr_start_f, wr_start_g : std_logic := '0';
--motion detector specific stuff
signal crit_1, crit_2, crit_3 : signed(23 downto 0) := (others => '0');
signal motion_threshold : integer range 0 to 63 := 15;
signal C_TEST_MODE : std_logic := '0';
---------------------------------------------------------------------------------
--general flags and stuff
signal first_line_flag : std_logic := '1';
signal sec_busy, output_zero : std_logic := '0';


begin
 
linebuffer: ma_linebuffer port map (
   i_clk => i_clk,
   i_rst_sync_0 => i_rst_n,
   --write
   we => we,
   wr_addr => ram_line_wr_addr,
   wr_data => datin,
   --read
   re => line_re_en,
   rd_addr => ram_rd_addr,
   rd_data_0 => data_0,
   rd_data_1 => data_1,
   rd_data_2 => data_2,
   rd_data_3 => data_3,
   rd_data_4 => data_4,
   rd_data_5 => data_5
); 

ringbuffer: ring_buffer
  port map (
    clk =>i_clk,
    rst_n =>ringbuffer_rst_o,
    i_valid =>i_valid,
    i_data =>i_data,
    o_we_en =>we_en,
    o_data =>datin,
    o_addr =>ram_line_wr_addr);
   
ringbuffer_rst_o <= i_rst_n and ringbuffer_rst_n;

                
primary_state_machine:process(i_clk, i_rst_n)
begin

if(i_rst_n = '0') then

    read_en <= '0';
    we_req <= "000001";
    prim_state <= state_wait_for_sync;
    sec_state_queue <= state_sec_idle;
else
    if(rising_edge(i_clk)) then
    
        if(ringbuffer_rst_n = '0') then
            ringbuffer_rst_n <= '1';
        end if;
       
        if(i_sync = '1') then
            prim_state <= state_init;
            sec_state_queue <= state_sec_idle;
            ringbuffer_rst_n <= '0';
        end if;
        
        --this also makes sure all lines after the last valid line are zero
        if(out_line_ctr >= max_lines) then
            output_zero <= '1';
        else
            output_zero <= '0';
        end if;
    
        case(prim_state) is
        
        when state_wait_for_sync =>
            null;
            
        when state_init =>
            ringbuffer_rst_n <= '1';
            in_counter <= 0;
            out_line_ctr <= 0;
            first_line_flag <= '1';
            we_req <= "000001";
            
            bank_A <= i_field_info.bank_count_A;
            bank_B <= i_field_info.bank_count_B;
            bank_C <= i_field_info.bank_count_C;
            bank_D <= i_field_info.bank_count_D;
            bank_last_odd_even <= i_field_info.bank_odd_even;
            
            max_pixels <= i_video_cfg.h_active_pixels;
            max_packed_pixels <= i_video_cfg.h_compressed_pixels;
            max_lines <= i_video_cfg.v_active_pixels;
            interlaced_flag <= i_video_cfg.interlaced_flag;
            motion_threshold <= i_video_cfg.motion_threshold;
            C_TEST_MODE <= i_video_cfg.deinterlacer_debug;
            
            prim_state <= state_odd_even_check;
         
         when state_odd_even_check =>
         
            case(interlaced_flag) is
                when MODE_PROGRESSIVE =>
                    prim_state <= state_progressive_A;
                when MODE_MOTION_ADAPTIVE =>
                    if(bank_last_odd_even = C_ODD_FIELD) then 
                        prim_state <= state_odd_first_line;
                    else
                        prim_state <= state_even_first_line;
                    end if;
                when MODE_LINE_DOUBLE =>
                    prim_state <= state_linedouble_A;
                when others => null;
             end case;
         
         when state_progressive_A =>
         
            addr_framebuffer <= bank_A & std_logic_vector(to_unsigned(out_line_ctr, 9)) & "0000000000";
            out_line_ctr <= out_line_ctr + 1;
            
            if(we_req = "000100") then
                --a few lines must be buffered for the state_sec_output_field_b state to output the first real line
                first_line_flag <= '0';
            end if;
            prim_state <= state_progressive_B;
         
         when state_progressive_B =>    --must be run twice for line doubling
            if(first_line_flag = '0') then
                if(sec_busy = '0') then 
                    sec_state_queue <= state_sec_output_field_a;
                else        
                    prim_state <= state_read;
                    prim_next_state <= state_wait;
                    prim_state_queue <= state_progressive_A;
                    sec_state_queue <= state_sec_idle;         
                end if;           
            else
                prim_state <= state_read;
                prim_next_state <= state_progressive_A;
            end if;
         
         when state_linedouble_A =>
         
            addr_framebuffer <= bank_A & std_logic_vector(to_unsigned(out_line_ctr, 9)) & "0000000000";
            out_line_ctr <= out_line_ctr + 1;
            
            if(we_req = "000100") then
                --a few lines must be buffered for the state_sec_output_field_b state to output the first real line
                first_line_flag <= '0';
            end if;
            prim_state <= state_linedouble_B;  
         
         when state_linedouble_B =>    --must be run twice for line doubling
            if(first_line_flag = '0') then
                if(sec_busy = '0') then 
                    sec_state_queue <= state_sec_output_field_a;
                else        
                    if(prim_state_queue = state_linedouble_A) then                  
                        prim_state <= state_wait;
                        prim_state_queue <= state_linedouble_B;
                    else
                        prim_state <= state_read;
                        prim_next_state <= state_wait;
                        prim_state_queue <= state_linedouble_A;   
                    end if;
                    sec_state_queue <= state_sec_idle;         
                end if;           
            else
                prim_state <= state_read;
                prim_next_state <= state_linedouble_A;
                prim_state_queue <= state_linedouble_A;
            end if;
                 
         when state_odd_first_line =>
            addr_framebuffer <= bank_A & std_logic_vector(to_unsigned(out_line_ctr, 9)) & "0000000000";
            prim_state <= state_read;
            prim_next_state <= state_read_field_C;

         when state_even_first_line =>
            addr_framebuffer <= bank_A & std_logic_vector(to_unsigned(out_line_ctr, 9)) & "0000000000";
            
            if(sec_busy = '0') then 
                sec_state_queue <= state_sec_output_blank;
            else
                prim_state <= state_read;
                prim_next_state <= state_read_field_C;
                sec_state_queue <= state_sec_idle;                    
            end if;          
            
         when state_read_field_A =>
            addr_framebuffer <= bank_A & std_logic_vector(to_unsigned(out_line_ctr, 9)) & "0000000000";
            prim_state <= state_read;
            prim_next_state <= state_read_field_C;
                        
         when state_read_field_B =>
            addr_framebuffer <= bank_B & std_logic_vector(to_unsigned(out_line_ctr, 9)) & "0000000000";
            
            if(output_zero = '0') then
                if(bank_last_odd_even = C_ODD_FIELD) then 
                    if(first_line_flag = '1') then
                        sec_state_queue <= state_sec_output_blank;
                    else
                        sec_state_queue <= state_sec_output_field_a;
                    end if;
                else
                    sec_state_queue <= state_sec_output_field_a; 
                end if;
            else
                sec_state_queue <= state_sec_output_blank;
            end if;
            
            if(sec_busy = '1') then
                prim_state <= state_read;
                prim_next_state <= state_read_field_D;
                sec_state_queue <= state_sec_idle;
            end if;
            
         when state_read_field_C =>
            addr_framebuffer <= bank_C & std_logic_vector(to_unsigned(out_line_ctr, 9)) & "0000000000";
            --only increment here when EVEN
            if(bank_last_odd_even = C_EVEN_FIELD) then 
                out_line_ctr <= out_line_ctr + 1;
            end if;
            
            if(first_line_flag = '1') then
                if(we_req = "100000") then
                    --only the second time C is read, we can calculate, ONLY for the first line
                    prim_state <= state_read;
                    prim_next_state <= state_calculate;
                else
                    prim_state <= state_read;
                    prim_next_state <= state_read_field_B;
                end if;
            else
                --for all other lines except the first, calculation is after C
                prim_state <= state_read;
                prim_next_state <= state_calculate;
            end if;
           
            
         when state_read_field_D =>
            addr_framebuffer <= bank_D & std_logic_vector(to_unsigned(out_line_ctr, 9)) & "0000000000";
            --only increment here when EVEN
            if(bank_last_odd_even = C_ODD_FIELD) then 
                out_line_ctr <= out_line_ctr + 1;
            end if;
            
            prim_state <= state_read;
            prim_next_state <= state_read_field_A;
            
         when state_calculate =>       
            first_line_flag <= '0';
            if(sec_busy = '0') then
                if(output_zero = '0') then
                    --else we just do the interpolation like normal
                    sec_state_queue <= state_sec_motion;
                else
                    --for the last line we must output blank, otherwise its value is undefined
                    sec_state_queue <= state_sec_output_blank;
                end if;
            else
                prim_state_queue <= state_read_field_B;
                prim_state <= state_wait;
                sec_state_queue <= state_sec_idle;                    
            end if;
            
         when state_wait =>
            --if(i_end = '1' and sec_busy = '0') then
            if(i_end = '1' and sec_busy = '0') then
                prim_state <= state_wait_for_sync;  
            elsif(i_ready = '0' and sec_busy = '0') then  
                prim_state <= prim_state_queue;    
            end if;   

         when state_read =>
         
            i_valid_r1 <= i_valid;
            i_valid_r2 <= i_valid_r1;

            if(i_valid_r1 = '1' and i_valid_r2 = '0') then  
                in_counter <= in_counter + 8;
            end if;
                
            if(in_counter < max_packed_pixels) then
                read_en <= '1';  --added 
                we_q <= we_req;
                addr_framebuffer <= addr_framebuffer(21 downto 10) & std_logic_vector(to_unsigned(in_counter, 10));    
                  
            else
                read_en <= '0';      
            end if;
                
            if (to_integer(unsigned(ram_line_wr_addr)) >= max_pixels and sec_busy = '0' and i_valid = '0') then   --640 ram_line_wr_addr = "1010000000"
                in_counter <= 0;  
                ringbuffer_rst_n <= '0';
                we_req <= std_logic_vector(rotate_left(unsigned(we_req), 1));
                         
                re_line_0 <= we_req;    --re_line_0 is only required for the next calculation cycle (f) --C2
                re_line_1 <= re_line_0; --A2
                --re_line_2 and re_line_3 need to be subtracted
                re_line_2 <= re_line_1; --D
                re_line_3 <= re_line_2; --B
                --re_line_4 and re_line_5 need to be subtracted
                re_line_4 <= re_line_3; --C1 
                re_line_5 <= re_line_4;  --A1
                --average between re_line_5 and re_line_1
                    
                prim_state <= prim_next_state;
            end if;
    
        when others =>
            prim_state <= state_init;
            prim_state_queue <= state_read_field_B;
        end case;

    end if;
end if;
end process;


secondary_state_machine:process(i_clk, i_rst_n)     
begin 

if(i_rst_n = '0') then
    sec_state <= state_sec_idle;
else
    if(rising_edge(i_clk)) then  
        if(i_sync = '1') then
            sec_state <= state_sec_idle;
        end if;
    ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------        
    --SECOND FSM FOR CALCULATIONS---------------
        case(sec_state) is     
        
        when state_sec_idle =>
            line_rd_addr <= 0;
            line_wr_addr <= 0;
            wr_start_b <= '0';
            wr_start_a <= '0';
            wr_start_c <= '0';
            wr_start_d <= '0';
            wr_start_e <= '0';
            wr_start_f <= '0';
            wr_start_g <= '0';
            sec_busy <= '0';
            o_we_r <= '0';
            line_re_en <= '0';
            
            if(sec_state_queue /= state_sec_idle) then
                sec_busy <= '1';
                sec_next_state <= sec_state_queue;
                sec_state <= state_sec_check_busy;
            end if;
            
        when state_sec_check_busy =>
            
            if(i_end = '1') then
                sec_state <= state_sec_idle;    --in case we get stuck waiting for the FIFO
            elsif(i_ready = '0') then
                sec_state <= sec_next_state;
            end if;
            
        when state_sec_output_blank => 

            line_wr_addr <= line_wr_addr + 1;
            o_we_r <= '1';
            o_data_r <= (others=>'0');

            --when all pixels of the line were done, go back to idle    
            if(line_wr_addr = max_pixels) then
                o_we_r <= '0';
                sec_state <= state_sec_idle;
            end if;
        
        when state_sec_output_field_a => 
            --write line C into the out line buffer 0
            line_re_en <= '1';
            line_rd_addr <= line_rd_addr + 1;
            
            --field D in this case is NOT field D -> it is the field before the last field to be written (always the last A field)
            o_data_r <= data_field_A2; 
            
            wr_start_a <= '1';
            wr_start_b <= wr_start_a;
            wr_start_c <= wr_start_b;
            wr_start_d <= wr_start_c;

            --wr_start is adding enough delay that the first value written is actually a correct one
            if(wr_start_d = '1') then
                line_wr_addr <= line_wr_addr + 1;
                o_we_r <= '1';
            end if;
            --when all pixels of the line were done, go back to idle    
            if(line_wr_addr = max_pixels) then
                o_we_r <= '0';
                line_re_en <= '0';
                sec_state <= state_sec_idle;
            end if;
        
        when state_sec_motion =>
            
            --calculations new
            --abs(data_field_A1 - data_field_C1) < TH   and
            --abs(data_field_B - data_field_D)   < TH   and
            --abs(data_field_A2 - data_field_C2) < TH   and
            --avg(data_field_A1, data_field_A2)

            line_re_en <= '1';
            line_rd_addr <= line_rd_addr + 1;
            wr_start_a <= '1';
            --CLOCK 1----------------------------------------           
            data_A1_r <= signed('0' & data_field_A1(23 downto 16));
            data_A1_g <= signed('0' & data_field_A1(15 downto 8));
            data_A1_b <= signed('0' & data_field_A1(7 downto 0));
            
            data_A2_r <= signed('0' & data_field_A2(23 downto 16));
            data_A2_g <= signed('0' & data_field_A2(15 downto 8));
            data_A2_b <= signed('0' & data_field_A2(7 downto 0));
            
            data_B_r <= signed('0' & data_field_B(23 downto 16));
            data_B_g <= signed('0' & data_field_B(15 downto 8));
            data_B_b <= signed('0' & data_field_B(7 downto 0));
            data_B_reg_1 <= data_field_B;
            
            data_C1_r <= signed('0' & data_field_C1(23 downto 16));
            data_C1_g <= signed('0' & data_field_C1(15 downto 8));
            data_C1_b <= signed('0' & data_field_C1(7 downto 0));
            
            data_C2_r <= signed('0' & data_field_C2(23 downto 16));
            data_C2_g <= signed('0' & data_field_C2(15 downto 8));
            data_C2_b <= signed('0' & data_field_C2(7 downto 0));
            
            data_D_r <= signed('0' & data_field_D(23 downto 16));
            data_D_g <= signed('0' & data_field_D(15 downto 8));
            data_D_b <= signed('0' & data_field_D(7 downto 0));
            -------------------------------------------------------
            --CLOCK 2-------------------------------------------------------------------------------
            wr_start_b <= wr_start_a;
            --SUBTRACTIONS--
            sub_A1C1_r <= resize(abs(data_A1_r - data_C1_r), 24);
            sub_A1C1_g <= resize(abs(data_A1_g - data_C1_g), 24);
            sub_A1C1_b <= resize(abs(data_A1_b - data_C1_b), 24);
            
            sub_BD_r <= resize(abs(data_B_r - data_D_r), 24);
            sub_BD_g <= resize(abs(data_B_g - data_D_g), 24);
            sub_BD_b <= resize(abs(data_B_b - data_D_b), 24);
            
            sub_A2C2_r <= resize(abs(data_A2_r - data_C2_r), 24);
            sub_A2C2_g <= resize(abs(data_A2_g - data_C2_g), 24);
            sub_A2C2_b <= resize(abs(data_A2_b - data_C2_b), 24);
            --AVERAGE--
            avg_A1A2_r <= shift_right(unsigned(data_A1_r) + unsigned(data_A2_r), 1);
            avg_A1A2_g <= shift_right(unsigned(data_A1_g) + unsigned(data_A2_g), 1);
            avg_A1A2_b <= shift_right(unsigned(data_A1_b) + unsigned(data_A2_b), 1);
            --store line B in case there is no motion
            data_B_reg_2 <= data_B_reg_1;
            --CLOCK 3------------------------------------------------------
            wr_start_c <= wr_start_b;
            --calculate motion criteria
            crit_1 <= sub_A1C1_r + sub_A1C1_g + sub_A1C1_b;
            crit_2 <= sub_BD_r + sub_BD_g + sub_BD_b;
            crit_3 <= sub_A2C2_r + sub_A2C2_g + sub_A2C2_b;
            data_B_reg_3 <= data_B_reg_2;
            avg_data_reg <= std_logic_vector(avg_A1A2_r(7 downto 0)) & std_logic_vector(avg_A1A2_g(7 downto 0)) & std_logic_vector(avg_A1A2_b(7 downto 0));
            --CLOCK 4------------------------------------------------------
            wr_start_d <= wr_start_c;
            --MOTION DETECTOR DECISION----------------------
             if(crit_1 < motion_threshold and crit_2 < motion_threshold and crit_3 < motion_threshold) then
                --there is no motion
                o_data_r <= data_B_reg_3;
             else
                --there is motion
                if(C_TEST_MODE = '0') then
                    o_data_r <= avg_data_reg;
                else
                    o_data_r <= x"ff00ff";
                end if;
                
             end if;
             
            --DELAY to discard the first 2(?) pixels of each line (RAM has an output delay of 2 clock cycles)
            wr_start_e <= wr_start_d;
            wr_start_f <= wr_start_e;
            wr_start_g <= wr_start_f;
            
            --write the calculated line to the line buffer after a delay
            if(wr_start_g = '1') then
                o_we_r <= '1';
                line_wr_addr <= line_wr_addr + 1;
            end if;
             
            --when all pixels of the line were done, go back to idle        
            if(line_wr_addr = max_pixels) then
                o_we_r <= '0';
                line_re_en <= '0';
                sec_state <= state_sec_idle;
            end if; 
                     
        when others =>
            null;
        end case;
     
    end if;
end if;
end process;

test:process(i_clk)
begin

if(rising_edge(i_clk)) then

    case(re_line_5) is
        when "000001" => data_field_A1 <= data_0;
        when "000010" => data_field_A1 <= data_1;
        when "000100" => data_field_A1 <= data_2;
        when "001000" => data_field_A1 <= data_3;
        when "010000" => data_field_A1 <= data_4; 
        when "100000" => data_field_A1 <= data_5;
        when others => data_field_A1 <= (others=>'0');
    end case;

    case(re_line_4) is
        when "000001" => data_field_C1 <= data_0;
        when "000010" => data_field_C1 <= data_1;
        when "000100" => data_field_C1 <= data_2;
        when "001000" => data_field_C1 <= data_3;
        when "010000" => data_field_C1 <= data_4; 
        when "100000" => data_field_C1 <= data_5;
        when others => data_field_C1 <= (others=>'0');
    end case;
    
    case(re_line_3) is
        when "000001" => data_field_B <= data_0;
        when "000010" => data_field_B <= data_1;
        when "000100" => data_field_B <= data_2;
        when "001000" => data_field_B <= data_3;
        when "010000" => data_field_B <= data_4; 
        when "100000" => data_field_B <= data_5;
        when others => data_field_B <= (others=>'0');
    end case;

    case(re_line_2) is
        when "000001" => data_field_D <= data_0;
        when "000010" => data_field_D <= data_1;
        when "000100" => data_field_D <= data_2;
        when "001000" => data_field_D <= data_3;
        when "010000" => data_field_D <= data_4; 
        when "100000" => data_field_D <= data_5;
        when others => data_field_D <= (others=>'0');
    end case;

    case(re_line_1) is
        when "000001" => data_field_A2 <= data_0;
        when "000010" => data_field_A2 <= data_1;
        when "000100" => data_field_A2 <= data_2;
        when "001000" => data_field_A2 <= data_3;
        when "010000" => data_field_A2 <= data_4; 
        when "100000" => data_field_A2 <= data_5;
        when others => data_field_A2 <= (others=>'0');
    end case;

    case(re_line_0) is
        when "000001" => data_field_C2 <= data_0;
        when "000010" => data_field_C2 <= data_1;
        when "000100" => data_field_C2 <= data_2;
        when "001000" => data_field_C2 <= data_3;
        when "010000" => data_field_C2 <= data_4; 
        when "100000" => data_field_C2 <= data_5;
        when others => data_field_C2 <= (others=>'0');
    end case;
end if;

end process;
                                         
ram_rd_addr <= std_logic_vector(to_unsigned(line_rd_addr, 10));

we <= we_q when (we_en = '1') else
      (others=>'0');

a_addr <= addr_framebuffer;
o_re <= read_en;
o_we <= o_we_r;
o_data <= o_data_r;
o_addr <= std_logic_vector(to_unsigned(line_wr_addr, 10));   
                   
end architecture;


--A EVEN FIELD
            --read c (field A1, line 1) -> output black line first time
            -----------
            --read d (field C1, line 1) -> increment line counter!
            --read e (field B, line 2) -> output line c first time
            --read f (field D, line 2)
            --read g (field A2, line 2)
            --read h (field C2, line 2) -> increment line counter!
            --calculate -> output calculated line
            --
            --read i (field B, line 3) -> output line g all other times
            --read j (field D, line 3) 
            --read k (field A2, line 3)
            --read l (field C2, line 3) -> increment line counter!



--A ODD FIELD
            --read a (field A1, line 1)
            --
            --read b (field C1, line 1)
            --read c (field B, line 1) -> output black line first time
            --read d (field D, line 1) -> increment line counter!
            --read e (field A2, line 2)
            --read f (field C2, line 2)
            --calculate -> output calculated line
            --
            --read g (field B, line 2) -> output line e all other times
            --read h (field D, line 2) -> increment line counter!
            --read i (field A2, line 3)
            --read j (field C2, line 3)

