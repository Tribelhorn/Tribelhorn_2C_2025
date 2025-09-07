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

int8_t  convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number) {
	uint32_t dato = data;

	for (int i=0; i<digits; i++) {
		float dato_sobre_10 = dato/10.0;
		int dato_entero_sobre_10 = dato_sobre_10;
		int dato_entero = dato_entero_sobre_10*10;
		int digito = dato - dato_entero;
		*(bcd_number+i) = digito;
		dato = dato_entero_sobre_10;
	};
	return 0;
}

/*==================[external functions definition]==========================*/
void app_main(void){
	uint32_t data=123;
	uint8_t num_digitos=3;
	uint8_t digitos[num_digitos];
	convertToBcdArray(data, num_digitos, digitos);
	for (int i=0; i<num_digitos; i++) {
		printf("%d\n", digitos[i]);
	};
}

/*==================[end of file]============================================*/