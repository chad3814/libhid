#ifndef __INCLUDED_DEBUG_H__
#define __INCLUDED_DEBUG_H__

#ifndef HID_INTERNAL
#  error "libhid's debug.h is only supposed to be used from within libhid."
#endif // HID_INTERNAL

#include <usb.h>
#include <stdio.h>

void trace_usb_bus(FILE* out, struct usb_bus const* usbbus);
void trace_usb_device(FILE* out, struct usb_device const* usbdev);
void trace_usb_device_descriptor(FILE* out, struct usb_device_descriptor const* descriptor);
void trace_usb_config_descriptor(FILE* out, struct usb_config_descriptor const* config);
void trace_usb_dev_handle(FILE* out, usb_dev_handle const* usbdev_h);

#endif // __INCLUDED_DEBUG_H__
