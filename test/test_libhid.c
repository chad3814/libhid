#include <hid.h>
#include <stdio.h>

int main(void) {

  hid_return ret = hid_init();

  HIDInterface hid;
  HIDInterfaceMatcher matcher = { 0x06c2, HID_ID_MATCH_ANY, NULL };

  ret = hid_prepare_interface(&hid);

  ret = hid_force_open(&hid, &matcher, 3);

  ret = hid_write_identification(stderr, &hid);

  ret = hid_close(&hid);
  
  ret = hid_cleanup();
  
  return 0;
}
