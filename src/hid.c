#define HID_INTERNAL

#include <hid.h>

#include <debug.h>
#include <assert.h>

/* TODO: this should probably be exported */
#define USB_TIMEOUT 5000

/* NOTE: temporary from usbi.h. for debugging */
struct usb_dev_handle {
  int fd;

  struct usb_bus *bus;
  struct usb_device *device;

  int config;
  int interface;
  int altsetting;

  void *impl_info;
};

static bool initialised = false;

hid_return hid_init()
{
  ASSERT(!hid_is_initialised());
  
  TRACE("initialising USB subsystem...");
  usb_init();

  TRACE("scanning for USB busses...");
  if (usb_find_busses() < 0) {
    ERROR("failed to scan for USB busses");
    return HID_RET_FAIL_FIND_BUSSES;
  }

  TRACE("scanning for USB devices...");
  if (usb_find_devices() < 0) {
    ERROR("failed to scan for USB devices");
    return HID_RET_FAIL_FIND_DEVICES;
  }

  initialised = true;
  NOTICE("successfully initialised HID library.");

  return HID_RET_SUCCESS;
}

static void reset_HIDInterface(HIDInterface* hidif)
{
  hidif->dev_handle = NULL;
  hidif->device = NULL;
  hidif->interface = -1;
  hidif->report_desc = NULL;
  hidif->report_len = 0;
}

HIDInterface hid_new_HIDInterface()
{
  TRACE("creating a new HIDInterface instance...");
  HIDInterface ret;
  reset_HIDInterface(&ret);
  return ret;
}

hid_return hid_cleanup()
{
  ASSERT(hid_is_initialised());

  initialised = false;
  NOTICE("successfully deinitialised HID library.");
  return HID_RET_SUCCESS;
}

bool hid_is_initialised()
{
  return initialised;
}

enum USBMatchFlags {
  USB_MATCH_NONE = 0x0,
  USB_MATCH_VENDOR = 0x1,
  USB_MATCH_PRODUCT = 0x2,
  USB_MATCH_CUSTOM = 0x4,
  USB_MATCH_ALL = USB_MATCH_VENDOR | USB_MATCH_PRODUCT | USB_MATCH_CUSTOM
};

static unsigned int match_usb_device(struct usb_dev_handle const* dev_h,
                                     HIDInterfaceMatcher const* match)
{
  ASSERT(hid_is_initialised());
  ASSERT(dev_h);
  ASSERT(match);

  TRACE("matching a USB device...");

  struct usb_device const* dev = usb_device(dev_h);

  unsigned int ret = USB_MATCH_NONE;
  
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

static hid_return find_usb_device(HIDInterface* hidif,
                                  HIDInterfaceMatcher const* match)
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
        unsigned int flags = match_usb_device(usbdev_h, match);
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

static hid_return get_usb_handle(HIDInterface* hidif,
                                 HIDInterfaceMatcher const* match)
{
  ASSERT(hid_is_initialised());
  ASSERT(!hid_is_opened(hidif));

  TRACE("acquiring handle for a USB device...");
  
  HIDInterface *oldhidif = hidif;
  hid_return ret = find_usb_device(hidif, match);
  if (ret != HID_RET_SUCCESS) {
    hidif = oldhidif;
    hidif->dev_handle = NULL;
    hidif->device = NULL;
    return ret;
  }

  return HID_RET_SUCCESS;
}

static hid_return init_hid_descriptor(HIDInterface* hidif)
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));

  TRACE("initialising the HID descriptor for USB device on %s/%s.",
      hidif->device->bus->dirname, hidif->device->filename);

  /* TODO: if BUFLEN is a standard for HID descriptors, export it to somewhere
   * central
   */
  byte const BUFLEN = 9;
  char buffer[BUFLEN];
  
  TRACE("retrieving HID descriptor from device");
  int len = usb_control_msg(hidif->dev_handle, USB_ENDPOINT_IN+1,
      USB_REQ_GET_DESCRIPTOR, (USB_DT_HID << 8) + 0, 0,
      buffer, BUFLEN, USB_TIMEOUT);

  if (len < 0) {
    WARNING("failed to get HID descriptor of USB device on %s/%s.",
        hidif->device->bus->dirname, hidif->device->filename);
    return HID_RET_NOT_HID_DEVICE;
  }

  if (len < BUFLEN) {
    WARNING("HID descriptor of device on %s/%s is too short: "
            "expected: %d bytes; got: %d bytes.\n",
        hidif->device->bus->dirname, hidif->device->filename, BUFLEN, len);
    return HID_RET_HID_DESC_SHORT;
  }

  /* TODO:
   * the constants 7 and 8 should be exported.
   */
  hidif->report_len = buffer[7] | (buffer[8] << 8);

  NOTICE("successfully initialised HID descriptor of device on %s/%s.",
      hidif->device->bus->dirname, hidif->device->filename);

  return HID_RET_SUCCESS;
}

static hid_return init_report_descriptor(HIDInterface* hidif)
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));

  TRACE("initialising the HID descriptor for USB device on %s/%s.",
      hidif->device->bus->dirname, hidif->device->filename);

  hidif->report_desc = (char*)malloc(hidif->report_len);
  if (!hidif->report_desc) {
    ERROR("failed to allocate memory for report descriptor.");
    return HID_RET_FAIL_ALLOC;
  }

  TRACE("retrieving report descriptor from device");
  int len = usb_control_msg(hidif->dev_handle, USB_ENDPOINT_IN+1,
      USB_REQ_GET_DESCRIPTOR, (USB_DT_REPORT << 8) + 0, 0,
      hidif->report_desc, hidif->report_len, USB_TIMEOUT);

  if (len < 0) {
    WARNING("failed to get report descriptor of USB device on %s/%s.",
        hidif->device->bus->dirname, hidif->device->filename);
    return HID_RET_FAIL_GET_REPORT;
  }

  if (len < hidif->report_len) {
    WARNING("HID descriptor of device on %s/%s is too short: "
            "expected: %d bytes; got: %d bytes.\n",
        hidif->device->bus->dirname, hidif->device->filename,
        hidif->report_len, len);
    return HID_RET_REPORT_DESC_SHORT;
  }

  NOTICE("successfully initialised report descriptor of device on %s/%s.",
      hidif->device->bus->dirname, hidif->device->filename);

  return HID_RET_SUCCESS;
}

hid_return hid_open(HIDInterface* hidif, int const interface,
                    HIDInterfaceMatcher const* match)
{
  ASSERT(hid_is_initialised());
  ASSERT(!hid_is_opened(hidif));

  if (!hidif) return HID_RET_INVALID_INTERFACE;

  TRACE("opening a device interface according to matching criteria...");
  hid_return ret = get_usb_handle(hidif, match);
  if (ret != HID_RET_SUCCESS) return ret;

  TRACE("claiming interface %d on USB device %s/%s...", interface,
      hidif->device->bus->dirname, hidif->device->filename);
  if (usb_claim_interface(hidif->dev_handle, interface) < 0) {
    WARNING("failed to claim interface %d on USB device %s/%s...", interface,
              hidif->device->bus->dirname, hidif->device->filename);
    hid_close(hidif);
    return HID_RET_FAIL_CLAIM_IFACE;
  }
  hidif->interface = interface;
  NOTICE("successfully claimed interface %d on USB device %s/%s...", interface,
      hidif->device->bus->dirname, hidif->device->filename);
  
  /* TODO: what's this anyway?
   * if (usb_set_altinterface(hidif->dev_handle, interface) < 0)
   * return HID_RET_FAIL_SET_ALTIFACE;
   */

  ret = init_hid_descriptor(hidif);
  if (ret != HID_RET_SUCCESS) {
    hid_close(hidif);
    return ret;
  }

  ret = init_report_descriptor(hidif);
  if (ret != HID_RET_SUCCESS) {
    hid_close(hidif);
    return ret;
  }

  NOTICE("successfully opened interface %d on USB device %s/%s...", interface,
      hidif->device->bus->dirname, hidif->device->filename);

  return HID_RET_SUCCESS;
}

hid_return hid_force_open(HIDInterface* hidif, int const interface,
                          HIDInterfaceMatcher const* match,
                          unsigned short retries)
{
  ASSERT(hid_is_initialised());
  ASSERT(!hid_is_opened(hidif));

  if (!hidif) return HID_RET_INVALID_INTERFACE;

  TRACE("forcefully opening a device interface "
        "according to matching criteria...");
  hid_return ret = get_usb_handle(hidif, match);
  if (ret != HID_RET_SUCCESS) return ret;

  TRACE("forcefully claiming interface %d on USB device %s/%s (%dd retries)...",
      interface, hidif->device->bus->dirname, hidif->device->filename,
      retries);
  if (usb_force_claim_interface(hidif->dev_handle, 0, retries) < 0) {
    WARNING("failed %dd times to claim interface %d on USB device %s/%s...",
        retries, interface,
        hidif->device->bus->dirname, hidif->device->filename);
    hid_close(hidif);
    return HID_RET_FAIL_CLAIM_IFACE;
  }
  hidif->interface = interface;
  NOTICE("successfully claimed interface %d on USB device %s/%s...", interface,
      hidif->device->bus->dirname, hidif->device->filename);
 
  /* TODO: what's this anyway?
   * if (usb_set_altinterface(hidif->dev_handle, 0) < 0)
   * return HID_RET_FAIL_SET_ALTIFACE;
   */

  ret = init_hid_descriptor(hidif);
  if (ret != HID_RET_SUCCESS) {
    hid_close(hidif);
    return ret;
  }

  ret = init_report_descriptor(hidif);
  if (ret != HID_RET_SUCCESS) {
    hid_close(hidif);
    return ret;
  }

  NOTICE("successfully opened interface %d on USB device %s/%s...", interface,
      hidif->device->bus->dirname, hidif->device->filename);

  return HID_RET_SUCCESS;
}

hid_return hid_close(HIDInterface* hidif)
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));

  if (!hidif) return HID_RET_INVALID_INTERFACE;

  TRACE("closing interface %d of device on %s/%s...", hidif->interface, 
      hidif->device->bus->dirname, hidif->device->filename);

  if (hid_is_opened(hidif)) {
    
    TRACE("releasing interface %d of device on %s/%s...", hidif->interface,
        hidif->device->bus->dirname, hidif->device->filename);
    if (usb_release_interface(hidif->dev_handle, hidif->interface) < 0)
      WARNING("failed to release interface %d of device on %s/%s.",
          hidif->interface, hidif->device->bus->dirname,
          hidif->device->filename);

    TRACE("closing USB device on %s/%s.",
        hidif->device->bus->dirname, hidif->device->filename);
    if (usb_close(hidif->dev_handle) < 0)
      WARNING("failed to close USB device on %s/%s.",
          hidif->device->bus->dirname, hidif->device->filename);
    NOTICE("successfully closed USB device.");

  }
    
  if (hidif->report_desc) free(hidif->report_desc);
  TRACE("resetting HIDInterface...");
  reset_HIDInterface(hidif);

  return HID_RET_SUCCESS;
}

bool hid_is_opened(HIDInterface const* hidif)
{
  ASSERT(hidif);
  return hidif && hidif->dev_handle != NULL;
}

hid_return hid_write_identification(FILE* out, HIDInterface const* hidif)
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));
  ASSERT(out);

  if (!hid_is_opened(hidif)) {
    return HID_RET_NOT_OPENED;
  }

  int len;
  unsigned short const BUFLEN = 256;
  char buffer[BUFLEN];
  
  fprintf(out, "-----------------------------------------------------------\n");
  fprintf(out, "device identification of HIDInterface at 0x%08x:\n", (unsigned int)hidif);
  fprintf(out, "  dev_handle:    0x%08x\n", (unsigned int)hidif->dev_handle);
  fprintf(out, "  device:        0x%08x\n", (unsigned int)hidif->device);
  fprintf(out, "  location:      %s/%s\n",
      hidif->device->bus->dirname, hidif->device->filename);

  if (hidif->device->descriptor.iManufacturer) {
    len = usb_get_string_simple(hidif->dev_handle,
        hidif->device->descriptor.iManufacturer, buffer, BUFLEN);

    if (len > 0)
      fprintf(out, "  manufacturer:  %s\n", buffer);
    else
      fprintf(out, "(unable to fetch manufacturer string)\n");
  }
  
  if (hidif->device->descriptor.iProduct) {
    len = usb_get_string_simple(hidif->dev_handle,
        hidif->device->descriptor.iProduct, buffer, BUFLEN);

    if (len > 0)
      fprintf(out, "  product:       %s\n", buffer);
    else
      fprintf(out, "(unable to fetch product string)\n");
  }
  
  if (hidif->device->descriptor.iSerialNumber) {
    len = usb_get_string_simple(hidif->dev_handle,
        hidif->device->descriptor.iSerialNumber, buffer, BUFLEN);

    if (len > 0)
      fprintf(out, "  serial number: %s\n", buffer);
    else
      fprintf(out, "(unable to fetch product string)\n");
  }
  
  return HID_RET_SUCCESS;
}
