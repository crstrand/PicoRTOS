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

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#endif