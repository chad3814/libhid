#define HID_INTERNAL

#include <hid.h>
#include <hid_helpers.h>

#include <debug.h>
#include <assert.h>

hid_return hid_write_identification(FILE* const out,
    HIDInterface const* const hidif)
{
  if (!hid_is_opened(hidif)) {
    ERROR("cannot write identification of unopened HIDinterface.");
    return HID_RET_DEVICE_NOT_OPENED;
  }

  if (!out) {
    ERROR("cannot write HIDinterface identification of USB device %s "
        "to NULL output stream.", hidif->id);
    return HID_RET_INVALID_PARAMETER;
  }
  
  int len;
  unsigned short const BUFLEN = 256;
  char buffer[BUFLEN];
  
  fprintf(out, "device identification of HIDInterface %s:\n", hidif->id);
  fprintf(out, "  dev_handle:    0x%08lx\n", (unsigned long)hidif->dev_handle);
  fprintf(out, "  device:        0x%08lx\n", (unsigned long)hidif->device);
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

hid_return hid_dump_tree(FILE* const out, HIDInterface* const hidif)
{
  if (!hid_is_opened(hidif)) {
    ERROR("cannot dump tree of unopened HIDinterface.");
    return HID_RET_DEVICE_NOT_OPENED;
  }

  if (!out) {
    ERROR("cannot dump HIDinterface tree of USB device %s to NULL output stream.",
        hidif->id);
    return HID_RET_INVALID_PARAMETER;
  }
  
  hid_reset_parser(hidif);
  
  TRACE("iterating the parse tree for USB device %s...", hidif->id);
      
  unsigned int i = 0;

  fprintf(out, "parse tree of HIDInterface %s:\n", hidif->id);

  while (HIDParse(hidif->hid_parser, hidif->hid_data)) {
    fprintf(out, "  path: ");
    for (i = 0; i < hidif->hid_data->Path.Size; ++i) {
      fprintf(out, "0x%08x", (hidif->hid_data->Path.Node[i].UPage << 16)
          | hidif->hid_data->Path.Node[i].Usage);
      if ((signed)i < hidif->hid_data->Path.Size - 1) fputc('.', out);
    }
    fprintf(out, "; type: 0x%02x\n", hidif->hid_data->Type);
  }

  hid_reset_parser(hidif);

  return HID_RET_SUCCESS;
}

/* COPYRIGHT --
 *
 * This file is part of libhid, a user-space HID access library.
 * libhid is (c) 2003-2005
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
