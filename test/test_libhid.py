#!/usr/bin/env python

# This script serves as a simple demonstration of the Python bindings.

import hid
import sys

def main():
    intf_num = 0
    hid.hid_set_debug_stream(sys.stderr)
    hid.hid_set_debug(hid.HID_DEBUG_TRACES)

    ret = hid.hid_init()

    if ret:
        print "Error initializing libhid: error code %d" % (ret)

    match = hid.HIDInterfaceMatcher()
    # TODO: adapt for your device
    match.vendor_id = 0x51d
    match.product_id = 2

    hid_int = hid.HIDInterface()

    if hid_int == 0:
        print "Error creating HIDInterface"

    ret = hid.hid_force_open(hid_int, intf_num, match, 2)

    if ret:
        print "Error opening interface %d: error code %d" % (intf_num, ret)

    hid.hid_write_identification(sys.stdout, hid_int)

if __name__ == '__main__':
    main()

# COPYRIGHT --
#
# This file is part of libhid, a user-space HID access library.
# libhid is (c) 2003-2004
#   Martin F. Krafft <libhid@pobox.madduck.net>
#   Charles Lepple <clepple@ghz.cc>
#   Arnaud Quette <arnaud.quette@free.fr> && <arnaud.quette@mgeups.com>
# and distributed under the terms of the GNU General Public License.
# See the file ./COPYING in the source distribution for more information.
#
# THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES
# OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
