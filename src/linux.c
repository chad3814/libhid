#define HID_INTERNAL

#include <hid.h>
#include <os.h>

#include <debug.h>

hid_return hid_os_force_claim(HIDInterface* hidif, int const interface,
                              HIDInterfaceMatcher const* match,
                              unsigned short retries)
{
  int ret;

  while ((ret = usb_claim_interface(hidif->dev_handle, interface)) != 0
            && retries-- > 0) {

    TRACE("failed to claim interface %d of USB device on %s/%s, "
          "trying %d more time(s)...", interface,
          hidif->device->bus->dirname, hidif->device->filename, retries);
    
    TRACE("detaching kernel driver from interface %d of USB device on %s/%s...",
        interface, hidif->device->bus->dirname, hidif->device->filename);
    if (usb_detach_kernel_driver_np(hidif->dev_handle, interface) < 0) {
      WARNING("failed to detach kernel driver from interface %d of USB device %s/%s.",
          interface, hidif->device->bus->dirname, hidif->device->filename);
      return HID_RET_FAIL_DETACH_DRIVER;
    }

    TRACE("trying again to claim interface %d of USB device on %s/%s...",
        interface, hidif->device->bus->dirname, hidif->device->filename);
  }

  if (ret == 0) return HID_RET_SUCCESS;
  return HID_RET_FAIL_CLAIM_IFACE;
}
