#define HID_INTERNAL

#include <debug.h>
#include <assert.h>

HIDDebugLevel hid_debug_level = HID_DEBUG_NONE;
FILE* hid_debug_stream = NULL;

void hid_set_debug(HIDDebugLevel const level)
{
  hid_debug_level = level;
}

void hid_set_debug_stream(FILE* const outstream)
{
  hid_debug_stream = outstream;
}

void hid_set_usb_debug(int const level)
{
  usb_set_debug(level);
}

/* NOTE: included from libusb/usbi.h. UGLY, i know, but so is libusb! */
struct usb_dev_handle {
  int fd;
  struct usb_bus *bus;
  struct usb_device *device;
  int config;
  int interface;
  int altsetting;
  void *impl_info;
};

void trace_usb_bus(FILE* out, struct usb_bus const* usbbus)
{
  fprintf(out, "usb_bus instance at: 0x%08lx\n", (unsigned long)usbbus);
  fprintf(out, "  dirname:           %s\n", usbbus->dirname);
  fprintf(out, "  devices:           0x%08lx\n", (unsigned long)usbbus->devices);
  fprintf(out, "  prev:              0x%08lx\n", (unsigned long)usbbus->prev);
  fprintf(out, "  next:              0x%08lx\n", (unsigned long)usbbus->next);
}

void trace_usb_device(FILE* out, struct usb_device const* usbdev)
{
  fprintf(out, "usb_device instance at: 0x%08lx\n", (unsigned long)usbdev);
  fprintf(out, "  prev:                 0x%08lx\n", (unsigned long)usbdev->prev);
  fprintf(out, "  next:                 0x%08lx\n", (unsigned long)usbdev->next);
  fprintf(out, "  filename:             %s\n", usbdev->filename);
  fprintf(out, "  bus:                  0x%08lx\n", (unsigned long)usbdev->bus);
  fprintf(out, "  descriptor:           0x%08lx\n", (unsigned long)&usbdev->descriptor);
  fprintf(out, "  config:               0x%08lx\n", (unsigned long)usbdev->config);
  fprintf(out, "  dev:                  0x%08lx\n", (unsigned long)usbdev->dev);
}

void trace_usb_device_descriptor(FILE* out, struct usb_device_descriptor const* descriptor)
{
  fprintf(out, "usb_device_descriptor instance at: 0x%08lx\n", (unsigned long)descriptor);
  fprintf(out, "  bLength:                         %d\n", descriptor->bLength);
  fprintf(out, "  bDescriptorType:                 %d\n", descriptor->bDescriptorType); 
  fprintf(out, "  bcdUSB:                          %d\n", descriptor->bcdUSB);
  fprintf(out, "  bDeviceClass:                    %d\n", descriptor->bDeviceClass);
  fprintf(out, "  bDeviceSubClass:                 %d\n", descriptor->bDeviceSubClass);
  fprintf(out, "  bDeviceProtocol:                 %d\n", descriptor->bDeviceProtocol);
  fprintf(out, "  bMaxPacketSize0:                 %d\n", descriptor->bMaxPacketSize0);
  fprintf(out, "  idVendor:                        %d\n", descriptor->idVendor);
  fprintf(out, "  idProduct:                       %d\n", descriptor->idProduct);
  fprintf(out, "  bcdDevice:                       %d\n", descriptor->bcdDevice);
  fprintf(out, "  iManufacturer:                   %d\n", descriptor->iManufacturer);
  fprintf(out, "  iProduct:                        %d\n", descriptor->iProduct);
  fprintf(out, "  iSerialNumber:                   %d\n", descriptor->iSerialNumber);
  fprintf(out, "  bNumConfigurations:              %d\n", descriptor->bNumConfigurations);
}

void trace_usb_config_descriptor(FILE* out, struct usb_config_descriptor const* config)
{
  fprintf(out, "usb_config_descriptor instance at: 0x%08lx\n", (unsigned long)config);
  fprintf(out, "  bLength:                         %d\n", config->bLength);
  fprintf(out, "  bDescriptorType:                 %d\n", config->bDescriptorType);
  fprintf(out, "  wTotalLength:                    %d\n", config->wTotalLength);
  fprintf(out, "  bNumInterfaces:                  %d\n", config->bNumInterfaces);
  fprintf(out, "  bConfigurationValue:             %d\n", config->bConfigurationValue);
  fprintf(out, "  iConfiguration:                  %d\n", config->iConfiguration);
  fprintf(out, "  bmAttributes:                    %d\n", config->bmAttributes);
  fprintf(out, "  MaxPower:                        %d\n", config->MaxPower);
}

void trace_usb_dev_handle(FILE* out, usb_dev_handle const* usbdev_h)
{
  fprintf(out, "usb_dev_handle instance at: 0x%08lx\n", (unsigned long)usbdev_h);
  fprintf(out, "  fd:                       %d\n", usbdev_h->fd);
  fprintf(out, "  bus:                      0x%08lx\n", (unsigned long)usbdev_h->bus);
  fprintf(out, "  device:                   0x%08lx\n", (unsigned long)usbdev_h->device);
  fprintf(out, "  config:                   %d\n", usbdev_h->config);
  fprintf(out, "  interface:                %d\n", usbdev_h->interface);
  fprintf(out, "  altsetting:               %d\n", usbdev_h->altsetting);
  fprintf(out, "  impl_info:                0x%08lx\n", (unsigned long)usbdev_h->impl_info);
}

/* COPYRIGHT --
 *
 * This file is part of libhid, a user-space HID access library.
 * libhid is (c) 2003-2004
 *   Martin F. Krafft <libhid@pobox.madduck.net>
 *   Charles Lepple <clepple@ghz.cc>
 *   Arnaud Quette <arnaud.quette@free.fr> && <arnaud.quette@mgeups.com>
 * and distributed under the terms of the GNU General Public License.
 * See the file ./COPYING in the source distribution for more information.
 *
 * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES
 * OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
