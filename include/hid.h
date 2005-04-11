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

typedef enum hid_return_t {
  HID_RET_SUCCESS = 0,
  HID_RET_INVALID_PARAMETER,
  HID_RET_NOT_INITIALISED,
  HID_RET_ALREADY_INITIALISED,
  HID_RET_FAIL_FIND_BUSSES,
  HID_RET_FAIL_FIND_DEVICES,
  HID_RET_FAIL_OPEN_DEVICE,
  HID_RET_DEVICE_NOT_FOUND,
  HID_RET_DEVICE_NOT_OPENED,
  HID_RET_DEVICE_ALREADY_OPENED,
  HID_RET_FAIL_CLOSE_DEVICE,
  HID_RET_FAIL_CLAIM_IFACE,
  HID_RET_FAIL_DETACH_DRIVER,
  HID_RET_NOT_HID_DEVICE,
  HID_RET_HID_DESC_SHORT,
  HID_RET_REPORT_DESC_SHORT,
  HID_RET_REPORT_DESC_LONG,
  HID_RET_FAIL_ALLOC,
  HID_RET_OUT_OF_SPACE,
  HID_RET_FAIL_SET_REPORT,
  HID_RET_FAIL_GET_REPORT,
	HID_RET_FAIL_INT_READ,
  HID_RET_NOT_FOUND
} hid_return;

struct usb_dev_handle;

typedef struct HIDInterface_t {
  struct usb_dev_handle *dev_handle;
  struct usb_device *device;
  int interface;
  char id[32];
  HIDData* hid_data;
  HIDParser* hid_parser;
} HIDInterface;

typedef bool (*matcher_fn_t)(struct usb_dev_handle const* usbdev,
    void* custom, unsigned int len);
typedef struct HIDInterfaceMatcher_t {
  unsigned short vendor_id;
  unsigned short product_id;
  matcher_fn_t matcher_fn;
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

HIDInterface* hid_new_HIDInterface();

void hid_delete_HIDInterface(HIDInterface** const hidif);

void hid_reset_HIDInterface(HIDInterface* const hidif);

hid_return hid_init();

hid_return hid_cleanup();

bool hid_is_initialised();

hid_return hid_open(HIDInterface* const hidif, int const interface,
    HIDInterfaceMatcher const* const matcher);
hid_return hid_force_open(HIDInterface* const hidif, int const interface,
    HIDInterfaceMatcher const* const matcher, unsigned short retries);

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

hid_return hid_interrupt_read(HIDInterface* const hidif, unsigned int const ep, char* const bytes, unsigned int const size, unsigned int const timeout);

#ifdef __cplusplus
}
#endif

#endif /* __INCLUDED_HID_H__ */

/* COPYRIGHT --
 *
 * This file is part of libhid, a user-space HID access library.
 * libhid is (c) 2003-2005
 *   Martin F. Krafft <libhid@pobox.madduck.net>
 *   Charles Lepple <clepple@ghz.cc>
 *   Arnaud Quette <arnaud.quette@free.fr> && <arnaud.quette@mgeups.com>
 * and distributed under the terms of the GNU General Public License.
 * See the file ./COPYING in the source distribution for more information.
 *
 * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES
 * OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
