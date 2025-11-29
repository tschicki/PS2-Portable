----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Patrick Haid
-- 
-- Create Date: 30.03.2025 21:30:01
-- Design Name: PS2 Video Processor
-- Module Name: image_scale - Behavioral
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

entity image_scale is
generic(
    MAX_OUTPUT_PIXELS : integer := 800;
    MAX_LINE_COUNT : integer := 480
    );
Port ( 
    --control inputs
    i_clk :             in std_logic;
    i_rst_n :           in std_logic;
    i_sync :            in std_logic;
    i_video_cfg :       in video_config_t; --video config
    --interface with line fifo
    i_data_a :          in std_logic_vector (23 downto 0);
    i_data_b :          in std_logic_vector (23 downto 0);
    i_data_c :          in std_logic_vector (23 downto 0);
    i_data_d :          in std_logic_vector (23 downto 0);
    o_addr_l :          out std_logic_vector(9 downto 0);            --address of lower pixel in line -> A & C
    o_addr_h :          out std_logic_vector(9 downto 0);            --address of higher pixel in line -> B & D
    o_re :              out std_logic;
    tail_ptr_inc :      out unsigned(1 downto 0);                     --increment tail pointer  
    i_fifo_empty :      in std_logic;                    --we should check after every line that this is not the case
    frame_end :         out std_logic;
    --interface with output buffer
    o_we :              out std_logic;
    o_data :            out std_logic_vector (23 downto 0);
    o_addr :            out std_logic_vector(9 downto 0);
    o_line_counter :    out integer range 0 to 1024;
    i_line_start :      in std_logic
);
end image_scale;

architecture Behavioral of image_scale is

component bilinear_scaler_pipeline is
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
end component;

type state_type is (state_init, 
                    state_store_inputs,
                    store_calculate_params,
                    state_wait_fifo, 
                    state_wait, 
                    state_line_ctr_inc,
                    state_handle_fifo,
                    state_line_end);
                    
signal state : state_type := state_init;
signal x_scaling_factor_r, y_scaling_factor_r : unsigned (17 downto 0);
signal x_scaling_target, y_scaling_target : unsigned(9 downto 0);
--interfacing with multipliers
signal x_low, x_high, y_low, y_low_last : unsigned (9 downto 0) := (others=>'0');
signal tail_ptr_inc_r : unsigned(1 downto 0) := (others=>'0');

signal line_inc_1, line_inc_2 : std_logic := '0';
signal data_valid_r, counter_en, re_reg : std_logic := '0';
signal calc_counter, border_counter : integer range 0 to 1023 := 0;
signal in_pixel_counter, out_pixel_counter : integer range 0 to 1023 := 1;
signal in_line_counter, out_line_counter  : integer range 0 to 1023 := 0;

signal data_output : unsigned(23 downto 0) := (others=>'0');

--borders
signal res_diff_x, res_diff_y, y_target_corrected, border_x, border_y : unsigned(9 downto 0) := (others=>'0');
signal border_y_upper : unsigned(9 downto 0) := (others=>'0');
signal start_pipeline, we_activate, we_deactivate : integer range 0 to 1023 := 0;
signal done_flag, pipeline_en, we_r, data_en, done_flag1, done_flag2 : std_logic := '0';
signal read_end : integer range 0 to 1023 :=              0;
signal data_valid_end : integer range 0 to 1023 :=        0;

signal init_done, init_done1, init_done2, init_done3, init_done4 : std_logic := '0';
signal y_l_delay, y_l_delay1, y_l_delay2, y_l_delay3 : std_logic := '0';

signal frame_end_flag : std_logic := '0';

--hardcoded timings for running the calculation pipeline
constant C_Y_L_AVAILABLE : integer range 0 to 1023 :=               5;
constant C_PIPELINE_START : integer range 0 to 1023 :=              10;
constant C_START_READ : integer range 0 to 1023 :=                  13; --14
constant C_END_READ_OFFSET : integer range 0 to 1023 :=             C_START_READ + 2;
constant C_DATA_VALID : integer range 0 to 1023 :=                  21; --20
constant C_DATA_VALID_END_OFFSET : integer range 0 to 1023 :=       C_DATA_VALID - 1; --turns off 1 pixel earlier because the last scales pixel horizontally is invalid
begin

pipeline:bilinear_scaler_pipeline port map( 
    --control inputs
    i_clk =>i_clk,
    i_rst_n =>i_rst_n,
    --signals fed into the pipeline
    x_scaling_factor =>x_scaling_factor_r,
    y_scaling_factor =>y_scaling_factor_r,
    pixel_counter =>to_unsigned(in_pixel_counter, 18),
    line_counter =>to_unsigned(in_line_counter, 18),
    i_data_a =>i_data_a,
    i_data_b =>i_data_b,
    i_data_c =>i_data_c,
    i_data_d =>i_data_d,
    --signals returning from the pipeline
    addr_x_low =>x_low,
    addr_x_high =>x_high,
    addr_y_low =>y_low,
    o_data =>data_output
);

parameters:process(i_clk, i_rst_n)
begin

if(i_rst_n = '0') then
init_done1 <= '0';
init_done2 <= '0';
init_done3 <= '0';
init_done4 <= '0';
else
    if(rising_edge(i_clk)) then  
        --stage 1
        res_diff_x <= to_unsigned(MAX_OUTPUT_PIXELS,10) - x_scaling_target;
        res_diff_y <= to_unsigned(MAX_LINE_COUNT,10) - y_scaling_target; 
        read_end <= C_END_READ_OFFSET + to_integer(x_scaling_target);
        data_valid_end <= to_integer(x_scaling_target) + C_DATA_VALID_END_OFFSET;
        init_done1 <= init_done;
        y_target_corrected <= y_scaling_target - 2;
        --stage 2
        border_x <= rotate_right(res_diff_x,1);
        border_y <= rotate_right(res_diff_y,1);
        init_done2 <= init_done1;
        --stage 3
        if(border_x <= to_unsigned(C_DATA_VALID,10)) then
            start_pipeline <= to_integer(border_x);
            we_activate <= C_DATA_VALID;
        else
            start_pipeline <= to_integer(border_x) - C_DATA_VALID;
            we_activate <= 0;
        end if;
        border_y_upper <= border_y + y_target_corrected; --border_y_upper <= border_y + y_scaling_target;   
        init_done3 <= init_done2;
        --stage 4
        we_deactivate <= we_activate + MAX_OUTPUT_PIXELS;
        init_done4 <= init_done3;
    end if;
end if;
end process;

fsm:process(i_clk, i_rst_n)
begin

 if(i_rst_n = '0') then
    state <= state_init;
    counter_en <= '0';
    init_done <= '0';
    data_en <= '0';
    out_line_counter <= 0;
    in_line_counter <= 0;
    y_l_delay <= '0';
    tail_ptr_inc_r <= (others=>'0');
    frame_end_flag <= '0';
 else
    if(rising_edge(i_clk)) then  
    
        --delay for y_l to become valid
        y_l_delay1 <= y_l_delay;
        y_l_delay2 <= y_l_delay1;
        y_l_delay3 <= y_l_delay2;
      
        case(state) is
            when state_init =>
                if(i_sync = '1') then
                    y_low_last <= (others=>'0');
                    frame_end_flag <= '0';
                    state <= state_store_inputs;   
                end if;
            
            when state_store_inputs =>
                x_scaling_factor_r <= i_video_cfg.x_scaling_factor;
                y_scaling_factor_r <= i_video_cfg.y_scaling_factor;
                x_scaling_target <= i_video_cfg.x_active_out;
                y_scaling_target <= i_video_cfg.y_active_out;
                init_done <= '1';
                state <= store_calculate_params;
            
            when store_calculate_params =>
                if(init_done4 = '1') then
                    init_done <= '0';
                    state <= state_wait_fifo;
                end if;

            when state_wait_fifo=>
                --add borders on top and bottom
                if(out_line_counter >= border_y and out_line_counter < border_y_upper) then
                    data_en <= '1';
                else
                    data_en <= '0';
                end if;
                
                --wait for the fifo to have data, needed for the first line?
                if(i_fifo_empty = '0') then
                    counter_en <= '1';
                    state <= state_wait;
                end if;
            
            when state_wait=>
                if(done_flag2 = '1') then
                    state <= state_line_ctr_inc;
                    counter_en <= '0';
                end if;
         
            when state_line_ctr_inc=>
                out_line_counter <= out_line_counter + 1;
                
                if(data_en = '1') then
                    in_line_counter <= in_line_counter + 1;
                    y_low_last <= y_low;
                    state <= state_handle_fifo;
                else
                    state <= state_line_end;
                end if;
 
            when state_handle_fifo =>
                if(y_l_delay3 = '1') then   --after the y_l delay has passed we go to decide whether we increment the fifo tail pointer
                    tail_ptr_inc_r <= y_low(1 downto 0) - y_low_last(1 downto 0); --can be 0 to 3
                    y_l_delay <= '0';
                    state <= state_line_end;
                else
                    y_l_delay <= '1';
                end if;
                
            
            when state_line_end=>
                tail_ptr_inc_r <= (others=>'0'); --important to let this be non-zero for only one clk cycle
                if(i_line_start = '1') then
                    state <= state_wait_fifo; 
                elsif(out_line_counter = MAX_LINE_COUNT) then
                    out_line_counter <= 0;
                    in_line_counter <= 0;
                    frame_end_flag <= '1';
                    state <= state_init;
                end if;
            when others => null;
        end case;

    end if;
end if;
end process;
tail_ptr_inc <= tail_ptr_inc_r;
frame_end <= frame_end_flag;

borders:process(i_clk, i_rst_n)
begin

if(i_rst_n = '0') then
    done_flag <= '0';
    done_flag1 <= '0';
    done_flag2 <= '0';
    pipeline_en <= '0';  
    border_counter <= 0;
    out_pixel_counter <= 0;
    we_r <= '0';  
else
    if(rising_edge(i_clk)) then
        
        if(counter_en = '1') then
            border_counter <= border_counter + 1;
            
            if(border_counter = start_pipeline) then
                done_flag <= '0';
                if(data_en = '1') then
                    pipeline_en <= '1';
                end if;
            end if;
            
            if((border_counter >= we_activate) and (border_counter < we_deactivate)) then
                out_pixel_counter <= out_pixel_counter + 1;  --this essentially serves as the write address on the output
                we_r <= '1';
            else
                out_pixel_counter <= 0;
                we_r <= '0';
            end if;
                    
            if(border_counter = we_deactivate) then
                done_flag <= '1';
                pipeline_en <= '0';
            end if;
            
        else
            border_counter <= 0;
            done_flag <= '0';
        end if;
        
        done_flag1 <= done_flag;
        if(done_flag = '1' and done_flag1 = '0') then
            done_flag2 <= '1';
        else
            done_flag2 <= '0';
        end if; 
        
    end if;
end if;
end process;

horizontal_control:process(i_clk, i_rst_n)
begin

 if(i_rst_n = '0') then
    data_valid_r <= '0';
    in_pixel_counter <= 1;
    calc_counter <= 0;
    re_reg <= '0';
    line_inc_1 <= '0';
    line_inc_2 <= '0';
 else
    if(rising_edge(i_clk)) then 
    
        if(pipeline_en = '1') then
            calc_counter <= calc_counter + 1;
        else
            calc_counter <= 0;
        end if;
    
        case(calc_counter) is
            when C_START_READ =>
                --here we start reading the ram to have the pixels at the right time
                re_reg <= '1';
                
            when C_DATA_VALID =>
                data_valid_r <= '1';
         
            when others =>
                line_inc_1 <= '0';
                line_inc_2 <= '0';
        end case;
        
        if(calc_counter = data_valid_end) then
            data_valid_r <= '0';
        end if;
        
        if(calc_counter = read_end) then
            re_reg <= '0';  --reset RE, as only here the line_inc is applied   
        end if;
        
        if(calc_counter >= C_PIPELINE_START and calc_counter < read_end) then
            in_pixel_counter <= in_pixel_counter + 1; --pixel counter on which the read address is based on
        else
            in_pixel_counter <= 1;
        end if;
    end if;
end if;
end process;

 o_data <= std_logic_vector(data_output) when(data_valid_r = '1') else
           (others=>'0');
 
 o_addr_l <= std_logic_vector(x_low);
 o_addr_h <= std_logic_vector(x_high);

o_we <= we_r;
o_re <= re_reg;
o_addr <= std_logic_vector(to_unsigned(out_pixel_counter,10));
o_line_counter <= out_line_counter;
 
end Behavioral;


--border logic
--HORIZONTAL:-------------------------------------------------------------
--re_diff_x = 800 - target_scaler_x
--border_x = re_diff_x/2

--START_PIPELINE = border_x if(border_x <= 20) else border_x - 20
--WE_ACTIVATE = 20 if(border_x <= 20) else 0
--WE_DEACTIVATE = WE_ACTIVATE + 800

--dat_out = dat_calc if(data_valid = 1) else 0

--VERTICAL:----------------------------------------------------------------
--we need 2 independent line counters:
    --output line counter
    --scaler line counter

--the scaler line counter should only be incremented when the output line counter is in the valid region
--actually, the calculation pipeline should not even be started (calc_counter) -> if it's not started, data_valid can never be high and we always output 0
--maybe in horizontal -> START_PIPELINE stage we read a flag whether to start the calc pipeline

--re_diffyx = 480 - target_scaler_y
--border_y = re_diff_y/2
--VERTICAL_BORDER_L = border_y
--VERTICAL_BORDER_H = border_y + target_scaler_y

--if(output line counter >= VERTICAL_BORDER_L and output line counter <= VERTICAL_BORDER_H)
    --cal_enable_flag = 1
    --scaler line counter = scaler line counter + 1


