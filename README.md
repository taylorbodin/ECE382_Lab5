ECE382_Lab5
===========

Evil Remote Controls

## Day 1

|Pulse| Duration(ms)| Timer A counts|
|-----|-------------|---------------|
|Start Logic 0|     9000    |     9000       |
|Start Logic 1|     4430    |     4430       |
|Data 1 Logic 0|    530     |     530        |
|Data 1 Logic 1|    1635    |     1635       |
|Data 0 Logic 0|    530     |     530        |
|Data 0 Logic 1|    530     |     530        |
|Stop Logic 0|      530     |     530        |
|Stop Logic 1|      LONG    |     LONG       |

|Button|code|
|-------|-----|
|Power|0x61A0F00E|
| 0 |0x61A0906E|
| 1 |0x61A000FE|
| 2 |0x61A0807E|
| 3 |0x61A040BE|
| CH UP|0x61A050AE|
| CH DW|0x61A0D02E|
| VOL UP|0x61A030CE|
| VOL DW|0x61A0B04E|

## Implementation Plan

Originally my implementation play was to follow the diagram provided to achieve functionality. However, I encountered significant difficulties in determining what had already been coded on the document and what had not. As a result my code would not compile and I started with a fresh slate. After some trial and error in the lab late at night and help from C2C Wooden, I came up with a different, albeit less efficient solution. My plan now went something like this; I would leave the pin 2.6 interupt mostly unchanged, except for classifying the pulse durations in accoradance with the measured values for 0,1, and start logic 1 half pulses. I would put these into an arrary roughly twice the size of a normal packet. This was so that if I capured an orphan packet, even in the worst case scenario, I'd still have a complete packet somewhere in the array. I would collect data from a remote control command via this interupt and store the results until the array filled up. After that, I scanned my array for a start bit which I coded as a two. I incremented the index by one and then bit shifted the data into a variable which I would compare with the known codes to do things on my MSP. 

## Functions
I didn't add any functions to this lab but I will cover my changes to main and the ISR's. 

### Main
```
void main(void) {
	initMSP430();
	x = 4;
	y = 4;
	drawMode = FALSE;
	erase = FALSE;

	while(1)  {

		if (packetIndex > 80) {
			_disable_interrupt();

			while(packetData[i] != 2 && i < 80){
				i++;
			}

			for(j = 0; j < 31; j++){
				i++;
				irPacket += packetData[i];
				irPacket <<= 1;
			}

			if(irPacket == CH_UP){
				if(y>=1) y += -1;
				drawMode = TRUE;

			}

			if(irPacket == CH_DW){
				if(y<=7) y += 1;
				drawMode = TRUE;
			}

			if(irPacket == VOL_UP){
				if(x<=10) x += 1;
				drawMode = TRUE;
			}

			if(irPacket == VOL_DW){
				if(x>=1) x += -1;
				drawMode = TRUE;
			}

			if(irPacket == ONE){
				erase = TRUE;
				drawMode = TRUE;
			}

			if(irPacket == TWO){
				erase = FALSE;
				drawMode = TRUE;
			}

			if(irPacket == THR){

			}

			if(irPacket == PWR){

			}

			if(drawMode == TRUE){
				init();
				initNokia();
				drawBlock(x,y,erase);
				Sleep(50);
				initMSP430();
				drawMode = FALSE;
			}

			i = 0;
			packetIndex = 0;
			_enable_interrupt();
		} // end if new IR packet arrived
	} // end infinite loop
} // end main
```

There are three major blocks of code here. The first simply intitializes the variables x, y, drawMode, and erase. x and y stored the x and y coordinates for the Nokia display. drawMode is a flag that indicates whether we should be set up to talk to the Nokia display or listen to the IR decoder. When it's false we're listening to the decoder. And finally erase is an input to drawBlock that was included in the nokia.asm file. When it is true you draw clear blocks.

The second section of code loads the ir data into a variable called ir packet. First it searches out a start bit with the while loop. Until it hits a 2 it will keep searching the array. After a start bit is found we start bit shifting in the data bits. If the data bit is a one we add one to IR packet and shift left. If the data gives a zero then we simply shift left, which is the equivelent of shifting in a zero. This worked very well to give the correct commands. 

Finally, the irPacket command was compared to various, known command codes stored in header file associated with this project. To implement required functionality, I just toggled bit 6 and 0 to make the LED's change. A functionality was more difficult. This functionality required me to use the draw mode flag. drawMode would get set when the block's address was moved in another command. When this flag was set it triggers an if statement that initializes the MSP430 for communication with the Nokia display. It then wrote that block to the display and reinitialized the MSP430 to communicate with the IR decoder. This gave a buggy, but functional, result. In the future I'd try to do something with the chip select so that I didn't have to constantly reinitialize for the Nokia. Or perhapse more simply, I would've changed my pulseDuration values to reflect the different clock speed required by the Nokia. 

### Pin Change Interupt
```
#pragma vector = PORT2_VECTOR			// This is from the MSP430G2553.h file

__interrupt void pinChange (void) {

	int8	pin;
	int16	pulseDuration;			// The timer is 16-bits

	if (IR_PIN)		pin=1;	else pin=0;

	switch (pin) {					// read the current pin level
		case 0:						// !!!!!!!!!NEGATIVE EDGE!!!!!!!!!!
			pulseDuration = TAR;

			if(pulseDuration <= maxStartPulse && pulseDuration >= minStartPulse){
				packetBit = 2;
			}

			if(pulseDuration <= maxLogic1Pulse && pulseDuration >= minLogic1Pulse){
				packetBit = 1;
			}

			if(pulseDuration <= maxLogic0Pulse && pulseDuration >= minLogic0Pulse){
				packetBit = 0;
			}


			packetData[packetIndex++] = packetBit;
			LOW_2_HIGH; 				// Setup pin interrupr on positive edge
			break;

		case 1:							// !!!!!!!!POSITIVE EDGE!!!!!!!!!!!
			TAR = 0x0000;	// time measurements are based at time 0
			HIGH_2_LOW; 	// Setup pin interrupr on positive edge
			break;
	} // end switch

	P2IFG &= ~BIT6;			// Clear the interrupt flag to prevent immediate ISR re-entry

} // end pinChange ISR
```

Most of this ISR was provided. My contribution was classifying the pulseDuration as a one, zero, or start. The code is farily self documenting. If a pulse is between the max and min durations for whatever kind of pulse it is, set packetBit to that value and add it to the array. On a side note, the differnces between a logic 0, 1, and start pulse were far enough apart that it made any kind of statistical analysis on pulse durations irrelevant. I chose 0x0100 for mine because it was much bigger than the standard deviation I roughly calculated in my head. As long as the max and min correction factor was much larger than the standard deviation it was safe to assume I would categorize ~99.9999% of all pulses correctly. 

## Debugging and Testing

For the first part of the lab there was little debugging that needed to be peformed after I got a working solution on capturing IR packet data. The process I used to get the packet decoding to work was to first make sure the MSP430 was recieving pin changes. Next, I made sure that it was correctly classifying the bits. Next I checked for consistency by pushing the same button a number of times and comparing the array that it produced. Finally, I bit shifted that consistent data into irPacket and compared it to the packets I had decoded on the oscilloscope. The end result was consistently correct irPackets. For testing I simply turned my LED's on and off with the remote commands. 

Since all the ground work had already been done for the A functionality I just had to modify the code to move the block on button presses instead of toggling the LED. The problem I encountered with this was that the clock settings were different for decoding IR packets and communicating to the Nokia. To work around this problem I introduced the drawMode flag which set up the MSP430 to operate in accordance with the peripheral's specifications as we needed it. The only problem with this is that the Nokia display did not like this and gave a somewhat glitchy readout. I couldn't solve this problem within the time constraints. 

A video of me displaying A functionality can be found ![here.](https://www.youtube.com/watch?v=c9cfKBmpMcQ)

## Conclusion
I learned quite a bit about ISR's in this lab which I think was the intended effect. More than anything I realized that I didn't know as much as I should about them. This lab challenged my understanding in a good way. It's tough to interact with hardware having never done it before. In the future I think I'll be able to handle them better. I also like that there was a problem in the lab that I didn't see coming and couldn't fix in time. It taught me to always consider than not all peripherals can run under the same conditions and that I need to figure out how to reconcile that fact. 

## Documentation
C2C Wooden helped me with understanding the basic idea behind the alternative implementation that I used in this lab. 
