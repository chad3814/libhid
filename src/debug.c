#define HID_INTERNAL

#include <debug.h>

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
  fprintf(out, "-----------------------------------------------------------\n");
  fprintf(out, "usb_bus instance at: 0x%08x\n", (unsigned int)usbbus);
  fprintf(out, "  dirname:           %s\n", usbbus->dirname);
  fprintf(out, "  devices:           0x%08x\n", (unsigned int)usbbus->devices);
  fprintf(out, "  prev:              0x%08x\n", (unsigned int)usbbus->prev);
  fprintf(out, "  next:              0x%08x\n", (unsigned int)usbbus->next);
}

void trace_usb_device(FILE* out, struct usb_device const* usbdev)
{
  fprintf(out, "-----------------------------------------------------------\n");
  fprintf(out, "usb_device instance at: 0x%08x\n", (unsigned int)usbdev);
  fprintf(out, "  prev:                 0x%08x\n", (unsigned int)usbdev->prev);
  fprintf(out, "  next:                 0x%08x\n", (unsigned int)usbdev->next);
  fprintf(out, "  filename:             %s\n", usbdev->filename);
  fprintf(out, "  bus:                  0x%08x\n", (unsigned int)usbdev->bus);
  fprintf(out, "  descriptor:           0x%08x\n", (unsigned int)&usbdev->descriptor);
  fprintf(out, "  config:               0x%08x\n", (unsigned int)usbdev->config);
  fprintf(out, "  dev:                  0x%08x\n", (unsigned int)usbdev->dev);
}

void trace_usb_device_descriptor(FILE* out, struct usb_device_descriptor const* descriptor)
{
  fprintf(out, "-----------------------------------------------------------\n");
  fprintf(out, "usb_device_descriptor instance at: 0x%08x\n", (unsigned int)descriptor);
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
  fprintf(out, "-----------------------------------------------------------\n");
  fprintf(out, "usb_config_descriptor instance at: 0x%08x\n", (unsigned int)config);
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
  fprintf(out, "-----------------------------------------------------------\n");
  fprintf(out, "usb_dev_handle instance at: 0x%08x\n", (unsigned int)usbdev_h);
  fprintf(out, "  fd:                       %d\n", usbdev_h->fd);
  fprintf(out, "  bus:                      0x%08x\n", (unsigned int)usbdev_h->bus);
  fprintf(out, "  device:                   0x%08x\n", (unsigned int)usbdev_h->device);
  fprintf(out, "  config:                   %d\n", usbdev_h->config);
  fprintf(out, "  interface:                %d\n", usbdev_h->interface);
  fprintf(out, "  altsetting:               %d\n", usbdev_h->altsetting);
  fprintf(out, "  impl_info:                0x%08x\n", (unsigned int)usbdev_h->impl_info);
}
