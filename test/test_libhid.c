#include <hid.h>
#include <stdio.h>
#include <string.h>

bool match_serial_number(struct usb_dev_handle const* usbdev, void* custom, unsigned int len)
{
  return true;
  char* buffer = (char*)malloc(len);
  usb_get_string_simple(usbdev, usb_device(usbdev)->descriptor.iSerialNumber,
      buffer, len);
  printf("DEBUG: %d: %s\n", __LINE__, usb_strerror());
  bool ret = strncmp(buffer, (char*)custom, len) == 0;
  free(buffer);
  return ret;
}

int main(void)
{
  hid_set_debug(HID_DEBUG_ALL);
  hid_set_debug_stream(stderr);
  hid_set_usb_debug(0);

  hid_return ret = hid_init();

  HIDInterface hid = hid_new_HIDInterface();
  char* serial = "01529";
  HIDInterfaceMatcher matcher = { 0x06c2, 0x0038, match_serial_number, (void*)serial, strlen(serial)+1 };

  ret = hid_force_open(&hid, 0, &matcher, 3);

  ret = hid_write_identification(stdout, &hid);

  ret = hid_dump_tree(stdout, &hid);

  int path1[] = { 0xffa00001, 0xffa00002, 0xffa10003 };
  int path2[] = { 0xffa00001, 0xffa00002, 0x0 };
  int path3[] = { 0xffa00001, 0xffa00002, 0xffa10004 };
  double res;
  hid_set_item_value(&hid, path1, 3, res);
  //hid_set_item_value(&hid, path2, 3, res);
  hid_set_item_value(&hid, path3, 3, res);

  ret = hid_close(&hid);

  ret = hid_cleanup();
  
  return 0;
}
