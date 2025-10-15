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

/*==================[macros and definitions]=================================*/

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

    for (int i = (digits-1); i > -1; i--) {			 // Recorre el arreglo desde el final //
        uint32_t dato_sobre_10 = data / 10;          // Divide el número por 10 //
        uint32_t dato_entero = dato_sobre_10 * 10;   // Obtiene la parte entera multiplicando por 10 para truncarlo //
        uint8_t digito = data - dato_entero;         // Se hace la diferencia para obtener el dígito menos significativo //
        *(bcd_number + i) = digito;                  // Guarda el dígito en el arreglo //
        data = dato_sobre_10;                        // Cambia el dato para la siguiente iteración //
    }
}


/*==================[external functions definition]==========================*/
void app_main(void){
	uint32_t data=123;
	uint8_t num_digitos=3;
	uint8_t digitos[num_digitos];                   //Crea una lista de num_digitos de largo//
	convertToBcdArray(data, num_digitos, digitos);
	for (int i=0; i<num_digitos; i++) {
		printf("%d\n", digitos[i]);
	};
}

/*==================[end of file]============================================*/