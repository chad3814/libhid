#define HID_INTERNAL

#include <hid.h>

#include <debug.h>

#include <usb.h>
#include <stdio.h>

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


hid_return hid_init()
{
  usb_init();
  if (usb_find_busses() < 0) return HID_RET_FAIL_FIND_BUSSES;
  if (usb_find_devices() < 0) return HID_RET_FAIL_FIND_DEVICES;
  
  return HID_RET_SUCCESS;
}

hid_return hid_prepare_interface(HIDInterface* hidif)
{
  hidif->dev_handle = NULL;
  hidif->device = NULL;
  hidif->report_len = 0;
  hidif->report_desc = NULL;

  return HID_RET_SUCCESS;
}

hid_return hid_cleanup()
{
  return HID_RET_SUCCESS;
}

enum USBMatchFlags {
  USB_MATCH_NONE = 0x0,
  USB_MATCH_VENDOR = 0x1,
  USB_MATCH_PRODUCT = 0x2,
  USB_MATCH_CUSTOM = 0x4,
  USB_MATCH_ALL = USB_MATCH_VENDOR | USB_MATCH_PRODUCT | USB_MATCH_CUSTOM
};

unsigned int match_usb_device(struct usb_device const* dev,
                              HIDInterfaceMatcher const* match)
{
  unsigned int ret = USB_MATCH_NONE;
  
  if (dev->descriptor.idVendor > 0 &&
      (dev->descriptor.idVendor & match->vendor_id) == match->vendor_id)
      ret |= USB_MATCH_VENDOR;
  if (dev->descriptor.idProduct > 0 && 
      (dev->descriptor.idProduct & match->product_id) == match->product_id)
      ret |= USB_MATCH_PRODUCT;

  if (match->matcher_fn)
    ret |= (*match->matcher_fn)(dev, match->custom_data) ? USB_MATCH_CUSTOM : 0;
  else
    ret |= USB_MATCH_CUSTOM;

  return ret;
}

hid_return find_usb_device(HIDInterface* hidif, HIDInterfaceMatcher const* match)
{
  /* WARNING: not thread-safe. usb_busses in libusb/usb.c is a global
   * variable.
   */
  struct usb_bus *usbbus = usb_busses;
  struct usb_device *usbdev;

  for (; usbbus; usbbus = usbbus->next) {
    //trace_usb_bus(stderr, usbbus);
    for (usbdev = usbbus->devices; usbdev; usbdev=usbdev->next) {
      //trace_usb_device(stderr, usbdev);

      usb_dev_handle *usbdev_h = usb_open(usbdev);

      if (usbdev_h) {
        unsigned int flags = match_usb_device(usbdev, match);
        if (flags == USB_MATCH_ALL) {
          /* WARNING: are we sure we want to give out pointers to outside the
           * library? this strikes me as horrible, but maybe their scope is
           * long enough...
           */
          hidif->dev_handle = usbdev_h;
          hidif->device = usbdev;
          return HID_RET_SUCCESS;
        }

        if (!(flags & USB_MATCH_VENDOR))
          fprintf(stderr, "vendor 0x%04x does not match 0x%04x, skipping.\n",
              usbdev->descriptor.idVendor, match->vendor_id);

        else if (!(flags & USB_MATCH_PRODUCT))
          fprintf(stderr, "product 0x%04x does not match 0x%04x, skipping.\n",
              usbdev->descriptor.idProduct, match->product_id);

        else if (!(flags & USB_MATCH_CUSTOM))
          fprintf(stderr, "device at %s/%s did not match custom matcher, skipping.\n",
              usbdev->bus->dirname, usbdev->filename);

        usb_close(usbdev_h);
      }
      else return HID_RET_FAIL_OPEN_DEVICE;
    }
  }
  return HID_RET_NOT_FOUND;
}

hid_return open_usb_device(HIDInterface* hidif, HIDInterfaceMatcher const* match)
{
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

hid_return init_hid_descriptor(HIDInterface* hidif)
{
  /* TODO: if BUFLEN is a standard for HID descriptors, export it to somewhere
   * central
   */
  byte const BUFLEN = 9;
  char buffer[BUFLEN];
  
  int len = usb_control_msg(hidif->dev_handle, USB_ENDPOINT_IN+1,
      USB_REQ_GET_DESCRIPTOR, (USB_DT_HID << 8) + 0, 0,
      buffer, BUFLEN, USB_TIMEOUT);

  if (len < 0) {
    fprintf(stderr, "device at %s/%s is not a HID device"
                    "(failed to get HID descriptor).",
        hidif->device->bus->dirname, hidif->device->filename);
    return HID_RET_NOT_HID_DEVICE;
  }

  if (len < BUFLEN) {
    fprintf(stderr, "HID descriptor of device at %s/%s is too short:"
                    "expected: %d bytes; got: %d bytes.\n",
        hidif->device->bus->dirname, hidif->device->filename, BUFLEN, len);
    return HID_RET_HID_DESC_SHORT;
  }

  /* TODO:
   * the constants 7 and 8 should be exported.
   */
  hidif->report_len = buffer[7] | (buffer[8] << 8);

  return HID_RET_SUCCESS;
}

hid_return init_report_descriptor(HIDInterface* hidif)
{
  hidif->report_desc = (char*)malloc(hidif->report_len);
  if (!hidif->report_desc)
    return HID_RET_FAIL_ALLOC;

  int len = usb_control_msg(hidif->dev_handle, USB_ENDPOINT_IN+1,
      USB_REQ_GET_DESCRIPTOR, (USB_DT_REPORT << 8) + 0, 0,
      hidif->report_desc, hidif->report_len, USB_TIMEOUT);

  if (len < 0) {
    fprintf(stderr, "failed to get report descriptor of %s/%s.\n",
        hidif->device->bus->dirname, hidif->device->filename);
    return HID_RET_FAIL_GET_REPORT;
  }

  if (len < hidif->report_len) {
    fprintf(stderr, "HID descriptor of device at %s/%s is too short:"
                    "expected: %d bytes; got: %d bytes.\n",
        hidif->device->bus->dirname, hidif->device->filename,
        hidif->report_len, len);
    return HID_RET_REPORT_DESC_SHORT;
  }

  return HID_RET_SUCCESS;
}

hid_return hid_open(HIDInterface* hidif, HIDInterfaceMatcher const* match)
{
  if (!hidif) return HID_RET_INVALID_INTERFACE;

  hid_return ret = open_usb_device(hidif, match);
  if (ret != HID_RET_SUCCESS) return ret;

  if (usb_claim_interface(hidif->dev_handle, 0) < 0)
    return HID_RET_FAIL_CLAIM_IFACE;
  
  if (usb_set_altinterface(hidif->dev_handle, 0) < 0)
    return HID_RET_FAIL_SET_ALTIFACE;

  ret = init_hid_descriptor(hidif);
  if (ret != HID_RET_SUCCESS) return ret;

  ret = init_report_descriptor(hidif);
  if (ret != HID_RET_SUCCESS) return ret;

  return HID_RET_SUCCESS;
}

hid_return hid_force_open(HIDInterface* hidif,
                          HIDInterfaceMatcher const* match,
                          unsigned short retries)
{
  if (!hidif) return HID_RET_INVALID_INTERFACE;

  hid_return ret = open_usb_device(hidif, match);
  if (ret != HID_RET_SUCCESS) return ret;

  if (usb_force_claim_interface(hidif->dev_handle, 0, retries) < 0)
    return HID_RET_FAIL_CLAIM_IFACE;
  
  if (usb_set_altinterface(hidif->dev_handle, 0) < 0)
    return HID_RET_FAIL_SET_ALTIFACE;

  ret = init_hid_descriptor(hidif);
  if (ret != HID_RET_SUCCESS) return ret;

  ret = init_report_descriptor(hidif);
  if (ret != HID_RET_SUCCESS) return ret;

  return HID_RET_SUCCESS;
}


hid_return hid_close(HIDInterface* hidif)
{
  if (!hidif) return HID_RET_INVALID_INTERFACE;

  if (hid_is_opened(hidif)) usb_close(hidif->dev_handle);
  hidif->dev_handle = NULL;
  hidif->device = NULL;
  if (hidif->report_desc) free(hidif->report_desc);
  
  return HID_RET_SUCCESS;
}

bool hid_is_opened(HIDInterface const* hidif)
{
  return hidif->dev_handle != NULL;
}

hid_return hid_write_identification(FILE* out, HIDInterface const* hidif)
{
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
