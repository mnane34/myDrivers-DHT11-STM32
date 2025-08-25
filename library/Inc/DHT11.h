#ifndef INC_DHT11_H_
#define INC_DHT11_H_

/* Header Directives Definitions */
#include "main.h"
#include "GPIO.h"
#include "TIME.h"

/* DHT11 Thresholding Definitions */
#define TIMEOUT_US 120U

/* DHT11 Sensor State Definitions */
typedef enum {
    SENSOR_ERROR = 0,
    SENSOR_SUCCESS = 1
}SensorState_t;

/* DHT11 Pin and Variables Definitions */
typedef struct{

	GPIO_TypeDef* DATA_GPIOx;
	uint16_t DATA_PINx;

	uint8_t RH_Bytes[2];
	uint8_t Temp_Bytes[2];
	uint8_t sumBytes;

	float temperature;
	float humidity;
}DHT11_TypeDef_t;

/* Function Header Definitions */
void DHT11_init(GPIO_TypeDef* DATA_GPIO, uint16_t DATA_PIN);
void DHT11_start(void);
SensorState_t DHT11_checkResponse(void);
uint8_t DHT11_readByte(SensorState_t state);
void DHT11_readFrame(SensorState_t state);

#endif
