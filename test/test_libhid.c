#include <hid.h>
#include <stdio.h>

int main(void) {

  hid_init();

  HIDInterface hid;
  HIDInterfaceMatcher matcher;
  byte bus, device;

  hid_find(matcher, &bus, &device);
  hid_open(&hid, bus, device);

  if (!hid_is_opened(&hid))
    fprintf(stderr, "opening failed.\n");

  hid_close(&hid);
  
  hid_cleanup();
  
  return 0;
}
