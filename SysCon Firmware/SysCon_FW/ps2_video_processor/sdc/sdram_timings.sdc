
# Efinity Interface Designer SDC
# Version: 2020.M.39
# Date: 2020-04-01 17:51

# Copyright (C) 2017 - 2020 Efinix Inc. All rights reserved.

# Device: T20F256
# Project: sdram_controller_test
# Timing Model: C4 (final)

#outer layer signal speed: 58ps/cm -> MAX delay at 23mm length is 133ps
#inner layer signal speed: 71ps/cm -> MAX delay at 23mm length is 156ps
#clock signal delay is 123ps

# PLL Constraints
#################
#create_clock -period 6.00 PLL_CLKOUT_0
#create_clock -period 6.00 PLL_CLKOUT_1
#create_clock -period 46.66 PLL_CLKOUT_2
#create_clock -period 18.5 clkin

create_clock -period 6.00 PLL_CLKOUT_0 -waveform {0.000 3.00}
create_clock -period 6.00 PLL_CLKOUT_1 -waveform {1.5 4.5}
create_clock -period 46.66 PLL_CLKOUT_2
create_clock -period 18.5 clkin

#output MINMAX
#MAX = -(clock period / 2 + input hold time);  negative because it happens after the falling clk
#MIN = -(clock period / 2 - input setup time);

#false path for CDC
set_false_path -from [get_clocks {PLL_CLKOUT_0}] -to [get_clocks {PLL_CLKOUT_2}]
set_false_path -from [get_clocks {PLL_CLKOUT_0}] -to [get_clocks {clkin}]
set_false_path -from [get_clocks {PLL_CLKOUT_2}] -to [get_clocks {PLL_CLKOUT_0}]
set_false_path -from [get_clocks {clkin}] -to [get_clocks {PLL_CLKOUT_0}]
set_false_path -from [get_clocks {clkin}] -to [get_clocks {PLL_CLKOUT_1}]
set_false_path -from [get_clocks {clkin}] -to [get_clocks {PLL_CLKOUT_2}]

#input MINMAX
#MAX = tOH + clock period (166MHz) / 2 + 2 x trace delay
#MIN = tAC - clock period (166MHz) / 2 - 2 * trace delay

# GPIO Constraints
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_address[0]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_address[0]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_address[1]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_address[1]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_address[2]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_address[2]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_address[3]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_address[3]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_address[4]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_address[4]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_address[5]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_address[5]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_address[6]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_address[6]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_address[7]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_address[7]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_address[8]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_address[8]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_address[9]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_address[9]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_address[10]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_address[10]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_address[11]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_address[11]}]

set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_bank_sel[0]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_bank_sel[0]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_bank_sel[1]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_bank_sel[1]}]

set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_dqm[0]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_dqm[0]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_dqm[1]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_dqm[1]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_dqm[2]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_dqm[2]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_dqm[3]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_dqm[3]}]

set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_cas}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_cas}]

set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_cke}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_cke}]

set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_cs}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_cs}]

set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_ras}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_ras}]

set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_we}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_we}]


set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[0]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[0]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[0]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[0]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[0]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[0]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[1]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[1]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[1]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[1]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[1]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[1]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[2]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[2]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[2]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[2]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[2]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[2]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[3]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[3]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[3]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[3]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[3]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[3]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[4]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[4]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[4]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[4]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[4]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[4]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[5]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[5]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[5]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[5]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[5]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[5]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[6]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[6]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[6]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[6]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[6]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[6]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[7]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[7]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[7]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[7]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[7]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[7]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[8]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[8]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[8]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[8]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[8]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[8]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[9]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[9]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[9]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[9]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[9]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[9]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[10]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[10]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[10]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[10]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[10]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[10]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[11]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[11]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[11]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[11]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[11]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[11]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[12]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[12]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[12]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[12]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[12]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[12]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[13]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[13]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[13]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[13]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[13]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[13]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[14]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[14]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[14]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[14]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[14]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[14]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[15]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[15]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[15]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[15]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[15]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[15]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[16]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[16]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[16]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[16]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[16]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[16]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[17]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[17]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[17]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[17]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[17]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[17]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[18]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[18]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[18]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[18]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[18]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[18]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[19]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[19]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[19]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[19]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[19]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[19]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[20]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[20]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[20]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[20]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[20]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[20]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[21]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[21]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[21]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[21]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[21]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[21]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[22]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[22]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[22]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[22]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[22]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[22]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[23]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[23]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[23]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[23]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[23]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[23]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[24]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[24]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[24]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[24]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[24]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[24]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[25]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[25]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[25]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[25]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[25]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[25]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[26]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[26]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[26]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[26]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[26]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[26]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[27]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[27]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[27]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[27]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[27]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[27]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[28]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[28]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[28]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[28]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[28]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[28]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[29]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[29]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[29]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[29]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[29]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[29]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[30]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[30]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[30]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[30]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[30]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[30]}]

set_input_delay -clock PLL_CLKOUT_1 -clock_fall -max 4.953 [get_ports {sdram_data_IN[31]}]
set_input_delay -clock PLL_CLKOUT_1 -clock_fall -min 2.029 [get_ports {sdram_data_IN[31]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OUT[31]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OUT[31]}]
set_output_delay -clock PLL_CLKOUT_0 -max -3.500 [get_ports {sdram_data_OE[31]}]
set_output_delay -clock PLL_CLKOUT_0 -min -1.971 [get_ports {sdram_data_OE[31]}]


# JTAG Constraints

####################

