/*
 * stepper.h
 *
 *  Created on: May 4, 2024
 *      Author: helsis
 */

#include "tim.h"
#include "gpio.h"

#ifndef INC_STEPPER_H_
#define INC_STEPPER_H_

#define MAX_STEPPERS_COUNT	3

#define DEFAULT_MIN_SPS		1000
#define DEFAULT_MAX_SPS		400000

typedef enum {
	SS_STOPPED,
	SS_RUNNING_FORWARD	= GPIO_PIN_SET,
	SS_RUNNING_BACKWARD	= GPIO_PIN_RESET,
} Stepper_Status;

typedef enum {
	SERR_OK,
	SERR_STATE_NOT_FOUND,
	SERR_STATE_MAX_COUNT,
	SERR_STATE_ALLREADY_EXIST,
} Stepper_Error;

typedef struct {
	char				name;
    Stepper_Status		status;

	TIM_HandleTypeDef	*STEP_TIMER;
	uint32_t			STEP_CHANNEL;

	GPIO_TypeDef		*DIR_GPIO;
	uint16_t			DIR_PIN;

    volatile int     	speed;
    volatile int     	targetPosition;
    volatile int     	currentPosition;
} StepperMotor;


Stepper_Error Stepper_Init(
	char name,
	TIM_HandleTypeDef *htim, uint32_t Chanel,
	GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin
);

StepperMotor* GetState(char name);

Stepper_Error Stepper_SetPosition(char name, int position, int speed);
Stepper_Error Stepper_SetSpeed(StepperMotor *stepper, int speed);
Stepper_Error Stepper_Update(char name);
Stepper_Error Stepper_Init(
    char name,
    TIM_HandleTypeDef *htim, uint32_t Channel,
    GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin
);

#endif /* INC_STEPPER_H_ */
