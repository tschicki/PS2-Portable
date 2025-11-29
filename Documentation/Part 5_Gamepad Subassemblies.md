# Part 5: Gamepad Subassemblies
This part of the assembly documentation will describe how to assemble the left and right gamepad sub-assemblies. These connect to the PS2 AIO Mainboard via Mezzanine connectors and also contain the battery compartments and speakers.

## Recommended Tools:
- Soldering iron
- Side cutters
- Scalpel or similar
- Tweezers
- Tool for M1.6 heat set inserts
- Ruler
- Crimping tool IWS-0302BS
- Screw driver kit with at least a T5 bit

## Recommended Materials:
- Solder

## PCB Assembly
- Assemble the *Gamepad Left* (REF4001) and *Gamepad Right* (REF4002) PCBs with the parts of their electrical BOM. 
    - For the action buttons (cross, circle, square, triangle, up, down, left, right) it is up to the user to choose which variant of the tactile switches is appropriate. 160gf and 100gf work best, I personally prefer 100gf for all; 160gf will give more tactile feedback.
    - Check thoroughly for shorts and open connections, especially around the RP2040 and SPI flash. 
    - To be safe, also measure the resistance between VCC and GND (easiest at the bulk decoupling capacitor next to the flex connector). 
- Assemble the *R1 R2 Flex* (REF4006) and *L1 L2 Flex* (REF4007) with two **KSC222JLFS** tactile switches each.
- Assemble the *Gamepad Flex Left* (REF4004) and *Gamepad Flex Right* (REF4005) with two **DF40C-10DP-0.4V(51)** plugs each. 
    - Make sure you use the plugs (the ones with the additional mounting pins) and not the sockets!

> For the flex PCBs it is recommended to set your soldering iron temperature to ~250°C and to use leaded solder. The thin PEI dielectric can and will be damaged by high temperatures.

## Mechanical Assembly
The procedure may use left and right gamepad assembly interchangably. For the most part, both left and right sides are just mirrored and very similar to assemble. Where the procedures for left and right differ, both sides are described.


1. Plug the *Switch Analog Stick* (REF1013) into the respective FFC socket, as seen in the image below.

    <img src="assets/Switch_Stick_Connection.jpg" width="641">

2. Press the *Gamepad PCB* into the respective *Button Cover Left* (REF2002) or *Button Cover Right* (REF2003). If too much force is required, trim the three notches in the 3D printed part slightly using a scalpel.
3. Fixate the *Switch Analog Stick* using two screws. 
    - These screws are not yet specified, I usually use the screws that come with switch analog sticks. If they are too long, you may need to shorten them with sidecutters...

    <img src="assets/Switch_Stick_Fixation.jpg" width="641">
 

4. Connect the *Gamepad Flex* as seen in the image below. Make sure it is fully seated.
    - REF4004 is for the left side, REF4005 for the right side

    <img src="assets/Gamepad_Flex.jpg" width="641">
 
5. Push the *Rumble Motor* (REF1004) into the *Rumble Sleeve* (REF2015). Remove the adhesive protector of the *Rumble Motor*. Test-fit the *Rumble Motor* in the Button cover to cut the wires to length. Pre-tin them and solder the wires to the rumble pads as seen in the image below.
    - This step is optional, omit if you don't want rumble support.

    <img src="assets/Rumble_Motors.jpg" width="641">

6. Install the *M1.6 Threaded Inserts* (REF1014) in the *Battery Compartment*. Two on the front-facing side and four on the back-facing side. I recommend to push them in to slightly below the surface and then remove any protruding plastic with a scalpel. Make sure the surfaces are flush.
 
    <img src="assets/Gamepad_Inserts_Back.jpg" width="641">

    <img src="assets/Gamepad_Inserts_Front.jpg" width="641">
 

7. Cut 4 pieces of wire (REF1018 & REF1019) to length for the battery connections:

    | Side	| Wire Color |	Length |
    |---------------|-----|------|
    | Left Gamepad	| Red |	13cm |
    |               | Black | 6cm |
    | Right Gamepad	| Red |	15cm |
    |               | Black | 22cm |

8. Remove the insulation on one end of each wire and solder it to a *Battery Contact* (REF1005) . Pay close attention to which side the wire enters the *Battery Contact* in the image below. This is important for easier wire routing.

    <img src="assets/Battery_Contact_Overview.jpg" width="641">  
    
    <img src="assets/Battery_Contact_Detail.jpg" width="641">
    
    <img src="assets/Battery_Contact_Closeup.jpg" width="641">
 

9. Install the battery contacts with the attached wires from the front-side, as shown below. Push the contacts in, until they click in place.
    > IMPORTANT:
    > -	Pay close attention to NOT swap the wires of left & right gamepads, as they are different in length!
    > -	Pay VERY close attention to NOT swap the polarity – install the red wire on the ‘+’ side and the black wire on the ‘-’ side.
    
    Left Gamepad:  
    <img src="assets/Contact_Left_Neg.jpg" width="641">
    <img src="assets/Contact_Left_Pos.jpg" width="641">  
    Right Gamepad:  
    <img src="assets/Contact_Right_Neg.jpg" width="641">
    <img src="assets/Contact_Right_Pos.jpg" width="641">

10. Guide the wires through their channels as seen in the image below. Again, pay close attention to the polarity! Then shorten the wires of the right gamepad to slightly above 10cm and the wires of the left gamepad to slightly above 2cm, see the image below for reference.
 
    <img src="assets/Left_Wire_Length.jpg" width="641">
    <img src="assets/Right_Wire_Length.jpg" width="641">
 
11. Remove the insulation at the end and crimp on the *EZMate HC Crimp contacts* (REF1017) for the *EZMate HC Housings* (REF1016). Beware that this requires a special crimping tool for optimal results (see Tools section). It is highly recommended to already consider the orientation of the plug, so the crimp contacts are facing the right way. Bending the Plug 180° can be a bit tricky afterwards, as the wires are quite stiff.

    <img src="assets/Crimping_Tool.jpg" height="550">
    <img src="assets/Crimped_Wires.jpg" height="550">
  
12. Slide the crimped wires into the *EZMate HC Housing*. Again, be very careful with the orientation. The wires shall go straight into the plug, see the reference image below for the correct polarity.  
The opening of theplug should face the back of the gamepad, so it can be plugged straight into the mainboard without twisting it 180°.

    <img src="assets/Battery_Plug.jpg" width="641">
 
 
13. Clip the *Nintendo Switch Lite speaker* (REF1002) into the *Speaker Cover* (REF2006 & REF2007) in the orientation shown below. Install the *Speaker Cover* in the respective gamepad, guide the plug and wires of the speaker through the opening in the *Battery Compartment*.
  
    Left Gamepad:  
    <img src="assets/Speaker_Left.jpg" width="641">

    Right Gamepad:  
    <img src="assets/Speaker_Right.jpg" width="641">

    Installed:  
    <img src="assets/Speaker_Assembled_1.jpg" width="641">

14. Turn the gamepad around and insert the S*peaker Plug* (REF2016) into the *Battery Compartment*. The *Speaker Plug* has a small cutout for the speaker wires to go through, it is very likely that you have to enlarge it manually using a scalpel, depending on your printer. The resulting assembly should look like this:  
    
    <img src="assets/Speaker_Assembled_2.jpg" width="641">
 
15. Connect the *R1 R2 Flex* (REF4006) and *L1 L2 Flex* (REF4007) to the left and right gamepad PCBs, as shown below. Do not forget to close the little latch.

    <img src="assets/Shoulder_Button_Flex.jpg" width="641">
 
16. Place a ~10*4mm big piece of the 1mm thick thermal (REF1023) pad onto the battery thermistor, as shown below. Don’t forget to remove the protective film on both sides.

    <img src="assets/Battery_Thermistor.jpg" width="641">
 

17. Assemble the *Button Cover* and *Battery Compartment*. Do this by first aligning the snap hook of the *Button Cover* with the respective cutout in the *Battery Compartment*. Do this at an angle and close close the assembly like a book, using the snap hook as guide and hinge. Pay special attention that the shoulder button flex PCB aligns with the slot in the *Battery Compartment*. Do not apply force, or you risk breaking the shoulder button flex PCBs.

    Afterwards check that the *R1 R2 Flex* (REF4006) and *L1 L2 Flex* (REF4007) are in place, and that the thermal pad is held in place by the battery Compartment. The part above the thermistor should be raised above the surface, to make contact with the battery later.
    Also check that the gap between *Button Cover* and *Battery Compartment* is as small as possible. A gap here could indicate that the touching surfaces of the 3D printed parts are uneven and need more rework.
 
    <img src="assets/Shoulder_Button_Flex_Assembled.jpg" height="500">
    <img src="assets/Battery_Thermistor_Assembled.jpg" height="500">
 
18. Slide the according *R2 Button* or *L2 Button* (REF2011 & REF2012) onto the *R1 R2 Flex* (REF4006) or *L1 L2 Flex* (REF4007) respectively. This can be tricky, depending on the accuracy of your printer. You might need to rework the slot in the button for guiding the flex PCB, if the PCB doesn’t quite fit (the slot is too narrow). When installed, the button shall sit flush with the Battery Compartment.
 
    <img src="assets/Trigger_Button_1.jpg" height="500">
    <img src="assets/Trigger_Button_2.jpg" height="500">
    <img src="assets/Trigger_Button_3.jpg" height="500">

19. So far the whole assembly was held together by magic. Insert M1.6x5mm screws (REF1007) in the following locations and tighten them. Two from the front and one from the back.

    <img src="assets/Gamepad_Screws_Front.jpg" height="500">
    <img src="assets/Gamepad_Screws_Back.jpg" height="500">
  

# End
Both gamepad sub-assemblies are now fully assembled and ready for the next steps!

