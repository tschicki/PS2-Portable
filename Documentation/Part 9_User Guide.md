# Part 9: User Guide
This part of the assembly documentation will give you a couple of hints when it comes to using the portable.

## Power On and Off
- Long-press the PlayStation button to turn on the portable
- Long-presss the PlayStation button to turn off the portable. Turning off has a couple of seconds delay, so no worries if the system doesn't shut down right away!

## Charging and Discharging
> As with Li-Ion powered devices:
> - Don't leave the system unattended while charging!  
> - Only charge at room temperature!
> - Don't use unnecessarily long USB cables for charging!

The portable supports USB-C PD charging with the following profiles:
- 5V 500mA (fallback if a USB-A charger or PC is plugged)
- 5V 3A
- 12V 3A
- 15V 3A

While the highest PDO could provide up to 45W in total, the portable is configured to charge at about 9W. During charge and play the portable is using about 15W.
For best compatibility choose a high-wattage adapter that supports up to 20V operation. If a power adapter doess not support the portable's power profiles, charging will fall back to 5V 500mA. 
From experience, USB-C chargers from laptops often don't work for charging the PS2 portable (looking at you, Dell). I personally use a UGREEN 65W dual-port USB-C charger, sometimes also a Steam Deck charger.

The RGB LED on the front is used as state-of-charge and status indicator. In normal usage and during charging it displays the state of charge using the following color gradients from 100% to 10% : dark blue (full) -> light blue -> green -> yellow (~50%) -> dark yellow -> orange -> red blinking(almost empty).
Currently there are two status LED patterns: 
- charging error: blinking red / blue; will block charging until USB cable is unplugged;
- low battery alert: blinking red; battery almost empty; Once the LED blinks red, you have about 30-45 minutes to save progress or find a charger.

## First Time Usage
For the first couple of charge cycles, the state of charge will not be very accurate. When you first use the PS2 portable, it is recommended to fully discharge it to about 6V. You find the battery voltage information in the General->PM Monitoring menu. Then you can fully charge it to 100%.
Do this for a couple of cycles and the state of charge accuracy should improve a lot!

## Menu
The config menu is kept very basic annd provides the most important settings. You enter it by short-pressing the PlayStation button while the PS2 is powered on. Note that the menu is only displayed when the PS2 outputs a valid video signal, i.e. it will not appear when the console is off or an unsupported resolution is displayed.

### Button mapping in the menu:
- Cross: selecting menu items and toggling some settings that can only be on or off
- Circle: going back to the previous menu page
- Up/Down: Selecting menu items
- Left/Right: Decrementing/Incrementing settings
- PlayStation button (short press): exiting the menu

Selected items are blue, deselected items are white.

### Main Menu:
- **SOC**: State of charge in %
- **CHARGE/DISCHARGE**: Charge and Discharge indicator. Will tell you whether a charger is plugged or not.
- **EE TEMP**: Emotion Engine temperature reading in °C. It's good to keep an eye on this when you're playing in a hot environment, the portable will shut down when the temperature reaches **50°C**.
- **Audio Settings**: Allows you to change the headphone and speaker volume
- **Video Settings**: Change resolutions, offsets, brightness, video motion threshold and more
- **Input Settings**: Perform analog stick calibration, invert sticks, set PS1 mode, adjust rumble
- **General**: Contains the power management monitoring and lets you check the estimated time to full/empty
- **Store Settings**: Pressing Cross to store the surrent settings in flash at the next shutdown. If successful will display 'OK' next to the entry.

### Audio Menu:
- **HP Volume**: HP stands for headphone. Setting to zero mutes headphone audio
- **SP Volume**: SP stands for speakers. Setting to zero mutes the speakers

### Video Menu:
- **Resolution Configs**: Configure resolutions and offsets for the currently displayed resolution.
- **Other Configs**: Change brightnesss, motion threshold, phase and other settings.
- **Current Resolution**: Displays the resolution your PS2 is currently outputting, for information only

The video settings are mirrored 5 times. There is a set of settings for 512i, 448i, 256p, 240p and 480p. Each set can be changed independently of the others and is only applied when that specific resolution is displayed. Note that you always change the set of settings for the resolution that is shown in 'Current Resolution'.

#### Resolution Configs Menu
- **X Position**: Move the image left and right
- **Y Position**: Move the image up and down
- **Input Res X**: Change the active horizontal input resolution
- **Input Res Y**: Change the active vertical input resolution
- **Output Res X**: Change the horizontal bilinear scaler target resolution
- **Output Res Y**: Change the vertical bilinear scaler target resolution
- **MAGH**: Change the horizontal magnification and input resolution; If you notice that your game has vertical lines, try changing this value

#### Other Configs Menu
- **Brightness**: Change display brightness from 0 to 100%
- **Horizontal Phase** : Configure in which clock cycle a pixel shall be sampled; this usually does nothing, but sometimes it can remove blurriness
- **Deinterlacer Debug** : When enabled, all pixels where motion is detected will be colored pink. This only works for interlaced inputs. Useful for tweaking the Motion Threshold.
- **Motion Threshold** : Sets the threshold for when the motion adaptive deinterlacer detects motion. Lower values will favor bob deinterlacing (more blurry and wobbly), higher values favor weave deinterlacing (more combing artefacts). Only applies to interlaced inputs.

### Input Settings Menu
- **Analog Stick Calibration**: Use this to configure the center point and min/max of both analog sticks. Launch the function and follow the instructions.
- **Invert Axis**: If you want to invert X/Y of either stick, use this function. Hold the stick in the axis you want to invert and press cross to invert it.
- **Deadzone**: Sets the analog stick deadzone (doh...). It's best to keep this below 10
- **PS1 Analog**: Useful for PS1 games. When enabled the left analog stick will be mapped to the DPAD
- **Rumble Intensity**: 0 is off and 3 is the strongest rumble. Only works if you installed rumble motors

### General Menu
- **About**: Self-explanatory
- **PM Monitoring**: Check battery power and voltage & TTF, TTE
- **Fan Low Temp**: Set the temperature where the cooling fan is off
- **Fan High Temp**: Set the temperature where the cooling fan is at 100%

The fan curve is linear between the lower and higher temperature limit.

#### PM Monitoring Menu
- **Battery Power**: A negative value indicates discharging and a positive one charging
- **Battery Voltage**: The Total battery pack voltage; useful for the first couple of discharge cycles
- **Time to Full**: More or less accurate, only applies when a charger is plugged
- **Time to Empty**: More or less accurate, only applies when a charger is unplugged


