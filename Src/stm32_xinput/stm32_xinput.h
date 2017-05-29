/*
	Interface between Racing wheel and XINPUT Controller library
  Compatible w/ PC
  
	Developer: Daniel Nesvera
	
	WTFPL lincense

*/

#ifndef __stm32_xinput_H
#define __stm32_xinput_H

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "stm32f1xx_hal.h"
	 
#define NUM_BUTTONS 15
#define NUM_ANALOG 	7
	 
#define ENCODER_PRECISION 1
#define STEERING_DEGREES 200
#define MAX_ADC_VALUE 4033
#define MIN_HANDBRAKE_POS 200
#define MAX_HANDBRAKE_POS	800
#define MID_HANDBRAKE_POS ((MAX_HANDBRAKE_POS-MIN_HANDBRAKE_POS)/2)
	 
#define MAX_THROTTLE 4020				// values from my pedals
#define MAX_BRAKE 3370
	 
struct _pin{
	GPIO_TypeDef* port;
	uint16_t pin;
	GPIO_PinState state;			/// OFF: 0(RESET)		ON: 1(SET)
};

extern int16_t wheelEncoderValue;
extern uint16_t handbrakeValue;
extern int16_t leftTriggerValue_ADC;
extern int16_t rightTriggerValue_ADC;
extern int16_t xLeftStickValue_ADC;				// value comes from wheelEncoderValue
extern int16_t yLeftStickValue_ADC;				// Not used
extern int16_t xRightStickValue_ADC;
extern int16_t yRightStickValue_ADC;
extern struct _pin digitalArray[NUM_BUTTONS];
extern struct _pin analogArray[NUM_ANALOG];
extern struct _pin encoderPins[2];

extern int8_t adcValueReady;
extern int16_t leftTriggerValue;
extern int16_t rightTriggerValue;
extern int16_t xLeftStickValue;
extern int16_t yLeftStickValue;				
extern int16_t xRightStickValue;
extern int16_t yRightStickValue;

void declareButtonPins();
void declareAnalogPins();
void declareEncoderPins();
int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max);
void readButtons();
void updateTriggers();
void updateSticks();
void readAdcValues();

#ifdef __cplusplus
}
#endif
#endif /*__stm32_xinput_H */
