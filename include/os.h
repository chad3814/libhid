#ifndef __INCLUDED_OS_H__
#define __INCLUDED_OS_H__

#ifndef HID_INTERNAL
#  error "this file is only supposed to be used from within libhid."
#endif // HID_INTERNAL

hid_return hid_os_force_claim(HIDInterface* const hidif, int const interface,
    HIDInterfaceMatcher const* const match, unsigned short retries);

#endif // __INCLUDED_OS_H__
