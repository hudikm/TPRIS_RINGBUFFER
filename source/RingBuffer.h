/*
 * RingBugger.h
 *
 *  Created on: Oct 17, 2019
 *      Author: martin
 */

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include <stdint.h>
#include <stdbool.h>

/*
 *
 */
typedef struct {
	uint8_t * volatile buf; // Address of buffer
	uint8_t * volatile rPt; // Address pointing on tail of ring buffer (reading)
	uint8_t * volatile wPt; // Address pointing on head of ring buffer (writing)
	size_t volatile count; // Number of written bytes
	size_t size; // size of ring buffer
	bool overflow; // Buffer overflow
} buffer_t;

/**
 * @brief Copy data from src buffer to ring buffer
 * @param buffer Address to buffer handler structure
 * @param src Address of data to write
 * @param count Size of data to write
 * @return Number of written bytes
 */
size_t RbufferWrite(buffer_t *buffer, const uint8_t *src, size_t count);
/**
 * @brief Copy data from Ring buffer to output buffer
 * @param buffer Address to buffer handler structure
 * @param out Address of output buffer
 * @param count Size of output buffer
 * @return Number of written bytes
 */
size_t RbufferRead(buffer_t *buffer, uint8_t *out, size_t count);
/**
 * @brief Initialize Ring Buffer structure
 * @param buf_t Address to buffer handler structure
 * @param buffer Address to memory buffer taht will be used as data storage inside ring buffer
 * @param size Size of memory buffer
 */
void RbufferInit(buffer_t *buf_t, uint8_t *buffer, size_t size);
/**
 *
 * @param buffer
 * @return Return size of ring buffer
 */
size_t RbufferCapacity(buffer_t *buffer);
/**
 *
 * @param cbuf
 * @return Return number of bytes that are available for read
 */
size_t RbufferNumOfElements(buffer_t *cbuf);
/**
 *
 * @param buffer
 * @return True if buffer is full
 */
bool RbufferIsFull(buffer_t *buffer);

/**
 *
 * @param buffer
 * @return True if buffer has overflow
 */
bool RBufferOverflow(buffer_t *buffer);

#endif /* RINGBUFFER_H_ */
