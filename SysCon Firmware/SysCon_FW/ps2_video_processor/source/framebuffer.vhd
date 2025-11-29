----------------------------------------------------------------------------------
-- Company:
-- Engineer: Patrick Haid
--
-- Create Date: 13.01.2025 21:36:02
-- Design Name: PS2 Video Processor
-- Module Name: framebuffer - Behavioral
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
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;
use ieee.std_logic_unsigned.all;

library work;
use work.scaler_types.all;
use work.ram_pkg.all;

entity framebuffer is
	port (
		--PLL interface----------------------------------------------------
		PLL_CLKOUT_0 : in std_logic; --clkin is 166MHz
		PLL_CLKOUT_1 : in std_logic; --clkin is 166MHz with phaseshift
		PLL_CLKOUT_2 : in std_logic; --clkin for video out 66408300Hz
		locked : in std_logic;
		--PS2 interface----------------------------------------------------
		rst_in : in std_logic;
		clkin : in std_logic;
		hsync_in : in std_logic;
		vsync_in : in std_logic;
		ntpal_in : in std_logic;
		video_in : in std_logic_vector (23 downto 0);
		--wrapper ports for PS2 I2C interface
		scl_IN : in std_logic;
		scl_OUT : out std_logic;
		scl_OE : out std_logic;
		sda_IN : in std_logic;
		sda_OUT : out std_logic;
		sda_OE : out std_logic;
		--LCD interface-----------------------------------------------------
		video_out : out std_logic_vector (23 downto 0);
		de_out : out std_logic;
		lcd_en : out std_logic;
		pwm_out : out std_logic;
		vsync_out : out std_logic;
		hsync_out : out std_logic; 
		--interface to SDRAM------------------------------------------------
		sdram_address : out std_logic_vector(11 downto 0);
		sdram_bank_sel : out std_logic_vector(1 downto 0);
		sdram_cas : out std_logic;
		sdram_cke : out std_logic;
		sdram_cs : out std_logic;
		sdram_dqm : out std_logic_vector(3 downto 0);
		sdram_ras : out std_logic;
		sdram_we : out std_logic;
		--wrapper ports for bidirectional buffer - only for Trion
		sdram_data_IN : in std_logic_vector(31 downto 0);
		sdram_data_OUT : out std_logic_vector(31 downto 0);
		sdram_data_OE : out std_logic_vector(31 downto 0); 
		--syscon SPI interface
		i_spi_clk : in std_logic;
		i_spi_rx : in std_logic;
		o_spi_tx : out std_logic;
		i_spi_cs : in std_logic
	);
end framebuffer;

architecture Behavioral of framebuffer is

	component framebuffer_scheduler is
		port (
			--RESET and CLK
			i_rst_sync0 : in std_logic; --reset syncronized with clk0
			i_rst_sync1 : in std_logic; --reset syncronized with clk1
			i_clk0 : in std_logic; --150MHz clock
			i_clk1 : in std_logic; --150MHz clock, shifted
			--interface to input line buffer 
			o_re : out std_logic; --read enable input
			i_data : in std_logic_vector(31 downto 0); --video data out; in 32 byte blocks
			o_addr : out std_logic_vector(9 downto 0);
			i_video_cfg : in video_config_t; --video config
			i_wr_req : in std_logic; --high when pixels are to be transmitted
			i_vsync_pulse : in std_logic;
			--interface to deinterlacer
			i_re_req : in std_logic;
			o_data : out std_logic_vector(31 downto 0); --video data
			i_addr : in std_logic_vector(21 downto 0);
			o_field_info : out field_info_t;
			o_frame_sync : out std_logic;
			o_valid : out std_logic;
			--interface to SDRAM
			sdram_address : out std_logic_vector(11 downto 0);
			sdram_bank_sel : out std_logic_vector(1 downto 0);
			sdram_cas : out std_logic;
			sdram_cke : out std_logic;
			sdram_cs : out std_logic;
			sdram_dqm : out std_logic_vector(3 downto 0);
			--wrapper ports for bidirectional buffer - only for Trion
			sdram_data_IN : in std_logic_vector(31 downto 0);
			sdram_data_OUT : out std_logic_vector(31 downto 0);
			sdram_data_OE : out std_logic_vector(31 downto 0);
			sdram_ras : out std_logic;
			sdram_we : out std_logic
			
		);
	end component;

	component reset_module is
		port (
			--PLL clock input
			PLL_CLKOUT_0 : in std_logic; --clkin is 166MHz
			PLL_CLKOUT_1 : in std_logic; --clkin is 166MHz with phaseshift
			PLL_CLKOUT_2 : in std_logic; --clkin for video out 66408300Hz
			--PLL locked flag input
			locked : in std_logic;
			--reset request input
			i_rst_req_n : in std_logic;
			--reset outputs; async assert, sync release
			o_rst_sync0 : out std_logic; --reset for PLL CLK 0 domain (most internal logic)
			o_rst_sync1 : out std_logic; --reset for PLL CLK 1 domain (shifted CLK0, only applicable for SDRAM controller)
			o_rst_sync2 : out std_logic --reset for PLL CLK 2 domain (video output clock)
		);
	end component;

	component video_sample is
		port (
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
        i_config_override :   in std_logic
		);
	end component;

	component ip_line_buffer is
		port (
			a_i_clk : in std_logic; --pixel clock of clock domain a
			a_i_we : in std_logic; --write enable input
			a_i_data : in std_logic_vector(31 downto 0); --video data in
			a_i_addr : in std_logic_vector(9 downto 0);
			a_i_video_cfg : in video_config_t; --video config
			a_i_line_active : in std_logic;
			a_i_vsync : in std_logic;
 
			b_i_rst_n : in std_logic; --reset of readout state machine 
			b_i_clk : in std_logic; --pixel clock of clock domain a
			b_i_re : in std_logic; --read enable input
			b_o_data : out std_logic_vector(31 downto 0); --video data out; in 32 byte blocks
			b_i_addr : in std_logic_vector(9 downto 0);
			b_o_video_cfg : out video_config_t;
			b_o_write_req : out std_logic; --high when pixels are to be transmitted
			b_vsync : out std_logic --single pulse falling edge of vsync);
		);
	end component;

	component ma_deinterlacer_4field is
		port (
			--interface to framebuffer
			i_rst_n : in std_logic;
			i_clk : in std_logic;
			o_re : out std_logic;
			i_data : in std_logic_vector(31 downto 0); --video data
			a_addr : out std_logic_vector(21 downto 0);
			i_video_cfg : in video_config_t; --video config
			i_field_info : in field_info_t;
			i_sync : in std_logic;
			i_valid : in std_logic;
			--interface to output buffer fifo
			o_we : out std_logic;
			o_data : out std_logic_vector(23 downto 0);
			o_addr : out std_logic_vector(9 downto 0);
			i_ready : in std_logic;
			i_end : in std_logic
		);
	end component;

	component line_fifo_improved is
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
    end component;

	component video_out_de is
		port (
			--interface with out line buffer
			i_rst_n : in std_logic;
			i_clk : in std_logic; --framebuffer side pixel clock
			o_re : out std_logic; --read request from the output line buffer
			i_data : in std_logic_vector(23 downto 0); --date to out line buffer
			o_addr : out std_logic_vector(9 downto 0);
			i_ntpal : in std_logic; --control word relevant for video out
			i_ready : in std_logic; --high as long as there is data to be read from the framebuffer; will go low when one whole frame was read
			o_end : out std_logic;
			--video output signals
			o_data : out std_logic_vector (23 downto 0);
			o_de : out std_logic;
			o_lcd_en : out std_logic
		);
	end component;

	component image_scale is
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
    end component;

	component output_linebuffer is
		port (
			--150MHz side control
            a_clk : in std_logic;
            a_sync : in std_logic;
            a_video_cfg : in video_config_t;
            --video out side control
            b_rst_n : in std_logic;
            b_clk : in std_logic;
            
            --data input from scaler
            a_we : in std_logic;                               
            a_data : in std_logic_vector(23 downto 0);             
            a_addr : in std_logic_vector(9 downto 0); 
            a_frame_end : out std_logic;
            a_line_start : out std_logic;  
            --interface with video output
            b_re : in std_logic;                               
            b_data : out std_logic_vector(23 downto 0);             
            b_addr : in std_logic_vector(9 downto 0);   
            b_sync : out std_logic;                    
            b_frame_end : in std_logic;
            b_ntpal : out std_logic
		);
	end component;

	component SPI_Controller is
		port (
			--syscon SPI interface
			i_spi_clk : in std_logic;
			i_spi_rx : in std_logic;
			o_spi_tx : out std_logic;
			i_spi_cs : in std_logic;
			--internal clock PLL0 and reset
			i_clk0 : in std_logic;
			i_rst_sync0 : in std_logic;
 
			--control interface to configure all the modules
			i_data : in std_logic_vector(7 downto 0);
			o_data : out std_logic_vector(7 downto 0);
			o_addr : out std_logic_vector(15 downto 0);
			o_rw : out std_logic;
			o_cs_video_in : out std_logic;
			o_cs_brightness : out std_logic;
			o_cs_sprite : out std_logic
		);
	end component;

	component Sprite_Module is
		port (
			i_rst_n : in std_logic;
			i_clk : in std_logic;

			--control interface to configure all the modules
			i_data_config : in std_logic_vector(7 downto 0);
			--o_data_config : out std_logic_vector(7 downto 0);
			i_addr_config : in std_logic_vector(15 downto 0);
			i_rw_config : in std_logic;
			i_cs_config : in std_logic;
 
			--data input from scaler
			i_we : in std_logic; 
			i_data : in std_logic_vector(23 downto 0); 
			i_addr : in std_logic_vector(9 downto 0); 
			i_line_count : in integer range 0 to 1024;
 
			--data output to CDC line buffer
			o_we : out std_logic; 
			o_data : out std_logic_vector(23 downto 0); 
			o_addr : out std_logic_vector(9 downto 0) 
		);
	end component;


    component video_setting_ram is
        Port ( 
        --control input
        i_clk0 :              in std_logic;                         --PS2 pixel clock input
        i_clk_ps2 :           in std_logic;
        i_rst_ps2_n :         in std_logic;
        --control interface to configure all the modules
        A_i_data_config :     in std_logic_vector(7 downto 0);
        A_o_data_config :     out std_logic_vector(7 downto 0);
        A_i_addr_config :     in std_logic_vector(15 downto 0);
        A_i_rw_config :       in std_logic;
        A_i_cs_config :       in std_logic;
        --read port
        B_o_data_ram :        out std_logic_vector(7 downto 0);
        B_i_addr_ram :        in std_logic_vector(7 downto 0);
        B_i_re_ram :          in std_logic;
        --registers
        B_i_video_res :       in integer range 0 to 128;
        B_o_video_en :        out std_logic;
        B_o_config_override : out std_logic;
        B_i_version_major :   in integer range 0 to 127;
        B_i_version_minor :   in integer range 0 to 127
        
        );
    end component;
    
    component backlight_pwm is
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
end component;

	component I2C_slave is
		generic (
			SLAVE_ADDR : std_logic_vector(6 downto 0)
		);
		port (
			scl_in : in std_logic;
			scl_out : out std_logic;
			scl_oe : out std_logic;
			sda_in : in std_logic;
			sda_out : out std_logic;
			sda_oe : out std_logic;
			mclk_in : in std_logic;
			rst_in : in std_logic
		);
	end component;

	-----wires between input line buffer and video sample
	signal w_data_ip : std_logic_vector(31 downto 0);
	signal w_addr_ip : std_logic_vector(9 downto 0);
	signal w_video_cfg_ps2 : video_config_t;
	signal w_we_ip, w_ready_ip : std_logic;
	-----wires between scaler and video output module
	signal w_re_op, w_ready_op, w_end_op : std_logic;
	signal w_data_op : std_logic_vector(23 downto 0);
	signal w_addr_op : std_logic_vector(9 downto 0);
	-----wires between deinterlacer and output fifo
	signal w_we_fifo, w_ready_fifo, w_frame_end, w_line_start, w_frame_end_test : std_logic;
	signal w_data_fifo : std_logic_vector(23 downto 0);
	signal w_addr_fifo : std_logic_vector(9 downto 0);
	-----wires between fifo and scaler
	signal w_re_scale, w_fifo_empty_scale : std_logic;
	signal w_line_inc1 : unsigned(1 downto 0);
	signal w_data_a_scale, w_data_b_scale, w_data_c_scale, w_data_d_scale : std_logic_vector(23 downto 0);
	signal w_addr_low, w_addr_high : std_logic_vector(9 downto 0);
	--resets--------------------------------------------------------
	signal rst_sync_0, rst_sync_1, rst_sync_2 : std_logic;
	--video input---------------------------------------------------
	signal rd_req_vid_in : std_logic; 
	signal buf_video_in : std_logic_vector(31 downto 0); --video data 
	signal video_config_0 : video_config_t;
	signal write_req_in : std_logic;
	signal rst_req_input : std_logic;
	signal b_i_addr : std_logic_vector(9 downto 0);
	signal output_sync : std_logic;
	--video out-----------------------------------------------------
	signal out_read_flag : std_logic;
	signal data_out_valid : std_logic;
	signal read_req_in : std_logic;
	signal read_word : std_logic_vector(31 downto 0) := (others => '0');
	signal addr_in : std_logic_vector(21 downto 0) := (others => '0');
	signal field_info_w : field_info_t;
	signal vsync_trigger : std_logic;
	--config bus----
	signal SI_data_config : std_logic_vector(7 downto 0);
	signal SO_data_config : std_logic_vector(7 downto 0);
	signal addr_config : std_logic_vector(15 downto 0);
	signal rw_config : std_logic;
	signal cs_sprites_config, cs_video_in, cs_brightness : std_logic;
	--sprtie stuff
	signal scaler_line_count : integer range 0 to 1024;
	signal w_we_sprite, w_we_lb : std_logic; 
	signal w_data_sprite, w_data_lb : std_logic_vector(23 downto 0); 
	signal w_addr_sprite, w_addr_lb : std_logic_vector(9 downto 0);

	signal w_ntpal : std_logic;
	--Video Settings
	signal w_data_ram : std_logic_vector(7 downto 0);
    signal w_addr_ram : std_logic_vector(7 downto 0);
    signal w_re_ram : std_logic;
    signal w_video_res : integer range 0 to 128;
    signal w_video_en : std_logic;
    signal w_config_override : std_logic;
    
    ---constants-----------------------
    constant RAM_WIDTH : integer := 24;
    constant RAM_DEPTH : integer := 801;
    constant OUTREG : boolean := true;
    -----------------------------------
    constant VERSION_MAJOR : integer range 0 to 127:= 0;
    constant VERSION_MINOR : integer range 0 to 127 := 1;

begin
	resets : reset_module
	port map(
		PLL_CLKOUT_0 => PLL_CLKOUT_0, 
		PLL_CLKOUT_1 => PLL_CLKOUT_1, 
		PLL_CLKOUT_2 => PLL_CLKOUT_2, 
		locked => locked, 
		i_rst_req_n => rst_req_input, 
		o_rst_sync0 => rst_sync_0, 
		o_rst_sync1 => rst_sync_1, 
		o_rst_sync2 => rst_sync_2
	);

	framebuffer : framebuffer_scheduler
	port map(
		i_rst_sync0 => rst_sync_0, 
		i_rst_sync1 => rst_sync_1, 
		i_clk0 => PLL_CLKOUT_0, 
		i_clk1 => PLL_CLKOUT_1, 
		o_re => rd_req_vid_in, 
		i_data => buf_video_in, 
		o_addr => b_i_addr, 
		i_video_cfg => video_config_0, 
		i_wr_req => write_req_in, 
		i_vsync_pulse => vsync_trigger, 
		i_re_req => read_req_in, 
		o_data => read_word, 
		i_addr => addr_in, 
		o_field_info => field_info_w, 
		o_frame_sync => out_read_flag, 
		o_valid => data_out_valid, 
		sdram_address => sdram_address, 
		sdram_bank_sel => sdram_bank_sel, 
		sdram_cas => sdram_cas, 
		sdram_cke => sdram_cke, 
		sdram_cs => sdram_cs, 
		sdram_dqm => sdram_dqm, 
		sdram_data_IN => sdram_data_IN, 
		sdram_data_OUT => sdram_data_OUT, 
		sdram_data_OE => sdram_data_OE, 
		sdram_ras => sdram_ras, 
		sdram_we => sdram_we
	);
	video_input : video_sample
	port map(
		i_rst_n => rst_in, 
		i_clk => clkin, 
		i_data => video_in, 
		i_hsync_n => hsync_in, 
		i_vsync_n => vsync_in, 
		i_ntpal => ntpal_in, 
		o_we => w_we_ip, 
		o_data => w_data_ip, 
		o_addr => w_addr_ip, 
		o_line_active => w_ready_ip, 
		o_video_cfg => w_video_cfg_ps2, 
		o_rst_req_n => rst_req_input, 
		o_sync => output_sync,
	
		--read port
        i_data_ram =>w_data_ram,
        o_addr_ram =>w_addr_ram,
        o_re_ram =>w_re_ram,
        --registers
        o_video_res =>w_video_res,
        i_video_en =>w_video_en,
        i_config_override =>w_config_override
      
	);

video_settings : video_setting_ram
	port map(
		--control input
        i_clk0 =>PLL_CLKOUT_0,
        i_clk_ps2 =>clkin,
        i_rst_ps2_n =>rst_in,
        --control interface to configure all the modules
        A_i_data_config =>SI_data_config,
        A_o_data_config =>SO_data_config,
        A_i_addr_config =>addr_config,
        A_i_rw_config =>rw_config,
        A_i_cs_config =>cs_video_in,
        --read port
        B_o_data_ram =>w_data_ram,
        B_i_addr_ram =>w_addr_ram,
        B_i_re_ram =>w_re_ram,
        --registers
        B_i_video_res =>w_video_res,
        B_o_video_en =>w_video_en,
        B_o_config_override =>w_config_override,
        B_i_version_major => VERSION_MAJOR,
        B_i_version_minor => VERSION_MINOR
	);
	
	input_buffer : ip_line_buffer
	port map(

		a_i_clk => clkin, 
		a_i_we => w_we_ip, 
		a_i_data => w_data_ip, 
		a_i_addr => w_addr_ip, 
		a_i_video_cfg => w_video_cfg_ps2, 
		a_i_line_active => w_ready_ip, 
		a_i_vsync => output_sync, 
		b_i_rst_n => rst_sync_0, 
		b_i_clk => PLL_CLKOUT_0, 
		b_i_re => rd_req_vid_in, 
		b_o_data => buf_video_in, 
		b_i_addr => b_i_addr, 
		b_o_video_cfg => video_config_0, 
		b_o_write_req => write_req_in, 
		b_vsync => vsync_trigger
	);

	vid_out : video_out_de
	port map(
		i_rst_n => rst_sync_2, 
		i_clk => PLL_CLKOUT_2, 
		o_re => w_re_op, 
		i_data => w_data_op, 
		o_addr => w_addr_op, 
		i_ntpal => w_ntpal, 
		i_ready => w_ready_op, 
		o_end => w_end_op, 
		o_data => video_out, 
		o_de => de_out, 
		o_lcd_en => lcd_en
	);

	deinterlacer : ma_deinterlacer_4field
	port map(
		--SDRAM CLOCK DOMAIN
		i_rst_n => rst_sync_0, 
		i_clk => PLL_CLKOUT_0, 
		o_re => read_req_in, 
		i_data => read_word, 
		a_addr => addr_in, 
		i_video_cfg => video_config_0, 
		i_field_info => field_info_w, 
		i_sync => out_read_flag, 
		i_valid => data_out_valid, 
		o_we => w_we_fifo, 
		o_data => w_data_fifo, 
		o_addr => w_addr_fifo, 
		i_ready => w_ready_fifo, 
		i_end => w_frame_end_test
	);
 
	op_fifo : line_fifo_improved
	generic map (RAM_WIDTH => RAM_WIDTH, RAM_DEPTH => RAM_DEPTH, OUTREG => OUTREG)
	port map(
		--deinterlacer side
        i_rst_n =>rst_sync_0,
        i_clk =>PLL_CLKOUT_0,
        i_we =>w_we_fifo,
        i_data =>w_data_fifo,
        i_addr =>w_addr_fifo,
        o_fifo_full =>w_ready_fifo,
        i_sync =>out_read_flag,
        
        --scaler side
        i_re =>w_re_scale,
        o_data_a =>w_data_a_scale,
        o_data_b =>w_data_b_scale,
        o_data_c =>w_data_c_scale,
        o_data_d =>w_data_d_scale,
        
        i_addr_low =>w_addr_low,
        i_addr_high =>w_addr_high,
        tail_ptr_inc =>w_line_inc1,                     
        o_fifo_empty =>w_fifo_empty_scale
	);

	scale : image_scale
	port map(
		i_clk =>PLL_CLKOUT_0,
        i_rst_n =>rst_sync_0,
        i_sync =>out_read_flag,
        i_video_cfg =>video_config_0,
        --interface with line fifo
        i_data_a =>w_data_a_scale,
        i_data_b =>w_data_b_scale,
        i_data_c =>w_data_c_scale,
        i_data_d =>w_data_d_scale,
        o_addr_l =>w_addr_low,
        o_addr_h =>w_addr_high,
        o_re =>w_re_scale,
        tail_ptr_inc =>w_line_inc1,
        i_fifo_empty =>w_fifo_empty_scale,
        frame_end =>w_frame_end_test,
        --interface with output buffer
        o_we =>w_we_sprite,
        o_data =>w_data_sprite,
        o_addr =>w_addr_sprite,
        o_line_counter =>scaler_line_count,
        i_line_start =>w_line_start
	);

	sprites : Sprite_Module
	port map(
		i_rst_n => rst_sync_0, 
		i_clk => PLL_CLKOUT_0, 
		--control interface to configure all the modules
		i_data_config => SI_data_config, 
		--o_data_config => SO_data_config, 
		i_addr_config => addr_config, 
		i_rw_config => rw_config, 
		i_cs_config => cs_sprites_config, 
		--data input from scaler
		i_we => w_we_sprite, 
		i_data => w_data_sprite, 
		i_addr => w_addr_sprite, 
		i_line_count => scaler_line_count, 
		--data output to CDC line buffer
		o_we => w_we_lb, 
		o_data => w_data_lb, 
		o_addr => w_addr_lb 
	);

	out_line_cdc : output_linebuffer
	port map(
		--150MHz side control
		a_clk => PLL_CLKOUT_0, 
		a_sync => out_read_flag, 
		a_video_cfg => video_config_0, 
		--video out side control
		b_rst_n => rst_sync_2, 
		b_clk => PLL_CLKOUT_2, 
		--data input from scaler
		a_we => w_we_lb, 
		a_data => w_data_lb, 
		a_addr => w_addr_lb, 
		a_frame_end => w_frame_end, 
		a_line_start => w_line_start, 
		--interface with video output
		b_re => w_re_op, 
		b_data => w_data_op, 
		b_addr => w_addr_op, 
		b_sync => w_ready_op, 
		b_frame_end => w_end_op, 
		b_ntpal => w_ntpal
	);

	spi_slave : SPI_Controller
	port map(
		i_spi_clk => i_spi_clk, 
		i_spi_rx => i_spi_rx, 
		o_spi_tx => o_spi_tx, 
		i_spi_cs => i_spi_cs, 
		i_clk0 => PLL_CLKOUT_0, 
		i_rst_sync0 => rst_sync_0, 
		i_data => SO_data_config, 
		o_data => SI_data_config, 
		o_addr => addr_config, 
		o_rw => rw_config, 
		o_cs_video_in => cs_video_in, 
		o_cs_brightness => cs_brightness, 
		o_cs_sprite => cs_sprites_config
	); 

	slave : I2C_slave
		generic map(SLAVE_ADDR => "0100001")
	port map(
		scl_in => scl_IN, 
		scl_out => scl_OUT, 
		scl_oe => scl_OE, 
		sda_in => sda_IN, 
		sda_out => sda_OUT, 
		sda_oe => sda_OE, 
		mclk_in => PLL_CLKOUT_0, 
		rst_in => '0'
	);
	
	pwm : backlight_pwm
	    port map( 
        i_clk => PLL_CLKOUT_0,
        i_rst_n => rst_sync_0,
        o_pwm => pwm_out,
        --control interface to configure all the modules
        i_data_config => SI_data_config,
        i_addr_config => addr_config,
        i_rw_config => rw_config,
        i_cs_config => cs_brightness
    );

end Behavioral;