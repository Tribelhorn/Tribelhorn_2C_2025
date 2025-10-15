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

//#define CONFIG_BLINK_PERIOD_LED_1_US 1000000//

#define ONOFF 79		//Comandos en ascii//
#define HOLD 72
#define INCHES 73
#define MAXIMO 77
#define FAST 70
#define SLOW 83

/*==================[internal data definition]===============================*/

uint8_t FLAG_O = 1;		//Bandera de encendido/apagado //

uint8_t FLAG_H = 1;		//Bandera de holdeo

uint8_t FLAG_I = 0;	

uint8_t FLAG_M = 0;

uint16_t DISTANCIA = 0;

uint32_t PERIODO = 1000000;

TaskHandle_t medir_task_handle = NULL;		//El handle es un manejador de tarea//
TaskHandle_t mostrar_task_handle = NULL;	

/*==================[internal functions declaration]=========================*/



static void tarea_medir(void *pvParameter) {
	while (1) {
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if (FLAG_O) {
			if (FLAG_I) {
				DISTANCIA = HcSr04ReadDistanceInInches(); 
			}
			else if (FLAG_M) {
				DISTANCIA = 300;		//Distancia máxima//
			}
			else {
				DISTANCIA = HcSr04ReadDistanceInCentimeters();	
			}	
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

		if (FLAG_H){
			LcdItsE0803Write(DISTANCIA);				/* Si la flag 2 está encendida actualiza la distancia en el LCD */
		}
	}
};

void leer_boton_1() {
	FLAG_O = !FLAG_O;									/* Niega la bandera actual */			
};

void leer_boton_2() {
	FLAG_H = !FLAG_H;									/* Niega la bandera actual */
};

void Fun_Timer(void* param){
    vTaskNotifyGiveFromISR(medir_task_handle, pdFALSE);		/* Envía una notificación a la tarea asociada a medir */
	vTaskNotifyGiveFromISR(mostrar_task_handle, pdFALSE);	/* Envía una notificación a la tarea asociada a mostrar */
};

void Func_Uart(void* param) {
	uint8_t caracter;
	UartReadByte (UART_PC, &caracter);
	switch (caracter) {
		case ONOFF:
			FLAG_O = !FLAG_O;												
			break;
	
		case HOLD:
			FLAG_H = !FLAG_H;											
			break;
		case INCHES:
			FLAG_I = !FLAG_I;
			break;
		case MAXIMO:
			FLAG_M = !FLAG_M;
			break;
		case FAST:	
			if (PERIODO<5000000) {					//Ésto no debe pasarse de los 32 bits//
				PERIODO = PERIODO+100000;
			}
			break;
		case SLOW:
			if (PERIODO>1000000) {					//Ésto no debe pasarse de los 32 bits//
				PERIODO = PERIODO-100000;
			}
			break;
	}
};

/*==================[external functions definition]==========================*/

void app_main(void) {

	HcSr04Init(GPIO_3, GPIO_2);
	LedsInit();
	LcdItsE0803Init();
	SwitchesInit();

	serial_config_t puerto = {
	 	.port = UART_PC,				//Para comunicarse por el UART-USB//
	 	.baud_rate = 9600,				//Clock
	 	.func_p = Func_Uart,			//Ésta función se ejecutará cada vez llegue unn dato//
	 	.param_p = NULL
	 };

	UartInit(&puerto);

	xTaskCreate(&tarea_medir, "Medir", 512, NULL, 5, &medir_task_handle); 			//El 5 es la prioridad//
    xTaskCreate(&tarea_mostrar, "Mostrar", 512, NULL, 5, &mostrar_task_handle);		//El puntero al handle permite permite asociarlo con su correspondiente tarea

	timer_config_t timer_1 = {				//Esta struct la define el fabricante del micro//
        .timer = TIMER_A,					//Hay 3 timers disponibles para usar
        .period = PERIODO,					//Periodo en microsegundos 
        .func_p = Fun_Timer,				//El nombre de una función es un puntero//
        .param_p = NULL
    };

    TimerInit(&timer_1);

    /* Inicialización del conteo de timers */
    TimerStart(timer_1.timer);

	SwitchActivInt(SWITCH_1, leer_boton_1, NULL);	//Interrupciones de cada botón
	SwitchActivInt(SWITCH_2, leer_boton_2, NULL);
}


/*==================[end of file]============================================*/