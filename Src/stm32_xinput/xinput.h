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

#ifndef __xinput_H
#define __xinput_H

//Includes
#include "stdint.h"

#include "usb_xinput.h"
#include "stm32_xinput.h"
#include "usbd_def.h"

//Defines
//----------------------------------------------

//General Declarations
#define interval 150	//interval in milliseconds to update LED
#define USB_TIMEOUT 12840	//packet timeout for USB

//LED Pattern Defines
#define ALLOFF 0x00
#define ALLBLINKING 0x01
#define FLASHON1 0x02
#define FLASHON2 0x03
#define FLASHON3 0x04
#define FLASHON4 0x05
#define ON1  0x06
#define ON2  0x07
#define ON3  0x08
#define ON4  0x09
#define ROTATING 0x0A
#define BLINK	 0x0B
#define SLOWBLINK 0x0C
#define ALTERNATE 0x0D

//LED STYLE DEFINES
#define NO_LED 0
#define LED_ENABLED 1

//BUTTON MASK DEFINES
#define R3_MASK_ON 0x80
#define R3_MASK_OFF 0x7F
#define L3_MASK_ON 0x40
#define L3_MASK_OFF 0xBF
#define BACK_MASK_ON 0x20
#define BACK_MASK_OFF 0xDF
#define START_MASK_ON 0x10
#define START_MASK_OFF 0xEF
#define DPAD_RIGHT_MASK_ON 0x08
#define DPAD_RIGHT_MASK_OFF 0xF7
#define DPAD_LEFT_MASK_ON 0x04
#define DPAD_LEFT_MASK_OFF 0xFB
#define DPAD_DOWN_MASK_ON 0x02
#define DPAD_DOWN_MASK_OFF 0xFD
#define DPAD_UP_MASK_ON 0x01
#define DPAD_UP_MASK_OFF 0xFE
#define Y_MASK_ON 0x80
#define Y_MASK_OFF 0x7F
#define X_MASK_ON 0x40
#define X_MASK_OFF 0xBF
#define B_MASK_ON 0x20
#define B_MASK_OFF 0xDF
#define A_MASK_ON 0x10
#define A_MASK_OFF 0xEF
#define LOGO_MASK_ON 0x04
#define LOGO_MASK_OFF 0xFB
#define RB_MASK_ON 0x02
#define RB_MASK_OFF 0xFD
#define LB_MASK_ON 0x01
#define LB_MASK_OFF 0xFE
#define DPAD_MASK_OFF 0xF0

//Byte location Definitions
#define BUTTON_PACKET_1 2
#define BUTTON_PACKET_2 3
#define LEFT_TRIGGER_PACKET 4
#define RIGHT_TRIGGER_PACKET 5
#define LEFT_STICK_X_PACKET_LSB 6
#define LEFT_STICK_X_PACKET_MSB 7
#define LEFT_STICK_Y_PACKET_LSB 8
#define LEFT_STICK_Y_PACKET_MSB 9
#define RIGHT_STICK_X_PACKET_LSB 10
#define RIGHT_STICK_X_PACKET_MSB 11
#define RIGHT_STICK_Y_PACKET_LSB 12
#define RIGHT_STICK_Y_PACKET_MSB 13

//Classification numbers for updating controller items
#define BUTTON_A 0x01
#define BUTTON_B 0x02
#define BUTTON_X 0x03
#define BUTTON_Y 0x04
#define BUTTON_LB 0x05
#define BUTTON_RB 0x06
#define BUTTON_L3 0x07
#define BUTTON_R3 0x08
#define BUTTON_START 0x09
#define BUTTON_BACK 0x0a
#define BUTTON_LOGO 0x0b
#define DPAD_UP 0x0c
#define DPAD_DOWN 0x0d
#define DPAD_LEFT 0x0e
#define DPAD_RIGHT 0x0f
#define TRIGGER_LEFT 0x10
#define TRIGGER_RIGHT 0x11
#define STICK_LEFT 0x12
#define STICK_RIGHT 0x13

//Data 
extern uint8_t TXData[20];  //Holds USB transmit packet data
extern uint8_t RXData[8]; //Holds USB receive packet data


extern uint8_t rumbleValues[2];	//Array to hold values for rumble motors. rumbleValues[0] is big weight rumbleValues[1] is small weight
extern uint8_t currentPlayer;	//Variable to access the current controller number attached to this device.  0 is no controller number assigned by host yet

//Functions
void XINPUT_init( uint8_t LEDMode, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin );
void XINPUT_buttonUpdate(uint8_t button, uint8_t buttonState);
void XINPUT_buttonArrayUpdate(uint8_t buttonArray[11]);
void XINPUT_dpadUpdate(uint8_t dpadUP, uint8_t dpadDOWN, uint8_t dpadLEFT, uint8_t dpadRIGHT);
void XINPUT_triggerUpdate(uint8_t triggerLeftValue, uint8_t triggerRightValue);
void XINPUT_singleTriggerUpdate(uint8_t trigger, uint8_t triggerValue);
void XINPUT_stickUpdate(uint8_t analogStick, int16_t stickXDirValue, int16_t stickYDirValue);
void XINPUT_sendXinput(void);
uint8_t XINPUT_receiveXinput(void);
void XINPUT_setLEDMode(uint8_t LEDMode, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin );
void XINPUT_LEDUpdate(void);

#endif
