----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Patrick Haid
-- 
-- Create Date: 16.02.2025 19:44:16
-- Design Name: PS2 Video Processor
-- Module Name: Sprite_Module - Behavioral
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


--8 bit wide sprite ram:
--sprite data #0 (1 bit per pixel)
--sprite data #1
--...

-- 8 bytes per sprite * 256 sprites = 2048 bytes in total

--Sprites
    --Color A 16 bit
    --Color B 16 bit
    
    --cursor x 16 bit (12 bit)
    --cursor y 16 bit (10 bit)
    
    --sprite#  16 bit
    
--sprite buffer
    --posX LSB
    --posX MSB
    --posY LSB
    --posY MSB
    --address
    
---> 3x block RAM: 1x 8 bit address; 1x position: 12 bit X + 10 bit Y; 12 bit color A + 12 bit color B
    --8+12+10+24 bits wide * 1024 sprites = 6912 bytes of BRAM

--firmware will need to remember the address in which the first element of each entity was written to, in order to erase it later
--should be fine, if the length of all elements is static, otherwise this could not work -> CHECK!!!

entity Sprite_Module is
Port (
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
end Sprite_Module;

architecture Behavioral of Sprite_Module is

component efx_simple_dual_port_2_clk_ram is
  generic (
    RAM_WIDTH : integer := 24;
    RAM_DEPTH : integer := 1024;
    OUTREG    : boolean := true
    );
  Port (
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


type state_type_write is (state_wait, 
                        state_analyze, 
                        state_write_regs,
                        state_write_buffer,
                        state_write_ram,
                        state_increment_position,
                        state_save_position
                        );
                    
type state_type_read is (state_wait, 
                        state_read_buffer,
                        state_check_empty,
                        state_check_overlap,
                        state_wait_for_drawing
                        );
                             
type state_type_draw is (state_wait, 
                        state_read_sprite,
                        state_draw_sprite
                        );
                                                            
signal write_state : state_type_write := state_wait;
signal read_state : state_type_read := state_wait;
signal draw_state : state_type_draw := state_wait;

type t_registers is array (10 downto 0) of std_logic_vector(7 downto 0);
signal register_array : t_registers;

signal current_data : std_logic_vector(7 downto 0) := (others=>'0');
signal current_addr : std_logic_vector(15 downto 0) := (others=>'0');

signal spritebuffer_data_wr, spritebuffer_data_re : std_logic_vector(53 downto 0) := (others=>'0');
signal spritebuffer_addr_wr, spritebuffer_addr_re : std_logic_vector(9 downto 0) := (others=>'0');
signal spritebuffer_we, spritebuffer_re : std_logic := '0';

signal spriteram_data_wr, spriteram_data_re : std_logic_vector(7 downto 0) := (others=>'0');
signal spriteram_addr_wr, spriteram_addr_re : std_logic_vector(10 downto 0) := (others=>'0');
signal spriteram_we, spriteram_re : std_logic := '0';
signal spriteram_addr_re_int : integer range 0 to 2048 := 0;

signal posX_wr, posY_wr, new_posX_wr, colorA_wr, colorB_wr : std_logic_vector(15 downto 0) := (others=>'0');
signal posX_int : integer range 0 to 1023 := 0;

signal read_addr : integer range 0 to 1024 := 0;

signal posX_re, posX_sec : integer range 0 to 1024 := 0;
signal posY_re, posY8_re : integer range 0 to 1024 := 0;
signal line_count : integer range 0 to 1024 := 0;
signal cur_sprite_offset : std_logic_vector(10 downto 0) := (others=>'0');
signal colorA_re, colorB_re, colorA_sec, colorB_sec : std_logic_vector(11 downto 0) := (others=>'0');

signal sec_fsm_start, sec_busy : std_logic := '0';

signal data_out : std_logic_vector(23 downto 0) := (others=>'0');             
signal addr_out : std_logic_vector(9 downto 0) := (others=>'0');
signal we_out : std_logic := '0';
signal sprite_line_to_draw : std_logic_vector(7 downto 0) := (others=>'0');
signal bit_counter, bit_sel : integer range 0 to 63 := 0;

signal we_end : std_logic := '0';
signal sprite_counter : integer range 0 to 63 := 0;
signal sprite_start, we_in1, we_in2 : std_logic := '0';
signal sprite_to_draw_flag : std_logic := '0';

signal scaling_factor, sprite_size : integer range 0 to 255 := 0;
signal sprite_line : integer range 0 to 255 := 0;

signal delay : std_logic := '0';

constant sprite_size_base : unsigned(7 downto 0) := "00001000";
begin

sprite_ram: efx_simple_dual_port_2_clk_ram
  generic map (RAM_WIDTH => 8, RAM_DEPTH => 2048, OUTREG => true) 
  port map (
    addra  => spriteram_addr_wr,
    addrb  => std_logic_vector(to_unsigned(spriteram_addr_re_int, 11)),
    dina   => spriteram_data_wr,
    clka   => i_clk,
    clkb   => i_clk,
    wea    => spriteram_we,
    enb    => spriteram_re,
    rstb   => i_rst_n,
    regceb => spriteram_re,
    doutb  => spriteram_data_re);

spritebuffer_ram: efx_simple_dual_port_2_clk_ram
  generic map (RAM_WIDTH => 30, RAM_DEPTH => 1024, OUTREG => true) 
  port map (
    addra  => spritebuffer_addr_wr,
    addrb  => spritebuffer_addr_re,
    dina   => spritebuffer_data_wr(53 downto 24),
    clka   => i_clk,
    clkb   => i_clk,
    wea    => spritebuffer_we,
    enb    => spritebuffer_re,
    rstb   => i_rst_n,
    regceb => spritebuffer_re,
    doutb  => spritebuffer_data_re(53 downto 24));

spritebuffer_color: efx_simple_dual_port_2_clk_ram
  generic map (RAM_WIDTH => 24, RAM_DEPTH => 1024, OUTREG => true) 
  port map (
    addra  => spritebuffer_addr_wr,
    addrb  => spritebuffer_addr_re,
    dina   => spritebuffer_data_wr(23 downto 0),
    clka   => i_clk,
    clkb   => i_clk,
    wea    => spritebuffer_we,
    enb    => spritebuffer_re,
    rstb   => i_rst_n,
    regceb => spritebuffer_re,
    doutb  => spritebuffer_data_re(23 downto 0));

--muxing the scaler lines and sprite lines for the linebuffer
o_we <= i_we when (we_out = '0') else
        we_out;                            
o_data <= i_data when (we_out = '0') else 
          data_out;            
o_addr <= i_addr when (we_out = '0') else 
          addr_out;    

--this FSM takes care of write accesses to the registers and ram
sprite_write:process(i_clk, i_rst_n)
begin

--if(i_rst_n = '0') then
--this should actually not reset, what if the syscon is writing something while a reset occurs?
--else
    if(rising_edge(i_clk)) then
    
    --the 2 variables required to scale the sprites properly vertically
    scaling_factor <= to_integer(unsigned(register_array(REG_SCALE)));
    sprite_size <= to_integer(unsigned(shift_left(sprite_size_base,scaling_factor)));
    
    --if cs is high and rw low
    --latch in the data and switch states to write
    --if writing position or color, just pipe it through
    --if writing sprite ram, just pipe it through
    --if writing spritebuffer, increment position after each write and write the current color and position in addition
    case(write_state) is
    when state_wait =>
        spriteram_we <= '0';
        spritebuffer_we <= '0';
        --wait for cs high
        if(i_cs_config = '1' and i_rw_config = '0') then
            current_data <= i_data_config;
            current_addr <= i_addr_config;
            write_state <= state_analyze;
        end if;
        
    when state_analyze =>
        --check which ram is accessed
        --0: reserved
        --1 to 10 -> pos and color registers
        --11 to 1034 -> spritebuffer
        --1035 to 3082 -> sprite ram
        case(current_addr(15 downto 12)) is
            when RANGE_SPRITE_REG =>
                write_state <= state_write_regs;
            when RANGE_SPRITE_BUF =>
                posX_wr <= register_array(REG_POSX_MSB) & register_array(REG_POSX_LSB);
                posY_wr <= register_array(REG_POSY_MSB) & register_array(REG_POSY_LSB);
                colorA_wr <= register_array(REG_COLORA_MSB) & register_array(REG_COLORA_LSB);
                colorB_wr <= register_array(REG_COLORB_MSB) & register_array(REG_COLORB_LSB);
                write_state <= state_write_buffer;
            when RANGE_SPRITE_RAM =>
                write_state <= state_write_ram;
            when others =>
                write_state <= state_wait;
        end case;
        
    when state_write_regs =>
        register_array(to_integer(unsigned(current_addr(4 downto 0)))) <= current_data;
        write_state <= state_wait;
        
    when state_write_buffer =>
        spritebuffer_data_wr <= current_data & posX_wr(11 downto 0) & posY_wr(9 downto 0) & colorA_wr(11 downto 0) & colorB_wr(11 downto 0);
        spritebuffer_addr_wr <= current_addr(9 downto 0);
        spritebuffer_we <= '1';
        posX_int <= to_integer(unsigned(posX_wr)) + sprite_size;
        write_state <= state_increment_position;
        
    when state_write_ram =>
        spriteram_data_wr <= current_data;
        spriteram_addr_wr <= current_addr(10 downto 0);
        spriteram_we <= '1';
        write_state <= state_wait;
    
    when state_increment_position =>
        spritebuffer_we <= '0';
        new_posX_wr <= std_logic_vector(to_unsigned(posX_int, new_posX_wr'length));
        write_state <= state_save_position;
        
    when state_save_position =>    
        register_array(REG_POSX_LSB) <= new_posX_wr(7 downto 0);
        register_array(REG_POSX_MSB) <= new_posX_wr(15 downto 8);
        write_state <= state_wait;
        
    when others => null;
    end case;
    
    end if;
--end if;

end process;

--this FSM takes care of iterating through the spritebuffer and reading its contents
sprite_read:process(i_clk, i_rst_n)
begin  

if(i_rst_n = '0') then
    read_state <= state_wait;
    spritebuffer_re <= '0';
else
    if(rising_edge(i_clk)) then
    
        we_in1 <= i_we;
        we_in2 <= we_in1;
        
        if(we_in2 = '1' and we_in1 = '0') then
            sprite_start <= '1';
        else
            sprite_start <= '0';
        end if; 
        
        if(posY8_re >= i_line_count and i_line_count > posY_re) then
            sprite_to_draw_flag <= '1';
        else
            sprite_to_draw_flag <= '0';
        end if;
        
        
        line_count <= i_line_count - 1; 
        
        case(read_state) is
        
        when state_wait =>
            delay <= '0';
            read_addr <= 0;
            sec_fsm_start <= '0';
            spritebuffer_re <= '1';
            sprite_counter <= 0;
            if(sprite_start = '1') then
                --wait for trigger from scaler
                read_state <= state_read_buffer;
            end if;
        when state_read_buffer =>
            sec_fsm_start <= '0';
            
            cur_sprite_offset <= spritebuffer_data_re(53 downto 46) & "000";
            posX_re <= to_integer(unsigned(spritebuffer_data_re(45 downto 34)));
            posY_re <= to_integer(unsigned(spritebuffer_data_re(33 downto 24)));
            posY8_re <= to_integer(unsigned(spritebuffer_data_re(33 downto 24))) + sprite_size;
            colorA_re <= spritebuffer_data_re(23 downto 12);
            colorB_re <= spritebuffer_data_re(11 downto 0);
            sprite_line <= ABS(line_count - to_integer(unsigned(spritebuffer_data_re(33 downto 24)))); --line_count is missing the -1
            
            delay <= '1';
            if(delay = '1') then
                read_addr <= read_addr + 1;
                read_state <= state_check_empty; 
            end if;
            --limit of sprites to be drawn on the screen PER LINE
            --if exceeded, might lead to undefined baaviour
            if(sprite_counter = 50) then
                read_state <= state_wait;           
            end if;
                      
        when state_check_empty =>
            delay <= '0';
            --address to read = address + (ABS(line#-(posY-8))) (offset given by address in spritebuffer + offset within the sprite data)
            --for 16 pixel sprites (2x scaling): (line_count - posY_re)>>factor -> factor would be 1 in this case
            --leads to : (line_count - posY_re)>>factor
            spriteram_addr_re_int <= to_integer(unsigned(cur_sprite_offset)) + to_integer(shift_right(to_unsigned(sprite_line, 8),scaling_factor));
            
            if(read_addr < 512) then
                if(cur_sprite_offset = "000000000000" or cur_sprite_offset = "UUUUUUUUUUUU") then
                    --no sprite to draw
                    read_state <= state_read_buffer;
                else        
                    read_state <= state_check_overlap;
                end if;
            else
                read_state <= state_wait;
            end if;
            
        when state_check_overlap =>
            --if data at address != 0 and if posY >= line# and line# >= (posY-8), then the sprite must be displayed
            if(sprite_to_draw_flag = '1') then
                --sprite to draw
                if(sec_busy = '0') then
                    --calculate the address in the sprite ram:
                    sec_fsm_start <= '1';
                    sprite_counter <= sprite_counter + 1;
                    read_state <= state_read_buffer;
                end if;
            else
                --no overlap, so no need to draw
                read_state <= state_read_buffer;
            end if;
  
        when others =>
            null;
        end case;
    
    end if;
end if;

end process;

--read an address from the spritebuffer
spritebuffer_addr_re <= std_logic_vector(to_unsigned(read_addr, spritebuffer_addr_re'length));

--this FSM takes care of reading the corresponding sprite ra entry and drawing it into the line buffer
sprite_draw:process(i_clk, i_rst_n)
begin

if(i_rst_n = '0') then
    spriteram_re <= '0';
    we_end <= '0';
else
    if(rising_edge(i_clk)) then
        case(draw_state) is
            when state_wait =>
                spriteram_re <= '1';
                we_end <= '0';
                bit_counter <= 1;
                bit_sel <= 0;
                --wait for command from primary state machine
                if(sec_fsm_start = '1') then
                    
                    draw_state <= state_read_sprite;
                    sec_busy <= '1';
                    --we store these here, so the read fsm can continue reading without disturbing the sprite drawing
                    --spriteram_addr_re <= std_logic_vector(to_unsigned(spriteram_addr_re_int, spriteram_addr_re'length));
                    posX_sec <= posX_re;
                    colorA_sec <= colorA_re;
                    colorB_sec <= colorB_re;
                else
                    sec_busy <= '0';
                end if;
                
            when state_read_sprite=>
                --buffer the sprite data
                sprite_line_to_draw <= spriteram_data_re;
                draw_state <= state_draw_sprite;

            when state_draw_sprite=>
                --calculate the horizontal offset where the sprite should be displayed:
                --h_pixel = from posX + bit_counter to posX + 8
                posX_sec <= posX_sec + 1;
                addr_out <= std_logic_vector(to_unsigned(posX_sec, addr_out'length));
                we_out <= '1';
                
                --write the applicable data from the color register into the line buffer until horizontal width of the sprite was reached 
                if(sprite_line_to_draw(bit_sel) = '1') then
                    data_out <= colorA_sec(11 downto 8) & "0000" & colorA_sec(7 downto 4) & "0000" & colorA_sec(3 downto 0) & "0000";
                else
                    data_out <= colorB_sec(11 downto 8) & "0000" & colorB_sec(7 downto 4) & "0000" & colorB_sec(3 downto 0) & "0000";
                end if;
                
                if(bit_counter < sprite_size) then
                    bit_counter <= bit_counter + 1;
                    bit_sel <= to_integer(shift_right(to_unsigned(bit_counter, 8),scaling_factor));
                else
                    we_end <= '1';
                end if;
                
                if(we_end = '1') then
                    we_out <= '0';
                    sec_busy <= '0';
                    draw_state <= state_wait;
                end if;
            
            when others => 
                null;
        
        end case;      
    end if;
end if;
end process;

end Behavioral;

--state machine for sprite drawing:

--iterate through all addresses in the sprite buffer
    --if data at address != 0, combine the address LSB and MSB and send the address to the sprite ram
        --Read X size and Y size
        --if posY >= line# and line# >= (posY-sizeY), then the sprite must be displayed
            --calculate the address in the sprite ram:
                --address to read = 1 + address + (ABS(line#-(posY-sizeY))*sizeX)
                --(1 byte offset for size + offset given by address in spritebuffer + offset within the sprite data(sizeX determines hoy many multiples of 8 the sprite is wide))
            --buffer the sprite data
            --calculate the horizontal offset where the sprite should be displayed:
                --h_pixel = from posX + pixel counter (increments after each pixel) to posX + (sizeX*8)
            --write the applicable data from the color register into the line buffer until horizontal width of the sprite was reached
            --read next address in spritebuffer
    --else read the next address in spritebuffer
--when done, wait until the next line was written  