# Firmware

In the */PS2_Gamepad_02/* directory you will find the firmware project for both gamepads. It's the same firmware for both, the side is determined by a pin on the RP2040 that is high on one and low on the other gamepad PCB.

In the */SysCon_FW/* directory you will find the SysCon firmware project. You will need this for the PS2 AIO Mainboard bringup described in the project doc Part 6.

In the */SysCon_FW/ps2_video_processor/* directory you will find the VHDL project for the Video Processor FPGA.

Some more hints and guidance about setting up the environment can be found in the project doc Part 10.