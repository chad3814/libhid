#ifndef __INCLUDED_HID_H__
#define __INCLUDED_HID_H__

#include <stdio.h>
#include <usb.h>
#include <hidparser.h>

#ifndef byte
  typedef unsigned char byte;
#endif

#ifdef HAVE_STDBOOL_H
#  include <stdbool.h>
#else
#  define bool  _Bool
#  define true  1
#  define false 0
#endif

typedef enum hid_return {
  HID_RET_SUCCESS = 0,
  HID_RET_FAIL_FIND_BUSSES,
  HID_RET_FAIL_FIND_DEVICES,
  HID_RET_INVALID_INTERFACE,
  HID_RET_FAIL_OPEN_DEVICE,
  HID_RET_FAIL_CLOSE_DEVICE,
  HID_RET_NOT_OPENED,
  HID_RET_NOT_FOUND,
  HID_RET_FAIL_SET_ALTIFACE,
  HID_RET_FAIL_CLAIM_IFACE,
  HID_RET_FAIL_DETACH_DRIVER,
  HID_RET_NOT_HID_DEVICE,
  HID_RET_HID_DESC_SHORT,
  HID_RET_FAIL_GET_REPORT,
  HID_RET_REPORT_DESC_SHORT,
  HID_RET_REPORT_DESC_LONG,
  HID_RET_REPORT_SHORT,
  HID_RET_FAIL_ALLOC,
  HID_RET_OUT_OF_SPACE,
  HID_RET_UNKNOWN_FAILURE
} hid_return;

struct usb_dev_handle;

typedef struct HIDInterface_t {
  struct usb_dev_handle *dev_handle;
  struct usb_device *device;
  int interface;
  HIDData* hid_data;
  HIDParser* hid_parser;
} HIDInterface;

typedef struct HIDInterfaceMatcher_t {
  unsigned short vendor_id;
  unsigned short product_id;
  bool (*matcher_fn)(struct usb_dev_handle const* usbdev, void* custom, unsigned int len);
  void* custom_data;
  unsigned int custom_data_length;
} HIDInterfaceMatcher;
#define HID_ID_MATCH_ANY 0x0000

typedef enum HIDDebugLevel_t {
  HID_DEBUG_NONE = 0x0,
  HID_DEBUG_ERRORS = 0x1,
  HID_DEBUG_WARNINGS = 0x2,
  HID_DEBUG_NOTICES = 0x4,
  HID_DEBUG_TRACES = 0x8,
  HID_DEBUG_ASSERTS = 0x10,
  HID_DEBUG_NOTRACES = HID_DEBUG_ERRORS | HID_DEBUG_WARNINGS | HID_DEBUG_NOTICES | HID_DEBUG_ASSERTS,
  HID_DEBUG_ALL = HID_DEBUG_ERRORS | HID_DEBUG_WARNINGS | HID_DEBUG_NOTICES | HID_DEBUG_TRACES | HID_DEBUG_ASSERTS
} HIDDebugLevel;
  
#ifdef __cplusplus
extern "C" {
#endif

void hid_set_debug(HIDDebugLevel const level);
void hid_set_debug_stream(FILE* const outstream);
void hid_set_usb_debug(int const level);

HIDInterface hid_new_HIDInterface();

void hid_reset_HIDInterface(HIDInterface* const hidif);

hid_return hid_init();

hid_return hid_cleanup();

bool hid_is_initialised();

hid_return hid_open(HIDInterface* const hidif, int const interface,
    HIDInterfaceMatcher const* const match);
hid_return hid_force_open(HIDInterface* const hidif, int const interface,
    HIDInterfaceMatcher const* const match, unsigned short retries);

hid_return hid_close(HIDInterface* const hidif);

bool hid_is_opened(HIDInterface const* const hidif);

hid_return hid_get_input_report(HIDInterface* const hidif, int const path[],
    unsigned int const depth, char* const buffer, unsigned int const size);

hid_return hid_set_output_report(HIDInterface* const hidif, int const path[],
    unsigned int const depth, char const* const buffer, unsigned int const size);

/*
hid_return hid_get_item_value(HIDInterface* const hidif,
                              int const path[], unsigned int const depth,
                              double *const value);

hid_return hid_get_item_string(HIDInterface* const hidif,
                               int const path[], unsigned int const depth,
                               char *const value, unsigned int const maxlen);

hid_return hid_set_item_value(HIDInterface* const hidif,
                              int const path[], unsigned int const depth,
                              double const value);
*/
hid_return hid_write_identification(FILE* const out,
    HIDInterface const* const hidif);

hid_return hid_dump_tree(FILE* const out, HIDInterface* const hidif);

#ifdef __cplusplus
}
#endif

#endif // __INCLUDED_HID_H__
