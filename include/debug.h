#ifndef __INCLUDED_DEBUG_H__
#define __INCLUDED_DEBUG_H__

#ifndef HID_INTERNAL
#  error "libhid's " __FILE__ " is only supposed to be used from within libhid."
#endif // HID_INTERNAL

#include <hid.h>

#define DEBUGPRINTF(t, s, a...) fprintf(debug_stream, "%s: %s(): ", t, __FUNCTION__); \
                                fprintf(debug_stream, s "\n", ##a);
#define TRACE(s, a...) if (debug_stream && debug_level & HID_DEBUG_TRACES) { DEBUGPRINTF("  TRACE", s, ##a) }
#define NOTICE(s, a...) if (debug_stream && debug_level & HID_DEBUG_NOTICES) { DEBUGPRINTF(" NOTICE", s, ##a) }
#define WARNING(s, a...) if (debug_stream && debug_level & HID_DEBUG_WARNINGS) { DEBUGPRINTF("WARNING", s, ##a) }
#define ERROR(s, a...) if (debug_stream && debug_level & HID_DEBUG_ERRORS) { DEBUGPRINTF("  ERROR", s, ##a) }

HIDDebugLevel debug_level;
FILE* debug_stream;

void trace_usb_bus(FILE* out, struct usb_bus const* usbbus);
void trace_usb_device(FILE* out, struct usb_device const* usbdev);
void trace_usb_device_descriptor(FILE* out, struct usb_device_descriptor const* descriptor);
void trace_usb_config_descriptor(FILE* out, struct usb_config_descriptor const* config);
void trace_usb_dev_handle(FILE* out, usb_dev_handle const* usbdev_h);

#endif // __INCLUDED_DEBUG_H__
