#define HID_INTERNAL

#include <hid.h>
#include <os.h>

#include <debug.h>
#include <assert.h>

/* TODO: this should probably be exported */
#define USB_TIMEOUT 5000

#define HID_REPORT_GET 0x01
#define HID_REPORT_SET 0x09

#define HID_RT_INPUT 0x01
#define HID_RT_OUTPUT 0x02
#define HID_RT_FEATURE 0x03

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
  hidif->hid_data = NULL;
  hidif->hid_parser = NULL;
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
  ASSERT(hidif->hid_parser);

  TRACE("initialising the HID descriptor for interface %d of USB device on %s/%s.",
      hidif->interface, hidif->device->bus->dirname, hidif->device->filename);

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
    WARNING("failed to get HID descriptor for interface %d of USB device on %s/%s.",
        hidif->interface, hidif->device->bus->dirname, hidif->device->filename);
    return HID_RET_NOT_HID_DEVICE;
  }

  if (len < BUFLEN) {
    WARNING("HID descriptor for interface %d of USB device on %s/%s is too short: "
            "expected: %d bytes; got: %d bytes.\n", hidif->interface,
        hidif->device->bus->dirname, hidif->device->filename, BUFLEN, len);
    return HID_RET_HID_DESC_SHORT;
  }

  /* TODO:
   * the constants 7 and 8 should be exported.
   */
  hidif->hid_parser->ReportDescSize = buffer[7] | (buffer[8] << 8);

  NOTICE("successfully initialised HID descriptor for interface %d of USB device on %s/%s.",
      hidif->interface, hidif->device->bus->dirname, hidif->device->filename);

  return HID_RET_SUCCESS;
}

static hid_return init_report_descriptor(HIDInterface* hidif)
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));
  ASSERT(hidif->hid_parser);

  TRACE("initialising the report descriptor for interface %d of USB device on %s/%s.",
      hidif->interface, hidif->device->bus->dirname, hidif->device->filename);

  if (hidif->hid_parser->ReportDescSize > REPORT_DSC_SIZE) {
    ERROR("report descriptor size for interface %d of USB device on %s/%s "
          "exceeds maximum size: %d > %d\n", hidif->interface,
          hidif->device->bus->dirname, hidif->device->filename,
          hidif->hid_parser->ReportDescSize, REPORT_DSC_SIZE);
    return HID_RET_REPORT_DESC_LONG;
  }

  TRACE("retrieving report descriptor from device");
  int len = usb_control_msg(hidif->dev_handle, USB_ENDPOINT_IN+1,
      USB_REQ_GET_DESCRIPTOR, (USB_DT_REPORT << 8) + 0, 0,
      hidif->hid_parser->ReportDesc, hidif->hid_parser->ReportDescSize,
      USB_TIMEOUT);

  if (len < 0) {
    WARNING("failed to get report descriptor for interface %d of USB device on %s/%s.",
        hidif->interface, hidif->device->bus->dirname, hidif->device->filename);
    return HID_RET_FAIL_GET_REPORT;
  }

  if (len < hidif->hid_parser->ReportDescSize) {
    WARNING("HID descriptor for interface %d of USB device on %s/%s is too short: "
            "expected: %d bytes; got: %d bytes.\n", hidif->interface,
        hidif->device->bus->dirname, hidif->device->filename,
        hidif->hid_parser->ReportDescSize, len);
    return HID_RET_REPORT_DESC_SHORT;
  }

  NOTICE("successfully initialised report descriptor for interface %d of USB device on %s/%s.",
      hidif->interface, hidif->device->bus->dirname, hidif->device->filename);

  return HID_RET_SUCCESS;
}

static hid_return init_hid_parser(HIDInterface* hidif)
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));

  TRACE("initialising the HID parser for interface %d of USB device on %s/%s.",
      hidif->interface, hidif->device->bus->dirname, hidif->device->filename);

  TRACE("allocating space for HIDData structure...");
  hidif->hid_data = (HIDData*)malloc(sizeof(HIDData));
  if (!hidif->hid_data) {
    ERROR("failed to allocate memory for HIDData structure.");
    return HID_RET_FAIL_ALLOC;
  }
  TRACE("successfully allocated memory for HIDData strcture.");

  TRACE("allocating space for HIDParser structure...");
  hidif->hid_parser = (HIDParser*)malloc(sizeof(HIDParser));
  if (!hidif->hid_parser) {
    ERROR("failed to allocate memory for HIDParser structure.");
    return HID_RET_FAIL_ALLOC;
  }
  TRACE("successfully allocated memory for HIDParser strcture.");

  NOTICE("successfully initialised the HID parser for interface %d of USB device on %s/%s.",
      hidif->interface, hidif->device->bus->dirname, hidif->device->filename);
  
  return HID_RET_SUCCESS;
}

static void reset_hid_parser(HIDInterface* hidif)
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));
  ASSERT(hidif->hid_parser);

  if (!hid_is_opened(hidif)) {
    WARNING("the device has not been opened.");
    return;
  }

  if (!hidif->hid_parser) {
    WARNING("the HID parser has not been initialised.");
    return;
  }

  TRACE("resetting the HID parser for interface %d of USB device on %s/%s...",
      hidif->interface, hidif->device->bus->dirname, hidif->device->filename);
  ResetParser(hidif->hid_parser);
}

static hid_return setup_hid_parser(HIDInterface* hidif)
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));
  ASSERT(hidif->hid_parser);
  ASSERT(hidif->hid_data);
  
  TRACE("setting up the HID parser for interface %d of USB device on %s/%s...",
      hidif->interface, hidif->device->bus->dirname, hidif->device->filename);

  reset_hid_parser(hidif);
  
  /* TODO: the return value here should be used, no? */
  TRACE("parsing the HID tree of interface %d of USB device on %s/%s...",
      hidif->interface, hidif->device->bus->dirname, hidif->device->filename);
  HIDParse(hidif->hid_parser, hidif->hid_data);

  NOTICE("successfully set up the HID parser for interface %d of USB device on %s/%s.",
      hidif->interface, hidif->device->bus->dirname, hidif->device->filename);

  return HID_RET_SUCCESS;
}

static hid_return init_interface(HIDInterface* hidif)
{
  hid_return ret = init_hid_parser(hidif);
  if (ret != HID_RET_SUCCESS) {
    hid_close(hidif);
    return ret;
  }

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

  ret = setup_hid_parser(hidif);
  if (ret != HID_RET_SUCCESS) {
    hid_close(hidif);
    return ret;
  }

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

  TRACE("claiming interface %d of USB device %s/%s...", interface,
      hidif->device->bus->dirname, hidif->device->filename);
  if (usb_claim_interface(hidif->dev_handle, interface) < 0) {
    WARNING("failed to claim interface %d of USB device %s/%s...", interface,
              hidif->device->bus->dirname, hidif->device->filename);
    hid_close(hidif);
    return HID_RET_FAIL_CLAIM_IFACE;
  }
  hidif->interface = interface;
  NOTICE("successfully claimed interface %d of USB device %s/%s...", interface,
      hidif->device->bus->dirname, hidif->device->filename);
  
  /* TODO: what's this anyway?
   * if (usb_set_altinterface(hidif->dev_handle, interface) < 0)
   * return HID_RET_FAIL_SET_ALTIFACE;
   */

  init_interface(hidif);

  NOTICE("successfully initialised interface %d of USB device %s/%s...", interface,
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

  TRACE("claiming interface %d of USB device %s/%s...", interface,
      hidif->device->bus->dirname, hidif->device->filename);
  hid_os_force_claim(hidif, interface, match, retries);
  if (ret != HID_RET_SUCCESS) {
    WARNING("failed to claim interface %d of USB device %s/%s...", interface,
        hidif->device->bus->dirname, hidif->device->filename);
    hid_close(hidif);
    return ret;
  }

  hidif->interface = interface;
  NOTICE("successfully claimed interface %d of USB device %s/%s...", interface,
      hidif->device->bus->dirname, hidif->device->filename);
 
  /* TODO: what's this anyway?
   * if (usb_set_altinterface(hidif->dev_handle, 0) < 0)
   * return HID_RET_FAIL_SET_ALTIFACE;
   */

  init_interface(hidif);

  NOTICE("successfully initialised interface %d of USB device %s/%s...", interface,
      hidif->device->bus->dirname, hidif->device->filename);

  return HID_RET_SUCCESS;
}

hid_return hid_close(HIDInterface* hidif)
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));

  if (!hidif) return HID_RET_INVALID_INTERFACE;

  if (hid_is_opened(hidif)) {

    TRACE("closing interface %d of USB device on %s/%s...", hidif->interface, 
        hidif->device->bus->dirname, hidif->device->filename);

    reset_hid_parser(hidif);
    //usb_reset(hidif->dev_handle);
    
    TRACE("releasing interface %d of USB device on %s/%s...", hidif->interface,
        hidif->device->bus->dirname, hidif->device->filename);
    /*if (usb_release_interface(hidif->dev_handle, hidif->interface) < 0)
      WARNING("failed to release interface %d of USB device on %s/%s.",
          hidif->interface, hidif->device->bus->dirname,
          hidif->device->filename);*/

    TRACE("closing USB device on %s/%s.",
        hidif->device->bus->dirname, hidif->device->filename);
    if (usb_close(hidif->dev_handle) < 0)
      WARNING("failed to close USB device on %s/%s.",
          hidif->device->bus->dirname, hidif->device->filename);
    NOTICE("successfully closed USB device.");

  }

  TRACE("freeing memory allocated for HID parser...");
  if(hidif->hid_parser) free(hidif->hid_parser);
  if(hidif->hid_data) free(hidif->hid_data);
    
  TRACE("resetting HIDInterface...");
  reset_HIDInterface(hidif);

  return HID_RET_SUCCESS;
}

bool hid_is_opened(HIDInterface const* hidif)
{
  ASSERT(hidif);
  return hidif && hidif->dev_handle != NULL;
}

static hid_return format_path(char* buffer, unsigned int length,
                 int const path[], unsigned int const depth)
{
  byte const ITEMSIZE = sizeof("0xdeadbeef");
  unsigned int i = 0;

  TRACE("formatting device path...");
  for (; i < depth; ++i) {
    if (length < ITEMSIZE) {
      WARNING("not enough space in buffer to finish formatting of path.")
      return HID_RET_OUT_OF_SPACE;
    }
    snprintf(buffer + i * ITEMSIZE, ITEMSIZE + 1, "0x%08x.", path[i]);
    length -= ITEMSIZE;
  }
  buffer[i * ITEMSIZE - 1] = '\0';

  return HID_RET_SUCCESS;
}

static void prepare_parse_path(HIDInterface* hidif,
                               int const path[], unsigned int const depth)
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));
  ASSERT(hidif->hid_data);
  ASSERT(path);

  unsigned int i = 0;

  TRACE("preparing search path of depth %d for parse tree of interface %d "
        "of USB device on %s/%s...", depth, hidif->interface,
        hidif->device->bus->dirname, hidif->device->filename);
  for (; i < depth; ++i) {
    hidif->hid_data->Path.Node[i].UPage = path[i] >> 16;
    hidif->hid_data->Path.Node[i].Usage = path[i] & 0x0000ffff;
  }

  hidif->hid_data->Path.Size = depth;

  TRACE("search path prepared for parse tree of interface %d "
      "of USB device on %s/%s.", hidif->interface,
      hidif->device->bus->dirname, hidif->device->filename);
}

hid_return find_object(HIDInterface* hidif,
                       int const path[], unsigned int const depth)
{
  prepare_parse_path(hidif, path, depth);

  if (FindObject(hidif->hid_parser, hidif->hid_data) == 1) {
    NOTICE("found requested item.");
    return HID_RET_SUCCESS;
  }

  byte const ITEMSIZE = sizeof("0xdeadbeef");
  char* buffer = (char*)malloc(depth * ITEMSIZE);
  format_path(buffer, depth * ITEMSIZE, path, depth); 
  WARNING("can't find requested item %s.\n", buffer);
  free(buffer);
  
  return HID_RET_NOT_FOUND;
}

hid_return hid_get_item_value(HIDInterface* hidif,
                              int const path[], unsigned int const depth,
                              double *const value)
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));
  ASSERT(value);

  if (!hid_is_opened(hidif)) {
    WARNING("the device has not been opened.");
    return HID_RET_NOT_OPENED;
  }

  hidif->hid_data->Type = ITEM_INPUT;
  hidif->hid_data->ReportID = 0;

  find_object(hidif, path, depth);

  char raw_buf[2];
  int len = usb_control_msg(hidif->dev_handle,
      USB_ENDPOINT_IN + USB_TYPE_VENDOR + USB_RECIP_INTERFACE,
      HID_REPORT_GET,
      hidif->hid_data->ReportID + (HID_RT_INPUT << 8),
      hidif->interface,
      raw_buf, 2, USB_TIMEOUT);

  printf("len: %d, err: %s\n", len, usb_strerror());


/* char raw_buf[10000];
  printf("ret: %d\n", usb_control_msg(hidif->dev_handle, 
       USB_ENDPOINT_IN + USB_TYPE_CLASS + USB_RECIP_INTERFACE,
       USB_REQ_GET_DESCRIPTOR,
       hidif->hid_data->ReportID + (USB_DT_STRING << 8),
       0, raw_buf, 10000, USB_TIMEOUT));

  int x = 0;
*/
//     if ((replen=libusb_get_report(hData.ReportID, raw_buf, 10)) > 0) {
        /* Extract the data value */
  //      GetValue((const unsigned char *) raw_buf, &hData);

  return HID_RET_SUCCESS;
}

hid_return hid_get_item_string(HIDInterface* hidif,
                               int const path[], unsigned int const depth,
                               char *const value, unsigned int const maxlen)
{
  return HID_RET_SUCCESS;
}

hid_return hid_set_item_value(HIDInterface* hidif,
                              int const path[], unsigned int const depth,
                              double const value)
{
  return HID_RET_SUCCESS;
}

hid_return hid_write_identification(FILE* out, HIDInterface const* hidif)
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));
  ASSERT(out);

  if (!hid_is_opened(hidif)) {
    WARNING("the device has not been opened.");
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
  fprintf(out, "-----------------------------------------------------------\n");
  
  return HID_RET_SUCCESS;
}

hid_return hid_dump_tree(FILE* out, HIDInterface* hidif)
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));
  ASSERT(out);

  if (!hid_is_opened(hidif)) {
    WARNING("the device has not been opened.");
    return HID_RET_NOT_OPENED;
  }

  reset_hid_parser(hidif);
  
  TRACE("iterating the parse tree for interface %d of USB device on %s/%s...",
      hidif->interface, hidif->device->bus->dirname, hidif->device->filename);
      
  unsigned int i = 0;

  fprintf(out, "-----------------------------------------------------------\n");
  fprintf(out, "parse tree of HIDInterface at 0x%08x:\n", (unsigned int)hidif);

  while (HIDParse(hidif->hid_parser, hidif->hid_data)) {
    fprintf(out, "  path: ");
    for (i = 0; i < hidif->hid_data->Path.Size; ++i) {
      fprintf(out, "0x%08x", (hidif->hid_data->Path.Node[i].UPage << 16)
          | hidif->hid_data->Path.Node[i].Usage);
      if (i < hidif->hid_data->Path.Size - 1) fputc('.', out);
    }
    fprintf(out, "; type: 0x%02x\n", hidif->hid_data->Type);
  }
  fprintf(out, "-----------------------------------------------------------\n");

  reset_hid_parser(hidif);

  return HID_RET_SUCCESS;
}
