/*!@file
 *@brief Initialization routines for libhid structures.
 */
#define HID_INTERNAL

#include <hid.h>
#include <hid_helpers.h>

#include <debug.h>
#include <assert.h>

/*!@brief Programmers can check this with hid_is_initialized().
 */
static bool initialised = false;

/*!@brief Initialize libhid: scan for USB busses and devices using libusb.
 *
 * Call this routine before making any other libhid calls.
 *
 * @return HID_RET_SUCCESS if everything was properly initialized.
 */
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

/*!@brief Complement to hid_init(): cleans up after libhid.
 */
hid_return hid_cleanup()
{
  ASSERT(hid_is_initialised());

  initialised = false;
  NOTICE("successfully deinitialised HID library.");
  return HID_RET_SUCCESS;
}

/*!@brief Check to see that hid_init() has been called.
 * @return Non-zero if libhid has been initialized.
 */
bool hid_is_initialised()
{
  return initialised;
}

HIDInterface hid_new_HIDInterface()
{
  TRACE("creating a new HIDInterface instance...");
  HIDInterface ret;
  hid_reset_HIDInterface(&ret);
  return ret;
}

void hid_reset_HIDInterface(HIDInterface* const hidif)
{
  hidif->dev_handle = NULL;
  hidif->device = NULL;
  hidif->interface = -1;
  hidif->hid_data = NULL;
  hidif->hid_parser = NULL;
}
