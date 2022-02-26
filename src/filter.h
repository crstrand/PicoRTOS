#ifndef _FILTER_H_
#define _FILTER_H_

#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
//#include "Ringbuffer.h"

static uint32_t ATSL=0; // initial setting for desired radio serial low (SL)
static uint8_t  flag7e=0;

	void filterSerial(void);
	void filterReset(void);
	uint8_t calcCS(uint8_t* cmd, int len);
	void query_SL(void);
#endif