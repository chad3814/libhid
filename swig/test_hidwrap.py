def main():
    import hidwrap
    import sys

    hidwrap.set_debug(hidwrap.HID_DEBUG_ALL)
    hidwrap.set_debug_stream(sys.stderr)
    hidwrap.set_usb_debug(0)

    iface = hidwrap.Interface(vendor_id=0x3351, product_id=0x3715)
    iface.write_identification(sys.stdout)
    iface.dump_tree(sys.stdout)

if __name__ == '__main__':
    main()
