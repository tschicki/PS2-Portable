----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Patrick Haid
-- 
-- Create Date: 13.01.2025 21:35:41
-- Design Name: PS2 Video Processor
-- Module Name: PS2 Video Input Module
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

entity video_sample is
    Port (
        i_rst_n :           in std_logic;                         --reset input from PS2
        i_clk :             in std_logic;                         --PS2 pixel clock input
        i_data :            in STD_LOGIC_VECTOR (23 downto 0);    --parallel RGB video from PS2
        i_hsync_n :         in std_logic;                         --HSYNC from PS2
        i_vsync_n :         in std_logic;                         --VSYNC from PS2
        i_ntpal :           in std_logic;
         
        o_we :              out std_logic;                        --write enable for line buffer
        o_data :            out std_logic_vector(31 downto 0);    --active video data out
        o_addr  :           out  std_logic_vector(9 downto 0);   --address for line buffer; includes bank sel in MSB
        o_line_active :     out std_logic;                        --pulses high at the beginning of every active line
        o_video_cfg :       out video_config_t;                     --stores information about the current line/frame for the output
        o_rst_req_n :       out std_logic;                       --requests the scaler to perform a reset
        o_sync:             out std_logic;
        
        --read port
        i_data_ram :        in std_logic_vector(7 downto 0);
        o_addr_ram :        out std_logic_vector(7 downto 0);
        o_re_ram :          out std_logic;
        
        --registers
        o_video_res :         out integer range 0 to 128;
        i_video_en :          in std_logic;
        i_config_override :   in std_logic;
        o_magh_det :          out std_logic_vector (3 downto 0) --detection of the subpixel count per pixel; updated every frame; non-zero values are valid
    );
end video_sample;

architecture Behavioral of video_sample is
    component MAGH_detector is
        Port ( 
            i_rst_n :           in std_logic;                         --reset input from PS2
            i_clk :             in std_logic;                         --PS2 pixel clock input
            i_data :            in STD_LOGIC_VECTOR (23 downto 0);    --parallel RGB video from PS2
            
            i_h_active :        in std_logic;                         --horizontal active
            i_v_active :        in std_logic;                         --vertical active
            o_magh_det :        out std_logic_vector (3 downto 0)     --detected MAGH value
            
        );
    end component;

    
    signal input_res : integer range 0 to 128 := 32;

    type state_type_input is (state_init,
                              state_active,
                              load_config,
                              state_check_resolution,
                              state_frame_start,
                              state_line_start,
                              state_ready,
                              state_send_sync);
                        
    signal state, next_state, state_queue : state_type_input := state_init;
    
    type state_type_write is (state_wait, 
                              state_analyze,
                              state_access_regs, 
                              state_write_ram,
                              state_read_ram
                              );
    signal config_state : state_type_write := state_wait;
    
    type state_type_read is (state_wait, 
                        state_read_configs,
                        state_wait_config,
                        state_update_res
                        );
    signal read_state : state_type_read := state_wait;
    
    signal pixel_counter : integer range 0 to 10000 := 0;
    signal line_counter, max_lines : integer range 0 to 1023 := 0;
    signal v_active, h_active, active : std_logic := '0';
    
    signal video_config : video_config_t := (h_active_pixels => 0,
                                             h_compressed_pixels => 0,
                                             v_active_pixels => 0,
                                             odd_even_flag => '0',
                                             interlaced_flag => "000",
                                             ntpal_flag => '0',
                                             deinterlacer_debug => '0',
                                             motion_threshold => 0,
                                             x_scaling_factor => (others=>'0'),
                                             y_scaling_factor => (others=>'0'),
                                             x_active_out => (others=>'0'),
                                             y_active_out => (others=>'0')
                                             );

    signal odd_even_flag : std_logic := '0';
    signal interlaced_flag : std_logic_vector(2 downto 0) := "000";
    signal ntpal_r : std_logic := '0';

    signal line_active : std_logic := '0';
    signal vsync_1, vsync_2 : std_logic := '0';
    signal hsync_1, hsync_2 : std_logic := '0';

    signal vsync_rising, hsync_falling : std_logic := '0';
    signal reset_req_reg : std_logic := '0';
    signal sampling_divider, sampler : integer range 0 to 9 := 0;
    
    signal h_image_offset : std_logic_vector(15 downto 0) := (others=>'0');
    signal v_image_offset : std_logic_vector(7 downto 0) := (others=>'0');
    
    signal scaling_factor_x, scaling_factor_y : std_logic_vector(17 downto 0) := (others=>'0');
    signal target_res_x, target_res_y : std_logic_vector(9 downto 0) := (others=>'0');

    signal h_image_active : std_logic_vector(15 downto 0) := (others=>'0');
    signal v_image_active : std_logic_vector(15 downto 0) := (others=>'0');
    
    signal h_active_pixels, h_packed_pixels : std_logic_vector(15 downto 0) := (others=>'0');
    signal first_line_even_flag, first_line_even_flag_last, block_first_line_increment : std_logic := '0';
    
    --test 4 byte packing
    signal outbuffer_1, outbuffer_2 : STD_LOGIC_VECTOR(31 downto 0) := (others => '0');
    signal out_we : STD_LOGIC_VECTOR(1 downto 0) := "00";
    signal out_we_en, out_we_test : std_logic := '0';
    signal step_counter : unsigned(3 downto 0) := "0001";
    signal packed_pxl_ctr : integer range 0 to 1023 := 0;
    signal output_sync : std_logic := '0';
    
    --control interface
    
    signal current_data : std_logic_vector(7 downto 0) := (others=>'0');
    signal current_addr : std_logic_vector(15 downto 0) := (others=>'0');
 
    signal re_data_ram_1, re_data_ram_2 : std_logic_vector(7 downto 0) := (others=>'0');
    
    signal re_ram : std_logic := '0';
    signal re_addr_ram : integer range 0 to 160 := 0;
    signal read_counter : integer range 0 to 31 := 0;
    signal read_done : std_logic := '0';
    
    signal request_config, load_config_done : std_logic := '0';
    signal motion_threshold : std_logic_vector(6 downto 0) := (others=>'0');
    signal deinterlacer_debug : std_logic := '0';
    signal config_override : std_logic := '0';
    signal resolution_cdc : integer range 0 to 127 := 0;
    signal video_enable : std_logic := '0';
    signal master_reset : std_logic := '1';
    
begin
    master_reset <= i_rst_n and video_enable;
    
    magh_detect: MAGH_detector port map(  
        i_rst_n => master_reset,
        i_clk => i_clk,
        i_data => i_data,
        
        i_h_active => h_active,
        i_v_active => v_active,
        o_magh_det => o_magh_det
        );

    h_v_flags:process(i_clk, master_reset)
    begin
    
    if(master_reset = '0') then
        vsync_1 <= '1';
        vsync_2 <= '1';
        hsync_1 <= '1';
        hsync_2 <= '1';
        vsync_rising <= '0';
        hsync_falling <= '0';
    else
        if(rising_edge(i_clk)) then
            --vsync_2 is the delayed pixel clk; if at one point vsync = 1 and vsync_2 = 0, we know that there is a rising edge on vsync
            vsync_1 <= i_vsync_n;
            vsync_2 <= vsync_1;
            hsync_1 <= i_hsync_n;
            hsync_2 <= hsync_1;

            --detect rising edge on VSYNC
            if(vsync_1 = '1' and vsync_2 = '0') then
                vsync_rising <= '1';
            else
                vsync_rising <= '0';
            end if;

            --detect falling edge on HSYNC
            if(hsync_1 = '0' and hsync_2 = '1') then
                hsync_falling <= '1';
            else
                hsync_falling <= '0';
            end if;

        end if;
    end if;
    end process;

    h_counters:process(i_clk, master_reset)
    begin
    
    if(master_reset = '0') then
        pixel_counter <= 0;
        h_active <= '0';
    else
        if(rising_edge(i_clk)) then
            --handle the vertical and horizontal counters
            if(i_hsync_n = '1') then
                pixel_counter <= pixel_counter + 1;
            else
                pixel_counter <= 0;
            end if;

            if(pixel_counter >= to_integer(unsigned(h_image_offset))) then
                --determine horizontal active area
                if(pixel_counter < (to_integer(unsigned(h_image_offset)) + h_image_active)) then
                    h_active <= '1';
                else
                    h_active <= '0';
                end if;
            else
                h_active <= '0';
            end if;
        end if;
    end if;
    end process;

    active <= '1' when (v_active = '1' and h_active = '1') else
              '0';

    fsm:process(i_clk, input_res, master_reset)
    begin
    
        if(master_reset = '0') then
            line_counter <= 0;
            max_lines <= 0;
            v_active <= '0';
            reset_req_reg <= '0';
            output_sync <= '0';
            state <= state_init;
            state_queue <= state_init;
        else
            if(rising_edge(i_clk)) then

                case(state) is
                    when state_init =>
                        --set default signal levels
                        state_queue <= state_init;

                        if(vsync_rising = '1') then
                            state <= state_frame_start;
                        elsif(hsync_falling = '1') then
                            state <= state_line_start;
                        end if;

                    when state_frame_start =>
                        line_counter <= 0;          --reset the line counter at the start of each field
                        max_lines <= line_counter;  --determine the maximum lines (for resolution detection)
                        
                        if(i_hsync_n = '0') then    --determine whether the field is odd or even
                            odd_even_flag <= C_ODD_FIELD; 
                            first_line_even_flag <= '0';
                        else
                            odd_even_flag <= C_EVEN_FIELD;
                            first_line_even_flag <= '1';
                            block_first_line_increment <= '1';
                        end if;
                        first_line_even_flag_last <= first_line_even_flag;
                        ntpal_r <= i_ntpal; --store the state of the ntpal pin for the scaling

                        state <= state_check_resolution;
                        next_state <= state_send_sync;
                        state_queue <= state_active;
                               
                    when state_check_resolution =>
                        --if even_last and even = 1 -> 256p
                        --if even_last and even = 0 + 262 lines -> 240p
                        case(max_lines) is
                            when 312 to 314 =>
                                if(first_line_even_flag_last = '1' and first_line_even_flag = '1') then
                                    input_res <= RES_256p;
                                else
                                    input_res <= RES_512i;
                                end if;
                                reset_req_reg <= '1';
                            when 262 to 263 =>
                                if(first_line_even_flag_last = '0' and first_line_even_flag = '0') then
                                    input_res <= RES_240p;
                                else
                                    input_res <= RES_480i;
                                end if;
                                reset_req_reg <= '1';
                            when 524 to 525 =>
                                input_res <= RES_480p;
                                reset_req_reg <= '1';
                            when others =>
                                next_state <= state_init;
                                reset_req_reg <= '0';
                        end case;
                        
                        state <= load_config;
                        request_config <= '1';
                        
                    when load_config =>
                        request_config <= '0';
                        if(load_config_done = '1') then
                             state <= next_state;
                        end if; 
    
                    when state_send_sync =>
                        video_config.h_active_pixels <= to_integer(unsigned(h_active_pixels));
                        video_config.h_compressed_pixels <= to_integer(unsigned(h_packed_pixels));
                        video_config.v_active_pixels <= to_integer(unsigned(v_image_active));
                        video_config.odd_even_flag <= odd_even_flag;
                        video_config.interlaced_flag <= interlaced_flag;
                        video_config.ntpal_flag <= ntpal_r;
                        video_config.deinterlacer_debug <= deinterlacer_debug;
                        video_config.motion_threshold <= to_integer(unsigned(motion_threshold));
                        video_config.x_scaling_factor <= unsigned(scaling_factor_x);
                        video_config.y_scaling_factor <= unsigned(scaling_factor_y);
                        video_config.x_active_out <= unsigned(target_res_x);
                        video_config.y_active_out <= unsigned(target_res_y);

                        output_sync <= '1';
                        state <= state_line_start;
                    
                    when state_line_start =>
                        
                        output_sync <= '0';
                        
                        if(block_first_line_increment = '1') then
                           block_first_line_increment <= '0';
                        else
                            line_counter <= line_counter + 1;
                        end if;
                 
                        --determine vertical active area
                        if(line_counter >= to_integer(unsigned(v_image_offset))) then
                            if(line_counter < (to_integer(unsigned(v_image_offset)) + to_integer(unsigned(v_image_active)))) then
                                v_active <= '1';
                            else
                                v_active <= '0';
                            end if;
                        else
                            v_active <= '0';
                        end if;    

                        state <= state_queue;
                    
                    when state_active =>

                        if(vsync_rising = '1') then
                            state <= state_frame_start;
                        elsif(hsync_falling = '1') then
                            state <= state_line_start;
                        end if;

                    when others =>

                end case;
           end if;
    end if;
    end process;
    
    data_sampling:process(i_clk, master_reset)
    begin

    if(master_reset = '0') then
            line_active <= '0';
            packed_pxl_ctr <= 0;
            sampler <= 0;
            step_counter <= "0001";
        else
            if(rising_edge(i_clk)) then
            
                 if(active = '1') then
                    line_active <= '1';
                    --sampling_divider sets the frequency with which the signal is sampled
                    if(sampler < sampling_divider) then
                        sampler <= sampler + 1;
                    else
                        sampler <= 0;
                    end if;
                    
                    if(sampler = 0) then
                        --PACKING THE PIXELS BELOW                       
                        --with this implementation, the horizontal input resolution must be dividable by 4, otherwise things may break!
                        --if this needs to be more flexible, think about the situation where a the case below stops at "0001" -> the last pixel would not be stored!
                        step_counter <= rotate_left(step_counter, 1);
                        --R = 23 downto 16
                        --G = 15 downto 8
                        --B = 7 downto 0
                         case(step_counter) is
                             when "0001" =>
                                    --shift in first pixel, not enough data to output
                                    outbuffer_1(23 downto 0) <= i_data;                   --buf1 = 0111
                                    out_we <= "00";

                             when "0010" =>
                                    --shift in second pixel, output buffer 1 (full)
                                    outbuffer_1(31 downto 24) <= i_data (7 downto 0);     --buf1 = 2111 -> output full buf1
                                    outbuffer_2(15 downto 0) <= i_data (23 downto 8);     --buf2 = 0022
                                    packed_pxl_ctr <= packed_pxl_ctr + 1;
                                    out_we <= "01";
                    
                             when "0100" =>
                                    --shift in third pixel, output buffer 2 (full)
                                    outbuffer_1(7 downto 0) <= i_data (23 downto 16);     --buf1 = 0003
                                    outbuffer_2(31 downto 16) <= i_data (15 downto 0);    --buf2 = 3322 -> output full buf2
                                    packed_pxl_ctr <= packed_pxl_ctr + 1;
                                    out_we <= "10";
                    
                             when "1000" =>
                                    --shift in fourth pixel, output buffer 1 (full again) -> from here the pattern repeats, go back to step "0001"
                                    outbuffer_1(31 downto 8) <= i_data;                   --buf1 = 4443 -> output full buf1
                                    packed_pxl_ctr <= packed_pxl_ctr + 1;
                                    out_we <= "01";
                    
                             when others => null;
                         end case;
       
                    else
                        out_we <= "00";
                    end if;
            
                else
                    line_active <= '0';
                    sampler <= 0;
                    step_counter <= "0001";
                    out_we <= "00";
                    packed_pxl_ctr <= 0;               
                end if;    
            
            end if;
        end if;

    end process;
              
    o_data <=   outbuffer_1 when (out_we = "01") else
                        outbuffer_2 when (out_we = "10") else
                        (others => '0');
                        
    o_we <= out_we(0) OR out_we(1);
    o_addr <=   std_logic_vector(to_unsigned(packed_pxl_ctr, 10)) when (out_we(0) = '1' or out_we(1) = '1') else 
                (others=>'0');    
                
    o_line_active <= line_active;                    
    o_video_cfg <= video_config;
    o_rst_req_n <= reset_req_reg;
    o_sync <= output_sync;


config_cdc:process(i_clk)
begin
    if(rising_edge(i_clk)) then
    
    config_override <= i_config_override;
    video_enable <= i_video_en;
    
    case(read_state) is
    
        when state_wait =>
            load_config_done <= '0';
            if(request_config = '1') then
                if(config_override = '1') then
                    read_state <= state_read_configs;
                else
                    read_state <= state_wait_config;
                end if;
            end if;
         when state_wait_config => --this state essentially does nothing, it's just there to consume the same amount of clk cycles as the config read to avoid timing issues
            re_ram <= '0';
            read_counter <= read_counter + 1;
            if(read_done = '1') then
                read_state <= state_update_res;
                read_counter <= 0;
                read_done <= '0';
            else null;
            end if;
         when state_read_configs =>
            read_counter <= read_counter + 1;
            re_ram <= '1';
            if(read_done = '1') then
                read_state <= state_update_res;
                re_ram <= '0';
                read_counter <= 0;
                read_done <= '0';
            else null;
            end if;
         when state_update_res =>
            --register_whatever <= input_res;
            load_config_done <= '1';
            read_state <= state_wait;
         when others => null; 
    end case;
    
    --the input state machine is triggering this when a configuration load is needed at the beginning of a frame
    re_addr_ram <= input_res + read_counter;
    re_data_ram_1 <= i_data_ram; --re_data_ram_1 is the LSB for 2-byte configs
    re_data_ram_2 <= re_data_ram_1; --re_data_ram_2 is the LSB for 3-byte configs
    
    if(re_ram = '1') then    
        case(read_counter) is
            when H_IMAGE_OFFSET_READ =>
                h_image_offset <=  i_data_ram & re_data_ram_1;
            when H_IMAGE_ACTIVE_READ =>    
                h_image_active <= i_data_ram & re_data_ram_1;
            when H_ACTIVE_PXL_READ =>
                h_active_pixels <= i_data_ram & re_data_ram_1;
            when H_PACKED_PXL_READ =>
                h_packed_pixels <= i_data_ram & re_data_ram_1;
            when V_IMAGE_OFFSET_READ =>
                v_image_offset <= i_data_ram;
            when V_IMAGE_ACTIVE_READ =>
                v_image_active <= i_data_ram & re_data_ram_1;
            when INTERLACE_DIVIDE_READ =>
                 interlaced_flag <= i_data_ram(2 downto 0);
                 sampling_divider <= to_integer(unsigned(i_data_ram(7 downto 4)));
            when DEINTERLACER_READ =>
                deinterlacer_debug <= i_data_ram(0);
                motion_threshold <= i_data_ram(7 downto 1);
 
            when SCALING_X_READ =>
                scaling_factor_x <= i_data_ram(1 downto 0) & re_data_ram_1 & re_data_ram_2;
            when SCALING_Y_READ =>
                scaling_factor_y <= i_data_ram(1 downto 0) & re_data_ram_1 & re_data_ram_2;
            when TARGET_RES_X_READ =>
                target_res_x <= i_data_ram(1 downto 0) & re_data_ram_1;
            when TARGET_RES_Y_READ =>
                target_res_y <= i_data_ram(1 downto 0) & re_data_ram_1;
                read_done <= '1';
            when others =>
                null;
        end case;
    else
        if(read_counter = TARGET_RES_Y_READ) then
            read_done <= '1';
        end if;
    end if;

    --base address bank:
    --RANGE_VIDEO_CONFIG "0001"
    --RANGE_VIDEO_REGS "0010"
    
    --registers:
    --REG_RESOLUTION_INFO RANGE_VIDEO_REGS + 0
    --REG_CONFIG_OVERRIDE RANGE_VIDEO_REGS + 1

    --config map per resolution: RANGE_VIDEO_CONFIG + resolution + register offset
    --h_image_offset_lsb
    --h_image_offset_msb
    --h_image_active_lsb
    --h_image_active_msb
    --h_active_pixels_lsb
    --h_active_pixels_msb
    --h_packed_pixels_lsb
    --h_packed_pixels_msb
    --v_image_offset
    --v_image_active_lsb
    --v_image_active_msb
    --sampling_divider(7-4) & interlaced_flag(0)
    --motion_threshold(7-1) & deinterlacer_debug(0)

    end if;

end process;

o_addr_ram <= std_logic_vector(to_unsigned(re_addr_ram, 8));
o_re_ram <= re_ram;
o_video_res <= input_res;

end Behavioral;
