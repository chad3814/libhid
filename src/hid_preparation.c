#define HID_INTERNAL

#include <hid.h>
#include <hid_helpers.h>
#include <constants.h>

#include <debug.h>
#include <assert.h>
#include <macros.h>

static hid_return hid_prepare_hid_descriptor(HIDInterface* const hidif)
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));
  ASSERT(hidif->hid_parser);

  TRACE("initialising the HID descriptor for "
      TRACEDEVICESTR "...", TRACEDEVICEARGS);

  /* TODO: BUFLEN seems to depend on the device, so we need to do something
   * about the following.
   */
  byte const BUFLEN = 9;
  char buffer[BUFLEN];
  
  TRACE("retrieving HID descriptor for "
      TRACEDEVICESTR "...", TRACEDEVICEARGS);
  int len = usb_control_msg(hidif->dev_handle,
      USB_ENDPOINT_IN+1,
      USB_REQ_GET_DESCRIPTOR,
      (USB_DT_HID << 8) + 0, 0,
      buffer, BUFLEN,
      USB_TIMEOUT);

  if (len < 0) {
    WARNING("failed to get HID descriptor for " 
        TRACEDEVICESTR ".", TRACEDEVICEARGS);
    return HID_RET_NOT_HID_DEVICE;
  }

  if (len < BUFLEN) {
    WARNING("HID descriptor for " TRACEDEVICESTR " is too short; "
        "expected: %d bytes; got: %d bytes.\n", TRACEDEVICEARGS, BUFLEN, len);
    return HID_RET_HID_DESC_SHORT;
  }

  /* TODO:
   * the constants 7 and 8 should be exported.
   */
  hidif->hid_parser->ReportDescSize = buffer[7] | (buffer[8] << 8);

  NOTICE("successfully initialised HID descriptor for "
      TRACEDEVICESTR ".", TRACEDEVICEARGS);

  return HID_RET_SUCCESS;
}

static hid_return hid_prepare_report_descriptor(HIDInterface* const hidif)
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));
  ASSERT(hidif->hid_parser);

  TRACE("initialising the report descriptor for "
      TRACEDEVICESTR "...", TRACEDEVICEARGS);

  if (hidif->hid_parser->ReportDescSize > REPORT_DSC_SIZE) {
    ERROR("report descriptor size for " TRACEDEVICESTR
          " exceeds maximum size: %d > %d.\n", TRACEDEVICEARGS,
          hidif->hid_parser->ReportDescSize, REPORT_DSC_SIZE);
    return HID_RET_REPORT_DESC_LONG;
  }

  TRACE("retrieving report descriptor for "
      TRACEDEVICESTR "...", TRACEDEVICEARGS);
  int len = usb_control_msg(hidif->dev_handle,
      USB_ENDPOINT_IN+1,
      USB_REQ_GET_DESCRIPTOR,
      (USB_DT_REPORT << 8) + 0, 0,
      hidif->hid_parser->ReportDesc, hidif->hid_parser->ReportDescSize,
      USB_TIMEOUT);

  if (len < 0) {
    WARNING("failed to get report descriptor for "
        TRACEDEVICESTR "...", TRACEDEVICEARGS);
    return HID_RET_FAIL_GET_REPORT;
  }

  if (len < hidif->hid_parser->ReportDescSize) {
    WARNING("HID descriptor for " TRACEDEVICESTR " is too short; "
        "expected: %d bytes; got: %d bytes.\n", TRACEDEVICEARGS,
        hidif->hid_parser->ReportDescSize, len);
    return HID_RET_REPORT_DESC_SHORT;
  }

  NOTICE("successfully initialised report descriptor for "
      TRACEDEVICESTR ".", TRACEDEVICEARGS);

  return HID_RET_SUCCESS;
}

hid_return hid_prepare_interface(HIDInterface* const hidif)
{
  hid_return ret = hid_init_parser(hidif);
  if (ret != HID_RET_SUCCESS) {
    hid_close(hidif);
    return ret;
  }

  ret = hid_prepare_hid_descriptor(hidif);
  if (ret != HID_RET_SUCCESS) {
    hid_close(hidif);
    return ret;
  }

  ret = hid_prepare_report_descriptor(hidif);
  if (ret != HID_RET_SUCCESS) {
    hid_close(hidif);
    return ret;
  }

  ret = hid_prepare_parser(hidif);
  if (ret != HID_RET_SUCCESS) {
    hid_close(hidif);
    return ret;
  }

  return HID_RET_SUCCESS;
}
