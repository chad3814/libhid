#ifndef __INCLUDED_MACROS_H__
#define __INCLUDED_MACROS_H__

#ifndef HID_INTERNAL
#  error "this file is only supposed to be used from within libhid."
#endif // HID_INTERNAL

#define TRACEDEVICESTR "interface %d of USB device %s/%s"
#define TRACEDEVICEARGS hidif->interface, hidif->device->bus->dirname, hidif->device->filename

#endif // __INCLUDED_MACROS_H__
