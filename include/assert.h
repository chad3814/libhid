#ifndef __INCLUDED_ASSERT_H__
#define __INCLUDED_ASSERT_H__

#ifndef HID_INTERNAL
#  error "libhid's " __FILE__ " is only supposed to be used from within libhid."
#endif // HID_INTERNAL

#include <hid.h>

#define ASSERT(a) if (!a && debug_stream && debug_level & HID_DEBUG_ASSERTS) \
  fprintf(debug_stream, "*** ASSERTION FAILURE in %s() [%s:%d]: %s\n", \
      __FUNCTION__, __FILE__, __LINE__, #a)

#endif // __INCLUDED_DEBUG_H__
