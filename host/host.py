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
import threading

import pywinusb.hid as hid
device = None

current_data = []
last_data = current_data

def get_next():
    return last_data

def sample_handler(data):
    global current_data
    current_data.append(data[1])
    global device
    report = device.find_output_reports()[0]
    target_usage = hid.get_full_usage_id(0xff00, 0x01)
    i = int(report[target_usage].get_value())
    report[target_usage] = 1
    report.send()
	
def clear_data():
    #do your calculation here
    global last_data
    global current_data
    if len(current_data) > 0:
        print len(current_data)
    last_data = current_data
    current_data = []
    threading.Timer(0.1, clear_data).start() #execute this function every 2 milliseconds

def setup_device():
    # simple test
    # browse devices...
    all_devices = hid.HidDeviceFilter(vendor_id = 0x1FC9).get_devices()
    if all_devices:
        device = all_devices[0]
        print device
        global device
        try:
			device.open()

			#set custom raw data handler
			device.set_raw_data_handler(sample_handler)

			report = device.find_output_reports()[0]
			target_usage = hid.get_full_usage_id(0xff00, 0x01)
			report[target_usage] = 1
			report.send()
			
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

def init():
    clear_data()
    setup_device()

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
	init()
	
