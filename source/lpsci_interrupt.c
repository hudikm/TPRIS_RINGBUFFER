/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <board.h>
#include <clock_config.h>
#include <fsl_clock.h>
#include <fsl_common.h>
#include <fsl_lpsci.h>
#include <MKL25Z4.h>
#include <pin_mux.h>
#include <RingBuffer.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_LPSCI UART0
#define DEMO_LPSCI_CLKSRC kCLOCK_CoreSysClk
#define DEMO_LPSCI_CLK_FREQ CLOCK_GetFreq(kCLOCK_CoreSysClk)
#define DEMO_LPSCI_IRQn UART0_IRQn
#define DEMO_LPSCI_IRQHandler UART0_IRQHandler

/*! @brief Ring buffer size (Unit: Byte). */
#define DEMO_RING_BUFFER_SIZE 16

#define RINGBUFFERWRITE() 	RbufferWrite(&rxBufferStruct, &data, 1); LPSCI_EnableInterrupts(DEMO_LPSCI, kLPSCI_TxDataRegEmptyInterruptEnable)

/*! @brief Ring buffer to save received data. */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

uint8_t g_tipString[] = "LPSCI functional API interrupt example\r\nBoard receives characters then sends them out\r\nNow please input:\r\n";
volatile bool new_line_flag = false;

buffer_t rxBuffer_handler;
buffer_t txBuffer_handler;
uint8_t rxBufferData[100];
uint8_t txBufferData[100];

/*******************************************************************************
 * Code
 ******************************************************************************/

void DEMO_LPSCI_IRQHandler(void) {
	uint8_t data;

	/* If new data arrived. */
	if ((kLPSCI_RxDataRegFullFlag) & LPSCI_GetStatusFlags(DEMO_LPSCI)) {

	}
	/*If there are data to send*/
	if ((kLPSCI_TxDataRegEmptyFlag & LPSCI_GetStatusFlags(DEMO_LPSCI))) {

	}

}

/*!
 * @brief Main function
 */

int main(void) {
	lpsci_config_t config;

	BOARD_InitPins();
	BOARD_BootClockRUN();
	CLOCK_SetLpsci0Clock(0x1U);

	/*
	 * config.parityMode = kLPSCI_ParityDisabled;
	 * config.stopBitCount = kLPSCI_OneStopBit;
	 * config.enableTx = false;
	 * config.enableRx = false;
	 */
	LPSCI_GetDefaultConfig(&config);
	config.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
	config.enableTx = true;
	config.enableRx = true;

	LPSCI_Init(DEMO_LPSCI, &config, DEMO_LPSCI_CLK_FREQ);

	/* Send g_tipString out. */
	LPSCI_WriteBlocking(DEMO_LPSCI, g_tipString, sizeof(g_tipString) / sizeof(g_tipString[0]));

	/* Setup RingBuffers */
	RbufferInit(&rxBuffer_handler, rxBufferData, sizeof(rxBufferData));
	RbufferInit(&txBuffer_handler, txBufferData, sizeof(txBufferData));

	/* Enable RX interrupt. */
	LPSCI_EnableInterrupts(DEMO_LPSCI, kLPSCI_RxDataRegFullInterruptEnable);
	EnableIRQ(DEMO_LPSCI_IRQn);

	while (1) {

		// Wait for new line
		while (!new_line_flag) {
		};


	}
}
