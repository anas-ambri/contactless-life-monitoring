import usb.core
import time

def print_data(file):
    dev = usb.core.find()
    ret = dev.read(0x81, 576)
    start = time.time()
    while time.time() - start < 10:
        ret = dev.read(0x81, 576)
        print >> file, "\n".join(str(x) for x in list(ret) if x != 0)
        print >> file, "\n"
                

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
	file = open('output.txt', 'a')
	print_data(file)
	file.close()
	
	
