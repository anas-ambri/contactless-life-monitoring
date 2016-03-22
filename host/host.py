"""
import sys
import pywinusb.hid as hid

if __name__ == '__main__':
    if sys.version_info < (3,):
        import codecs
        output = codecs.getwriter('mbcs')(sys.stdout)
    else:
        # python3, you have to deal with encodings, try redirecting to any file
        output = sys.stdout
    try:
        hid.core.show_hids(target_vid = 0x1FC9, target_pid = 0x0081)
    except UnicodeEncodeError:
        print("\nError: Can't manage encodings on terminal, try to run the script on PyScripter or IDLE")
"""

from time import sleep
from msvcrt import kbhit

import pywinusb.hid as hid
device = None

def sample_handler(data):
    print("Raw data: {0}".format(data))

def raw_test():
    # simple test
    # browse devices...
    all_hids = hid.find_all_hid_devices()
    if all_hids:
        while True:
            print("Choose a device to monitor raw input reports:\n")
            print("0 => Exit")
            for index, device in enumerate(all_hids):
                device_name = unicode("{0.vendor_name} {0.product_name}" \
                        "(vID=0x{1:04x}, pID=0x{2:04x})"\
                        "".format(device, device.vendor_id, device.product_id))
                print("{0} => {1}".format(index+1, device_name))
            print("\n\tDevice ('0' to '%d', '0' to exit?) " \
                    "[press enter after number]:" % len(all_hids))
            index_option = raw_input()
            if index_option.isdigit() and int(index_option) <= len(all_hids):
                # invalid
                break;
        int_option = int(index_option)
        if int_option:
            global device
            device = all_hids[int_option-1]
            try:
                device.open()

                #set custom raw data handler
                device.set_raw_data_handler(sample_handler)
                
                print("\nWaiting for data...\nPress any (system keyboard) key to stop...")
                while not kbhit() and device.is_plugged():
                    #just keep the device opened to receive events
                    pass

                return
            finally:
                device.close()
    else:
        print("There's not any non system HID class device available")
#
if __name__ == '__main__':
    # first be kind with local encodings
    import sys
    if sys.version_info >= (3,):
        # as is, don't handle unicodes
        unicode = str
        raw_input = input
    else:
        # allow to show encoded strings
        import codecs
        sys.stdout = codecs.getwriter('mbcs')(sys.stdout)
    raw_test()
