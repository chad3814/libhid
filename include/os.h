#ifndef __INCLUDED_OS_H__
#define __INCLUDED_OS_H__

#ifndef HID_INTERNAL
#  error "libhid's " __FILE__ " is only supposed to be used from within libhid."
#endif // HID_INTERNAL

hid_return hid_os_force_claim(HIDInterface* hidif, int const interface,
                              HIDInterfaceMatcher const* match,
                              unsigned short retries);

#endif // __INCLUDED_OS_H__
