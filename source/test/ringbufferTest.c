/*
 * ringbufferTest.c
 *
 *  Created on: Oct 17, 2019
 *      Author: martin
 */

#include <board.h>
#include <clock_config.h>
#include <fsl_clock.h>
#include <fsl_lpsci.h>
#include <MKL25Z4.h>
#include <pin_mux.h>
#include <RingBuffer.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/libconfig-arm.h>
#include <unity/unity.h>
#include <unity/unity_internals.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEBUG_LPSCI UART0
#define DEBUG_LPSCI_CLKSRC kCLOCK_CoreSysClk
#define DEBUG_LPSCI_CLK_FREQ CLOCK_GetFreq(kCLOCK_CoreSysClk)

extern uint8_t * incrementAddress(buffer_t *buffer, uint8_t* ptr);

#define BUFF_SIZE UINT16_C(100)
#define BUFF_HALF_SIZE BUFF_SIZE/2
#define BUFF_DOUBLE_SIZE BUFF_SIZE*2

static buffer_t rxBuffer_handler;

struct {
	uint8_t beforBuffer[2];
	uint8_t rxBufferData[BUFF_SIZE];
	uint8_t afterBuffer[2];
} rxBufferStruct;

static void rand_string(uint8_t *str, size_t size) {
	const uint8_t charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK...";
	if (size) {
		--size;
		for (size_t n = 0; n < size; n++) {
			int key = rand() % (int) (sizeof charset - 1);
			str[n] = charset[key];
		}
		str[size] = '\0';
	}
}

void setUp() {
	memset(&rxBuffer_handler, 0, sizeof(rxBuffer_handler));
	memset(&rxBufferStruct, 0xBA, BUFF_SIZE + 4);
	RbufferInit(&rxBuffer_handler, rxBufferStruct.rxBufferData, BUFF_SIZE);
}

void tearDown() {

}

void test_InitTest(void) {
	TEST_ASSERT_EACH_EQUAL_UINT8(0xBA, &rxBufferStruct, BUFF_SIZE+4);
	TEST_ASSERT_EQUAL_PTR(rxBufferStruct.rxBufferData, rxBuffer_handler.buf);
	TEST_ASSERT_EQUAL_PTR(rxBufferStruct.rxBufferData, rxBuffer_handler.rPt);
	TEST_ASSERT_EQUAL_PTR(rxBufferStruct.rxBufferData, rxBuffer_handler.wPt);
	TEST_ASSERT(rxBuffer_handler.overflow==false);
}

void test_Empty(void) {
	TEST_ASSERT_EQUAL(0, RbufferIsFull(&rxBuffer_handler));
	TEST_ASSERT_EQUAL(BUFF_SIZE, RbufferCapacity(&rxBuffer_handler));
}

void test_OverFlow(void) {
	TEST_ASSERT_EACH_EQUAL_UINT8_MESSAGE(0xBA, rxBufferStruct.beforBuffer, 2, "OverFlow below buffer");
	TEST_ASSERT_EACH_EQUAL_UINT8_MESSAGE(0xBA, rxBufferStruct.afterBuffer, 2, "OverFlow after buffer");
}

void test_IncrementAddress(void) {

	//Test increment standard
	uint8_t * newPt = incrementAddress(&rxBuffer_handler, rxBuffer_handler.buf);
	TEST_ASSERT_EQUAL_PTR(rxBuffer_handler.buf + 1, newPt);

	//Test overflow of address
	newPt = incrementAddress(&rxBuffer_handler, rxBuffer_handler.buf + BUFF_SIZE - 1);
	TEST_ASSERT_EQUAL_PTR(rxBuffer_handler.buf, newPt);
	rxBuffer_handler.wPt = newPt;

}

void test_WriteReadInit(void) {

	uint16_t count = RbufferWrite(&rxBuffer_handler, (uint8_t *) "TEST", sizeof("TEST"));

	TEST_ASSERT_EQUAL_UINT16((uint16_t )sizeof("TEST"), count);
	TEST_ASSERT_EQUAL_PTR(rxBufferStruct.rxBufferData, rxBuffer_handler.rPt);
	TEST_ASSERT_EQUAL_PTR(rxBufferStruct.rxBufferData + sizeof("TEST"), rxBuffer_handler.wPt);
	test_OverFlow();

	uint16_t nCount = RbufferNumOfElements(&rxBuffer_handler);
	TEST_ASSERT_EQUAL_UINT16((uint16_t )sizeof("TEST"), nCount);

	uint8_t readBuffer[nCount];
	RbufferRead(&rxBuffer_handler, readBuffer, count);
	TEST_ASSERT_EQUAL_STRING("TEST", readBuffer);
}

void test_WriteOverFlow(void) {

	static uint8_t bufferTest[BUFF_DOUBLE_SIZE];

	rand_string(bufferTest, BUFF_DOUBLE_SIZE);

	{
		// Write to first half of buffer
		uint16_t count = RbufferWrite(&rxBuffer_handler, bufferTest,
		BUFF_HALF_SIZE);
		TEST_ASSERT_EQUAL_UINT16(BUFF_HALF_SIZE, count);
		TEST_ASSERT_EQUAL_PTR(rxBufferStruct.rxBufferData, rxBuffer_handler.rPt);
		TEST_ASSERT_EQUAL_PTR(rxBufferStruct.rxBufferData + BUFF_HALF_SIZE, rxBuffer_handler.wPt);
		test_OverFlow();
	}

	{
		// Write to second half of buffer
		uint16_t count = RbufferWrite(&rxBuffer_handler, bufferTest,
		BUFF_HALF_SIZE);

		TEST_ASSERT_EQUAL_UINT16(BUFF_HALF_SIZE, count);
		TEST_ASSERT_EQUAL_PTR(rxBufferStruct.rxBufferData, rxBuffer_handler.rPt);
		TEST_ASSERT_EQUAL_PTR(rxBufferStruct.rxBufferData, rxBuffer_handler.wPt);
		test_OverFlow();

	}

	{	// Read the number of written bytes
		uint16_t nCount = RbufferNumOfElements(&rxBuffer_handler);
		TEST_ASSERT_EQUAL_UINT16(BUFF_SIZE, nCount);

		// Read the first half of buffer
		uint8_t readBuffer[nCount / 2];
		RbufferRead(&rxBuffer_handler, readBuffer, sizeof(readBuffer));
		TEST_ASSERT_EQUAL_UINT8_ARRAY(bufferTest, readBuffer, sizeof(readBuffer));
	}

	{   // Again write to first half of buffer
		uint16_t count = RbufferWrite(&rxBuffer_handler, bufferTest,
		BUFF_HALF_SIZE);

		TEST_ASSERT_EQUAL_UINT16(BUFF_HALF_SIZE, count);
		TEST_ASSERT_EQUAL_PTR(rxBufferStruct.rxBufferData + BUFF_HALF_SIZE, rxBuffer_handler.rPt);
		TEST_ASSERT_EQUAL_PTR(rxBufferStruct.rxBufferData + BUFF_HALF_SIZE, rxBuffer_handler.wPt);
		test_OverFlow();
	}

	{
		//Read the number of written bytes -> Should be BUFF_SIZE
		uint16_t nCount = RbufferNumOfElements(&rxBuffer_handler);
		TEST_ASSERT_EQUAL_UINT16(BUFF_SIZE, nCount);

		// Read all data from buffer
		uint8_t readBuffer[nCount];
		RbufferRead(&rxBuffer_handler, readBuffer, sizeof(readBuffer));
		TEST_ASSERT_EQUAL_UINT8_ARRAY(bufferTest, readBuffer, BUFF_HALF_SIZE);
		TEST_ASSERT_EQUAL_UINT8_ARRAY(bufferTest, readBuffer+BUFF_HALF_SIZE, BUFF_HALF_SIZE);
	}

}

void test_WriteToFullAndReadAll(void) {
	//Test overflow
	static uint8_t bufferTest[BUFF_SIZE];
	rand_string(bufferTest, BUFF_SIZE);
	{
		// Write buffer full
		uint16_t count = RbufferWrite(&rxBuffer_handler, bufferTest, BUFF_SIZE);

		TEST_ASSERT_EQUAL_UINT16(BUFF_SIZE, count);
		test_OverFlow();
		TEST_ASSERT(RBufferOverflow(&rxBuffer_handler)==false);
		TEST_ASSERT(RbufferIsFull(&rxBuffer_handler)==true);

		{	// Read the number of written bytes
			uint16_t nCount = RbufferNumOfElements(&rxBuffer_handler);
			TEST_ASSERT_EQUAL_UINT16(BUFF_SIZE, nCount);

			// Read all data from buffer
			uint8_t readBuffer[nCount];
			RbufferRead(&rxBuffer_handler, readBuffer, sizeof(readBuffer));
			TEST_ASSERT_EQUAL_UINT8_ARRAY(bufferTest, readBuffer, sizeof(readBuffer));
		}
		// Test status flags
		TEST_ASSERT(RBufferOverflow(&rxBuffer_handler)==false);
		TEST_ASSERT(RbufferIsFull(&rxBuffer_handler)==false);

	}

	// Second attempt to write full buffer
	rand_string(bufferTest, BUFF_SIZE);
	{
		// Write buffer full
		uint16_t count = RbufferWrite(&rxBuffer_handler, bufferTest, BUFF_SIZE);

		TEST_ASSERT_EQUAL_UINT16(BUFF_SIZE, count);
		test_OverFlow();
		TEST_ASSERT(RBufferOverflow(&rxBuffer_handler)==false);
		TEST_ASSERT(RbufferIsFull(&rxBuffer_handler)==true);

		{	// Read the number of written bytes
			uint16_t nCount = RbufferNumOfElements(&rxBuffer_handler);
			TEST_ASSERT_EQUAL_UINT16(BUFF_SIZE, nCount);

			// Read all data from buffer
			uint8_t readBuffer[nCount];
			RbufferRead(&rxBuffer_handler, readBuffer, sizeof(readBuffer));
			TEST_ASSERT_EQUAL_UINT8_ARRAY(bufferTest, readBuffer, sizeof(readBuffer));
		}
		// Test status flags
		TEST_ASSERT(RBufferOverflow(&rxBuffer_handler)==false);
		TEST_ASSERT(RbufferIsFull(&rxBuffer_handler)==false);

	}

}

void test_BufferOverFlow(void) {
	//Test overflow
	static uint8_t bufferTest[BUFF_DOUBLE_SIZE];
	rand_string(bufferTest, BUFF_DOUBLE_SIZE);
	{
		//Write buffer overflow
		uint16_t count = RbufferWrite(&rxBuffer_handler, bufferTest, BUFF_DOUBLE_SIZE);

		TEST_ASSERT_EQUAL_UINT16(BUFF_DOUBLE_SIZE, count);
		test_OverFlow();
		TEST_ASSERT(RBufferOverflow(&rxBuffer_handler)==true);
		TEST_ASSERT(RbufferIsFull(&rxBuffer_handler)==true);

		{	// Read the number of written bytes
			uint16_t nCount = RbufferNumOfElements(&rxBuffer_handler);
			TEST_ASSERT_EQUAL_UINT16(BUFF_SIZE, nCount);

			// Read all data from buffer
			uint8_t readBuffer[nCount];
			RbufferRead(&rxBuffer_handler, readBuffer, sizeof(readBuffer));
			TEST_ASSERT_EQUAL_UINT8_ARRAY(bufferTest + nCount, readBuffer, sizeof(readBuffer));
		}
		// Test status flags
		TEST_ASSERT(RBufferOverflow(&rxBuffer_handler)==false);
		TEST_ASSERT(RbufferIsFull(&rxBuffer_handler)==false);

	}
}

void test_BufferReadEmpty(void) {
	/*Implement test that will check if read empty buffer will return 0 bytes length and will not increment address of rPt */

	{	// Read the number of written bytes
		uint16_t nCount = RbufferNumOfElements(&rxBuffer_handler);
		TEST_ASSERT_EQUAL_UINT16(0, nCount);

		// Read all data from buffer
		uint8_t readBuffer[nCount];
		nCount = RbufferRead(&rxBuffer_handler, readBuffer, sizeof(readBuffer));
		TEST_ASSERT_EQUAL_UINT16(0, nCount);
	}

	TEST_ASSERT_EQUAL_PTR(rxBufferStruct.rxBufferData, rxBuffer_handler.buf);
	TEST_ASSERT_EQUAL_PTR(rxBufferStruct.rxBufferData, rxBuffer_handler.rPt);
	TEST_ASSERT_EQUAL_PTR(rxBufferStruct.rxBufferData, rxBuffer_handler.wPt);
	TEST_ASSERT(rxBuffer_handler.overflow==false);
}

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
	LPSCI_Init(DEBUG_LPSCI, &config, DEBUG_LPSCI_CLK_FREQ);
	LPSCI_EnableTx(DEBUG_LPSCI, true);
	LPSCI_EnableRx(DEBUG_LPSCI, true);

	UNITY_BEGIN();
	RUN_TEST(test_InitTest);
	RUN_TEST(test_Empty);
	RUN_TEST(test_IncrementAddress);
	RUN_TEST(test_WriteReadInit);
	RUN_TEST(test_WriteToFullAndReadAll);
	RUN_TEST(test_WriteOverFlow);
	RUN_TEST(test_BufferOverFlow);
	RUN_TEST(test_BufferReadEmpty);
	return UNITY_END();
}

