/**
 * @file    DHT11.h
 * @brief   DHT11 Sensor Driver Development  for STM32
 * @author  Mertcan NANE
 * @date    2025-08-09
 * @version 1.0
 *
 * @contact
 *  https://github.com/mnane34
 *  https://www.youtube.com/@mnane34
 *  https://www.linkedin.com/in/mertcan-nane/
 */

#ifndef INC_DHT11_H_
#define INC_DHT11_H_

#include "main.h"
#include "GPIO.h"
#include "TIME.h"

/* HCSR-04 Sensor Thresholding Definitions */
#define TIMEOUT_US 120U

/* HCSR-04 Sensor State Definitions */
typedef enum {
    SENSOR_ERROR = 0,
    SENSOR_SUCCESS = 1
}SensorState_t;

typedef struct{

	volatile GPIO_TypeDef* DATA_GPIOx;
	volatile uint16_t DATA_PINx;

	float temperature;
	float humidity;

	int8_t sensorState;
	uint8_t RH_Bytes[2];
	uint8_t Temp_Bytes[2];
	uint8_t sumBytes;

}DHT11_TypeDef_t;

/* Function Header Definitions */
void DHT11_init(volatile GPIO_TypeDef* DATA_GPIO, volatile uint16_t DATA_PIN);
void DHT11_Start(void);
SensorState_t DHT11_checkResponse(void);
uint8_t DHT11_read(SensorState_t state);

#endif
