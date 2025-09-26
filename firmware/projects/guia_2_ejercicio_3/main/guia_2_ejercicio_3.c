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
#include "timer_mcu.h"
#include "uart_mcu.h"



/*==================[macros and definitions]=================================*/

#define CONFIG_BLINK_PERIOD_LED_1_US 1000000

/*==================[internal data definition]===============================*/

uint8_t FLAG_1 = 1;

uint8_t FLAG_2 = 1;

uint16_t DISTANCIA = 0;

TaskHandle_t medir_task_handle = NULL;		//El handle es un manejador de tarea//
TaskHandle_t mostrar_task_handle = NULL;	

/*==================[internal functions declaration]=========================*/



static void tarea_medir(void *pvParameter) {
	while (1) {
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (FLAG_1) {
			DISTANCIA = HcSr04ReadDistanceInCentimeters();	/* El sensor toma la distancia */
		}

		else {
			DISTANCIA = 0;									/* Si la bandera está apagada */
		}
	}
};

static void tarea_mostrar(void *pvParameter) {
	while (1){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
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
			LcdItsE0803Write(DISTANCIA);				/* Si la flag 2 está encendida actualiza la distancia en el LCD */
		}
	}
}

void leer_boton_1() {
	FLAG_1 = !FLAG_1;									/* Niega la bandera actual */			
};

void leer_boton_2() {
	FLAG_2 = !FLAG_2;									/* Niega la bandera actual */
};

void Fun_Timer(void* param){
    vTaskNotifyGiveFromISR(medir_task_handle, pdFALSE);		/* Envía una notificación a la tarea asociada a medir */
	vTaskNotifyGiveFromISR(mostrar_task_handle, pdFALSE);	/* Envía una notificación a la tarea asociada a mostrar */
};


/*==================[external functions definition]==========================*/

void app_main(void) {

	HcSr04Init(GPIO_3, GPIO_2);
	LedsInit();
	LcdItsE0803Init();
	SwitchesInit();
	UartInit();

	xTaskCreate(&tarea_medir, "Medir", 512, NULL, 5, &medir_task_handle); 			//El 5 es la prioridad//
    xTaskCreate(&tarea_mostrar, "Mostrar", 512, NULL, 5, &mostrar_task_handle);		////

	timer_config_t timer_1 = {
        .timer = TIMER_A,
        .period = CONFIG_BLINK_PERIOD_LED_1_US,
        .func_p = Fun_Timer,						//El nombre de una función es un puntero//
        .param_p = NULL
    };

    TimerInit(&timer_1);

    												/* Inicialización del conteo de timers */
    TimerStart(timer_1.timer);

	SwitchActivInt(SWITCH_1, leer_boton_1, NULL);
	SwitchActivInt(SWITCH_2, leer_boton_2, NULL);
}


/*==================[end of file]============================================*/