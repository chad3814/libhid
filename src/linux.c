#define HID_INTERNAL

#include <hid.h>

#include <usb.h>

hid_return hid_get_item_value(HIDInterface const* hidif,
                              char const *const itempath,
                              double *const value)
{
  return HID_RET_SUCCESS;
}

hid_return hid_get_item_string(HIDInterface const* hidif,
                               char const *const itempath,
                               char *const value, unsigned int const maxlen)
{
  return HID_RET_SUCCESS;
}

hid_return hid_set_item_value(HIDInterface const* hidif,
                              char const *const itempath,
                              double const value)
{
  return HID_RET_SUCCESS;
}
