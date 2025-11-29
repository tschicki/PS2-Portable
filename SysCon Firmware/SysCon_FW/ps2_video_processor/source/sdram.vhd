----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Patrick Haid
-- 
-- Create Date: 11.04.2025 21:38:08
-- Design Name: PS2 Video Processor
-- Module Name: sdram_controller - Behavioral
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


entity sdram_controller is

generic(
------------delays for 166MHz input CLK----------------------------------------
FREQ                : integer range 0 to 200 := 100;
DLY_STARTUP         : integer range 0 to 20000 := 17000; --100us
REFRESH_TIME        : integer := 2616;
--------commands---------------------------------------------------------------
--sequence is CKE, CS, RAS, CAS, WE , BA1, BA0, A10 from left to right, as in dds
CMD_NOP             : std_logic_vector(7 downto 0) := "10111000";
CMD_BST             : std_logic_vector(7 downto 0) := "10110000";
CMD_READ            : std_logic_vector(7 downto 0) := "10101000";
CMD_READA           : std_logic_vector(7 downto 0) := "10101001";
CMD_WRITE           : std_logic_vector(7 downto 0) := "10100000";
CMD_WRITEA          : std_logic_vector(7 downto 0) := "10100001";
CMD_ACT             : std_logic_vector(7 downto 0) := "10011000";
CMD_PRE             : std_logic_vector(7 downto 0) := "10010000";
CMD_PALL            : std_logic_vector(7 downto 0) := "10010001";
CMD_REF             : std_logic_vector(7 downto 0) := "10001000";
CMD_SELF            : std_logic_vector(7 downto 0) := "10001000";
CMD_MRS             : std_logic_vector(7 downto 0) := "10000000";
------------MODE REGISTER------------------------------------------------------
BURST_LENGTH_1      : std_logic_vector(2 downto 0) := "000";
BURST_LENGTH_2      : std_logic_vector(2 downto 0) := "001";
BURST_LENGTH_4      : std_logic_vector(2 downto 0) := "010";
BURST_LENGTH_8      : std_logic_vector(2 downto 0) := "011";
BURST_TYPE_SEQ      : std_logic := '0';
BURST_TYPE_INTER    : std_logic := '1';
CAS_LATENCY_2       : std_logic_vector(2 downto 0) := "010";
CAS_LATENCY_3       : std_logic_vector(2 downto 0) := "011";
OPMODE_STD          : std_logic_vector(1 downto 0) := "00";
BURST_MODE_PROG     : std_logic := '0';
BURST_MODE_SINGLE   : std_logic := '1';
MODE_RESERVED       : std_logic_vector(3 downto 0) := "0000"
);

Port ( 
       --interface to SDRAM
       sdram_address :    out std_logic_vector(11 downto 0);
       sdram_bank_sel :   out std_logic_vector(1 downto 0);
       sdram_cas :        out std_logic;
       sdram_cke :        out std_logic;
       --sdram_clk :        out std_logic;
       sdram_cs :         out std_logic;
       sdram_dqm :        out std_logic_vector(3 downto 0);
       --wrapper ports for bidirectional buffer - only for Trion
       sdram_datin :    in std_logic_vector(31 downto 0);
       sdram_datout :   out std_logic_vector(31 downto 0);
       sdram_datoe :    out std_logic_vector(31 downto 0);
       --sdram_data :    inout std_logic_vector(31 downto 0);
       sdram_ras :        out std_logic;
       sdram_we :         out std_logic;
       --user access port
       in_addr :          in std_logic_vector(21 downto 0);
       in_data :          in std_logic_vector(31 downto 0);
       out_data :         out std_logic_vector(31 downto 0);
       in_rst_0 :           in std_logic;   --reset memory controller
       in_rst_1 :           in std_logic;   --reset memory controller
       in_rw :            in std_logic;   --0 -> read; 1 -> write
       in_cke :           in std_logic;    --1 indicates a pending a read or write; needs to be reset by the controller when in_valid or out_valid turn high
       in_dqm:            in std_logic_vector(3 downto 0);
       --status outputs
       out_data_valid :   out std_logic;  --high when read data is valid/ready; indicate to shift out the data with falling edge of clk
       busy :             out std_logic;   --high when the controller is busy executing a cmd
       --clock input
       in_clk :           in std_logic;   --clkin is 100MHz
       in_clk_dly:        in std_logic --delayed input clkin to read data from the sdram
);
end sdram_controller;

architecture Behavioral of sdram_controller is

type state_type is (
                    state_init,
                    state_delay,
                    state_precharge_init, 
                    state_mode_reg,
                    state_idle, 
                    state_activate, 
                    state_write, 
                    state_read, 
                    state_read_res, 
                    state_precharge, 
                    state_refresh,
                    state_check_row);
--_q is synchronized with the falling edge of CLK, _d is asynchronous
signal state, next_state, state_queue : state_type := state_init;

-------------------------------------------------------------------------------
signal MODE_REG : std_logic_vector(13 downto 0) := MODE_RESERVED &
                        BURST_MODE_PROG &
                        OPMODE_STD & 
                        CAS_LATENCY_3 &
                        BURST_TYPE_SEQ &
                        BURST_LENGTH_8;
                        
type open_row_addr_array is array (3 downto 0) of std_logic_vector(11 downto 0);
signal open_row_addr_q : open_row_addr_array := (others=>(others=>'0'));
signal open_row_flag_q : std_logic_vector(3 downto 0) := "0000";
signal current_open_row_addr : std_logic_vector(11 downto 0) := (others=>'1');
signal current_open_row_flag : std_logic := '0';
--SDRAM interface signals
signal sdram_cmd_q : std_logic_vector(7 downto 0) := CMD_NOP;
signal sdram_read_en_d, sdram_read_en_q  : std_logic := '0';
signal sdram_data_in_q, sdram_data_in_d : std_logic_vector(31 downto 0) := x"00000000";
signal sdram_data_out_q: std_logic_vector(31 downto 0) := x"00000000";
signal sdram_addr_out_q : std_logic_vector(11 downto 0) := x"000";
signal sdram_bank_sel_q : std_logic_vector(1 downto 0) := "00";
signal sdram_dqm_q : std_logic_vector(3 downto 0) := "1111";

--command and state machine signals
signal cmd_ctr : integer range 0 to 31 := 0;
signal startup_ctr : integer range 0 to 20000 := 0;
signal startup_flag : std_logic := '0';
signal precharge_bank_sel_q : std_logic_vector(2 downto 0) := "000";
signal saved_addr_q : std_logic_vector(21 downto 0) := (others=>'0');
signal saved_dqm_q : std_logic_vector(3 downto 0) := "0000";
signal wr_en_q : std_logic := '0';
signal out_valid_q : std_logic := '0';
signal pending_cmd_flag : std_logic := '0';

signal busy_q : std_logic := '1';
signal cke_reg : std_logic := '0';

signal write_0, write_1, write_2, write_3, write_4, write_5, write_6, write_7 : std_logic_vector(31 downto 0) := x"00000000";

--refresh counter signals
signal refresh_flag : std_logic := '0';
signal refresh_ctr : integer range 0 to 4095 := 0;

--data input buffer signals
signal write_count : integer range 0 to 32000 := 0;
begin

latch_data:process(in_clk_dly, in_rst_1)
begin
    if(in_rst_1 = '0') then
        sdram_data_in_d <= x"00000000";
    else
        if(falling_edge(in_clk_dly)) then
            if(sdram_read_en_q = '1') then
                sdram_data_in_d <= sdram_datin;             
            else
                sdram_data_in_d <= x"00000000";
            end if;
        end if;
    end if;
end process;

next_state_logic:process(in_clk, in_rst_0)
begin

if(in_rst_0 = '0') then
    state <= state_init;
    startup_flag <= '0';
    startup_ctr <= 0;
    refresh_ctr <= 0;
    refresh_flag <= '0';
    cmd_ctr <= 0;
    write_count <= 0;
    busy_q <= '1';
    wr_en_q <= '0';
                    
    sdram_cmd_q <= CMD_NOP;
    sdram_read_en_q <= '0';
    sdram_read_en_d <= '0';
    sdram_addr_out_q <= x"000";
    sdram_bank_sel_q <= "00";
    sdram_dqm_q <= "1111";
    out_valid_q <= '0';
                    
    open_row_addr_q <= (others=>(others=>'0'));
    open_row_flag_q <= "0000";
                    
elsif(rising_edge(in_clk)) then

                ----------startup counter-------------------------------------------
                if(startup_ctr /= DLY_STARTUP) then
                    startup_ctr <= startup_ctr + 1;
                else
                    startup_flag <= '1';
                end if;
                --lathcing in data for writing--------------------------------------
                if(in_cke = '1' and in_rw = '1') then
                    write_count <= write_count +1;
                    
                    case(write_count) is
                    when 0 => write_0 <= in_data;
                    when 1 => write_1 <= in_data;
                    when 2 => write_2 <= in_data;
                    when 3 => write_3 <= in_data;
                    when 4 => write_4 <= in_data;
                    when 5 => write_5 <= in_data;
                    when 6 => write_6 <= in_data;
                    when 7 => write_7 <= in_data;
                    when others => write_count <= 0;
                    end case;
                else
                    write_count <= 0;
                end if;
                ----------refresh counter-------------------------------------------
                if(refresh_ctr > REFRESH_TIME) then
                    refresh_ctr <= 0;
                    refresh_flag <= '1';
                else
                    refresh_ctr <= refresh_ctr + 1;
                end if;
             
                cmd_ctr <= cmd_ctr + 1;

                --latch in user inputs----------------------------------------------
                sdram_data_in_q <= sdram_data_in_d;
                sdram_read_en_q <= sdram_read_en_d;
                --reading and storing next command------------------------------------------------
                --this is not part of the state machine because it was interfering with the refresh
                --in case the refresh flag turns the moment the source wants to send a command
                cke_reg <= in_cke;
                if(cke_reg = '0' and in_cke = '1') then
                    --save values for later
                    busy_q <= '1';
                    saved_dqm_q <= in_dqm;
                    saved_addr_q <= in_addr;
                    pending_cmd_flag <= '1';
                    
                     --check whether it is a read or a write and queue it
                    if(in_rw = '1') then
                       state_queue <= state_write;
                    else
                       state_queue <= state_read;
                    end if;
                end if;
                --state machine----------------------------------------------------------------------
        case(state) is
            when state_init =>
                ---startup delay, only send NOP commands
                if(startup_flag = '1') then
                    state <= state_precharge_init;
                    cmd_ctr <= 0;
                    sdram_cmd_q <= CMD_NOP;
                else
                    sdram_cmd_q <= CMD_NOP;
                end if;

            when state_precharge_init =>
                case(cmd_ctr) is
                    when 0 =>
                        sdram_cmd_q <= CMD_PALL;
                        sdram_addr_out_q <= '0' & CMD_PALL(0) & "0000000000";
                        sdram_bank_sel_q <= CMD_PALL(2 downto 1);
                    when 3 =>
                        sdram_cmd_q <= CMD_REF;
                    when 13 =>
                        sdram_cmd_q <= CMD_REF;
                    when 22 =>
                        sdram_cmd_q <= CMD_NOP;
                        state <= state_mode_reg;
                        cmd_ctr <= 0;
                    when others =>
                        sdram_cmd_q <= CMD_NOP;
                end case;

            when state_mode_reg =>
                case(cmd_ctr) is
                    when 0 =>
                        sdram_cmd_q <= CMD_MRS;
                        sdram_addr_out_q <= MODE_REG(11 downto 0);
                        sdram_bank_sel_q <= MODE_REG(13 downto 12);
                    when 2 =>
                        sdram_cmd_q <= CMD_NOP;
                        state <= state_idle;
                        cmd_ctr <= 0;
                    when others =>
                        sdram_cmd_q <= CMD_NOP;
                end case;

            when state_activate =>
                case(cmd_ctr) is
                    when 0 =>
                        sdram_cmd_q <= CMD_ACT;
                        sdram_addr_out_q <= saved_addr_q(19 downto 8); -- set address bits 11 to 0
                        sdram_bank_sel_q <= saved_addr_q(21 downto 20);
                    when 2 =>   --this timing may be improved
                        cmd_ctr <= 0;
                        sdram_cmd_q <= CMD_NOP;
                        --next state is either read or write
                        state <= state_queue;
                        --row is now open
                        open_row_flag_q(to_integer(unsigned(saved_addr_q(21 downto 20)))) <= '1';
                        open_row_addr_q(to_integer(unsigned(saved_addr_q(21 downto 20)))) <= saved_addr_q(19 downto 8);
                    when others =>
                        sdram_cmd_q <= CMD_NOP;
                end case;

            when state_idle =>
                if(refresh_flag = '1') then
                    state <= state_precharge;
                    next_state <= state_refresh;
                    cmd_ctr <= 0;
                    refresh_flag <= '0';
                    precharge_bank_sel_q <= "100"; -- precharge all banks
                    busy_q <= '1';
                elsif(pending_cmd_flag = '1') then
                    --address:
                    --21 and 20 -> bank
                    --19 to 8 -> row
                    --7 to 0 -> column
                    --the following 2 regs are separated out from the state machine to here, to improve timings
                    --current_open_row_flag <= open_row_flag_q(to_integer(unsigned(in_addr(21 downto 20))));
                    current_open_row_addr <= open_row_addr_q(to_integer(unsigned(in_addr(21 downto 20))));
                       
                    if(open_row_flag_q(to_integer(unsigned(in_addr(21 downto 20)))) = '1') then
                        state <= state_check_row;
                        cmd_ctr <= 0;
                    else
                        state <= state_activate;
                        cmd_ctr <= 0;
                    end if;
                elsif(in_cke = '0') then busy_q <= '0';
                end if;
            when state_check_row =>
                
                if(current_open_row_addr = saved_addr_q(19 downto 8)) then
                    -- the row is open already
                    state <= state_queue;
                    cmd_ctr <= 0;
                else 
                    --different row is open
                    state <= state_precharge;
                    cmd_ctr <= 0;
                    precharge_bank_sel_q <= '0' & saved_addr_q(21 downto 20); -- selects the current bank to close the other open row
                    next_state <= state_activate; -- activate the new row to perform operation
                end if;
            when state_refresh =>
                case(cmd_ctr) is
                    when 0 =>
                        sdram_cmd_q <= CMD_REF;
                        sdram_addr_out_q <= '0' & CMD_REF(0) & "0000000000";
                        sdram_bank_sel_q <= CMD_REF(2 downto 1);
                    when 7 =>
                        sdram_cmd_q <= CMD_NOP;
                        state <= state_idle;
                        cmd_ctr <= 0;
                        --busy_q <= '0';
                    when others =>
                        sdram_cmd_q <= CMD_NOP;
                end case;

            when state_precharge =>
                case(cmd_ctr) is
                    when 0 =>
                        sdram_cmd_q <= CMD_PRE;
                        sdram_addr_out_q <= '0' & precharge_bank_sel_q(2) & "0000000000";
                        sdram_bank_sel_q <= precharge_bank_sel_q(1 downto 0);
                    when 2 =>
                        state <= next_state;
                        cmd_ctr <= 0;
                        sdram_cmd_q <= CMD_NOP;
                    when others =>
                        sdram_cmd_q <= CMD_NOP;
                end case;

                if(precharge_bank_sel_q(2) = '1') then
                    --all rows were closed
                    open_row_flag_q <= "0000";
                else
                    --one row was closed
                    open_row_flag_q(to_integer(unsigned(precharge_bank_sel_q(1 downto 0)))) <= '0';
                end if;

            when state_write =>
                case(cmd_ctr) is
                    when 0 =>
                        sdram_cmd_q <= CMD_WRITE;
                        sdram_addr_out_q <= '0' & '0' & "00" & saved_addr_q(7 downto 0); -- set address bits 11 to 0
                        sdram_bank_sel_q <= saved_addr_q(21 downto 20);
                        sdram_data_out_q <= write_0;
                        wr_en_q <= '1';
                        sdram_dqm_q <= saved_dqm_q;

                    when 1 => 
                        sdram_data_out_q <= write_1;
                        sdram_cmd_q <= CMD_NOP;
                    when 2 => sdram_data_out_q <= write_2;
                    when 3 => sdram_data_out_q <= write_3;
                    when 4 => sdram_data_out_q <= write_4;
                    when 5 => sdram_data_out_q <= write_5;
                    when 6 => sdram_data_out_q <= write_6;
                              --busy_q <= '0';
                    when 7 => sdram_data_out_q <= write_7;     
                        --latch in the 8 data transfers
                    when 8 =>
                        state <= state_idle;
                        cmd_ctr <= 0;
                        wr_en_q <= '0';
                        sdram_dqm_q <= "1111";
                        sdram_data_out_q <= x"00000000"; 
                        pending_cmd_flag <= '0';
                    when others =>
                        sdram_cmd_q <= CMD_NOP;
                end case;

            when state_read =>
                case(cmd_ctr) is
                    when 0 =>
                        sdram_cmd_q <= CMD_READ;
                        sdram_addr_out_q <= '0' & '0' & "00" & saved_addr_q(7 downto 0); -- set address bits 11 to 0
                        sdram_bank_sel_q <= saved_addr_q(21 downto 20);
                        sdram_dqm_q <= saved_dqm_q;
                    when 4 => --was 3
                        --set data latch en to true
                        --this will latch in the data
                        sdram_read_en_d <= '1';
                    when 6 => -- was 5
                        out_valid_q <= '1';
                    when 12 => -- was 11
                        --latch in the 8 data transfers
                        sdram_read_en_d <= '0';
                    when 14 => -- was 13
                        sdram_dqm_q <= "1111";
                        state <= state_idle;
                        cmd_ctr <= 0;
                        out_valid_q <= '0';
                        pending_cmd_flag <= '0';
                        --busy_q <= '0';
                    when others =>
                        sdram_cmd_q <= CMD_NOP;
                end case;

            when others => 
                state <= state_init;
                cmd_ctr <= 0;
        end case; 
   end if;
end process;
--writing outputs--------------
sdram_cas <= sdram_cmd_q(4);
sdram_ras <= sdram_cmd_q(5);
sdram_we <= sdram_cmd_q(3);
sdram_cs <= sdram_cmd_q(6);
sdram_cke <= sdram_cmd_q(7);
sdram_bank_sel <= sdram_bank_sel_q;
sdram_address <= sdram_addr_out_q;
sdram_dqm <= sdram_dqm_q;
--sdram_data <= sdram_data_out_q when (wr_en_q = '1') else
--        (others=>'Z'); 
sdram_datout <= sdram_data_out_q;
sdram_datoe <= (others=>wr_en_q);
busy <= busy_q;
out_data <= sdram_data_in_q;
out_data_valid <= out_valid_q;
--sdram_data_in_a <= sdram_datin;

end architecture;


--166MHz clock -> 6ns
--for 8 burst
--precharge - activate - read/write
-- 3clk   - 3clk - 3clk + 8clk        17clk per 32byte (24byte) 102ns

--for single read/write
-- 3clk   - 3clk - 3clk + 1clk        10clk per 4byte (3byte) 60ns
 
-- 4000 refresh every 64ms
-- precharge - refresh - refresh - active
-- 3clk - 10clk - 10clk - 3clk        26clk * 62500 per second    
 
-- 1 refresh every 16us -> can read/write 155 * 24 bytes OR 155*8 pixels in 16us OR 13ns per pixel
 
-- one pixel of the video out takes 37ns (progressive. worst case) -> 640 pixels take ~23us
 
-- a refresh takes 156ns + 1 possible write 102ns -> at least 7 pixels need to be bridged by the buffer
