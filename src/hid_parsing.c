#define HID_INTERNAL

#include <hid.h>
#include <hid_helpers.h>

#include <debug.h>
#include <assert.h>

static void hid_prepare_parse_path(HIDInterface* const hidif,
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

hid_return hid_init_parser(HIDInterface* const hidif)
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

hid_return hid_prepare_parser(HIDInterface* const hidif)
{
  ASSERT(hid_is_initialised());
  ASSERT(hid_is_opened(hidif));
  ASSERT(hidif->hid_parser);
  ASSERT(hidif->hid_data);
  
  TRACE("setting up the HID parser for interface %d of USB device on %s/%s...",
      hidif->interface, hidif->device->bus->dirname, hidif->device->filename);

  hid_reset_parser(hidif);
  
  /* TODO: the return value here should be used, no? */
  TRACE("parsing the HID tree of interface %d of USB device on %s/%s...",
      hidif->interface, hidif->device->bus->dirname, hidif->device->filename);
  HIDParse(hidif->hid_parser, hidif->hid_data);

  NOTICE("successfully set up the HID parser for interface %d of USB device on %s/%s.",
      hidif->interface, hidif->device->bus->dirname, hidif->device->filename);

  return HID_RET_SUCCESS;
}

void hid_reset_parser(HIDInterface* const hidif)
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

hid_return hid_find_object(HIDInterface* const hidif,
    int const path[], unsigned int const depth)
{
  hid_prepare_parse_path(hidif, path, depth);

  if (FindObject(hidif->hid_parser, hidif->hid_data) == 1) {
    NOTICE("found requested item.");
    return HID_RET_SUCCESS;
  }

  byte const ITEMSIZE = sizeof("0xdeadbeef");
  char* buffer = (char*)malloc(depth * ITEMSIZE);
  hid_format_path(buffer, depth * ITEMSIZE, path, depth); 
  WARNING("can't find requested item %s.\n", buffer);
  free(buffer);
  
  return HID_RET_NOT_FOUND;
}

hid_return hid_format_path(char* const buffer, unsigned int length,
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
