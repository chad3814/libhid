#define HID_INTERNAL

#include <hid.h>
#include <hid_helpers.h>
#include <os.h>

#include <debug.h>
#include <assert.h>
#include <macros.h>

enum USBMatchFlags {
  USB_MATCH_NONE = 0x0,
  USB_MATCH_VENDOR = 0x1,
  USB_MATCH_PRODUCT = 0x2,
  USB_MATCH_CUSTOM = 0x4,
  USB_MATCH_ALL = USB_MATCH_VENDOR | USB_MATCH_PRODUCT | USB_MATCH_CUSTOM
};

static unsigned int hid_compare_usb_device(struct usb_dev_handle const* dev_h,
    HIDInterfaceMatcher const* const match)
{
  ASSERT(hid_is_initialised());
  ASSERT(dev_h);
  ASSERT(match);

  TRACE("comparing match specifications to USB device...");
  unsigned int ret = USB_MATCH_NONE;

  struct usb_device const* dev = usb_device((usb_dev_handle*)dev_h);
  
  TRACE("inspecting vendor ID...");
  if (dev->descriptor.idVendor > 0 &&
      (dev->descriptor.idVendor & match->vendor_id) == match->vendor_id) {
      TRACE("match on vendor ID: 0x%04x.", dev->descriptor.idVendor);
      ret |= USB_MATCH_VENDOR;
  }
  else TRACE("no match on vendor ID.");

  TRACE("inspecting product ID...");
  if (dev->descriptor.idProduct > 0 &&
      (dev->descriptor.idProduct & match->product_id) == match->product_id) {
      TRACE("match on product ID: 0x%04x.", dev->descriptor.idProduct);
      ret |= USB_MATCH_PRODUCT;
  }
  else TRACE("no match on product ID.");

  if (match->matcher_fn) {
    TRACE("calling custom matching function...");
    if ((*match->matcher_fn)(dev_h, match->custom_data, match->custom_data_length)) {
      TRACE("match on custom matching function.");
      ret |= USB_MATCH_CUSTOM;
    }
    else TRACE("no match on custom matching function.");
  }
  else {
    TRACE("no custom matching function supplied.");
    ret |= USB_MATCH_CUSTOM;
  }

  return ret;
}

static hid_return hid_find_usb_device(HIDInterface* const hidif,
    HIDInterfaceMatcher const* const match)
{
  ASSERT(hid_is_initialised());
  ASSERT(!hid_is_opened(hidif));
  ASSERT(match);

  /* WARNING: not thread-safe. usb_busses in libusb/usb.c is a global
   * variable.
   */
  struct usb_bus *usbbus = usb_get_busses();
  struct usb_device *usbdev;

  TRACE("enumerating USB busses...");
  for (; usbbus; usbbus = usbbus->next) {
    TRACE("enumerating USB devices on bus %s...", usbbus->dirname);
    //trace_usb_bus(stderr, usbbus);
    for (usbdev = usbbus->devices; usbdev; usbdev=usbdev->next) {
      //trace_usb_device(stderr, usbdev);

      TRACE("inspecting USB device %s/%s...", usbbus->dirname, usbdev->filename);
      usb_dev_handle *usbdev_h = usb_open(usbdev);

      if (usbdev_h) {
        unsigned int flags = hid_compare_usb_device(usbdev_h, match);
        if (flags == USB_MATCH_ALL) {
          /* WARNING: are we sure we want to give out pointers to outside the
           * library? this strikes me as horrible, but maybe their scope is
           * long enough...
           */
          NOTICE("found a matching USB device on %s/%s.",
              usbbus->dirname, usbdev->filename);
          hidif->dev_handle = usbdev_h;
          hidif->device = usb_device(usbdev_h);
          return HID_RET_SUCCESS;
        }

        if (!(flags & USB_MATCH_VENDOR)) {
          NOTICE("vendor 0x%04x of USB device on %s/%s does not match 0x%04x.",
             usbdev->descriptor.idVendor, usbbus->dirname, usbdev->filename,
             match->vendor_id);
        }
        else if (!(flags & USB_MATCH_PRODUCT)) {
          NOTICE("product 0x%04x of USB device on %s/%s does not match 0x%04x.",
             usbdev->descriptor.idProduct, usbbus->dirname, usbdev->filename,
             match->product_id);
        }
        else if (!(flags & USB_MATCH_CUSTOM)) {
          NOTICE("custom matching function returned false on %s/%s.",
              usbbus->dirname, usbdev->filename);
        }
        usb_close(usbdev_h);
      }
      else {
        ERROR("failed to open USB device on %s/%s",
            usbbus->dirname, usbdev->filename);
        return HID_RET_FAIL_OPEN_DEVICE;
      }
    }
  }
  WARNING("no matching USB device found.");
  return HID_RET_NOT_FOUND;
}

static hid_return hid_get_usb_handle(HIDInterface* const hidif,
    HIDInterfaceMatcher const* const match)
{
  ASSERT(hid_is_initialised());
  ASSERT(!hid_is_opened(hidif));

  TRACE("acquiring handle for a USB device...");
  
  hid_return ret = hid_find_usb_device(hidif, match);
  if (ret != HID_RET_SUCCESS) {
    hidif->dev_handle = NULL;
    hidif->device = NULL;
    return ret;
  }

  return HID_RET_SUCCESS;
}

hid_return hid_open(HIDInterface* const hidif, int const interface,
    HIDInterfaceMatcher const* const match)
{
  ASSERT(hid_is_initialised());
  ASSERT(!hid_is_opened(hidif));

  if (!hidif) return HID_RET_INVALID_INTERFACE;

  TRACE("opening a device interface according to matching criteria...");
  hid_return ret = hid_get_usb_handle(hidif, match);
  if (ret != HID_RET_SUCCESS) return ret;

  hidif->interface = interface;

  TRACE("claiming " TRACEDEVICESTR ".", TRACEDEVICEARGS);
  if (usb_claim_interface(hidif->dev_handle, interface) < 0) {
    WARNING("failed to claim " TRACEDEVICESTR ".", TRACEDEVICEARGS);
    hid_close(hidif);
    return HID_RET_FAIL_CLAIM_IFACE;
  }
  NOTICE("successfully claimed " TRACEDEVICESTR ".", TRACEDEVICEARGS);
  
  /* TODO: what's this anyway?
   * if (usb_set_altinterface(hidif->dev_handle, interface) < 0)
   * return HID_RET_FAIL_SET_ALTIFACE;
   */

  hid_prepare_interface(hidif);

  NOTICE("successfully opened " TRACEDEVICESTR ".", TRACEDEVICEARGS);

  return HID_RET_SUCCESS;
}

hid_return hid_force_open(HIDInterface* const hidif, int const interface,
    HIDInterfaceMatcher const* const match, unsigned short retries)
{
  ASSERT(hid_is_initialised());
  ASSERT(!hid_is_opened(hidif));

  if (!hidif) return HID_RET_INVALID_INTERFACE;

  TRACE("forcefully opening a device interface "
        "according to matching criteria...");
  hid_return ret = hid_get_usb_handle(hidif, match);
  if (ret != HID_RET_SUCCESS) return ret;

  hidif->interface = interface;

  TRACE("claiming " TRACEDEVICESTR ".", TRACEDEVICEARGS);
  ret = hid_os_force_claim(hidif, interface, match, retries);
  if (ret != HID_RET_SUCCESS) {
    WARNING("failed to claim " TRACEDEVICESTR ".", TRACEDEVICEARGS);
    hid_close(hidif);
    return ret;
  }

  NOTICE("successfully claimed " TRACEDEVICESTR ".", TRACEDEVICEARGS);
 
  /* TODO: what's this anyway?
   * if (usb_set_altinterface(hidif->dev_handle, 0) < 0)
   * return HID_RET_FAIL_SET_ALTIFACE;
   */

  hid_prepare_interface(hidif);

  NOTICE("successfully opened " TRACEDEVICESTR ".", TRACEDEVICEARGS);

  return HID_RET_SUCCESS;
}

hid_return hid_close(HIDInterface* const hidif)
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));

  if (!hidif) return HID_RET_INVALID_INTERFACE;

  int ret;

  TRACE("closing " TRACEDEVICESTR "...", TRACEDEVICEARGS);

  if (hid_is_opened(hidif)) {

    hid_reset_parser(hidif);
    //usb_reset(hidif->dev_handle);
    
    /*TRACE("releasing " TRACEDEVICESTR "...", TRACEDEVICEARGS);
    if (usb_release_interface(hidif->dev_handle, hidif->interface) < 0)
      WARNING("failed to release " TRACEDEVICESTR ".", TRACEDEVICEARGS);*/

    TRACE("closing handle of " TRACEDEVICESTR "...", TRACEDEVICEARGS);
    ret = usb_close(hidif->dev_handle);
    if (ret < 0) {
      WARNING("failed to close " TRACEDEVICESTR ".", TRACEDEVICEARGS);
    }
  }

  TRACE("freeing memory allocated for HID parser...");
  if(hidif->hid_parser) free(hidif->hid_parser);
  if(hidif->hid_data) free(hidif->hid_data);
    
  if (ret == 0)
    NOTICE("successfully closed " TRACEDEVICESTR ".", TRACEDEVICEARGS);

  TRACE("resetting HIDInterface...");
  hid_reset_HIDInterface(hidif);

  if (ret < 0) return HID_RET_FAIL_CLOSE_DEVICE;

  return HID_RET_SUCCESS;
}

bool hid_is_opened(HIDInterface const* hidif)
{
  ASSERT(hidif);
  return hidif && hidif->dev_handle != NULL;
}
