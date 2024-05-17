/*
 * stepper.c
 *
 *  Created on: May 4, 2024
 *      Author: helsis
 */

#include <stdio.h>

#include "stepper.h"

StepperMotor steppers[MAX_STEPPERS_COUNT];

static int steppers_count;
static uint32_t STEP_TIMER_CLOCK;
static uint32_t STEP_CONTROLLER_PERIOD_US;

StepperMotor* GetState(char name) {
	for(int i = 0; i < steppers_count; i++)
		if (steppers[i].name == name) return &steppers[i];

	return (StepperMotor*)NULL;
}

Stepper_Error Stepper_Init(
	char name,
	TIM_HandleTypeDef *htim, uint32_t Chanel,
	GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin
) {
	StepperMotor *stepper = GetState(name);

    if (stepper != NULL)
		return SERR_STATE_ALLREADY_EXIST;

    if (steppers_count == MAX_STEPPERS_COUNT)
    	return SERR_STATE_MAX_COUNT;

    STEP_TIMER_CLOCK = HAL_RCC_GetHCLKFreq();
    STEP_CONTROLLER_PERIOD_US =  1000000U /(STEP_TIMER_CLOCK / htim -> Init.Period);

    HAL_TIM_PWM_Stop(htim, Chanel);

    stepper 					= &steppers[steppers_count++];
    stepper -> name				= name;
    stepper -> status			= SS_STOPPED;

    stepper -> STEP_TIMER       = htim;
    stepper -> STEP_CHANNEL     = Chanel;

    stepper -> DIR_GPIO         = GPIOx;
    stepper -> DIR_PIN          = GPIO_Pin;

    stepper -> speed			= DEFAULT_MIN_SPS;
    stepper -> targetPosition	= 0;
    stepper -> currentPosition	= 0;

	stepper -> STEP_TIMER -> Instance -> ARR = htim -> Init.Period;
	stepper -> STEP_TIMER -> Instance -> DIER |= TIM_DIER_UIE;

    HAL_TIM_PWM_Init(stepper -> STEP_TIMER);

	printf ("============== Stepper: %c ==============\r\n", stepper -> name);
	printf ("  CPU Clock: %lu MHz StepperCtrl: %lu us\r\n", STEP_TIMER_CLOCK/1000000, STEP_CONTROLLER_PERIOD_US);
	printf ("=========================================\r\n\r\n");

    return SERR_OK;
}


Stepper_Error Stepper_SetSpeed(StepperMotor *stepper, int speed) {
    stepper -> speed = speed;

    uint32_t period = STEP_TIMER_CLOCK / stepper -> speed;
    uint32_t prescaler = 0;

    if (period > 0xFFFF) {
        prescaler = period/0xFFFF;
        period /= (prescaler + 1);
    }

    stepper -> STEP_TIMER -> Instance -> PSC = prescaler;
    stepper -> STEP_TIMER -> Instance -> ARR = period;
    stepper -> STEP_TIMER -> Instance -> EGR = TIM_EGR_UG;

	return SERR_OK;
}

Stepper_Error Stepper_SetPosition(char name, int position, int speed) {
	StepperMotor *stepper = GetState(name);

    if (stepper == NULL)
		return SERR_STATE_NOT_FOUND;

	HAL_TIM_PWM_Stop(stepper -> STEP_TIMER, stepper -> STEP_CHANNEL);

    stepper -> targetPosition = position;
	stepper -> status = stepper -> targetPosition > 0
			? SS_RUNNING_FORWARD
			: SS_RUNNING_BACKWARD;

	stepper -> DIR_GPIO -> BSRR = stepper -> DIR_PIN << (16U * stepper -> status);

	Stepper_SetSpeed(stepper, speed);

	HAL_TIM_PWM_Start(stepper -> STEP_TIMER, stepper -> STEP_CHANNEL);

	return SERR_OK;
}

Stepper_Error Stepper_Update(char name) {
	StepperMotor *stepper = GetState(name);

    if (stepper == NULL || stepper -> status & SS_STOPPED)
		return SERR_STATE_NOT_FOUND;

    if(stepper -> targetPosition != stepper -> currentPosition)
    	return stepper -> currentPosition += stepper -> status == SS_RUNNING_FORWARD ? 1 : -1;

	stepper -> status = SS_STOPPED;
    stepper -> STEP_TIMER -> Instance -> CR1 &= ~TIM_CR1_CEN;

	return SERR_OK;
}
