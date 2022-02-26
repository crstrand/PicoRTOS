#ifndef _picoRTOS_H_
#define _picoRTOS_H_

//#include <stdlib.h>
#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "hardware/uart.h"
#include "hardware/irq.h"

#define UART_ID uart0
#define POLLED_UART_RX
#define BUFFERSIZE 128

#include "get_serial.h"
#include "RingBuffer.h"
#include "filter.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"


/** Circular buffer to hold data from the host before it is sent to the device via the serial port. */
extern RingBuffer_t USBtoUSART_Buffer;

/** Underlying data buffer for \ref USBtoUSART_Buffer, where the stored bytes are located. */
extern uint8_t      USBtoUSART_Buffer_Data[BUFFERSIZE];

/** Circular buffer to hold data from the serial port before it is sent to the host. */
extern RingBuffer_t USARTtoFilter_Buffer;

/** Underlying data buffer for \ref USARTtoFilter_Buffer, where the stored bytes are located. */
extern uint8_t      USARTtoFilter_Buffer_Data[BUFFERSIZE];

/** Circular buffer to hold data from the serial port to be filtered before forwarding to USB. */
extern RingBuffer_t FilterToUSB_Buffer;

/** Underlying data buffer for \ref FilterToUSB_Buffer, where the stored bytes are located. */
extern uint8_t      FilterToUSB_Buffer_Data[BUFFERSIZE];

extern uint32_t syncCount, validCount, badCS, estopCount, timeCount, desiredCount, atslCount;

/* Function Prototypes: */
	//#if DEBUG || STATS
	void send_ascii_toUSB(const char* textToSend, int textlen);
	void send_hex_toUSB(uint8_t nextbyte);
  void dump_packet(uint8_t *inBuffer, int size);
	//#endif

#endif