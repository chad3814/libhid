#define HID_INTERNAL

#include <hid.h>
#include <os.h>

#include <debug.h>
#include <assert.h>
#include <macros.h>

hid_return hid_os_force_claim(HIDInterface* const hidif, int const interface,
    HIDInterfaceMatcher const* const match, unsigned short retries)
{
  int ret;

  while ((ret = usb_claim_interface(hidif->dev_handle, interface)) != 0
            && retries-- > 0) {

    TRACE("failed to claim " TRACEDEVICESTR ", trying %d more time(s)...",
        TRACEDEVICEARGS, retries);
    
    TRACE("detaching kernel driver from " TRACEDEVICESTR "...", TRACEDEVICEARGS);
    if (usb_detach_kernel_driver_np(hidif->dev_handle, interface) < 0) {
      WARNING("failed to detach kernel driver from "
          TRACEDEVICESTR "...", TRACEDEVICEARGS);
      return HID_RET_FAIL_DETACH_DRIVER;
    }

    TRACE("trying again to claim " TRACEDEVICESTR "...", TRACEDEVICEARGS);
  }

  if (ret == 0) return HID_RET_SUCCESS;
  return HID_RET_FAIL_CLAIM_IFACE;
}
