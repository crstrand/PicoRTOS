#include "picoRTOS.h"

static uint8_t  inBuffer[BUFFERSIZE];
static uint16_t inBufferIndex=0;
static uint16_t lengthFromPacket=0;

void filterSerial(void)
{
	if(!flag7e)
	{
		// check inBuffer for 0x7e
		if(!RingBuffer_IsEmpty(&USARTtoFilter_Buffer))
      flag7e = (RingBuffer_Remove(&USARTtoFilter_Buffer)==0x7e); // if we aren't sync'd toss the character
		if(flag7e) 
		{
			syncCount++;
			//RingBuffer_Insert(&FilterToUSB_Buffer,'S'); // send an 's' to indicate start of packet found
			inBufferIndex=0;
			inBuffer[inBufferIndex++] = 0x7e; // set first character of inBuffer to 0x7e as found
		}
	}
	else // flag7e is set so hopefully we are synchronized at the start of a packet
	{
		while ((!RingBuffer_IsEmpty(&USARTtoFilter_Buffer)) && inBufferIndex<sizeof(inBuffer) && inBufferIndex<(lengthFromPacket+3+1))
		// don't overflow and don't get more characters than we expect or we might grab the start of the next packet
		{
			// append characters from ring buffer to inBuffer
			inBuffer[inBufferIndex++] = RingBuffer_Remove(&USARTtoFilter_Buffer);
		}
		// we'll happy path it for now, but we should have a timeout in case we only get a partial message then nothing

		// if buffer size > 2
		uint16_t size = inBufferIndex; // inBufferIndex is 1 greater than the position of the last character received because of the ++
		if (size > 2)
		{
			if (lengthFromPacket <= 0)
			{
				if ((inBuffer[1] == 0) && (inBuffer[2] > 0) && (inBuffer[2] < (sizeof(inBuffer)-3-1)))
				{
					// start of packet == inBuffer[0]
					lengthFromPacket = inBuffer[2];
					if((lengthFromPacket+3+1) > sizeof(inBuffer)) lengthFromPacket=sizeof(inBuffer)-3-1; // make sure we don't step off the end of inBuffer
				}
				else
				{
					// false flag7e. reset and flush 7e from inBuffer
					filterReset();
				}
			}
			else if (size == (lengthFromPacket+3+1))
			{
				// whole packet received
				//RingBuffer_Insert(&FilterToUSB_Buffer,'P');
        #if (DEBUG==2)
        //RingBuffer_Insert(&FilterToUSB_Buffer,'L');
        send_hex_toUSB(inBuffer[2]);
        #endif
				// calculate checksum
				uint8_t receivedCS = inBuffer[lengthFromPacket+3];
				uint8_t *CSstart = inBuffer+3;
				uint8_t calculatedCS = calcCS(CSstart,lengthFromPacket);
				if (calculatedCS == receivedCS)
				{
					validCount++;
          #if DEBUG==2
            // dump packet as ascii
            dump_packet(inBuffer,size);
          #endif

					// if packet is AT
					if (inBuffer[3] == 0x88)
					{
            if(inBuffer[2] == 0x09 && inBuffer[5] == 'S' && inBuffer[6] == 'L' && inBuffer[7] == 0) // if packet is ATSL (byte 7 : 0=OK)
            {
                atslCount++;
                // read ATSL
                ATSL = (uint32_t) inBuffer[8]  << 24;
                ATSL |= (uint32_t) inBuffer[9] << 16;
                ATSL |= (uint32_t) inBuffer[10] << 8;
                ATSL |= (uint32_t) inBuffer[11];
                #if DEBUG
                sprintf(temp,"ATSL received and saved as %08X\rn",ATSL);
                send_ascii_toUSB(temp,strlen(temp));
                #endif
            }
            // pass along the AT packet as-is
            for(int i=0;i<lengthFromPacket+3+1;i++)
            {
                RingBuffer_Insert(&FilterToUSB_Buffer,inBuffer[i]);
            }

					}
					else
					{
						// process packet
						if (inBuffer[15] == 0x81 || inBuffer[15] == 0x82) // if broadcast 
						{
							// send to outBuffer
							for(int i=0;i<lengthFromPacket+3+1;i++)
							{
								RingBuffer_Insert(&FilterToUSB_Buffer,inBuffer[i]);
							}
							if (inBuffer[15] == 0x81) timeCount++;
							if (inBuffer[15] == 0x82) estopCount++;
						}
						else // not a broadcast
						{
							// get destination from packet
							uint32_t destination = 0;
							destination = (uint32_t) inBuffer[16]  << 24;
							destination |= (uint32_t) inBuffer[17] << 16;
							destination |= (uint32_t) inBuffer[18] << 8;
							destination |= (uint32_t) inBuffer[19];
							if(destination == ATSL || ATSL == 0) // if destination = ATSL or we haven't set ATSL
							{
								desiredCount++;
								// send to outBuffer
								for(int i=0;i<lengthFromPacket+3+1;i++)
								{
									RingBuffer_Insert(&FilterToUSB_Buffer,inBuffer[i]);
								}
							}
							else
							{
                #if DEBUG
                sprintf(temp,"destination %08X, ATSL %08X\r",destination,ATSL);
                send_ascii_toUSB(temp,strlen(temp));
                #endif
							}

						}
					} // end of not AT packet (IS regular)
				}
				else
				{
					#if DEBUG
					send_ascii_toUSB(" BAD CS\rcalc: ",15);
					send_hex_toUSB(calculatedCS);
					send_ascii_toUSB(" received: ",11);
					send_hex_toUSB(receivedCS);
					send_ascii_toUSB("\r",1);
					#endif
					badCS++;
				}
				filterReset();
			}
		}
	}
}

uint8_t calcCS(uint8_t* cmd, int len)
{
  uint8_t sum=0;
  for(int i=0;i<len;i++)
    sum+=cmd[i];
  return 0xff-sum;
}

void filterReset(void)
{
	flag7e = false;
	lengthFromPacket=0;
	inBufferIndex=0;
	memset(inBuffer,0,sizeof(inBuffer));
}

void query_SL(void)
{
	// query the serial low from the radio
	uint8_t cmd[] = {0x7e, 0x00, 0x04, 0x08, 0x01, 'S', 'L', 0x57};
	for(int i=0;i<sizeof(cmd);i++)
	{
		//if (Serial_IsSendReady()) Serial_SendByte(cmd[i]);
		// preload the USBtoUSART buffer with the serial low packet
		if (!(RingBuffer_IsFull(&USBtoUSART_Buffer)))
			RingBuffer_Insert(&USBtoUSART_Buffer, cmd[i]);
	}
}