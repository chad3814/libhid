#ifndef __INCLUDED_HID_HELPERS_H__
#define __INCLUDED_HID_HELPERS_H__

#ifndef HID_INTERNAL
#  error "this file is only supposed to be used from within libhid."
#endif // HID_INTERNAL

#include <hid.h>

hid_return hid_prepare_interface(HIDInterface* const hidif);

hid_return hid_init_parser(HIDInterface* const hidif);
void hid_reset_parser(HIDInterface* const hidif);
hid_return hid_prepare_parser(HIDInterface* const hidif);
hid_return hid_find_object(HIDInterface* const hidif,
    int const path[], unsigned int const depth);
hid_return hid_format_path(char* const buffer, unsigned int length,
    int const path[], unsigned int const depth);

void hid_reset_HIDInterface(HIDInterface* const hidif);

#endif // __INCLUDED_HID_HELPERS_H__