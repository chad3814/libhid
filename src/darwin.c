#define HID_INTERNAL

#include <hid.h>
#include <os.h>

#include <debug.h>

hid_return hid_os_force_claim(HIDInterface* hidif, int const interface,
                              HIDInterfaceMatcher const* match,
                              unsigned short retries)
{
  WARNING("code not tested on the Darwin platform!");
  TRACE("claiming interface %d of USB device %s/%s...", interface,
      hidif->device->bus->dirname, hidif->device->filename);
  if (usb_claim_interface(hidif->dev_handle, interface) < 0) {
    WARNING("failed to claim interface %d of USB device %s/%s...", interface,
              hidif->device->bus->dirname, hidif->device->filename);
    return HID_RET_FAIL_CLAIM_IFACE;
  }
  return HID_RET_SUCCESS;
}
