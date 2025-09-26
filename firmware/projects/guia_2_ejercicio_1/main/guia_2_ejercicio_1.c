/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>

#include "led.h"
#include "switch.h"
#include "gpio_mcu.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lcditse0803.h"
#include "hc_sr04.h"


/*==================[macros and definitions]=================================*/


/*==================[internal data definition]===============================*/

uint8_t FLAG_1 = 0;

uint8_t FLAG_2 = 1;

uint16_t DISTANCIA = 0;

TaskHandle_t medir_task_handle = NULL;
TaskHandle_t mostrar_task_handle = NULL;
TaskHandle_t leer_boton_task_handle = NULL;

/*==================[internal functions declaration]=========================*/



static void tarea_medir(void *pvParameter) {
	while (1) {
		if (FLAG_1) {
			DISTANCIA = HcSr04ReadDistanceInCentimeters();	/* El sensor toma la distancia */
		}

		else {
			DISTANCIA = 0;									/* Si la bandera está apagada */
		}

		vTaskDelay(1000/ portTICK_PERIOD_MS);				/* Retraso para que no se quede colgado */
	}
};

static void tarea_mostrar(void *pvParameter) {
	while (1){

		if (DISTANCIA > 30)	{								/* EValua si la distancia es mayor a DISTANCIA */
			LedOn(LED_1);
			LedOn(LED_2);			
			LedOn(LED_3);
		}

		else if (DISTANCIA > 20) {
			LedOn(LED_1);
			LedOn(LED_2);
			LedOff(LED_3);
		}

		else if (DISTANCIA > 10)	{
			LedOn(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
		}

		else if (DISTANCIA < 10)	{
			LedOff(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
		}

		if (FLAG_2){
			LcdItsE0803Write(DISTANCIA);					/* Si la flag 2 está encendida actualiza la distancia en el LCD */
		}

		vTaskDelay(1000/ portTICK_PERIOD_MS);	
	}
};

static void tarea_leer_boton(void *pvParameter) {
	while (1) {
		uint8_t teclas = SwitchesRead();					/* Lee las teclas */
		switch(teclas){
			case SWITCH_1:
				FLAG_1 = !FLAG_1;			
				vTaskDelay(100/ portTICK_PERIOD_MS);		/* Niega la bandera actual */
			break;

			case SWITCH_2:
				FLAG_2 = !FLAG_2;
				vTaskDelay(100/ portTICK_PERIOD_MS);
			break;
		}
		vTaskDelay(50/ portTICK_PERIOD_MS);
	}
};


/*==================[external functions definition]==========================*/

void app_main(void) {
	
	HcSr04Init(GPIO_3, GPIO_2);
	LedsInit();
	LcdItsE0803Init();
	SwitchesInit();
	xTaskCreate(&tarea_medir, "Medir", 512, NULL, 5, NULL);
    xTaskCreate(&tarea_mostrar, "Mostrar", 512, NULL, 5, NULL);
    xTaskCreate(&tarea_leer_boton, "Leer Boton", 512, NULL, 5, NULL);
}

/*==================[end of file]============================================*/