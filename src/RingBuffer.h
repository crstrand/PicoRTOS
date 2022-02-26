// static inline functions are used in the .h file because they introduce
// less overhead than using normal function calls
// less overhead means faster?

#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "picoRTOS.h"

static inline void uart_set_rx_irq(bool enabled)
{
  #ifndef POLLED_UART_RX
  uart_set_irq_enables(UART_ID, enabled, false);
  #endif
}

typedef struct
{
	uint8_t* In; /**< Current storage location in the circular buffer. */
	uint8_t* Out; /**< Current retrieval location in the circular buffer. */
	uint8_t* Start; /**< Pointer to the start of the buffer's underlying storage array. */
	uint8_t* End; /**< Pointer to the end of the buffer's underlying storage array. */
	uint16_t Size; /**< Size of the buffer's underlying storage array. */
	uint16_t Count; /**< Number of bytes currently stored in the buffer. */
} RingBuffer_t;

static inline void RingBuffer_InitBuffer(RingBuffer_t* Buffer, uint8_t* const DataPtr, const uint16_t Size)
{
  uart_set_rx_irq(false);

  Buffer->In     = DataPtr;
  Buffer->Out    = DataPtr;
  Buffer->Start  = &DataPtr[0];
  Buffer->End    = &DataPtr[Size];
  Buffer->Size   = Size;
  Buffer->Count  = 0;

  uart_set_rx_irq(true);
}

static inline uint16_t RingBuffer_GetCount(RingBuffer_t* const Buffer)
{
  uint16_t Count;
  uart_set_rx_irq(false);
  Count = Buffer->Count;
  uart_set_rx_irq(true);
  return Count;
}

static inline uint8_t RingBuffer_Remove(RingBuffer_t* Buffer)
{
  uint8_t Data = *Buffer->Out;
  if (++Buffer->Out == Buffer->End) Buffer->Out = Buffer->Start;
  uart_set_rx_irq(false);
  Buffer->Count--;
  uart_set_rx_irq(true);
  return Data;
}
static inline void RingBuffer_Insert(RingBuffer_t* Buffer, const uint8_t Data)
{
  *Buffer->In = Data;
  if (++Buffer->In == Buffer->End) Buffer->In = Buffer->Start;
  uart_set_rx_irq(false);
  Buffer->Count++;
  uart_set_rx_irq(true);
}
// Returns the next element stored in the ring buffer, without removing it.
static inline uint8_t RingBuffer_Peek(RingBuffer_t* const Buffer)
{
  return *Buffer->Out;
}

static inline uint16_t RingBuffer_GetFreeCount(RingBuffer_t* const Buffer)
{
  return (Buffer->Size - RingBuffer_GetCount(Buffer));
}

static inline bool RingBuffer_IsEmpty(RingBuffer_t* const Buffer)
{
  return (RingBuffer_GetCount(Buffer) == 0);
}

static inline bool RingBuffer_IsFull(RingBuffer_t* const Buffer)
{
  return (RingBuffer_GetCount(Buffer) == Buffer->Size);
}


#ifdef __cplusplus
 }
#endif

#endif

