#include <hid.h>
#include <stdio.h>
#include <string.h>

bool match_serial_number(struct usb_dev_handle const* usbdev, void* custom, unsigned int len)
{
  char* buffer = (char*)malloc(len);
  usb_get_string_simple(usbdev, usb_device(usbdev)->descriptor.iSerialNumber,
      buffer, len);
  bool ret = strncmp(buffer, (char*)custom, len) == 0;
  free(buffer);
  return ret;
}

int main(void) {

  hid_set_debug(HID_DEBUG_NOTRACES);
  hid_set_debug_stream(stderr);
  hid_set_usb_debug(0);

  hid_return ret = hid_init();

  HIDInterface hid = hid_new_HIDInterface();
  char* serial = "01529";
  HIDInterfaceMatcher matcher = { 0x06c2, HID_ID_MATCH_ANY, match_serial_number, (void*)serial, strlen(serial)+1 };

  ret = hid_force_open(&hid, 0, &matcher, 3);

  ret = hid_write_identification(stderr, &hid);

  ret = hid_close(&hid);

  ret = hid_cleanup();
  
  return 0;
}
