#include <hid_swig_helpers.h>

/*extern void* null;*/

HIDInterfaceMatcher hid_new_HIDInterfaceMatcher(unsigned short vendor_id,
    unsigned short product_id, matcher_fn_t matcher_fn, void* custom_data,
    unsigned int custom_data_length)
{
  HIDInterfaceMatcher ret = { vendor_id, product_id,
    matcher_fn, custom_data, custom_data_length};
  return ret;
}

/* COPYRIGHT --
 *
 * This file is part of libhid, a user-space HID access library.
 * libhid is (c) 2003-2004
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
