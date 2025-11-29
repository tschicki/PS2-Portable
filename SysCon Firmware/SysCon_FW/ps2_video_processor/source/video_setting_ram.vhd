----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Patrick Haid
-- 
-- Create Date: 21.03.2025 07:18:39
-- Design Name: PS2 Video Processor
-- Module Name: video_setting_ram - Behavioral
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

entity video_setting_ram is
  Port ( 
        --control input
        i_clk0 :              in std_logic;--main system clock                       
        i_clk_ps2 :           in std_logic;--PS2 pixel clock input
        i_rst_ps2_n :         in std_logic;--PS2 reset input
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
end video_setting_ram;

architecture Behavioral of video_setting_ram is

    component efx_simple_dual_port_2_clk_ram is
  generic (
    RAM_WIDTH : integer := 8;
    RAM_DEPTH : integer := 160;
    OUTREG    : boolean := true
    );
  port (
    addra  : in  std_logic_vector((logb2(RAM_DEPTH)-1) downto 0);
    addrb  : in  std_logic_vector((logb2(RAM_DEPTH)-1) downto 0);
    dina   : in  std_logic_vector(RAM_WIDTH-1 downto 0);
    clka   : in  std_logic;
    clkb   : in  std_logic;
    wea    : in  std_logic;
    enb    : in  std_logic;
    rstb   : in  std_logic;
    regceb : in  std_logic;
    doutb  : out std_logic_vector(RAM_WIDTH-1 downto 0));
    end component efx_simple_dual_port_2_clk_ram;

    --control interface
    type t_registers is array (15 downto 0) of std_logic_vector(7 downto 0);
    signal register_array : t_registers := (X"00",X"FF",X"00",X"00",X"00",X"00",X"00",X"00",X"00",X"00",X"00",X"00",X"00",X"00",X"00",X"00");
    
    type state_type_write is (state_wait, 
                              state_analyze,
                              state_access_regs, 
                              state_write_ram,
                              state_read_ram
                              );
    signal config_state : state_type_write := state_wait;
    
    signal current_data : std_logic_vector(7 downto 0) := (others=>'0');
    signal current_addr : std_logic_vector(15 downto 0) := (others=>'0');
  
    signal wr_data_ram : std_logic_vector(7 downto 0) := (others=>'0');
    signal wr_addr_ram : std_logic_vector(7 downto 0) := (others=>'0');
    signal we_ram, rw_flag : std_logic := '0';

    signal o_data_config_r : std_logic_vector(7 downto 0) := (others=>'0');
    signal config_override_cdc, config_override : std_logic := '0';
    signal resolution_cdc : integer range 0 to 128 := 0;
    signal video_enable_cdc, video_enable : std_logic := '0';
    
begin

  config_ram: efx_simple_dual_port_2_clk_ram
  generic map (RAM_WIDTH => 8, RAM_DEPTH => 160, OUTREG => true)
  port map (
    addra  => wr_addr_ram,
    addrb  => B_i_addr_ram,
    dina   => wr_data_ram,
    clka   => i_clk0,
    clkb   => i_clk_ps2,
    wea    => we_ram,
    enb    => B_i_re_ram,
    rstb   => i_rst_ps2_n,
    regceb => B_i_re_ram,
    doutb  => B_o_data_ram);

--how to config (at boot):
--1 set config_override to 0
--2 set all the registers
--3 set config override to 1
--4 set video enable to 1   (only at boot)

config_write:process(i_clk0, i_rst_ps2_n)
begin

    if(rising_edge(i_clk0)) then
        --CDC for the resolution
        resolution_cdc <= B_i_video_res;
    
        case(config_state) is
            when state_wait =>
                --wait for cs high
                register_array(REG_RESOLUTION_INFO) <= std_logic_vector(to_unsigned(resolution_cdc, 8));
                register_array(REG_VERSION_MAJOR) <= std_logic_vector(to_unsigned(B_i_version_major, 8));
                register_array(REG_VERSION_MINOR) <= std_logic_vector(to_unsigned(B_i_version_minor, 8));
                we_ram <= '0';
                if(A_i_cs_config = '1') then
                    current_data <= A_i_data_config;
                    current_addr <= A_i_addr_config;
                    rw_flag <= A_i_rw_config;
                    config_state <= state_analyze;
                end if;
                
            when state_analyze =>
                --check which ram is accessed
                case(current_addr(15 downto 12)) is
                    when RANGE_VIDEO_REGS =>
                        config_state <= state_access_regs;
                    when RANGE_VIDEO_CONFIG =>
                        config_state <= state_write_ram;
                    when others =>
                        config_state <= state_wait;
                end case;
 
            when state_access_regs =>
                if(rw_flag = '0') then
                    register_array(to_integer(unsigned(current_addr(3 downto 0)))) <= current_data;
                else
                    o_data_config_r <= register_array(to_integer(unsigned(current_addr(3 downto 0))));
                end if;
                
                if(A_i_cs_config = '0') then
                    config_state <= state_wait;
                else
                    null;
                end if;
        
            when state_write_ram =>
                wr_data_ram <= current_data;
                wr_addr_ram <= current_addr(7 downto 0);
                we_ram <= '1';
                config_state <= state_wait;
                               
            when state_read_ram =>
                --not implemented yet
                config_state <= state_wait;
                    
            when others => null;
        end case;
    
    end if;

end process;

cdc: process(i_clk_ps2)
begin

    if(rising_edge(i_clk_ps2)) then
        if(config_state = state_wait) then
            config_override_cdc <= register_array(REG_CONFIG_OVERRIDE)(0);
            video_enable_cdc <= register_array(REG_CONFIG_OVERRIDE)(1);      
        end if;
        config_override <= config_override_cdc;
        video_enable <= video_enable_cdc;     
    end if;

end process;

A_o_data_config <= o_data_config_r when (A_i_cs_config = '1') else
                (others=>'Z');
                
B_o_video_en <= video_enable;
B_o_config_override <= config_override;


end Behavioral;
