# Part 10: Development and Debugging
This part of the assembly documentation will describe what you need to get started with developing and debugging SysCon and video processor code.
It is highly recommended to build the PS2 Debugger described in [Part 8](https://github.com/tschicki/PS2-Portable/blob/main/Documentation/Part%208_Accessories%20Assembly.md) for this!


## Tooling
### Firmware (SysCon & Gamepads)
You need to setup the tooling in order to program and/or debug the SysCon and gamepads. What you need to do first:
- Install VSCode
- Install the [Raspberry Pi Pico extension](https://marketplace.visualstudio.com/items?itemName=raspberry-pi.raspberry-pi-pico)

In the Raspberry Pi Pico Quick Access Tab on the left you can import the [SysCon project](https://github.com/tschicki/PS2-Portable/tree/main/SysCon%20Firmware/SysCon_FW) and [gamepad project](https://github.com/tschicki/PS2-Portable/tree/main/SysCon%20Firmware/PS2_Gamepad_02). Clean CMake and you should be good to go for developing and compiling both SysCon and gamepad code! Don't forget to set the build type to debug for debugging!

This setup is also required for manufacturing the *PS2 AIO Mainboard*, as you will need to compile the SysCon and gamepad firmware.

### Gateware
The Video Processor is an Efinix Trion T20 FPGA, so you need to download the [Efinity software](https://www.efinixinc.com/products-efinity.html) for creating a valid bistream for it. You need to create an account, but the software is free.
The FPGA project is located [here](https://github.com/tschicki/PS2-Portable/tree/main/SysCon%20Firmware/SysCon_FW/ps2_video_processor) and called sdram.xml. Open it in Efinity and from here on you should be able to run the full workflow.

## SysCon

### Manufacturing
For the SysCon, the only file you will need to touch for manufacturing is called [MCU_Interface.h](https://github.com/tschicki/PS2-Portable/blob/main/SysCon%20Firmware/SysCon_FW/inc/MCU_interface.h) and can be found in the ./inc/ folder. Right below all the includes are the three defines of interest *MFG_STEP_1*, *MFG_STEP_2*, *MFG_STEP_3*. They need to be included (or not) according to the instructions given in [Part 6](https://github.com/tschicki/PS2-Portable/blob/main/Documentation/Part%206_Mainboard%20Bringup.md). When these are set as required, make sure the build type is set to *release* and compile the project. Afterwards, you should find the file *main.uf2* in the *build* folder inside the SysCon project, this is the file to flash the the SysCon via USB. Just check the plausibility of its timestamp before you flash it. 

### Developing
For developing you will need the *PS2 Debugger* to interface with the SysCon. It connects to the debug header on the heatsink-side of the *PS2 AIO Mainboard*. The debugger features SWD, UART and JTAG (for the T20), but does not provide power - so you will need to connect the mainboard to a charger for debugging. The *MCU_Interface.h* file also has some interesting debug flags to print status information via UART, which can be observed using the debugger's COM port.

## Gamepads
### Manufacturing and Developing
The tooling for both gamepads is the same as the SysCon, but they don't have a USB interface to load the firmware. Here the debugger is mandatory (either the *PS2 Debugger* or a pico debug probe) to upload the code using the *Flash Project (SWD)* button in the Pico SDK. In order for this to work, the system needs to be plugged in a charger to power the gamepads.

## Video Processor
Luckily you don't need to touch the video processor during manufacturing.
### Developing
After setting up the tooling you can modify the gateware to your hearts content. For testing your code on hardware you have two options:
- After you created the bitstream, recompile the SysCon firmware and upload it to the SysCon. The FPGA bitstream is embedded in the firmware at compile time and the SysCon takes care of loading it onto the T20. The script that takes care of the conversion from bitstream to C header file is here: */SysCon Firmware/SysCon_FW/generated/bit2source.py*
- Use the FT2232H Mini Module on the PS2 Debugger to load the bitstream via JTAG in the Efinity software (select the programmer on the top). It connects to the debug header on the heatsink-side of the *PS2 AIO Mainboard*.
    - The Syscon blocks JTAG access to the T20 by default, to disable this find the line `#define BLOCK_T20_JTAG` in MCU_Interface.h and comment it out; reflash the firmware to apply the change.
    - Now you can do all your T20 JTAG shenanigans while the PS2 is running!

No IP was used in the creation of the video processor, every module was written from scratch in VHDL. This should make the design quite portable with minimal changes! Have fun!