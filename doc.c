/*
 * This file is a collection of general libhid documentation which
 * does not fit in any particular source file.
 *
 * To regenerate the project documentation, grab a recent version of
 * Doxygen (http://www.doxygen.org/) and type 'doxygen' in this
 * directory. The configuration file is 'Doxyfile'.
 *
 * The rest of the comments in this file are intended to be parsed by
 * Doxygen. Read the Doxygen manual for more information about the
 * structure of these comments.
 */
/*!
  @mainpage libhid - a portable HID API

  @section intro Introduction

  libhid is a <A HREF="http://www.gnu.org/licenses/gpl.txt">GPL</A> project that aims 
  to provide a layer of abstraction for accessing
  <A HREF="http://www.usb.org/developers/hidpage">HID-class</A> devices over USB or 
  serial lines (similar to what <A HREF="http://libusb.sourceforge.net">libusb</A> provides
  for the base USB protocol).

  @section compat Compatibility

  libhid will initially provide a "Generic HID Access", using the 
 <A HREF="http://www.mgeups.com/opensource/projects/hidparser.htm">MGE HID parser</A>,
 for all platforms supported by libusb, namely:
  	- GNU/Linux, FreeBSD, NetBSD, OpenBSD, Darwin and MacOS X

  libhid may eventually provide "Native HID Access" for:
  
  - GNU/Linux using kernel v2.5 and above with the new hiddev driver,
  - Mac OS X using the <A HREF="http://developer.apple.com/techpubs/macosx/Darwin/IOKit/DeviceInterfaces/HID/HID/HIDChapter/index.html">HID Manager</A>.

  Later versions will provide "Native HID Access" for:

  - GNU/Linux kernel v2.4 with the old hiddev driver,
  - Win32 (Windows 98 SE and later).

  @section lists Mailing List

  <p>Development of libhid is currently coordinated with mailing lists and CVS
  (see further down). The following three mailing lists exist (click on the list
  name to go to the list information page):</p>

  <ul>

  <li><a href="http://lists.ailab.ch/listinfo/libhid-announce"
  target="_blank"><strong>libhid-announce</strong></a> -- a moderated list with
  very low volume for announcements. Everyone should subscribe here.</li>

  <li><a href="http://lists.ailab.ch/listinfo/libhid-discuss"
  target="_blank"><strong>libhid-discuss</strong></a> -- a subscriber-only list
  for general discussion. Everyone is cordially invited to join and
  participate.</li>

  <li><a href="http://lists.ailab.ch/listinfo/libhid-cvs"
  target="_blank"><strong>libhid-cvs</strong></a> -- a mailing list receiving
  CVS commit mails. If you want to closely track libhid development, you
  probably want to subscribe to this list.</li>

  </ul>
  

  @section design Design & architecture

  <table cellpadding="6" cellspacing="2" border="1" style="text-align: center; width: 75%;" align="center">
    <tr align="center">
      <td colspan="4">User Level API<br>
      </td>
    </tr>
    <tr>
      <td colspan="2">Native HID Access<br>
      </td>
      <td colspan="2">
      Generic HID Access<br> (using MGE HID parser)
      </td>
    </tr>
    <tr>
      <td width="25%">GNU/Linux hiddev<br> (2.4 &amp; 2.6)
      </td>
      <td width="25%">Mac OS X<br> HID API
      </td>
      <td width="25%">libusb<br> GNU/Linux, *BSD, Mac OS X/Darwin
      </td>
      <td width="25%">MGE SHUT<br> (Serial HID UPS Transfer)
      </td>
    </tr>
  </table>

  @section comps Components

  libhid includes components from the following projects:

  - the <A HREF="http://www.mgeups.com/opensource/projects/hidparser.htm">MGE
    HID parser</A>

  @section api_overview API Overview

  libhid will provide function calls for:

  - Finding device by combining (and/or) several criterias:
	- Vendor ID, 
	- Product ID, 
	- Application Usage, 
	- index (if several devices exist with same previous criterias values)

  - Managing HID devices, much as is done by SNMP MIB parsing libraries, for:
    - Browsing <A HREF="../../images/MGE_Evolution_HID_Tree.jpg">hierarchy created by collections</A>
    - Getting full HID item informations (including value) by string path
    - Getting HID item value (integer format) by string path
      - e.g. "UPS.PowerSummary.RemainingCapacity" = 100 %
    - Getting HID item value (string format) by string path
    - Setting HID item value (integer format) by string path

  - Manage HID device's events

  @section authors Authors
  
  - Martin Krafft
  - Charles Lepple 
  - Arnaud Quette

*/

/*!@file doc.c
 * @brief miscellaneous documentation 
 */
