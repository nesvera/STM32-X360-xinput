#ifndef __usb_xinput_H
#define __usb_xinput_H

#include <inttypes.h>
#include "stdint.h"

int usb_xinput_recv(void *buffer, uint32_t timeout);
int usb_xinput_available(void);
int usb_xinput_send(const void *buffer, uint32_t timeout);


#endif // USBxinput_h_
