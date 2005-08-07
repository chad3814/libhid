#define HID_INTERNAL

#include <hid.h>
#include <hid_helpers.h>
#include <os.h>
#include <constants.h>
#include <compiler.h>

#include <debug.h>
#include <assert.h>

/*!@brief Send a control message to retrieve an entire input report
 *
 * @param[in] hidif Which interface to query
 * @param[in] path  Path to input item (to find Report ID)
 * @param[in] depth See hid_find_object()
 * @param[out] buffer Result is stored here
 * @param[in] size  How many bytes to fetch
 */
hid_return hid_get_input_report(HIDInterface* const hidif, int const path[],
    unsigned int const depth, char* const buffer, unsigned int const size)
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));
  ASSERT(buffer);

  if (!buffer) return HID_RET_INVALID_PARAMETER;

  if (!hid_is_opened(hidif)) {
    WARNING("the device has not been opened.");
    return HID_RET_DEVICE_NOT_OPENED;
  }

  TRACE("looking up report ID...");
  hidif->hid_data->Type = ITEM_INPUT;
  hidif->hid_data->ReportID = 0;

  hid_find_object(hidif, path, depth);

  TRACE("retrieving report ID %d from USB device %s...", 
        hidif->hid_data->ReportID, hidif->id);

  int len = usb_control_msg(hidif->dev_handle,
      USB_ENDPOINT_IN + USB_TYPE_CLASS + USB_RECIP_INTERFACE,
      HID_REPORT_GET,
      hidif->hid_data->ReportID + (HID_RT_INPUT << 8),
      hidif->interface,
      buffer, size, USB_TIMEOUT);

  if (len < 0) {
    WARNING("failed to retrieve report from USB device %s.", hidif->id);
    return HID_RET_FAIL_GET_REPORT;
  }

  if (len != (signed)size) {
    WARNING("failed to retrieve complete report from USB device %s; "
        "requested: %d bytes, got: %d bytes.", hidif->id, size, len);
    return HID_RET_FAIL_GET_REPORT;
  }

  NOTICE("successfully retrieved report from USB device %s.", hidif->id);
  return HID_RET_SUCCESS;
}

/*!@brief Send an entire output report to the device
 *
 * @param[in] hidif Which interface to send to
 * @param[in] path  Path to an output item (to find Report ID)
 * @param[in] depth See hid_find_object()
 * @param[in] buffer Output Report
 * @param[in] size  How many bytes to send
 */
hid_return hid_set_output_report(HIDInterface* const hidif, int const path[],
    unsigned int const depth, char const* const buffer, unsigned int const size)
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));
  ASSERT(buffer);

  if (!buffer) return HID_RET_INVALID_PARAMETER;

  if (!hid_is_opened(hidif)) {
    WARNING("the device has not been opened.");
    return HID_RET_DEVICE_NOT_OPENED;
  }

  TRACE("looking up report ID...");
  hidif->hid_data->Type = ITEM_OUTPUT;
  hidif->hid_data->ReportID = 0;

  hid_find_object(hidif, path, depth);

  TRACE("sending report ID %d to USB device %s...", 
        hidif->hid_data->ReportID, hidif->id);

  int len = usb_control_msg(hidif->dev_handle,
      USB_ENDPOINT_OUT + USB_TYPE_CLASS + USB_RECIP_INTERFACE,
      HID_REPORT_SET,
      hidif->hid_data->ReportID + (HID_RT_OUTPUT << 8),
      hidif->interface,
      (char*)buffer, size, USB_TIMEOUT);

  if (len < 0) {
    WARNING("failed to send report to USB device %s.", hidif->id);
    return HID_RET_FAIL_SET_REPORT;
  }

  if (len != (signed)size) {
    WARNING("failed to send complete report to USB device %s; "
        "requested: %d bytes, sent: %d bytes.", hidif->id, 
        size, len);
    return HID_RET_FAIL_SET_REPORT;
  }

  NOTICE("successfully sent report to USB device %s.", hidif->id);
  return HID_RET_SUCCESS;
}

/*!@brief Retrieve a numeric input item
 *
 * @param[in] hidif Which interface to send to
 * @param[in] path  Path to input item
 * @param[in] depth See hid_find_object()
 * @param[out] value Result from hid_extract_value()
 *
 * @todo Handle exponent and unit conversion (separate library?)
 */
hid_return hid_get_item_value(HIDInterface* const hidif, int const path[],
    unsigned int const depth, double *const value)
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));

  unsigned int size;
  unsigned char buffer[32]; /*! @todo Dynamically allocate the item buffer */

  if (!hid_is_opened(hidif)) {
    WARNING("the device has not been opened.");
    return HID_RET_DEVICE_NOT_OPENED;
  }

  TRACE("retrieving report from USB device %s...", hidif->id);
  hidif->hid_data->Type = ITEM_FEATURE;
  hidif->hid_data->ReportID = 0;

  /* TODO: i think this and the buffer stuff should be passed in */
  hid_find_object(hidif, path, depth);
  hid_get_report_size(hidif, hidif->hid_data->ReportID,
      hidif->hid_data->Type, &size);

  ASSERT(size <= 32); /* remove when buffer situation is fixed. */

  int len = usb_control_msg(hidif->dev_handle,
      USB_ENDPOINT_IN + USB_TYPE_CLASS + USB_RECIP_INTERFACE,
      HID_REPORT_GET,
      hidif->hid_data->ReportID + (HID_RT_FEATURE << 8),
      hidif->interface,
      (char*)buffer, size, USB_TIMEOUT);

  if (len < 0) {
    WARNING("failed to retrieve report from USB device %s.", hidif->id);
    return HID_RET_FAIL_GET_REPORT;
  }

  if ((unsigned)len != size) {
    WARNING("failed to retrieve complete report from USB device %s; "
        "requested: %d bytes, got: %d bytes.", hidif->id, 
        size, len);
    return HID_RET_FAIL_GET_REPORT;
  }

  if (hid_extract_value(hidif, buffer, value) != HID_RET_SUCCESS) {
    return HID_RET_FAIL_GET_REPORT;
  }

  NOTICE("successfully retrieved report from USB device %s.", hidif->id);
  return HID_RET_SUCCESS;
}

/*!@brief Currently unimplemented */
hid_return hid_get_item_string(HIDInterface* const hidif UNUSED,
    int const path[] UNUSED, unsigned int const depth UNUSED,
    char *const value UNUSED, unsigned int const maxlen UNUSED)
{
  bool const not_yet_implemented = false;
  ASSERT(not_yet_implemented);
  return HID_RET_SUCCESS;
}


/*!@brief Currently unimplemented */
hid_return hid_set_item_value(HIDInterface* const hidif UNUSED,
    int const path[] UNUSED, unsigned int const depth UNUSED,
    double const value UNUSED)
{
  bool const not_yet_implemented = false;
  ASSERT(not_yet_implemented);
  return HID_RET_SUCCESS;
}

/*!@brief Read from the interrupt endpoint
 *
 * @param[in]  hidif  Which interface to send to
 * @param[in]  ep     Which endpoint to read
 * @param[out] bytes  Buffer to store results
 * @param[in]  size   How many bytes to read
 * @param[in] timeout How long to wait, in milliseconds
 *
 * Upon successful completion, the @a bytes array will contain an input report
 * from the device. It is up to the programmer to determine which input
 * endpoint to read, and to ensure that the endpoint is bitwise-ORed with 0x80
 * (USB_ENDPOINT_IN).
 */
hid_return hid_interrupt_read(HIDInterface * const hidif,
     unsigned int const ep, char* const bytes, unsigned int const size, 
     unsigned int const timeout) 
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));
  ASSERT(bytes);

  if (!bytes) return HID_RET_INVALID_PARAMETER;

  if (!hid_is_opened(hidif)) {
    WARNING("the device has not been opened.");
    return HID_RET_DEVICE_NOT_OPENED;
  }

  TRACE("retrieving interrupt report from device %s ...", hidif->id);

  int len = usb_interrupt_read(hidif->dev_handle,
                               ep,
                               bytes,
                               size,
                               timeout);

  if (len < 0) {
    WARNING("failed to get interrupt read from device %s", hidif->id);
    return HID_RET_FAIL_INT_READ;
  }

  if (len != (signed)size) {
    WARNING("failed to get all of interrupt report from device %s; "
      "requested: %d bytes, sent: %d bytes.", hidif->id,
      size, len);
    return HID_RET_FAIL_INT_READ;
  }

  NOTICE("successfully got interrupt report from device %s", hidif->id);
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
