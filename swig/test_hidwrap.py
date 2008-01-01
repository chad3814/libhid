#!/usr/bin/env python

def main():
    import sys, os

    # allow it to run right out of the build dir:
    libsdir = os.getcwd() + '/.libs'
    if os.path.isdir(libsdir) and os.path.isfile(libsdir + '/_hid.so'):
        sys.path.insert(0, libsdir)

    import hidwrap

    hidwrap.set_debug(hidwrap.HID_DEBUG_ALL)
    hidwrap.set_debug_stream(sys.stderr)
    hidwrap.set_usb_debug(0)

    iface = hidwrap.Interface(vendor_id=0x51d, product_id=0x0002)
    iface.write_identification(sys.stdout)
    iface.dump_tree(sys.stdout)

if __name__ == '__main__':
    main()
