#define HID_INTERNAL

#include <hid.h>
#include <os.h>

#include <debug.h>
#include <assert.h>
#include <macros.h>

hid_return hid_os_force_claim(HIDInterface* const hidif, int const interface,
    HIDInterfaceMatcher const* const match, unsigned short retries)
{
  WARNING("code not tested on the Darwin platform!");
  TRACE("claiming " TRACEDEVICESTR "...", TRACEDEVICEARGS);
  if (usb_claim_interface(hidif->dev_handle, interface) < 0) {
    WARNING("failed to claim " TRACEDEVICESTR ".", TRACEDEVICEARGS);
    return HID_RET_FAIL_CLAIM_IFACE;
  }
  return HID_RET_SUCCESS;
}
