%module hid
%{
%}

void hid_set_debug(HIDDebugLevel const level);
void hid_set_debug_stream(FILE* const outstream);
void hid_set_usb_debug(int const level);

HIDInterface* hid_new_HIDInterface();

void hid_delete_HIDInterface(HIDInterface** const hidif);

void hid_reset_HIDInterface(HIDInterface* const hidif);

hid_return hid_init();

hid_return hid_cleanup();

bool hid_is_initialised();

hid_return hid_open(HIDInterface* const hidif, int const interface,
    HIDInterfaceMatcher const* const matcher);
hid_return hid_force_open(HIDInterface* const hidif, int const interface,
    HIDInterfaceMatcher const* const matcher, unsigned short retries);

hid_return hid_close(HIDInterface* const hidif);

bool hid_is_opened(HIDInterface const* const hidif);

hid_return hid_get_input_report(HIDInterface* const hidif, int const path[],
    unsigned int const depth, char* const buffer, unsigned int const size);

hid_return hid_set_output_report(HIDInterface* const hidif, int const path[],
    unsigned int const depth, char const* const buffer, unsigned int const size);

hid_return hid_write_identification(FILE* const out,
    HIDInterface const* const hidif);

hid_return hid_dump_tree(FILE* const out, HIDInterface* const hidif);
