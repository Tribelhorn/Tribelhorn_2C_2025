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

gpioConf_t gpios_multiplexor[3];    //Se define otra lista con 3 structs//

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/**
 * @brief Convierte un número entero en un arreglo de dígitos BCD.
 *
 * Esta función toma un número entero de 32 bits y lo descompone en sus dígitos decimales,
 * almacenándolos en un arreglo pasado por puntero.
 *
 * @param data Número entero de 32 bits a convertir (uint32_t).
 * @param digits Cantidad de dígitos que se desean extraer (uint8_t).
 * @param bcd_number Puntero a un arreglo donde se almacenarán los dígitos resultantes.
 *
 * @note La función sobrescribe los elementos del arreglo bcd_number. Se debe asegurar
 *       que el arreglo tenga al menos 'digits' posiciones.
 *
 * @example
 * uint8_t arreglo[3];
 * convertToBcdArray(123, 3, arreglo);
 * // arreglo[0] = 1, arreglo[1] = 2, arreglo[2] = 3
 */
void convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number) {

    for (int i = (digits-1); i > -1; i--) {			 // Recorre la el arreglo desde el final 
        uint32_t dato_sobre_10 = data / 10;          // Divide el número por 10
        uint32_t dato_entero = dato_sobre_10 * 10;   // Obtiene la parte entera multiplicando por 10 para truncarlo
        uint8_t digito = data - dato_entero;         // Se hace la diferencia para obtener el dígito menos significativo
        *(bcd_number + i) = digito;                  // Guarda el dígito en el arreglo
        data = dato_sobre_10;                        // Cambia el dato para la siguiente iteración
    }
}

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

/**
 * @brief Muestra un número en un display de 7 segmentos multiplexado.
 *
 * Esta función toma un número entero y lo descompone en dígitos BCD usando
 * convertToBcdArray. Luego, enciende los pines correspondientes a cada dígito
 * y activa los multiplexores para mostrar cada dígito en el display.
 *
 * @param gpio_digito Puntero a un arreglo de gpioConf_t que representa los pines
 *                    de los segmentos del display (se espera un arreglo de 4 elementos).
 * @param gpio_mux Puntero a un arreglo de gpioConf_t que representa los pines
 *                 de control del multiplexor (cada pin activa un dígito del display).
 * @param numero Número entero que se desea mostrar en el display.
 * @param digitos Cantidad de dígitos que tiene el número a mostrar.
 *
 * @note La función asume que el número de dígitos especificado no excede la cantidad
 *       de pines de multiplexor disponibles en gpio_mux. No realiza retardo entre
 *       activaciones de dígitos, por lo que la frecuencia de refresco depende
 *       del tiempo de ejecución del código que llama a la función.
 *
 * @example
 * gpioConf_t segmentos[4] = {
 *     {GPIO_20, GPIO_OUTPUT},
 *     {GPIO_21, GPIO_OUTPUT},
 *     {GPIO_22, GPIO_OUTPUT},
 *     {GPIO_23, GPIO_OUTPUT}
 * };
 * gpioConf_t multiplexor[3] = {
 *     {GPIO_19, GPIO_OUTPUT},
 *     {GPIO_18, GPIO_OUTPUT},
 *     {GPIO_9,  GPIO_OUTPUT}
 * };
 * mostrarNum(segmentos, multiplexor, 321, 3);
 */
void mostrarNum(gpioConf_t* gpio_digito, gpioConf_t* gpio_mux, uint32_t numero, uint8_t digitos) {
    uint8_t arreglo[digitos]; //Crea el arreglo con los digitos necesarios

    // Toma el número y lo agrega a un arreglo de dígitos BCD
    convertToBcdArray(numero, digitos, arreglo);

    // Inicializa los pines de control del multiplexor
    for (int i = 0; i < digitos; i++) {
        GPIOInit(gpio_mux[i].pin, gpio_mux[i].dir);
    }

    // Muestra cada dígito en su correspondiente multiplexor
    escribirGPIO(arreglo[0], gpio_digito);
    GPIOOn(gpio_mux[2].pin);	// Multiplexa el digito en el display
    GPIOOff(gpio_mux[2].pin);   

    escribirGPIO(arreglo[1], gpio_digito);
    GPIOOn(gpio_mux[1].pin);
    GPIOOff(gpio_mux[1].pin);

    escribirGPIO(arreglo[2], gpio_digito);
    GPIOOn(gpio_mux[0].pin);
    GPIOOff(gpio_mux[0].pin);
}

/*==================[external functions definition]==========================*/
void app_main(void){
	for (int i = 0; i < 4; i++) {           //Setea los GPIO como salidas //
		gpios_lcd[i].dir = GPIO_OUTPUT;
	};

	for (int i = 0; i < 3; i++) {
		gpios_multiplexor[i].dir = GPIO_OUTPUT;
	};

	gpios_lcd[0].pin = GPIO_20;             //Cada GPIO se mapea como se indica en la guia//
	gpios_lcd[1].pin = GPIO_21;
	gpios_lcd[2].pin = GPIO_22;
	gpios_lcd[3].pin = GPIO_23;

	gpios_multiplexor[0].pin = GPIO_19;
	gpios_multiplexor[1].pin = GPIO_18;
	gpios_multiplexor[2].pin = GPIO_9;


	uint32_t numero_prueba = 321;
	uint8_t size = 3;

	mostrarNum(gpios_lcd, gpios_multiplexor, numero_prueba, size);

}

/*==================[end of file]============================================*/