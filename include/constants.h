#ifndef __INCLUDED_CONSTANTS_H__
#define __INCLUDED_CONSTANTS_H__

#ifndef HID_INTERNAL
#  error "this file is only supposed to be used from within libhid."
#endif // HID_INTERNAL

#define USB_TIMEOUT 5000

#define HID_REPORT_GET 0x01
#define HID_REPORT_SET 0x09
  
#define HID_RT_INPUT 0x01
#define HID_RT_OUTPUT 0x02
#define HID_RT_FEATURE 0x03

#endif // __INCLUDED_CONSTANTS_H__

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
