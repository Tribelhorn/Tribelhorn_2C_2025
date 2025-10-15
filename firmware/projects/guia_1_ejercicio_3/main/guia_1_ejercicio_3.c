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
#include "freertos/FreeRTOS.h"
/*==================[macros and definitions]=================================*/
#define ON 1
#define OFF 2
#define TOGGLE 3

#define CONFIG_BLINK_PERIOD 100	//Recomendado en la guia//


/*==================[internal data definition]===============================*/
struct leds {
    uint8_t mode;       //ON, OFF, TOGGLE//
	uint8_t n_led;        //indica el número de led a controlar//
	uint8_t n_ciclos;   //indica la cantidad de ciclos de encendido/apagado//
	uint16_t periodo;    //indica el tiempo de cada ciclo//
} my_leds; 


/*==================[internal functions declaration]=========================*/
void funcion_led(struct leds *ptr_struct) {			//Recibe una struct leds//
	uint16_t retardo = (ptr_struct->periodo)/100;	//Define el retardo como el periodo sobre 100//
	LedsInit();  {									//Inicializa los leds(debió hacerse en app_main)//
		if (ptr_struct->mode == ON) { 
			switch (ptr_struct->n_led) {
				case 1:
					LedOn(LED_1);
					break;

				case 2:
					LedOn(LED_2);
					break;
				
				case 3:
					LedOn(LED_3);
					break;

				default:
					break; 
				}
			}

		else if (ptr_struct->mode == OFF) { 
			switch (ptr_struct->n_led) 
				{
				case 1:
					LedOff(LED_1);
					break;

				case 2:
					LedOff(LED_2);
					break;
				
				case 3:
					LedOff(LED_3);
					break;

				default:
					break; 
				}
			}


		else if (ptr_struct->mode == TOGGLE ) {
			for (int i=0; i<(ptr_struct->n_ciclos); i++) {
				switch (ptr_struct->n_led) {
					case 1:
						LedToggle(LED_1);
						break;

					case 2:
						LedToggle(LED_2);
						break;
					
					case 3:
						LedToggle(LED_3);
						break;

					default:
						break;
					};

				for (int j=0; j < (retardo); j++) {							//Pasa el retardo en los ms que se indique en periodo//
					vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);	
				}
			}
		}

	}
}
;
in
/*==================[external functions definition]==========================*/
void app_main(void) {

	//Defino los parametros de la struct//
	my_leds.mode = 3;	
	my_leds.n_led = 1;
	my_leds.n_ciclos = 10;
	my_leds.periodo = 500;

	funcion_led(&my_leds);
}




/*==================[end of file]============================================*/