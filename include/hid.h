
typedef enum hid_return_t {
  HID_RETURN_SUCCESS = 0,
  HID_RETURN_UNKNOWN_FAILURE
} hid_return;

typedef struct HIDInterface_t {
} HIDInterface;

typedef struct HIDInterfaceMatcher_t {
} HIDInterfaceMatcher;

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

hid_return hid_init();

hid_return hid_cleanup();

hid_return hid_find(HIDInterfaceMatcher const match,
                    byte *const bus, byte *const device);

hid_return hid_open(HIDInterface* hidif, byte const bus,
                    byte const device);

hid_return hid_find_open(HIDInterface* hidif, HIDInterfaceMatcher const match);

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
