#define HID_INTERNAL

#include <hid.h>
#include <hid_helpers.h>
#include <os.h>
#include <constants.h>
#include <macros.h>

#include <debug.h>
#include <assert.h>

hid_return hid_get_input_report(HIDInterface* const hidif, int const path[],
    unsigned int const depth, char* const buffer, unsigned int const size)
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));
  ASSERT(buffer);

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
      hidif->hid_data->ReportID + (HID_RT_INPUT << 8),
      hidif->interface,
      buffer, size, USB_TIMEOUT);

    

//     if ((replen=libusb_get_report(hData.ReportID, raw_buf, 10)) > 0) {
        /* Extract the data value */
  //      GetValue((const unsigned char *) raw_buf, &hData);

  return HID_RET_SUCCESS;
}

hid_return hid_set_output_report(HIDInterface* const hidif, int const path[],
    unsigned int const depth, char const* const buffer, unsigned int const size)
{
  return HID_RET_SUCCESS;
}

hid_return hid_get_item_string(HIDInterface* const hidif, int const path[],
    unsigned int const depth, char *const value, unsigned int const maxlen)
{
  return HID_RET_SUCCESS;
}

hid_return hid_set_item_value(HIDInterface* const hidif, int const path[],
    unsigned int const depth, double const value)
{
  return HID_RET_SUCCESS;
}
