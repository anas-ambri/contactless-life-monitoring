import usb.core
import time

dev = usb.core.find()
dev.read(0x81, 576)

def get_next():
    global dev
    ret = dev.read(0x81, 576)
    return ret
