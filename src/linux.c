/*!@file 
 *@brief Linux-specific support routines.
 */
#define HID_INTERNAL

#include <hid.h>
#include <os.h>

#include <debug.h>
#include <assert.h>
#include <macros.h>

/*!@brief Forcibly claim an interface from an attached kernel driver.
 *
 * This function is necessary in Linux because the kernel prefers that
 * applications and drivers claim a device's appropriate interface
 * before accessing it.  For libhid to successfully claim an interface
 * that has already been claimed by hiddev (for instance), libhid must
 * first tell the kernel interface to relinquish control over the
 * interface.  This may take several tries.
 *
 * If the return code is anything but #HID_RET_SUCCESS, there will
 * probably be more information in the kernel message buffer
 * (available from the "dmesg" command).
 */
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
