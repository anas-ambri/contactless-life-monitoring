import usb.core

def list_devices():
    dev = usb.core.find()
    while 1:
        dev.read(0x81, 4)
                

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
	list_devices()
	
