#ifndef __INCLUDED_CONSTANTS_H__
#define __INCLUDED_CONSTANTS_H__

#ifndef HID_INTERNAL
#  error "this file is only supposed to be used from within libhid."
#endif // HID_INTERNAL

#define USB_TIMEOUT 5000

#define HID_REPORT_GET 0x01
#define HID_REPORT_SET 0x09
  
#define HID_RT_INPUT 0x01
#define HID_RT_OUTPUT 0x02
#define HID_RT_FEATURE 0x03

#endif // __INCLUDED_CONSTANTS_H__
