#define HID_INTERNAL

#include <hid.h>
#include <os.h>

#include <debug.h>
#include <assert.h>

hid_return hid_os_force_claim(HIDInterface* const hidif, int const interface,
    HIDInterfaceMatcher const* const match, unsigned short retries)
{
  WARNING("code not tested on the BSD platform!");
  TRACE("claiming interface %d of USB device %s/%s...", interface,
      hidif->device->bus->dirname, hidif->device->filename);
  if (usb_claim_interface(hidif->dev_handle, interface) < 0) {
    WARNING("failed to claim interface %d of USB device %s/%s...", interface,
              hidif->device->bus->dirname, hidif->device->filename);
    return HID_RET_FAIL_CLAIM_IFACE;
  }
  return HID_RET_SUCCESS;
}
