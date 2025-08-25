/* Header Directives Definitions */
#include "DHT11.h"

/* Structure Object Definitions */
DHT11_TypeDef_t DHT11;
SensorState_t sensorState = SENSOR_ERROR;

/**
 * @brief  Initializes the DHT11 sensor interface. Configures the data pin as an output and sets it HIGH (idle state).
 * @param  DATA_GPIO Pointer to the GPIO port to which the DHT11 data pin is connected (e.g., GPIOA).
 * @param  DATA_PIN  Pin mask corresponding to the DHT11 data pin (e.g., GPIO_PIN_5).
 * @note   This function also enables the GPIOA clock by default. If the sensor is connected to a different port, adjust the RCC clock enable line accordingly.
 */
void DHT11_init(GPIO_TypeDef* DATA_GPIO, uint16_t DATA_PIN){

	DHT11.DATA_GPIOx = DATA_GPIO;
	DHT11.DATA_PINx = DATA_PIN;

	GPIO_INIT_PIN_OUTPUT(DHT11.DATA_GPIOx, DHT11.DATA_PINx); // start as output for initilization
	GPIO_SET_PIN(DHT11.DATA_GPIOx, DHT11.DATA_PINx);
}

/**
 * @brief  Sends the start signal to the DHT11 sensor.  Pulls the data line LOW for at least 18 ms, then switches to input mode to await the sensor's response.
 * @param void
 * @note   The sequence timing is critical for proper communication with the DHT11. The function includes the necessary delays for the start sequence.
 */
void DHT11_start(void){

	GPIO_INIT_PIN_OUTPUT(DHT11.DATA_GPIOx, DHT11.DATA_PINx); // output to input transition
	GPIO_RESET_PIN(DHT11.DATA_GPIOx, DHT11.DATA_PINx);
	DELAY_MS(18);
	GPIO_INIT_PIN_INPUT(DHT11.DATA_GPIOx, DHT11.DATA_PINx);
	DELAY_US(30);
}

/**
 * @brief  Waits for and verifies the DHT11 sensor's response after the start signal.
 * @param void
 * @retval SENSOR_SUCCESS if the sensor responded correctly.
 * @retval SENSOR_ERROR   if the sensor response was invalid or missing.
 * @note   The DHT11 should pull the line LOW for ~80 µs, then HIGH for ~80 µs to indicate readiness.
 */
SensorState_t DHT11_checkResponse(void){

	DELAY_US(40);
	if(GPIO_READ_PIN(DHT11.DATA_GPIOx, DHT11.DATA_PINx) == 0){ // control low to high transition
		DELAY_US(80);
		if(GPIO_READ_PIN(DHT11.DATA_GPIOx, DHT11.DATA_PINx) == 1){
			sensorState = SENSOR_SUCCESS;
		}
		else{
			sensorState = SENSOR_ERROR;
		}
	}
	while(GPIO_READ_PIN(DHT11.DATA_GPIOx, DHT11.DATA_PINx) == 1); // wait end of high time
	return sensorState;
}

/**
 * @brief  Reads a single byte from the DHT11 sensor.
 * @param  state The current sensor state from the response check (SENSOR_SUCCESS or SENSOR_ERROR).
 * @retval The byte read from the sensor if state == SENSOR_SUCCESS.
 * @retval SENSOR_ERROR if a timeout occurs or if state != SENSOR_SUCCESS.
 * @note   Each bit is determined by timing the HIGH pulse duration:
 *         ~26–28 µs HIGH = bit 0, ~70 µs HIGH = bit 1.
 *         This function implements timeouts to avoid blocking if the signal is lost.
 */
uint8_t DHT11_readByte(SensorState_t state){

	if(state == SENSOR_SUCCESS){

		uint8_t readingByte = 0, bitCounter = 0;
		uint32_t timeCounter = 0;

		for(bitCounter=0; bitCounter<8; bitCounter++){
			timeCounter = 0;
			while(GPIO_READ_PIN(DHT11.DATA_GPIOx, DHT11.DATA_PINx) == 0){ // wait low time
				if(timeCounter > TIMEOUT_US){
					return SENSOR_ERROR;
				}
				timeCounter++;
				DELAY_US(1);
			}
			DELAY_US(40);
			if(GPIO_READ_PIN(DHT11.DATA_GPIOx, DHT11.DATA_PINx) == 0){ // wait high time
				readingByte &= ~(1U << (7-bitCounter));
			}
			else{
				readingByte |= (1U << (7-bitCounter));
			}
			timeCounter = 0;
			while(GPIO_READ_PIN(DHT11.DATA_GPIOx, DHT11.DATA_PINx) == 1){ // wait end of high time
				if(timeCounter > TIMEOUT_US){
					return SENSOR_ERROR;
				}
				timeCounter++;
				DELAY_US(1);
			}
		}
		return readingByte;
	}
	else{
		return SENSOR_ERROR;
	}
}

/**
 * @brief  Reads a full 5-byte data frame from the DHT11 sensor.
 * @param  state  Current sensor state (SENSOR_SUCCESS or SENSOR_ERROR)
 * @note   The DHT11 requires at least 1 second between two measurements. This function enforces this by including a 1200 ms delay at the end.
 */
void DHT11_readFrame(SensorState_t state){

	  DHT11.RH_Bytes[0] = DHT11_readByte(sensorState); // read sensor byte to byte
	  DHT11.RH_Bytes[1] = DHT11_readByte(sensorState);
	  DHT11.Temp_Bytes[0] = DHT11_readByte(sensorState);
	  DHT11.Temp_Bytes[1] = DHT11_readByte(sensorState);
	  DHT11.sumBytes = DHT11_readByte(sensorState);

	  DHT11.temperature = DHT11.Temp_Bytes[0];
	  DHT11.humidity = DHT11.RH_Bytes[0];
	  DELAY_MS(1200);
}
