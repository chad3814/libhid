#ifndef __INCLUDED_ASSERT_H__
#define __INCLUDED_ASSERT_H__

#ifndef HID_INTERNAL
#  error "this file is only supposed to be used from within libhid."
#endif // HID_INTERNAL

#define ASSERT(a) if (!a && hid_debug_stream && hid_debug_level & HID_DEBUG_ASSERTS) \
  fprintf(hid_debug_stream, "*** ASSERTION FAILURE in %s() [%s:%d]: %s\n", \
      __FUNCTION__, __FILE__, __LINE__, #a)

#endif // __INCLUDED_DEBUG_H__
