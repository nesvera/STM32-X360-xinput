/*
	Mechanical Squid Factory presents to you:
    XINPUT Controller library for TeensyLC, ported for STM32F103C8T6
    Compatible w/ PC
    
    Developer: Zachery Littell
    Email: zlittell@gmail.com
    www.zlittell.com
	
	This tricks the computer into loading the xbox 360 controller driver.
    Then it sends and receives reports in the same way as the xbox 360 controller.
	
	Credit where credit is due:
    Paul Stoffregen - for the awesome teensy and all the awesome examples he has included
    Hamaluik.com - for allowing me to not murder the arduino "IDE" out of frustration of hidden "magic"
    BeyondLogic.org - for a great resource on all things USB protocol related
    BrandonW - I contacted him a long time ago for a different project to get log files from his
             - beagle usb 12 between the 360 and controller.  I used them again for verification
             - and understanding during this project. (brandonw.net)
    free60.org - for their page on the x360 gamepad and its lusb output plus the explanations of the descriptors
    Microsoft - Windows Message Analyzer.  It wouldn't have been possible at times without this awesome message
              - analyzer capturing USB packets.  Debugged many issues with enumerating the device using this.
			  
	Also one final shoutout to Microsoft... basically **** you for creating xinput and not using HID to do so.
    XINPUT makes signing drivers necessary again, which means paying you.  Also you have ZERO openly available
    documentation on the XUSB device standard and I hate you for that.
*/	

#include "xinput.h"

//Private variables and functions

//Data
uint8_t TXData[20] = {0x00, 0x14, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  //Holds USB transmit packet data
uint8_t RXData[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  //Holds USB receive packet data

//LED Patterns
uint8_t patternAllOff[10] = {0,0,0,0,0,0,0,0,0,0};
uint8_t patternBlinkRotate[10] = {1,0,1,0,1,0,1,0,1,0};
uint8_t patternPlayer1[10] = {1,0,0,0,0,0,0,0,0,0};
uint8_t patternPlayer2[10] = {1,0,1,0,0,0,0,0,0,0};
uint8_t patternPlayer3[10] = {1,0,1,0,1,0,0,0,0,0};
uint8_t patternPlayer4[10] = {1,0,1,0,1,0,1,0,0,0};
uint8_t patternCurrent[10] = {0,0,0,0,0,0,0,0,0,0};	//Variabled to hold the current pattern selected by the host

uint8_t rumbleValues[2] = {0x00,0x00};	//Array to hold values for rumble motors. rumbleValues[0] is big weight rumbleValues[1] is small weight
uint8_t currentPlayer = 0;	//Variable to access the current controller number attached to this device.  0 is no controller number assigned by host yet


//LED Pattern Tracking
uint8_t _modeLED = 0;			//Track LED mode
struct _pin _pinLED;			//Track LED pin
uint8_t _LEDState = 0;		//used to set the pin for the LED
uint32_t _previousMS = 0; //used to store the last time LED was updated
uint8_t _LEDtracker = 0;	//used as an index to step through a pattern on interval

void LEDPatternSelect(uint8_t rxPattern);

/* Initialize controller 
*
*/
void XINPUT_init( uint8_t LEDMode, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin ){
	XINPUT_setLEDMode( LEDMode, GPIOx, GPIO_Pin );
}

/* 	Update button in packet
*	 	Buttons L3,R3,START,BACK are in Packet 1
*		Buttons A,B,X,Y,LB,RB,LOGO are in Packet 2
*/
void XINPUT_buttonUpdate(uint8_t button, uint8_t buttonState)
{
	//BUTTON_A
	if (button == BUTTON_A)
	{
		if(buttonState)
		{
			TXData[BUTTON_PACKET_2] |= A_MASK_ON;
		}
		else
		{
			TXData[BUTTON_PACKET_2] &= A_MASK_OFF;
		}
	}
	//BUTTON_B
	else if(button == BUTTON_B)
	{
		if(buttonState)
		{
			TXData[BUTTON_PACKET_2] |= B_MASK_ON;
		}
		else
		{
			TXData[BUTTON_PACKET_2] &= B_MASK_OFF;
		}
	}
	//BUTTON_X
	else if(button == BUTTON_X)
	{
		if(buttonState)
		{
			TXData[BUTTON_PACKET_2] |= X_MASK_ON;
		}
		else
		{
			TXData[BUTTON_PACKET_2] &= X_MASK_OFF;
		}
	}
	//BUTTON_Y
	else if(button == BUTTON_Y)
	{
		if(buttonState)
		{
			TXData[BUTTON_PACKET_2] |= Y_MASK_ON;
		}
		else
		{
			TXData[BUTTON_PACKET_2] &= Y_MASK_OFF;
		}
	}
	//BUTTON_LB
	else if(button == BUTTON_LB)
	{
		if(buttonState)
		{
			TXData[BUTTON_PACKET_2] |= LB_MASK_ON;
		}
		else
		{
			TXData[BUTTON_PACKET_2] &= LB_MASK_OFF;
		}
		
	}
	//BUTTON_RB
	else if(button == BUTTON_RB)
	{
		if(buttonState)
		{
			TXData[BUTTON_PACKET_2] |= RB_MASK_ON;
		}
		else
		{
			TXData[BUTTON_PACKET_2] &= RB_MASK_OFF;
		}
	}
	//BUTTON_L3
	else if(button == BUTTON_L3)
	{
		if(buttonState)
		{
			TXData[BUTTON_PACKET_1] |= L3_MASK_ON;
		}
		else
		{
			TXData[BUTTON_PACKET_1] &= L3_MASK_OFF;
		}
	}
	//BUTTON_R3
	else if(button == BUTTON_R3)
	{
		if(buttonState)
		{
			TXData[BUTTON_PACKET_1] |= R3_MASK_ON;
		}
		else
		{
			TXData[BUTTON_PACKET_1] &= R3_MASK_OFF;
		}
	}
	//BUTTON_START
	else if(button == BUTTON_START)
	{
		if(buttonState)
		{
			TXData[BUTTON_PACKET_1] |= START_MASK_ON;
		}
		else
		{
			TXData[BUTTON_PACKET_1] &= START_MASK_OFF;
		}
	}
	//BUTTON_BACK
	else if(button == BUTTON_BACK)
	{
		if(buttonState)
		{
			TXData[BUTTON_PACKET_1] |= BACK_MASK_ON;
		}
		else
		{
			TXData[BUTTON_PACKET_1] &= BACK_MASK_OFF;
		}
	}
	//BUTTON_LOGO
	else if(button == BUTTON_LOGO)
	{
		if(buttonState)
		{
			TXData[BUTTON_PACKET_2] |= LOGO_MASK_ON;
		}
		else
		{
			TXData[BUTTON_PACKET_2] &= LOGO_MASK_OFF;
		}
	}
	//Unknown Button
	else {}
}

/*	Update all buttons with a single array
*		Order is as follows A,B,X,Y,LB,RB,L3,R3,START,BACK,LOGO
*		11 buttons 0-10 in the array
*/
void XINPUT_buttonArrayUpdate(uint8_t buttonArray[11])
{
	//BUTTON_A
	if (buttonArray[0]){TXData[BUTTON_PACKET_2] |= A_MASK_ON;}
	else{TXData[BUTTON_PACKET_2] &= A_MASK_OFF;}
	//BUTTON_B
	if(buttonArray[1]){TXData[BUTTON_PACKET_2] |= B_MASK_ON;}
	else{TXData[BUTTON_PACKET_2] &= B_MASK_OFF;}
	//BUTTON_X
	if(buttonArray[2]){TXData[BUTTON_PACKET_2] |= X_MASK_ON;}
	else{TXData[BUTTON_PACKET_2] &= X_MASK_OFF;}
	//BUTTON_Y
	if(buttonArray[3]){TXData[BUTTON_PACKET_2] |= Y_MASK_ON;}
	else{TXData[BUTTON_PACKET_2] &= Y_MASK_OFF;}
	//BUTTON_LB
	if(buttonArray[4]){TXData[BUTTON_PACKET_2] |= LB_MASK_ON;}
	else{TXData[BUTTON_PACKET_2] &= LB_MASK_OFF;}
	//BUTTON_RB
	if(buttonArray[5]){TXData[BUTTON_PACKET_2] |= RB_MASK_ON;}
	else{TXData[BUTTON_PACKET_2] &= RB_MASK_OFF;}
	//BUTTON_L3
	if(buttonArray[6]){TXData[BUTTON_PACKET_1] |= L3_MASK_ON;}
	else{TXData[BUTTON_PACKET_1] &= L3_MASK_OFF;}
	//BUTTON_R3
	if(buttonArray[7]){TXData[BUTTON_PACKET_1] |= R3_MASK_ON;}
	else{TXData[BUTTON_PACKET_1] &= R3_MASK_OFF;}
	//BUTTON_START
	if(buttonArray[8]){TXData[BUTTON_PACKET_1] |= START_MASK_ON;}
	else{TXData[BUTTON_PACKET_1] &= START_MASK_OFF;}
	//BUTTON_BACK
	if(buttonArray[9]){TXData[BUTTON_PACKET_1] |= BACK_MASK_ON;}
	else{TXData[BUTTON_PACKET_1] &= BACK_MASK_OFF;}
	//BUTTON_LOGO
	if(buttonArray[10]){TXData[BUTTON_PACKET_2] |= LOGO_MASK_ON;}
	else{TXData[BUTTON_PACKET_2] &= LOGO_MASK_OFF;}
}

/* 	Update dpad values in the packet
*		SOCD cleaner included
*		Programmed behavior is UP+DOWN=UP and LEFT+RIGHT=NEUTRAL
*		SOCD makes fightsticks tournament legal and helps prevent erroneous states 
*/
void XINPUT_dpadUpdate(uint8_t dpadUP, uint8_t dpadDOWN, uint8_t dpadLEFT, uint8_t dpadRIGHT)
{
	//Clear DPAD
	TXData[BUTTON_PACKET_1] &= DPAD_MASK_OFF;
	//DPAD Up
	if (dpadUP) {TXData[BUTTON_PACKET_1] |= DPAD_UP_MASK_ON;}
	//DPAD Down
	if (dpadDOWN && !dpadUP) {TXData[BUTTON_PACKET_1] |= DPAD_DOWN_MASK_ON;}
	//DPAD Left
	if (dpadLEFT && !dpadRIGHT) {TXData[BUTTON_PACKET_1] |= DPAD_LEFT_MASK_ON;}
	//DPAD Right
	if (dpadRIGHT && !dpadLEFT) {TXData[BUTTON_PACKET_1] |= DPAD_RIGHT_MASK_ON;}
}

/*	Update the trigger values in the packet		
*		0x00 to 0xFF
*/
void XINPUT_triggerUpdate(uint8_t triggerLeftValue, uint8_t triggerRightValue)
{
	TXData[LEFT_TRIGGER_PACKET] = triggerLeftValue;
	TXData[RIGHT_TRIGGER_PACKET] = triggerRightValue;
}

/*	Update a single trigger value in the packet		
*		0x00 to 0xFF
*/
void XINPUT_singleTriggerUpdate(uint8_t trigger, uint8_t triggerValue)
{
	if (trigger == TRIGGER_LEFT)
	{
		TXData[LEFT_TRIGGER_PACKET] = triggerValue;
	}
	else if (trigger == TRIGGER_RIGHT)
	{
		TXData[RIGHT_TRIGGER_PACKET] = triggerValue;
	}
	else{/*invalid parameter*/}
}

/*	Analog Sticks
*		Each axis is a signed 16 bit integer
*		-32,768 to 32,767 is the range of value
*/
void XINPUT_stickUpdate(uint8_t analogStick, int16_t stickXDirValue, int16_t stickYDirValue)
{
	if (analogStick == STICK_LEFT)
	{
		//Left Stick X Axis
		TXData[LEFT_STICK_X_PACKET_LSB] = LOBYTE(stickXDirValue);		// (CONFERIR)
		TXData[LEFT_STICK_X_PACKET_MSB] = HIBYTE(stickXDirValue);
		//Left Stick Y Axis
		TXData[LEFT_STICK_Y_PACKET_LSB] = LOBYTE(stickYDirValue);
		TXData[LEFT_STICK_Y_PACKET_MSB] = HIBYTE(stickYDirValue);
	}
	else if(analogStick == STICK_RIGHT)
	{
		//Right Stick X Axis
		TXData[RIGHT_STICK_X_PACKET_LSB] = LOBYTE(stickXDirValue);
		TXData[RIGHT_STICK_X_PACKET_MSB] = HIBYTE(stickXDirValue);
		//Right Stick Y Axis
		TXData[RIGHT_STICK_Y_PACKET_LSB] = LOBYTE(stickYDirValue);
		TXData[RIGHT_STICK_Y_PACKET_MSB] = HIBYTE(stickYDirValue);
	}
	else{/*invalid parameter*/}
}

/* Send an update packet to the PC
*
*/
void XINPUT_sendXinput()
{
	//Send TXData
	//XInputUSB.send(TXData, USB_TIMEOUT);		//(ALTERAR)
	
	//Zero out data
	//Start at 2 so that you can keep the message type and packet size
	//Then fill the rest with 0x00's
	for (int i=2; i<13; i++) {TXData[i] = 0x00;}
}

/*
*		(ALTERAR)
*/
uint8_t XINPUT_receiveXinput()
{
	/*
	//Check if packet available
	if (XInputUSB.available() > 0)
	{
		//Grab packet and store it in RXData array
		XInputUSB.recv(RXData, USB_TIMEOUT);
		
		//If the data is a rumble command parse it
		//Rumble Command
		//8bytes for this command. In hex looks like this
		//000800bbll000000
		//the bb is speed of the motor with the big weight
		//the ll is the speed of the motor with the small weight
		//0x00 to 0xFF (0-255) is the range of these values
		if ((RXData[0] == 0x00) & (RXData[1] == 0x08))
		{
			//Process Rumble
			rumbleValues[0] = RXData[3];	//Big weight
			rumbleValues[1] = RXData[4];  //Small weight
			return 1;
		}
		
		//If the data is an LED command parse it
		else if (RXData[0] == 1)
		{
			LEDPatternSelect(RXData[2]);	//Call LED Pattern Select and pass the pattern byte to it
			return 2;
		}
		
		//Some other command we don't parse came through
		else{return 3;}
	}
	//Packet not available return 0
	else{return 0;}
	*/
}

/* Set the LED mode and pin settings
*
*/
void XINPUT_setLEDMode(uint8_t LEDMode, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin ){
	//Check LED mode
	if (LEDMode == LED_ENABLED)
	{
		/** STM32CUBE DO THIS */
		//LED ENABLED
		//pinMode(LEDPin, OUTPUT);		//Set pin output mode
		//digitalWrite(LEDPin, LOW);	//Set pin low initially to turn light off
		
		_modeLED = LED_ENABLED;				//Set LED Mode
		_pinLED.port = GPIOx;					//Set LED Pin
		_pinLED.pin = GPIO_Pin;
	}
	else
	{
		//Invalid entry or No Led
		_modeLED = NO_LED;		//Clear LED Mode
		_pinLED.port = NULL;	//Clear LED Pin
		_pinLED.pin = 0;
	}
}

/*	Process and update the current LED Pattern
*		(ALTERAR)
*/
void XINPUT_LEDUpdate()
{
	/*
	if (_modeLED == LED_ENABLED)
	{
		//Grab the current time in mS that the program has been running
		uint32_t currentMS = millis();
	
		//subtract the previous update time from the current time and see if interval has passed
		if ((currentMS - _previousMS)>interval)
		{
			//Set the led state correctly according to next part of pattern
			_LEDState = patternCurrent[_LEDtracker];
			//update the previous time
			_previousMS = currentMS;
			//increment the pattern tracker
			_LEDtracker++;
			//write the state to the led
			digitalWrite(_pinLED, _LEDState);
		}
	
		//if we increased ledtracker to 10, it needs to rollover
		if (_LEDtracker==10) {_LEDtracker=0;}
	}
	else{LED mode is NO_LED}

	*/
}

/*	Select the correct LED pattern according to received packets
*
*/
void XINPUT_LEDPatternSelect(uint8_t rxPattern){
	/*
	Process the LED Pattern
	0x00 OFF
	0x01 All Blinking
	0x02 1 Flashes, then on
	0x03 2 Flashes, then on
	0x04 3 Flashes, then on
	0x05 4 Flashes, then on
	0x06 1 on
	0x07 2 on
	0x08 3 on
	0x09 4 on
	0x0A Rotating (1-2-4-3)
	0x0B Blinking*
	0x0C Slow Blinking*
	0x0D Alternating (1+4-2+3)*
	*Does Pattern and then goes back to previous
	*/
	//All blinking or rotating
	if((rxPattern==ALLBLINKING)||(rxPattern==ROTATING))
	{
		//Copy the pattern array into the current pattern
		memcpy(patternCurrent, patternBlinkRotate, 10);
		//Reset the index to beginning of pattern
		_LEDtracker = 0;
		//Set the current player to 0 to indicate not being handshaked completely yet
		currentPlayer = 0;
	}
	//Device is player 1
	else if ((rxPattern==FLASHON1)||(rxPattern==ON1))
	{
		//Copy the pattern array into the current pattern
		memcpy(patternCurrent, patternPlayer1, 10);
		//Reset the index to beginning of pattern
		_LEDtracker = 0;
		//Set the current player to 1
		currentPlayer = 1;
	}
	//Device is player 2
	else if ((rxPattern==FLASHON2)||(rxPattern==ON2))
	{
		//Copy the pattern array into the current pattern
		memcpy(patternCurrent, patternPlayer2, 10);
		//Reset the index to beginning of pattern
		_LEDtracker = 0;
		//Set the current player to 2
		currentPlayer = 2;
	}
	//Device is player 3
	else if ((rxPattern==FLASHON3)||(rxPattern==ON3))
	{
		//Copy the pattern array into the current pattern
		memcpy(patternCurrent, patternPlayer3, 10);
		//Reset the index to beginning of pattern
		_LEDtracker = 0;
		//Set the current player to 3
		currentPlayer = 3;
	}
	//Device is player 4
	else if ((rxPattern==FLASHON4)||(rxPattern==ON4))
	{
		//Copy the pattern array into the current pattern
		memcpy(patternCurrent, patternPlayer4, 10);
		//Reset the index to beginning of pattern
		_LEDtracker = 0;
		//Set the current player to 4
		currentPlayer = 4;
	}
	//If pattern is not specified perform no pattern
	else
	{
		//Copy the pattern array into the current pattern
		memcpy(patternCurrent, patternAllOff, 10);
		//Pattern is all 0's so we don't care where LEDtracker is at
	}
}
