----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Patrick Haid
-- 
-- Create Date: 13.01.2025 21:35:41
-- Design Name: PS2 Video Processor
-- Module Name: scaler_types
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
use ieee.numeric_std.all;

package scaler_types is

type video_config_t is record
    h_active_pixels :       integer range 0 to 1023;    --horizontal active area in pixels
    h_compressed_pixels :   integer range 0 to 1023;    --horizontal active area in pixels * 0.75 -> compressed state
    v_active_pixels :       integer range 0 to 1023;    --vertical active area in pixels
    odd_even_flag :         std_logic;                  --odd/even flag of the current field
    interlaced_flag :       std_logic_vector(2 downto 0);                  --interlaced flag of the current resolution
    ntpal_flag :            std_logic;                  --ntpal flag sampled from PS2
    deinterlacer_debug :    std_logic;                  --turns on/off debug colors of the motion detector
    motion_threshold :      integer range 0 to 127;     --threshold for motion detection of deinterlacer
    x_scaling_factor :      unsigned(17 downto 0);
    y_scaling_factor :      unsigned(17 downto 0);
    x_active_out :          unsigned(9 downto 0);
    y_active_out :          unsigned(9 downto 0);

end record video_config_t;

type field_info_t is record
    bank_odd_even :         std_logic;                      --odd/even flag of the latest stored field
    bank_count_A :          std_logic_vector(2 downto 0);   --bank address of field A
    bank_count_B :          std_logic_vector(2 downto 0);   --bank address of field B
    bank_count_C :          std_logic_vector(2 downto 0);   --bank address of field C
    bank_count_D :          std_logic_vector(2 downto 0);   --bank address of field D
end record field_info_t;

constant C_ODD_FIELD :      std_logic := '1';
constant C_EVEN_FIELD :     std_logic := '0';
constant C_PAL :            std_logic := '0';
constant C_NTSC :           std_logic := '1';

--selection for video processing
constant MODE_PROGRESSIVE : std_logic_vector(2 downto 0) := "000";
constant MODE_MOTION_ADAPTIVE : std_logic_vector(2 downto 0) := "001";
constant MODE_LINE_DOUBLE : std_logic_vector(2 downto 0) := "010";
--supported vertical resolutions
constant RES_512i : integer range 0 to 128 := 0;
constant RES_480i : integer range 0 to 128 := 32;
constant RES_480p : integer range 0 to 128 := 64;
constant RES_256p : integer range 0 to 128 := 96;
constant RES_240p : integer range 0 to 128 := 128;
  
--constants for SPI transmisson
constant DUMMY_BYTE : std_logic_vector(7 downto 0) := "01010110";
constant CMD_WRITE_REG : std_logic_vector(7 downto 0) := "00000001";
constant CMD_READ_REG : std_logic_vector(7 downto 0) := "00000010";
--video processor SPI banks
constant RANGE_VIDEO_CONFIG : std_logic_vector(3 downto 0) := "0001";
constant RANGE_VIDEO_REGS : std_logic_vector(3 downto 0) := "0010";
constant RANGE_BRIGHTNESS : std_logic_vector(3 downto 0) := "0100";
constant RANGE_SPRITE_REG : std_logic_vector(3 downto 0) := "0101";
constant RANGE_SPRITE_BUF : std_logic_vector(3 downto 0) := "0110";
constant RANGE_SPRITE_RAM : std_logic_vector(3 downto 0) := "0111";
--registers
constant REG_RESOLUTION_INFO :      integer range 0 to 3 := 0;
constant REG_CONFIG_OVERRIDE :      integer range 0 to 3 := 1;
constant REG_VERSION_MAJOR :        integer range 0 to 3 := 2;
constant REG_VERSION_MINOR :        integer range 0 to 3 := 3;
--video setting register offsets  
constant H_IMAGE_OFFSET_READ :      integer := 4;
constant H_IMAGE_ACTIVE_READ :      integer := 6;
constant H_ACTIVE_PXL_READ :        integer := 8;
constant H_PACKED_PXL_READ :        integer := 10;
constant V_IMAGE_OFFSET_READ :      integer := 11;
constant V_IMAGE_ACTIVE_READ :      integer := 13;
constant INTERLACE_DIVIDE_READ :    integer := 14;
constant DEINTERLACER_READ :        integer := 15;
constant SCALING_X_READ :           integer := 18;
constant SCALING_Y_READ :           integer := 21;
constant TARGET_RES_X_READ :        integer := 23;
constant TARGET_RES_Y_READ :        integer := 25;
--sprite register offsets
constant REG_POSX_LSB : integer range 0 to 10 := 0;
constant REG_POSX_MSB : integer range 0 to 10 := 1;
constant REG_POSY_LSB : integer range 0 to 10 := 2;
constant REG_POSY_MSB : integer range 0 to 10 := 3;
constant REG_COLORA_LSB : integer range 0 to 10 := 4;
constant REG_COLORA_MSB : integer range 0 to 10 := 5;
constant REG_COLORB_LSB : integer range 0 to 10 := 6;
constant REG_COLORB_MSB : integer range 0 to 10 := 7;
constant REG_SCALE : integer range 0 to 10 := 8;
--backlight register offsets
constant REG_MAX_COUNT : integer range 0 to 2 := 0;
constant REG_TRIGGER_COUNT : integer range 0 to 2 := 1;


end scaler_types;

