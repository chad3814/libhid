#include <hid.h>

hid_return hid_init()
{
  return HID_RETURN_SUCCESS;
}

hid_return hid_cleanup()
{
  return HID_RETURN_SUCCESS;
}

hid_return hid_find(HIDInterfaceMatcher const match,
                    byte *const bus, byte *const device)
{
  return HID_RETURN_SUCCESS;
}

hid_return hid_open(HIDInterface* hidif, byte const bus,
                    byte const device)
{
  return HID_RETURN_SUCCESS;
}

hid_return hid_find_open(HIDInterface* hidif, HIDInterfaceMatcher const match)
{
  return HID_RETURN_SUCCESS;
}

hid_return hid_close(HIDInterface* hidif)
{
  return HID_RETURN_SUCCESS;
}

bool hid_is_opened(HIDInterface const* hidif)
{
  return true;
}

hid_return hid_get_item_value(HIDInterface const* hidif,
                              char const *const itempath,
                              double *const value)
{
  return HID_RETURN_SUCCESS;
}

hid_return hid_get_item_string(HIDInterface const* hidif,
                               char const *const itempath,
                               char *const value, unsigned int const maxlen)
{
  return HID_RETURN_SUCCESS;
}

hid_return hid_set_item_value(HIDInterface const* hidif,
                              char const *const itempath,
                              double const value)
{
  return HID_RETURN_SUCCESS;
}
