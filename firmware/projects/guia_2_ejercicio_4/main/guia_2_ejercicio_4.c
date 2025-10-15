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

#include "analog_io_mcu.h"
#include "uart_mcu.h"
#include "timer_mcu.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*==================[macros and definitions]=================================*/

#define PERIODO_MUESTREO 2000 //2 ms//

#define PERIODO_ECG 4000 

#define BUFFER_SIZE 231

/*==================[internal data definition]===============================*/

TaskHandle_t main_task_handle = NULL;

const char ecg[BUFFER_SIZE] = {
    76, 77, 78, 77, 79, 86, 81, 76, 84, 93, 85, 80,
    89, 95, 89, 85, 93, 98, 94, 88, 98, 105, 96, 91,
    99, 105, 101, 96, 102, 106, 101, 96, 100, 107, 101,
    94, 100, 104, 100, 91, 99, 103, 98, 91, 96, 105, 95,
    88, 95, 100, 94, 85, 93, 99, 92, 84, 91, 96, 87, 80,
    83, 92, 86, 78, 84, 89, 79, 73, 81, 83, 78, 70, 80, 82,
    79, 69, 80, 82, 81, 70, 75, 81, 77, 74, 79, 83, 82, 72,
    80, 87, 79, 76, 85, 95, 87, 81, 88, 93, 88, 84, 87, 94,
    86, 82, 85, 94, 85, 82, 85, 95, 86, 83, 92, 99, 91, 88,
    94, 98, 95, 90, 97, 105, 104, 94, 98, 114, 117, 124, 144,
    180, 210, 236, 253, 227, 171, 99, 49, 34, 29, 43, 69, 89,
    89, 90, 98, 107, 104, 98, 104, 110, 102, 98, 103, 111, 101,
    94, 103, 108, 102, 95, 97, 106, 100, 92, 101, 103, 100, 94, 98,
    103, 96, 90, 98, 103, 97, 90, 99, 104, 95, 90, 99, 104, 100, 93,
    100, 106, 101, 93, 101, 105, 103, 96, 105, 112, 105, 99, 103, 108,
    99, 96, 102, 106, 99, 90, 92, 100, 87, 80, 82, 88, 77, 69, 75, 79,
    74, 67, 71, 78, 72, 67, 73, 81, 77, 71, 75, 84, 79, 77, 77, 76, 76,
};

uint16_t dato_ecg = ecg[0];

uint16_t dato_recibido = 0;	

uint16_t indice = 0;

TaskHandle_t muestrear_task_handle = NULL;	

TaskHandle_t ecg_task_handle = NULL;	

/*==================[internal functions declaration]=========================*/

static void tarea_muestreo(void *pvParameter) {
	while (1) {
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		char letra[10];
		AnalogInputReadSingle(CH1, &dato_recibido);		//Recibo el dato de CH1//
		sprintf(letra, "%u\r\n", dato_recibido);  
		UartSendString(UART_PC, letra);	
	}	
}

static void tarea_ecg(void *pvParameter) {
	while (1) {
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		AnalogOutputWrite(dato_ecg);
		indice++ ;

		if (indice > BUFFER_SIZE) {
			indice = 0;
		}

		dato_ecg = ecg[indice];	
	}
}


void Fun_Muestreo(void* param) {
	vTaskNotifyGiveFromISR(muestrear_task_handle, pdFALSE);		
};

void Fun_ECG(void* param) {
	vTaskNotifyGiveFromISR(ecg_task_handle, pdFALSE);
};

/*==================[external functions definition]==========================*/

void app_main(void) {

	timer_config_t timer_muestreo = {		//Esta struct la define el fabricante del micro//
        .timer = TIMER_A,					//Hay 3 timers disponibles para usar
        .period = PERIODO_MUESTREO,			//Periodo en microsegundos 
        .func_p = Fun_Muestreo,				//El nombre de una función es un puntero//
        .param_p = NULL
    };

	timer_config_t timer_ecg = {
		.timer = TIMER_B,
		.period = PERIODO_ECG,
		.func_p = Fun_ECG,
		.param_p = NULL
	};
		
	analog_input_config_t canal_1 = {
		.input = CH1,
		.mode = ADC_SINGLE,				
		.func_p = NULL,
		.param_p = NULL,
		.sample_frec = 0
	};	

	serial_config_t puerto = {
	 	.port = UART_PC,				//Para comunicarse por el UART-USB//
	 	.baud_rate = 115200,				//Clock
	 	.func_p = NULL,			//Ésta función se ejecutará cada vez llegue unn dato//
	 	.param_p = NULL
	 };

	UartInit(&puerto);

	AnalogInputInit(&canal_1); 

	AnalogOutputInit();

	xTaskCreate(&tarea_muestreo, "Muestrear", 2048, NULL, 5, &muestrear_task_handle); 		
    xTaskCreate(&tarea_ecg, "ECG", 512, NULL, 5, &ecg_task_handle);	

	TimerInit(&timer_muestreo);	//Le paso el primer elemento de la struct

	TimerStart(timer_muestreo.timer);

	TimerInit(&timer_ecg);	

	TimerStart(timer_ecg.timer);
};


/*==================[end of file]============================================*/