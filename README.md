# STM32-STEPPER_LIB
A simple and small library for controlling stepper drivers such as the TMC2208 in 3-pin mode

> Also need to manually configure the PWM pulsation according to your driver's documentation, for my example it is 4.2ns, i.e. sConfigOC.Pulse = 200;

## Example:

### TIM3
![image](https://github.com/H6LS1S/STM32-STEPPER_LIB/assets/13855054/5cc0a521-d191-4fb5-9d14-c71d5dd7c53c)
![image](https://github.com/H6LS1S/STM32-STEPPER_LIB/assets/13855054/80194f58-e1e8-40ba-979e-03eb53d92c22)

### TMI16
![image](https://github.com/H6LS1S/STM32-STEPPER_LIB/assets/13855054/ccf508ae-9c6c-4179-a3ff-d685278cbc48)
![image](https://github.com/H6LS1S/STM32-STEPPER_LIB/assets/13855054/76466aa7-fc59-45e3-b919-898c96c96d5c)

### GPIO
![image](https://github.com/H6LS1S/STM32-STEPPER_LIB/assets/13855054/e7f6574c-5bf1-4a97-b094-ce8e8d7f5043)

### Code
```c
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stepper.h"
/* USER CODE END Includes */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  MX_TIM16_Init();
  /* USER CODE BEGIN 2 */

  Stepper_Init('X', &htim3, TIM_CHANNEL_4, STEPPER_X_DIR_GPIO_Port, STEPPER_X_DIR_Pin);
  Stepper_Init('Y', &htim16, TIM_CHANNEL_1, STEPPER_Y_DIR_GPIO_Port, STEPPER_Y_DIR_Pin);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  HAL_Delay(2000);

	  Stepper_SetPosition('X', 1600, 6400);
	  Stepper_SetPosition('Y', 1600, 6400);

	  HAL_Delay(2000);

	  Stepper_SetPosition('X', 0, 3200);
	  Stepper_SetPosition('Y', 0, 3200);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}


/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles TIM3 global interrupt.
  */
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */
  if (__HAL_TIM_GET_FLAG(&htim3, TIM_FLAG_UPDATE))
	if (__HAL_TIM_GET_ITSTATUS(&htim3, TIM_IT_UPDATE)) {
	      __HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_UPDATE);
	      Stepper_Update('X');
	}
  /* USER CODE END TIM3_IRQn 0 */
  HAL_TIM_IRQHandler(&htim3);
  /* USER CODE BEGIN TIM3_IRQn 1 */

  /* USER CODE END TIM3_IRQn 1 */
}

/**
  * @brief This function handles TIM16 global interrupt.
  */
void TIM16_IRQHandler(void)
{
  /* USER CODE BEGIN TIM16_IRQn 0 */
	  if (__HAL_TIM_GET_FLAG(&htim16, TIM_FLAG_UPDATE))
		if (__HAL_TIM_GET_ITSTATUS(&htim16, TIM_IT_UPDATE)) {
		      __HAL_TIM_CLEAR_FLAG(&htim16, TIM_FLAG_UPDATE);
		      Stepper_Update('Y');
		}
  /* USER CODE END TIM16_IRQn 0 */
  HAL_TIM_IRQHandler(&htim16);
  /* USER CODE BEGIN TIM16_IRQn 1 */

  /* USER CODE END TIM16_IRQn 1 */
}

```
