----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Patrick Haid
-- 
-- Create Date: 16.02.2025 19:43:45
-- Design Name: PS2 Video Processor
-- Module Name: SPI_Controller - Behavioral
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
--registers:
--Video Input
    --PAL
        --h_image_offset 16 bit
        --h_image_active 16 bit
        --v_image_offset 16 bit
        --v_image_active 16 bit
    --NTSC
        --h_image_offset
        --h_image_active
        --v_image_offset
        --v_image_active
    --480p
        --h_image_offset
        --h_image_active
        --v_image_offset
        --v_image_active
    --240p (reserved)
        --h_image_offset
        --h_image_active
        --v_image_offset
        --v_image_active
--Deinterlacer
    --debug color flag
--Scaler
    --scaling ratio x (input_resolution_h - ) / (output_resolution_h - 1) 32 bits (float)
    --scaling ratio y (input_resolution_v - ) / (output_resolution_v - 1) 32 bits (float)
    --enable/disable scaling flag (4:3 mode) 16 bit    
--Display
    --display brightness    16 bit

--Sprites
    --Color A 24 bit
    --Color B 24 bit
    --Scale 8 bit
    
    --cursor x 16 bit
    --cursor y 16 bit
    
    --sprite#  16 bit



--commands:
    --WRITE_FONT    
    --WRITE_REGISTER
    --DRAW_SPRITE
    --ERASE_SPRITE

entity SPI_Controller is
Port ( 
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
end SPI_Controller;

architecture Behavioral of SPI_Controller is

--SPI RX
signal rx_buffer, rx_register, rx_register1 : std_logic_vector(7 downto 0) := (others => '0');
signal rx_counter : integer range 0 to 8 := 0;
signal rx_done_flag, rx_done_flag_cdc1, rx_done_flag_cdc2, rx_done_flag_clk0 : std_logic := '0';
signal spi_cs_clk0, spi_cs_cdc : std_logic := '0';
--SPI TX
signal tx_buffer, tx_register : std_logic_vector(7 downto 0) := (others => '0');
signal tx_counter : integer range 0 to 8 := 0;
signal o_spi_tx_r : std_logic := '0';
--FSM
type state_type is (state_init, 
                    state_save_cmd, 
                    state_save_address_lsb,
                    state_save_address_msb,
                    state_determine_command,
                    state_write_register, 
                    state_read_register,
                    state_apply_config 
                    );
signal spi_state, spi_next_state : state_type := state_init;

signal current_cmd, o_data_r, i_data_r : std_logic_vector(7 downto 0) := (others => '0');
signal current_address, o_addr_r, data_addr_cs : std_logic_vector(15 downto 0) := (others => '0');
signal data_address : integer range 0 to 65535 := 0;
signal rw_flag : std_logic := '0';

signal cs_enable, cs_video_in, cs_brightness, cs_sprite : std_logic := '0';

signal handle_tx, handle_rx : std_logic := '1';
signal read_counter : integer range 0 to 7 := 0;

begin

--SPI syscon
--spi_set_format(spi, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
--CPOL = 1
--CPHA = 1 -> sample at rising edge, output at falling edge
--SPI Mode 3
--8 bits per transfer

SPI_RX:process(i_spi_clk, i_spi_cs, i_clk0)
begin

    if(i_spi_cs = '1') then
        --if SPI CE is high shift in the data    
        if(rising_edge(i_spi_clk)) then
            rx_buffer <= rx_buffer(6 downto 0) & i_spi_rx;
            
            if(rx_counter < 7) then
                rx_counter <= rx_counter + 1;
                rx_done_flag <= '0';
            else
                --if the 8 bits are full, reset the counter and notify the fsm that the data is ready
                rx_counter <= 0;
                rx_done_flag <= '1';
            end if;
        end if;
    else
        rx_counter <= 0;
        rx_done_flag <= '0';
    end if;

    --bring the flag to the PLL0 clock domain
    if(rising_edge(i_clk0)) then
        rx_done_flag_cdc1 <= rx_done_flag;
        rx_done_flag_cdc2 <= rx_done_flag_cdc1;
        
        spi_cs_cdc <= i_spi_cs;
        spi_cs_clk0 <= spi_cs_cdc;
        
        if(rx_done_flag_cdc2 = '0' and rx_done_flag_cdc1 = '1') then
            --this edge detection is done so the flag is only active for one clock cycle (important!)
            rx_done_flag_clk0 <= '1';
            --save the RX buffer when notifying the FSM, when doing it above the LSB is missed
            rx_register <= rx_buffer;
        else
            rx_done_flag_clk0 <= '0';
        end if;
        
    end if;

end process;


SPI_TX:process(i_spi_clk, i_spi_cs, tx_register)
begin

    if(i_spi_cs = '1') then
        --if SPI CE is high shift out the data    
        if(falling_edge(i_spi_clk)) then
        
                o_spi_tx_r <= tx_buffer(7);
                tx_buffer <= std_logic_vector(shift_left(unsigned(tx_buffer), 1));
                
                if(tx_counter < 7) then
                    tx_counter <= tx_counter + 1;     
                else
                    --if the 8 bits are sent, reset the counter
                    tx_counter <= 0;
                    tx_buffer <= tx_register;
                end if;
        end if;
    else
        tx_counter <= 0;
        tx_buffer <= tx_register;
    end if;


end process;

fsm:process(i_clk0, spi_cs_clk0)
begin

--Byte 1: Command
--Byte 2: Address LSB
--Byte 3: Address MSB
--Byte 4: Data

if(spi_cs_clk0 = '0') then
    spi_state <= state_save_cmd;
    handle_tx <= '1';
    handle_rx <= '0';
    tx_register <= DUMMY_BYTE;
else
    if(rising_edge(i_clk0)) then
    
        if(rx_done_flag_clk0 = '1') then
            --states are only switched after each byte, except for the determination of the command
            spi_state <= spi_next_state;
            --this case only serves to corretly store the command and address and to synchronize reads and writes
            case(spi_state) is
                when state_save_cmd =>
                    current_cmd <= rx_register;
                when state_save_address_lsb =>
                    current_address(7 downto 0) <= rx_register;
                when state_save_address_msb =>
                    current_address <= rx_register & current_address(7 downto 0);
                when state_write_register =>
                    handle_rx <= '1';
                when state_read_register =>
                    handle_tx <= '1';
                when others => null;
            end case;       
        end if;

        case(spi_state) is
    
            when state_save_cmd =>
                spi_next_state <= state_save_address_lsb;
                cs_video_in <= '0';
                cs_brightness <= '0';
                cs_sprite <= '0';
            
            when state_save_address_lsb =>
                spi_next_state <= state_save_address_msb;
                
            when state_save_address_msb =>
                spi_next_state <= state_determine_command;
            
            when state_determine_command =>
                data_address <= to_integer(unsigned(current_address));
                data_addr_cs <= current_address;
                case(current_cmd) is             
                    when CMD_WRITE_REG =>
                        spi_state <= state_write_register;
                        spi_next_state <= state_write_register;              
                    when CMD_READ_REG =>
                        spi_state <= state_read_register;
                        spi_next_state <= state_read_register;  
                    when others => 
                        spi_next_state <= state_save_cmd; 
                end case;
                
            when state_write_register =>
                rw_flag <= '0';
                o_data_r <= rx_register;
                o_addr_r <= std_logic_vector(to_unsigned(data_address, o_addr_r'length));
                data_addr_cs <= o_addr_r;
                if(handle_rx = '1') then
                    --output data and address
                    cs_enable <= '1';
                    handle_rx <= '0';
                    --auto-increment address
                    data_address <= data_address + 1;
                else
                    cs_enable <= '0';
                end if;
                
            when state_read_register =>
                o_data_r <= (others=>'0');    
                rw_flag <= '1';
                data_addr_cs <= o_addr_r;
                if(handle_tx = '1') then
                    read_counter <= read_counter + 1;
                    case(read_counter) is
                        when 0 =>
                            --set address on bus
                            o_addr_r <= std_logic_vector(to_unsigned(data_address, o_addr_r'length));
                            cs_enable <= '1';
                        when 4 =>
                            --wait 2 CLK
                            --latch in data
                            tx_register <= i_data;
                        when 5 =>
                            --auto-increment address
                            data_address <= data_address + 1;
                            handle_tx <= '0';
                            cs_enable <= '0';
                        when others => null;
                    end case;
                else
                    read_counter <= 0;
                end if;
         
            when others =>
                null;
        end case;
    
    --set the CS based on the hardcoded addres range
    case(data_addr_cs(15 downto 12)) is   
        when RANGE_VIDEO_CONFIG =>
            cs_video_in <= '1';
        when RANGE_VIDEO_REGS =>
            cs_video_in <= '1';
        when RANGE_BRIGHTNESS =>
            cs_brightness <= '1';
        when RANGE_SPRITE_REG =>
            cs_sprite <= '1';
        when RANGE_SPRITE_BUF =>
            cs_sprite <= '1';
        when RANGE_SPRITE_RAM =>
            cs_sprite <= '1';
        when others =>
            null;
        end case;
    
    end if;

end if;

end process;

            

o_spi_tx <= o_spi_tx_r;

o_data <= o_data_r when (cs_enable = '1') else (others => '0');
o_addr <= o_addr_r when (cs_enable = '1') else (others => '0');
o_rw <= rw_flag when (cs_enable = '1') else '0'; 

o_cs_video_in <= cs_video_in when (cs_enable = '1') else '0';
o_cs_brightness <= cs_brightness when (cs_enable = '1') else '0';
o_cs_sprite <= cs_sprite when (cs_enable = '1') else '0';
 
end Behavioral;
