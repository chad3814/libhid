/*!@file 
 *@brief BSD-specific support routines.
 */
#define HID_INTERNAL

#include <hid.h>
#include <os.h>

#include <debug.h>
#include <assert.h>

hid_return hid_os_force_claim(HIDInterface* const hidif, int const interface,
    HIDInterfaceMatcher const* const matcher, unsigned short retries)
{
  if (!hidif) {
    ERROR("cannot open NULL HIDInterface.");
    return HID_RET_INVALID_PARAMETER;
  }

  if (hid_is_opened(hidif)) {
    ERROR("cannot open already opened HIDInterface.");
    return HID_RET_DEVICE_ALREADY_OPENED;
  }

  if (!matcher) {
    ERROR("cannot match against NULL HIDInterfaceMatcher.");
    return HID_RET_INVALID_PARAMETER;
  }

  WARNING("code not tested on the BSD platform!");
  TRACE("claiming " TRACEDEVICESTR "...", TRACEDEVICEARGS);
  if (usb_claim_interface(hidif->dev_handle, interface) < 0) {
    WARNING("failed to claim " TRACEDEVICESTR ".", TRACEDEVICEARGS);
    return HID_RET_FAIL_CLAIM_IFACE;
  }
  return HID_RET_SUCCESS;
}
