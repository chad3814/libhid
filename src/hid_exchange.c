#define HID_INTERNAL

#include <hid.h>
#include <hid_helpers.h>
#include <os.h>
#include <constants.h>
#include <macros.h>

#include <debug.h>
#include <assert.h>

/* FIXME: Temporary solution, while waiting for hidparser re inclusion in libhid! */
uchar* GetReportOffset(HIDParser* pParser, const uchar ReportID, const uchar ReportType);

hid_return hid_get_input_report(HIDInterface* const hidif, int const path[],
    unsigned int const depth, char* const buffer, unsigned int const size)
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));
  ASSERT(buffer);

  if (!buffer) return HID_RET_INVALID_PARAMETER;

  if (!hid_is_opened(hidif)) {
    WARNING("the device has not been opened.");
    return HID_RET_NOT_OPENED;
  }

  TRACE("retrieving report from " TRACEDEVICESTR "...", TRACEDEVICEARGS);
  hidif->hid_data->Type = ITEM_INPUT;
  hidif->hid_data->ReportID = 0;

  hid_find_object(hidif, path, depth);

  int len = usb_control_msg(hidif->dev_handle,
      USB_ENDPOINT_IN + USB_TYPE_CLASS + USB_RECIP_INTERFACE,
      HID_REPORT_GET,
      hidif->hid_data->ReportID + (HID_RT_OUTPUT << 8),
      hidif->interface,
      buffer, size, USB_TIMEOUT);

  if (len < 0) {
    WARNING("failed to retrieve report from " TRACEDEVICESTR ".", TRACEDEVICEARGS);
    return HID_RET_FAIL_GET_REPORT;
  }

  if (len != size) {
    WARNING("failed to retrieve complete report to " TRACEDEVICESTR
        "; requested: %d bytes, got: %d bytes.", TRACEDEVICEARGS, 
        size, len);
    return HID_RET_FAIL_GET_REPORT;
  }

  NOTICE("successfully retrieved report from " TRACEDEVICESTR ".", TRACEDEVICEARGS);
  return HID_RET_SUCCESS;
}

hid_return hid_set_output_report(HIDInterface* const hidif, int const path[],
    unsigned int const depth, char const* const buffer, unsigned int const size)
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));
  ASSERT(buffer);

  if (!buffer) return HID_RET_INVALID_PARAMETER;

  if (!hid_is_opened(hidif)) {
    WARNING("the device has not been opened.");
    return HID_RET_NOT_OPENED;
  }

  TRACE("sending report to " TRACEDEVICESTR "...", TRACEDEVICEARGS);
  hidif->hid_data->Type = ITEM_OUTPUT;
  hidif->hid_data->ReportID = 0;

  hid_find_object(hidif, path, depth);

  int len = usb_control_msg(hidif->dev_handle,
      USB_ENDPOINT_OUT + USB_TYPE_CLASS + USB_RECIP_INTERFACE,
      HID_REPORT_SET,
      hidif->hid_data->ReportID + (HID_RT_OUTPUT << 8),
      hidif->interface,
      buffer, size, USB_TIMEOUT);

  if (len < 0) {
    WARNING("failed to send report to " TRACEDEVICESTR ".", TRACEDEVICEARGS);
    return HID_RET_FAIL_SET_REPORT;
  }

  if (len != size) {
    WARNING("failed to send complete report to " TRACEDEVICESTR
        "; requested: %d bytes, sent: %d bytes.", TRACEDEVICEARGS, 
        size, len);
    return HID_RET_FAIL_SET_REPORT;
  }

  NOTICE("successfully sent report to " TRACEDEVICESTR ".", TRACEDEVICEARGS);
  return HID_RET_SUCCESS;
}

hid_return hid_get_item_value(HIDInterface* const hidif, int const path[],
			      unsigned int const depth, double *const value)
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));

  int size;
  unsigned char buffer[32]; /* FIXME: dyn alloc or argument */

  if (!hid_is_opened(hidif)) {
    WARNING("the device has not been opened.");
    return HID_RET_NOT_OPENED;
  }

  TRACE("retrieving report from " TRACEDEVICESTR "...", TRACEDEVICEARGS);
  hidif->hid_data->Type = ITEM_FEATURE;
  hidif->hid_data->ReportID = 0;

  hid_find_object(hidif, path, depth);
  size = *GetReportOffset(hidif->hid_parser,
			  hidif->hid_data->ReportID,
			  hidif->hid_data->Type);

  int len = usb_control_msg(hidif->dev_handle,
      USB_ENDPOINT_IN + USB_TYPE_CLASS + USB_RECIP_INTERFACE,
      HID_REPORT_GET,
      hidif->hid_data->ReportID + (HID_RT_FEATURE << 8),
      hidif->interface,
      buffer, size, USB_TIMEOUT);

  if (len < 0) {
    WARNING("failed to retrieve report from " TRACEDEVICESTR ".", TRACEDEVICEARGS);
    return HID_RET_FAIL_GET_REPORT;
  }

  if (len != size) {
    WARNING("failed to retrieve complete report to " TRACEDEVICESTR
        "; requested: %d bytes, got: %d bytes.", TRACEDEVICEARGS, 
        size, len);
    return HID_RET_FAIL_GET_REPORT;
  }

  NOTICE("successfully retrieved report from " TRACEDEVICESTR ".", TRACEDEVICEARGS);
  return HID_RET_SUCCESS;
}

hid_return hid_get_item_string(HIDInterface* const hidif, int const path[],
    unsigned int const depth, char *const value, unsigned int const maxlen)
{
  bool const not_yet_implemented = false;
  ASSERT(not_yet_implemented);
  return HID_RET_SUCCESS;
}

hid_return hid_set_item_value(HIDInterface* const hidif, int const path[],
    unsigned int const depth, double const value)
{
  bool const not_yet_implemented = false;
  ASSERT(not_yet_implemented);
  return HID_RET_SUCCESS;
}
