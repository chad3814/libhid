#include <hid.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

bool match_serial_number(struct usb_dev_handle* usbdev, void* custom, unsigned int len)
{
  bool ret;
  char* buffer = (char*)malloc(len);
  usb_get_string_simple(usbdev, usb_device(usbdev)->descriptor.iSerialNumber,
      buffer, len);
  ret = strncmp(buffer, (char*)custom, len) == 0;
  free(buffer);
  return ret;
}

int tripp_lite_send(HIDInterface* hid, const char *msg)
{
  int const path_out[] = {0xffa00001,0xffa00002,0xffa10005 };
  int i, ret, csum = 0;
  char buffer[8];
  
  for(i=1;i<8;i++) buffer[i] = 0;

  buffer[0] = ':';

  for(i=0;msg[i];i++) {
    buffer[i+1] = msg[i];
    csum += msg[i];
  }
  buffer[i+1] = 255-(csum & 0xff);
  buffer[i+2] = 0x0d;
  if((i+2) > 8) {
    fprintf(stderr, "tripp_lite_send: message too long (%d)\n", i+1);
    return -1;
  }

  for(i=0;i<8;i++) {
    printf("%02x ", buffer[i]);
  }
  printf("-> ");

  ret = hid_set_output_report(hid, path_out, 3, buffer, 8);
  if (ret != HID_RET_SUCCESS) {
    fprintf(stderr, "hid_set_output_report failed with return code %d\n", ret);
  }

  return ret;
}
  
int tripp_lite_receive(HIDInterface* hid, char *msg)
{
  int ret = hid_interrupt_read(hid, 0x81, msg, 8, 100);

  if (ret != HID_RET_SUCCESS) {
    fprintf(stderr, "hid_interrupt_read failed with return code %d\n", ret);
    return ret;
  }
  return 0;
}


int main(void)
{
  HIDInterface* hid;
  hid_return ret;

  /* How to use a custom matcher function:
   * 
   * The third member of the HIDInterfaceMatcher is a function pointer, and
   * the forth member will be passed to it on invocation, together with the
   * USB device handle. The fifth member holds the length of the buffer
   * passed. See above. This can be used to do custom selection e.g. if you
   * have multiple identical devices which differ in the serial number.
   *
   *   char const* const serial = "01518";
   *   HIDInterfaceMatcher matcher = {
   *     0x06c2,                      // vendor ID
   *     0x0038,                      // product ID
   *     match_serial_number,         // custom matcher function pointer
   *     (void*)serial,               // custom matching data
   *     strlen(serial)+1             // length of custom data
   *   };
   *
   * If you do not want to use this, set the third member to NULL.
   * Then the match will only be on vendor and product ID.
   */

  // HIDInterfaceMatcher matcher = { 0x0463, 0xffff, NULL, NULL, 0 };
  HIDInterfaceMatcher matcher = { 0x09ae, 0x01, NULL, NULL, 0 };

  /* see include/debug.h for possible values */
  hid_set_debug(HID_DEBUG_ALL);
  hid_set_debug_stream(stderr);
  /* passed directly to libusb */
  hid_set_usb_debug(5);
  
  ret = hid_init();
  if (ret != HID_RET_SUCCESS) {
    fprintf(stderr, "hid_init failed with return code %d\n", ret);
    return 1;
  }

  hid = hid_new_HIDInterface();
  if (hid == 0) {
    fprintf(stderr, "hid_new_HIDInterface() failed, out of memory?\n");
    return 1;
  }

  /* How to detach a device from the kernel HID driver:
   * 
   * The hid.o or usbhid.ko kernel modules claim a HID device on insertion,
   * usually. To be able to use it with libhid, you need to blacklist the
   * device (which requires a kernel recompilation), or simply tell libhid to
   * detach it for you. hid_open just opens the device, hid_force_open will
   * try n times to detach the device before failing.
   * In the following, n == 3.
   *
   * To open the HID, you need permission to the file in the /proc usbfs
   * (which must be mounted -- most distros do that by default):
   *   mount -t usbfs none /proc/bus/usb
   * You can use hotplug to automatically give permissions to the device on
   * connection. Please see
   *   http://cvs.ailab.ch/cgi-bin/viewcvs.cgi/external/libphidgets/hotplug/
   * for an example. Try NOT to work as root!
   */

  ret = hid_force_open(hid, 0, &matcher, 3);
  if (ret != HID_RET_SUCCESS) {
    fprintf(stderr, "hid_force_open failed with return code %d\n", ret);
    return 1;
  }

  ret = hid_write_identification(stdout, hid);
  if (ret != HID_RET_SUCCESS) {
    fprintf(stderr, "hid_write_identification failed with return code %d\n", ret);
    return 1;
  }

#if 0
  ret = hid_dump_tree(stdout, hid);
  if (ret != HID_RET_SUCCESS) {
    fprintf(stderr, "hid_dump_tree failed with return code %d\n", ret);
    return 1;
  }
#endif

  /* How to write to and read from a device:
   *
   * Writing to a device requires the HID usage path, a buffer, and the length
   * of the latter. You must also know the exact length of a packet expected
   * by the device, and the protocol to speak over HID.
   *
   * libhid uses the MGE hidparser, which parses the HID usage tree and places
   * the available usages at its leafs (leaves?). The path information can be
   * read from the `lsusb -vvv` output, or by inspecting the output of
   * hid_dump_tree. In the output, 0x80 denotes an input endoint (sent by the
   * device), and 0x90 an output endpoint (sent to the device). These are then
   * used to communicate with the device.
   *
   * In the example of the Phidgets QuadServoController (www.phidgets.com, and
   * libphidgets.alioth.debian.org), the following two paths identify the
   * input and output descriptors respectively.
   *
   *   unsigned char const PATHLEN = 3;
   *   int const PATH_IN[PATHLEN] = { 0xffa00001, 0xffa00002, 0xffa10003 };
   *   int const PATH_OUT[PATHLEN] = { 0xffa00001, 0xffa00002, 0xffa10004 };
   *
   * This is derived from the output of `lsusb -d 0x06c2:0x0038 -vvv` as
   * follows. You need to run `libhid_detach_device 06c2:0038` before lsusb
   * will output this info:
   *
   *   Bus 001 Device 028: ID 06c2:0038 GLAB Chester 4-Motor PhidgetServo v3.0
   *   Device Descriptor:
   *     [...]
   *     Configuration Descriptor:
   *       [...]
   *       Interface Descriptor:
   *         [...]
   *         iInterface
   *           HID Device Descriptor:
   *           [...]
   *              Report Descriptor:
   *              [...]
   *                Item(Global): Usage Page, data= [ 0xa0 0xff ] 65440
   *                [...]
   *                Item(Local ): Usage, data= [ 0x01 ] 1
   *                [...]
   *                
   *                Item(Local ): Usage, data= [ 0x02 ] 2
   *                [...]
   *                
   *                Item(Global): Usage Page, data= [ 0xa1 0xff ] 65441
   *                [...]
   *                
   *                Item(Local ): Usage, data= [ 0x03 ] 3
   *                [...]
   *                Item(Main  ): Input, data= [ 0x02 ] 2
   *                [...]
   *                
   *                Item(Local ): Usage, data= [ 0x04 ] 4
   *                [...]
   *                Item(Main  ): Output, data= [ 0x02 ] 2
   *                [...]
   *
   * So working backwards,
   *   "Item(Main) Output" is usage 4 of usage page 65441,
   *   which is rooted at "Item(Local) ... 2" of usage page 65440,
   *   which is rooted at "Item(Local) ... 1" of usage page 65440
   *
   * A path component is 32 bits, the high 16 bits identify the usage page,
   * and the low 16 bits the item number. Thus (now working forwards):
   *
   *   65440 << 16 + 1      -> 0xffa00001
   *   65440 << 16 + 2      -> 0xffa00002
   *   65441 << 16 + 4      -> 0xffa10004
   *
   * which gives the path the the output usage of the HID. The input usage may
   * be found analogously.
   * 
   * Now, to send 6 bytes:
   *
   *   unsigned char const SEND_PACKET_LEN = 6;
   *   // fill an example packet:
   *   char const PACKET[SEND_PACKET_LEN] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5 };
   *
   *   ret = hid_set_output_report(hid, PATH_IN, PATHLEN, PACKET, SEND_PACKET_LEN);
   *   if (ret != HID_RET_SUCCESS) {
   *     fprintf(stderr, "hid_set_output_report failed with return code %d\n", ret);
   *   }
   *
   * And reading works similarly:
   *   char packet[RECV_PACKET_LEN];
   *   ret = hid_get_input_report(hid, PATH_OUT, PATHLEN, packet, RECV_PACKET_LEN);
   *   if (ret != HID_RET_SUCCESS) {
   *     fprintf(stderr, "hid_get_input_report failed with return code %d\n", ret);
   *   }
   *   // now use the RECV_PACKET_LEN bytes starting at *packet.
   */

   char buffer_in[8], buffer_out[8] = "L";

   hid_set_debug(HID_DEBUG_ALL & ~(HID_DEBUG_TRACES | HID_DEBUG_NOTICES));

   int cmd, i, count, done;
   for(cmd='A';cmd<='Z';cmd++) {
     buffer_out[0] = cmd;
     printf("'%c': ", cmd);
     tripp_lite_send(hid, buffer_out);
     fflush(stdout);
     // sleep(1);
     usleep(1000*250);
     done = count = 0;

     // Each tripp_lite_receive call should take ~10 ms (because of the way
     // Interrupt In requests are queued)
     while(!done && count++ < 100*5) {
       tripp_lite_receive(hid, buffer_in);

       done = (buffer_in[0] == buffer_out[0]);
     }

     // printf(" (%d cycles) ", count);

     if(done) {
       for (i = 0; i < 8; i++) {
	 printf("%02x ", buffer_in[i]);
       }
       printf("\"");
       for(i=0; i<8; i++) {
	 printf("%c", isprint(buffer_in[i]) ? buffer_in[i] : '.');
       }
       printf("\"\n");
     } else {
       printf("?\n");
     }
     fflush(stdout);
   }

   ret = hid_close(hid);
   if (ret != HID_RET_SUCCESS) {
     fprintf(stderr, "hid_close failed with return code %d\n", ret);
     return 1;
   }

   hid_delete_HIDInterface(&hid);

   ret = hid_cleanup();
   if (ret != HID_RET_SUCCESS) {
     fprintf(stderr, "hid_cleanup failed with return code %d\n", ret);
     return 1;
   }

   return 0;
}

/* COPYRIGHT --
 *
 * This file is part of libhid, a user-space HID access library.
 * libhid is (c) 2003-2005
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
// vim:sw=2
