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
#include "gpio_mcu.h"

/*==================[macros and definitions]=================================*/

typedef struct
{
	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;

gpioConf_t gpios_lcd[4];


/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/**
 * @brief Enciende o apaga los pines GPIO según un dígito BCD.
 *
 * Esta función recibe un dígito en formato BCD (4 bits) y un arreglo de estructuras
 * gpioConf_t que representan los pines físicos del microcontrolador.  
 * Cada bit del dígito determina si el pin correspondiente se enciende (1) o apaga (0).
 *
 * @param bcd Dígito BCD (4 bits) a mostrar en los GPIOs.
 * @param gpios Puntero a un arreglo de estructuras gpioConf_t que contiene
 *              la configuración de los pines (pin y dirección). Se esperan 4 elementos.
 *
 * @note La función inicializa cada pin con GPIOInit antes de encender/apagar.  
 *       Se debe asegurar que el arreglo gpios tenga al menos 4 elementos válidos.
 *
 * @example
 * gpioConf_t gpios[4] = {
 *     {GPIO_20, GPIO_OUTPUT},
 *     {GPIO_21, GPIO_OUTPUT},
 *     {GPIO_22, GPIO_OUTPUT},
 *     {GPIO_23, GPIO_OUTPUT}
 * };
 * escribirGPIO(5, gpios);  // Muestra el número 5 en binario: 0101
 */
void escribirGPIO(uint8_t bcd, gpioConf_t* gpios) {

    for (int i = 0; i < 4; i++) {			   
        GPIOInit(gpios[i].pin, gpios[i].dir);  // Inicializa el pin

        if (bcd & (1 << i)) {                  // Recorre cada bit del bcd preguntando si está en 1
            GPIOOn(gpios[i].pin);              // Enciende el pin
        } else {                               // Si el bit i está en 0
            GPIOOff(gpios[i].pin);             // Apaga el pin
        }
    }
}

/*==================[external functions definition]==========================*/

void app_main(void) {

	for (int i = 0; i < 4; i++) {
		gpios_lcd[i].dir = GPIO_OUTPUT;
	};

	gpios_lcd[0].pin = GPIO_20;
	gpios_lcd[1].pin = GPIO_21;
	gpios_lcd[2].pin = GPIO_22;
	gpios_lcd[3].pin = GPIO_23;

	uint32_t numero = 7;

	escribirGPIO(numero, gpios_lcd);
}

/*==================[end of file]============================================*/