/*
 * unity_config.h
 *
 *  Created on: Jun 6, 2019
 *      Author: martin
 */

#ifndef UNITY_UNITY_CONFIG_H_
#define UNITY_UNITY_CONFIG_H_
#ifndef UNITY_OUTPUT_CHAR

#include <MKL25Z4.h>
#include <fsl_lpsci.h>

/*
 * Print one character to uart0
 */
static inline void UART0_WriteByte(uint8_t data)
{
	while (!(UART0->S1 & UART0_S1_TDRE_MASK))
	        {
	        }
	UART0->D = data;
}


#define UNITY_OUTPUT_CHAR(a) UART0_WriteByte(a)

#define UNITY_PRINT_EOL() UNITY_OUTPUT_CHAR('\r'); UNITY_OUTPUT_CHAR('\n')

#endif

#endif /* UNITY_UNITY_CONFIG_H_ */
