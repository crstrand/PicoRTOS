#include "picoRTOS.h"
static QueueHandle_t xQueue = NULL;
static SemaphoreHandle_t mutex;

// static task for usbd
// Increase stack size when debug log is enabled
#if CFG_TUSB_DEBUG
  #define USBD_STACK_SIZE     (3*configMINIMAL_STACK_SIZE)
#else
  #define USBD_STACK_SIZE     (3*configMINIMAL_STACK_SIZE/2)
#endif

StackType_t  usb_device_stack[USBD_STACK_SIZE];
StaticTask_t usb_device_taskdef;

// static task for cdc
#define CDC_STACK_SZIE      configMINIMAL_STACK_SIZE
StackType_t  cdc_stack[CDC_STACK_SZIE];
StaticTask_t cdc_taskdef;

void usb_device_task(void* param);
void cdc_task(void* params);

int main()
{
  board_init();
  usb_serial_init(); // get the serial number of the pico board (really the RUID of the flash device)

  // Create a task for tinyusb device stack
  (void) xTaskCreate( usb_device_task, "usbd", USBD_STACK_SIZE, NULL, configMAX_PRIORITIES-2, NULL);

  // Create CDC task
  (void) xTaskCreate( cdc_task, "cdc", CDC_STACK_SZIE, NULL, configMAX_PRIORITIES-2, NULL);

  vTaskStartScheduler();

}

// USB Device Driver task
// This top level thread process all usb events and invoke callbacks
void usb_device_task(void* param)
{
  (void) param;

  // This should be called after scheduler/kernel is started.
  // Otherwise it could cause kernel issue since USB IRQ handler does use RTOS queue API.
  tusb_init();

  // RTOS forever loop
  while (1)
  {
    // tinyusb device task
    tud_task();
  }
}


//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+
void cdc_task(void* params)
{
  (void) params;

  // RTOS forever loop
  while ( 1 )
  {
    // connected() check for DTR bit
    // Most but not all terminal client set this when making connection
    uint8_t itf;

    for (itf = 0; itf < CFG_TUD_CDC; itf++)
    {
      // if ( tud_cdc_n_connected(itf) )
      {
        // There are data available
        if ( tud_cdc_n_available(itf) )
        {
          uint8_t buf[64];

          // read from interface itf
          uint32_t count = tud_cdc_n_read(itf, buf, sizeof(buf));
          (void) count;

          // Echo back
          // Note: Skip echo by commenting out write() and write_flush()
          // for throughput test e.g
          //    $ dd if=/dev/zero of=/dev/ttyACM0 count=10000
          tud_cdc_n_write(itf, buf, count);
          tud_cdc_n_write_flush(itf);
        }
      }
    }
  }
}
