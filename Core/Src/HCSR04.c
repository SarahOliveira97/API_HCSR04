/*
 * ***************************************************************
 *  API name    : HCSR04
 *  API files   : HCSR04.c, HCSR04.h
 *  file		: HCSR04.c
 *  authors		: Italo Dias, Sarah Oliveira
 *  university  : Federal University of Minas Gerais
 *  license     : GNU General Public License v3.0
 *  date        : 02/18/21
 *  modified	: 02/18/21
 *  This code implements a library for HCSR04
 *
 *  This API was developed as an assignment for Embedded Systems
 *  Programming class at the Federal University of Minas Gerais
 ***************************************************************
 *  SOFTWARE SETUP:
 *  Include HCSR04.h in main.c
 *
 ***************************************************************
 *  HARDWARE SETUP:
 *
 *  VSS = GND
 *  VDD = 5v
 *  Trigger = - Connect in Digital Pin - Output
 *  Echo = - Connect in Digital Pin - Input
 *****************************************************************
 */

#include "HCSR04.h"

///************************************** Function definitions **************************************/

/**
  * @brief Reads ports and pins
  * @param[port_trig] HCSR04 trig port 
  * @param[pin_trig] HCSR04 trig pin
  * @param[port_echo] HCSR04 echo port 
  * @param[pin_echo] HCSR04 echo pin 
  * @retval  Value read by the sensor [ultrasonic]
  */
ultrasonic HCSR04_generate(GPIO_Port port_trig[],GPIO_Pin pin_trig,GPIO_Port port_echo[],GPIO_Pin pin_echo){
	ultrasonic hcsr04;
	hcsr04.trig_port = port_trig;
	hcsr04.trig_pin = pin_trig;
	hcsr04.echo_port = port_echo;
	hcsr04.echo_pin = pin_echo;
	return hcsr04;
}

/**
  * @brief ?
  * @param[microSec] ?
  * @retval None
  */
void uDelay(uint32_t microSec){
	if(microSec < 2){
		microSec = 2;
	}
	microTIM->ARR = microSec - 1;
	microTIM->EGR = 1;
	microTIM->SR &= ~1;
	microTIM->CR1 |= 1;
	while((microTIM->SR&0x0001) != 1);
	microTIM->SR &= ~(0x0001);
}

/**
  * @brief Converts the sensor reading to centimeters
  * @param[hcsr04] Value read by the sensor 
  * @retval Object distance from the sensor in cm [float]
  */
float distance_cm(ultrasonic hcsr04){
	uint32_t cont = 0;

	HAL_GPIO_WritePin(hcsr04.trig_port, hcsr04.trig_pin, GPIO_PIN_RESET);
	uDelay(2);

	HAL_GPIO_WritePin(hcsr04.trig_port, &hcsr04.trig_pin, GPIO_PIN_SET);
	uDelay(10);
	HAL_GPIO_WritePin(hcsr04.trig_port, hcsr04.trig_pin, GPIO_PIN_RESET);

	while(HAL_GPIO_ReadPin(hcsr04.echo_port,hcsr04.echo_pin) == GPIO_PIN_RESET);

	while(HAL_GPIO_ReadPin(hcsr04.echo_port,hcsr04.echo_pin) == GPIO_PIN_SET)
	{
		 cont++;
		 uDelay(2);
	};

	return (cont + 0.0f)*2.8*speedSound;
}

/**
  * @brief Converts the sensor reading to meters 
  * @param[hcsr04] Value read by the sensor 
  * @retval Object distance from the sensor in m [float]
  */
float distance_m(ultrasonic hcsr04){
	return distance_cm(hcsr04)/100;
}

/**
  * @brief Converts the sensor reading to millimeters
  * @param[hcsr04] Value read by the sensor 
  * @retval Object distance from the sensor in mm [float]
  */
float distance_mm(ultrasonic hcsr04){
	return distance_cm(hcsr04)*10;
}

/**
  * @brief Calculates the speed that the object approaches the sensor
  * @param[hcsr04] Value read by the sensor 
  * @param[time] ?
  * @retval Object speed in m/s [float]
  */
float forwardSpeed(ultrasonic hcsr04, float time){
	float x_i = distance_m(hcsr04);
	int delta_t = (int)1000*time;
	HAL_Delay(delta_t);
	float x_f = distance_m(hcsr04);
	return (x_f-x_i)/time;
}

/**
  * @brief Calculates the speed that the object ? the sensor
  * @param[hcsr04] Value read by the sensor 
  * @param[distance] ?
  * @param[underLimit] ?
  * @retval (Object speed in m/s) ? [float]
  */
float crossSpeed(ultrasonic hcsr04, float distance, float underLimit){
	int flag = 0;
	int cont = 0;
	while(1){
		if((distance_cm(hcsr04) > underLimit) & (flag == 0)){
			pass;
		}else if((distance_cm(hcsr04) < underLimit) & (flag == 0)){
			flag = 1;
		}else if((distance_cm(hcsr04) > underLimit) & (flag == 1)){
			cont++;
			uDelay(2);
		}else if((distance_cm(hcsr04) < underLimit) & (flag == 1)){
			break;
		}
	}
	return distance/((cont + 0.0f)*2.8);
}

/**
  * @brief Tests if the indentified object is within a distance range, in cm
  * @param[hcsr04] Value read by the sensor 
  * @param[underLimit] Lower distance in the range in cm
  * @param[upperLimit] Upper distance in the range in cm
  * @retval Object speed in m/s [float]
  */
bool itsBetween(ultrasonic hcsr04, float underLimit, float upperLimit){
	if((distance_cm(hcsr04) > underLimit) & (distance_cm(hcsr04) > upperLimit)){
		return true;
	}
	return false;
}
