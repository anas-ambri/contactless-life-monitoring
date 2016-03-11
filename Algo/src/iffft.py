import sys
from scipy.io import wavfile
#from matplotlib.pyplot
from matplotlib import pyplot as plt
import numpy as np
from collections import deque
import math
import scipy.fftpack

import os
os.system('cls')


#######################################
# Aquire test samples from wav
# 
#
#
#######################################

testfile = '../wavs/static2monoshort.wav'

fs,data = wavfile.read(testfile)



dat_length = data.size     #
#dat_sample = data[6324:6424]
print('length:',dat_length);
print('rate:',fs);

# slide variables
samp_slide = 1000
samp_min =6324
count = 0
#samp_queue = []
samp_queue = deque([])

#########################################

# read 6 sample batches
# convert to phase
#fs corresponds to signal frq
#wav_len corresponds to wavelength

#########################################

wav_len =300000000/float(fs)


while len(samp_queue)<6:
    #global samp_slide,samp_min,samp_queue
    dat_sample = data[samp_min:samp_min+samp_slide]
    
    #print(len(dat_sample))
    # check data
    #print('trial: ',dat_sample)

    # convert values to phase
    hold = []
    for x in range (0,len(dat_sample)):
        # vital radio eq
        value = float((2* math.pi* dat_sample[x]))/wav_len
        # rounding values -- just for checking
        val = float("{0:.6f}".format(value))
        hold.append(val)
    #samp_queue.append(dat_sample)
    hold2 =scipy.fftpack.fft(hold,len(hold))
    samp_queue.append(hold)
    # print samples
    #print 'queue size:',len(samp_queue)
    print 'Slot #:',count
    print samp_queue[count]
    count+=1
    samp_min+=samp_slide
    
    # reset hold variable
    #del hold[:]
    #print 'Here lies hold ',hold

    #TODO handle removing 5 batches to make way for next 5-- 

# vital radio modified data- stage 1
#f=plt.figure(5)
#plt.title('modified samp wav..')
#plt.plot(samp_queue[2])
#f.show()
# get 5 diff matrices
diff_queue = deque([])

for x in range (0,len(samp_queue)-1):
    w1 = np.array(samp_queue[x])
    w2 = np.array(samp_queue[x+1])
    w3 = w1-w2
    diff_queue.append(w3)
    print 'Diff[',x,']: ', diff_queue[x]

# vital radio modified data- stage 1- diff matrix
f=plt.figure(6)
plt.title('diff wav..')
plt.plot(diff_queue[0])
f.show()
