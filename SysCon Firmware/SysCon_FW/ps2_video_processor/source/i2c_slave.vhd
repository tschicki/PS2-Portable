----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Patrick Haid
-- 
-- Create Date: 11.04.2025 21:38:08
-- Design Name: PS2 Video Processor
-- Module Name: I2C_slave - Behavioral
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

entity I2C_slave is
    generic (
        SLAVE_ADDR       : std_logic_vector(6 downto 0)
    );
    port (
        scl_in           : in std_logic;
        scl_out          : out std_logic;
        scl_oe           : out std_logic;
        sda_in           : in std_logic;
        sda_out          : out std_logic;
        sda_oe           : out std_logic;
        mclk_in          : in    std_logic;
        rst_in           : in    std_logic
    );
end I2C_slave;

architecture Behavioral of I2C_slave is
    type state_t is (idle,
                     read_address,
                     check_address,
                     send_ack,
                     send_nack,
                     state_write,
                     state_read,
                     wait_ack,
                     read_stop);

    signal state, next_state : state_t := idle;
    signal bit_counter : integer range 0 to 8 := 0;
    signal stop_ack : std_logic := '0';
    signal rw_mode : std_logic := '0';

    signal shiftreg : std_logic_vector(7 downto 0);
    signal data_in_reg : std_logic_vector(7 downto 0) := (others => '0');

    signal scl_o_reg, scl_oe_reg  : std_logic := '0';
    signal sda_o_reg, sda_oe_reg  : std_logic := '0';

    signal sda_q, sda_qq : std_logic := '1';
    signal scl_q, scl_qq : std_logic := '1';
    signal scl_rising_reg : std_logic := '0';
    signal scl_falling_reg : std_logic := '0';

    signal start_flag : std_logic := '0';
    signal stop_flag : std_logic := '0';

    signal data_valid_reg : std_logic := '0';

begin

    edge_detection:process (mclk_in) is
    begin

        if rising_edge(mclk_in) then
            scl_q <= scl_in;
            sda_q <= sda_in;

            scl_qq <= scl_q;
            sda_qq <= sda_q;

            --detect rising and falling edges of scl
            if (scl_q = '1' and scl_qq = '0') then
                scl_rising_reg <= '1';
            else
                scl_rising_reg <= '0';
            end if;

            if (scl_q = '0' and scl_qq = '1') then
                scl_falling_reg <= '1';
            else
                scl_falling_reg <= '0';
            end if;
        end if;
    end process;

    start_stop:process (mclk_in) is
    begin
        if rising_edge(mclk_in) then
            --I2C START condition
            start_flag <= '0';
            stop_flag  <= '0';
            if (scl_q = '1' and scl_qq = '1' and sda_q = '0' and sda_qq = '1') then
                start_flag <= '1';
                stop_flag  <= '0';
            end if;

            --I2C STOP condition
            if (scl_q = '1' and scl_qq = '1' and sda_q = '1' and sda_qq = '0') then
                start_flag <= '0';
                stop_flag  <= '1';
            end if;

        end if;
    end process;


    fsm:process (mclk_in, rst_in) is
    begin

        if(rst_in = '1') then
            state <= idle;
        else
            if rising_edge(mclk_in) then

                case (state) is

                    when idle =>
                        stop_ack <= '0';
                        sda_oe_reg <= '0';
                        bit_counter <= 0;
                        --wait for the next START condition
                        if (start_flag = '1') then
                            state <= read_address;
                        end if;
                    -- read and compare slave address
                    when read_address =>

                        if (scl_rising_reg = '1') then

                            case(bit_counter) is
                                when 0 to 6 =>
                                    --shift in address
                                    shiftreg(0) <= sda_q;
                                    shiftreg(7 downto 1) <= shiftreg(6 downto 0);
                                    bit_counter <= bit_counter + 1;
                                when 7 =>
                                    --check address and get r/w bit
                                    bit_counter <= 0;
                                    if (shiftreg(6 downto 0) = SLAVE_ADDR) then
                                        state <= send_ack;

                                        if (sda_q = '1') then
                                            rw_mode <= '1';
                                            next_state <= state_read;
                                        else
                                            rw_mode <= '0';
                                            next_state <= state_write;
                                        end if;
                                    else
                                        state <= send_nack;
                                    end if;
                                when others => null;
                            end case;
                        end if;

                    -- send ack to master
                    when send_ack =>
                        if (scl_falling_reg = '1') then
                            if(stop_ack = '0') then
                                stop_ack <= '1';
                                sda_oe_reg <= '1';
                                sda_o_reg <= '0';
                            else
                                stop_ack <= '0';
                                sda_oe_reg <= '0';
                                state <= next_state;
                            end if;
                        end if;
                    
                    -- send nack to master 
                    when send_nack =>
                        if (scl_falling_reg = '1') then
                            sda_oe_reg <= '0';
                            state <= idle;
                        end if;

                    -- write: get data from master
                    when state_write =>
                        if (scl_rising_reg = '1') then

                            case(bit_counter) is
                                when 0 to 7 =>
                                    --shift in data
                                    shiftreg(0) <= sda_q;
                                    shiftreg(7 downto 1) <= shiftreg(6 downto 0);
                                    bit_counter <= bit_counter + 1;

                                    if(bit_counter = 7) then
                                        data_in_reg <= shiftreg;
                                        data_valid_reg <= '1';
                                        bit_counter <= 0;
                                        state <= send_ack;
                                    end if;
                                when others => null;
                            end case;
                        end if;

                    -- read: send data to master
                    when state_read =>
                        sda_oe_reg <= '1';

                        if scl_falling_reg = '1' then

                            case(bit_counter) is
                                when 0 to 7 =>
                                    --shift out data
                                    sda_o_reg <= shiftreg(7);
                                    shiftreg(7 downto 1) <= shiftreg(6 downto 0);
                                    bit_counter <= bit_counter + 1;
                                    if(bit_counter = 7) then
                                        bit_counter <= 0;
                                        sda_oe_reg <= '0';
                                        state <= wait_ack;
                                    end if;
                                when others => null;
                            end case;
                        end if;

                    -- read: wait for ack from master
                    when wait_ack =>
                        if scl_rising_reg = '1' then
                            if sda_q = '1' then
                                state <= read_stop;
                            else
                                state <= next_state;
                            end if;
                        end if;

                    -- Wait for START or STOP
                    when read_stop =>
                        null;

                    -- Wait for START or STOP
                    when others =>
                        state <= idle;
                end case;

                -- Reset counter and state on start/stop condition
                if start_flag = '1' then
                    state <= read_address;
                    bit_counter <= 0;
                end if;

                if stop_flag = '1' then
                    state <= idle;
                    bit_counter <= 0;
                end if;

            end if;
        end if;
    end process;


    sda_out <= sda_o_reg;
    sda_oe <= sda_oe_reg;

    scl_out <= scl_o_reg;
    scl_oe <= scl_oe_reg;

end architecture Behavioral;