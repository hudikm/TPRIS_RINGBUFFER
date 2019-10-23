/*
 * RingBuffer.c
 *
 *  Created on: Oct 17, 2019
 *      Author: martin
 */

#include <stdint.h>
#include <assert.h>
#include <RingBuffer.h>

/*! Private function for incrementing ring buffer pointer address */
/*  increment_address_one = (address + 1) % Length*/
uint8_t * incrementAddress(buffer_t *buffer, uint8_t* ptr) {
	return buffer->buf + ((size_t) (++ptr - buffer->buf) % buffer->size);
}

bool RbufferIsFull(buffer_t *buffer) {
//	return (incrementAddress(buffer, buffer->wPt) == buffer->rPt);
	return buffer->count >= buffer->size;
}

size_t RbufferWrite(buffer_t *buffer, const uint8_t *src, size_t count) {
	size_t nBytes = 0;

/**
 *
 *
 *
 *
 *
 *
 *
 */

	return nBytes;
}

size_t RbufferRead(buffer_t *buffer, uint8_t *out, size_t count) {
	size_t nBytes = 0;
	 /*
	  *
	  *
	  *
	  *
	  * */

	return nBytes;
}

void RbufferInit(buffer_t *buf_t, uint8_t *buffer, size_t size) {
	buf_t->buf = buffer;
	buf_t->rPt = buf_t->buf;
	buf_t->wPt = buf_t->buf;
	buf_t->count = 0;
	buf_t->size = size;
	buf_t->overflow = false;
}

size_t RbufferCapacity(buffer_t *buffer) {
	return buffer->size;
}

size_t RbufferNumOfElements(buffer_t *cbuf) {
	assert(cbuf);
	return cbuf->count;
}

bool RBufferOverflow(buffer_t *buffer) {
	return buffer->overflow;
}
