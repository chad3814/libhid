#ifndef __INCLUDED_HID_H__
#define __INCLUDED_HID_H__

#include <stdio.h>
#include <usb.h>

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

typedef enum hid_return_t {
  HID_RET_SUCCESS = 0,
  HID_RET_FAIL_FIND_BUSSES,
  HID_RET_FAIL_FIND_DEVICES,
  HID_RET_INVALID_INTERFACE,
  HID_RET_FAIL_OPEN_DEVICE,
  HID_RET_NOT_FOUND,
  HID_RET_FAIL_SET_ALTIFACE,
  HID_RET_FAIL_CLAIM_IFACE,
  HID_RET_NOT_HID_DEVICE,
  HID_RET_HID_DESC_SHORT,
  HID_RET_FAIL_GET_REPORT,
  HID_RET_REPORT_DESC_SHORT,
  HID_RET_FAIL_ALLOC,
  HID_RET_UNKNOWN_FAILURE
} hid_return;

struct usb_dev_handle;

typedef struct HIDInterface_t {
  struct usb_dev_handle *dev_handle;
  struct usb_device *device;
  unsigned int report_len;
  char* report_desc;
} HIDInterface;

typedef struct HIDInterfaceMatcher_t {
  unsigned short vendor_id;
  unsigned short product_id;
  bool (*matcher_fn)(struct usb_device const*, void* custom);
  void* custom_data;
} HIDInterfaceMatcher;
#define HID_ID_MATCH_ANY 0x0000

#ifdef __cplusplus
extern "C" {
#endif

hid_return hid_init();

hid_return hid_prepare_interface(HIDInterface* hidif);

hid_return hid_cleanup();

hid_return hid_open(HIDInterface* hidif, HIDInterfaceMatcher const* match);
hid_return hid_force_open(HIDInterface* hidif,
                          HIDInterfaceMatcher const* match,
                          unsigned short retries);

hid_return hid_close(HIDInterface* hidif);

bool hid_is_opened(HIDInterface const* hidif);

hid_return hid_get_item_value(HIDInterface const* hidif,
                              char const *const itempath,
                              double *const value);

hid_return hid_get_item_string(HIDInterface const* hidif,
                               char const *const itempath,
                               char *const value, unsigned int const maxlen);

hid_return hid_set_item_value(HIDInterface const* hidif,
                              char const *const itempath,
                              double const value);

hid_return hid_write_identification(FILE* out, HIDInterface const* hidif);

#ifdef __cplusplus
}
#endif

#endif // __INCLUDED_HID_H__
