import sys
from scipy.io import wavfile
#from matplotlib.pyplot
from matplotlib import pyplot as plt
import numpy as np
from collections import deque
import math
import scipy.fftpack
import time


#mark time
start_time = time.time() 



#######################################
# Aquire test samples from wav
# 
#
#
#######################################

#testfile = '../wavs/static2monoshort.wav'
#fs,data = wavfile.read(testfile)
#dat_length = data.size     #
#dat_sample = data[6324:6424]
#print('length:',dat_length);
#print('rate:',fs);


#######################################
# Aquire test samples from file
# 
#
#
#######################################
#file = open('10_sec_heavy_breathing_non_exagerated.txt', "r")
file = open('testread.txt', "r")
strnumbers = file.read().split()
data= map(int, strnumbers)
#polyShape= map(int, strnumbers)
#print polyShape

# slide variables
samp_slide = 576
samp_min =0
count = 0
#samp_queue = []
samp_queue = deque([])



#########################################

# read 6 sample batches
# convert to phase
#fs corresponds to signal frq
#wav_len corresponds to wavelength

#########################################
fs = 5.8 *( 10e9)
wav_len =300000000/float(fs)

while len(samp_queue)<6:
    #global samp_slide,samp_min,samp_queue
    dat_sample = data[samp_min:samp_min+samp_slide]
    # get fft of sample
    hold =[]
    for x in range (0,len(dat_sample)):
        # vital radio eq
        #value = float((2* math.pi* dat_sample[x]))/wav_len
        # rounding values -- just for checking
        #val = float("{0:.6f}".format(value))
        #hold.append(val)
        hold.append(dat_sample[x])
    #samp_queue.append(dat_sample)
    hold2 =scipy.fftpack.fft(hold,len(hold))
    hold3 = scipy.angle(hold2)
    samp_queue.append(hold2)
    # print samples
    #print 'queue size:',len(samp_queue)
    #print 'Slot #:',count
    #print samp_queue[count]
    count+=1
    samp_min+=samp_slide
    
    # reset hold variable
    #del hold[:]
    #print 'Here lies hold ',hold

    #TODO handle removing 5 batches to make way for next 5--
#hold4 = abs()
dtx = 12.0/576
tempx= np.arange(0.0, 12.0,dtx)
# show me phase of sample
#f=plt.figure(5)
#plt.title('modified samp wav..')
#plt.plot(tempx,samp_queue[2])
#plt.plot(abs(hold2))
#
#f.show()
# get 5 diff matrices
diff_queue = deque([])

for x in range (0,len(samp_queue)-1):
    w1 = np.array(samp_queue[x])
    w2 = np.array(samp_queue[x+1])
    w3 = w1-w2
    diff_queue.append(w3)
    #print 'Diff[',x,']: ', diff_queue[x]

# vital radio modified data- stage 1-
# show me cleaned phase
#f=plt.figure(6)
#plt.title('diff wav..')
#plt.plot(diff_queue[0])
#f.show()

# unlock diff_lock mutex here
##############################################
    
##############################################
#end of part 1
# pass diff_queue  to 2nd func
# mutex  lock diff queue
##############################################
# average out diffs
##############################################
# start mutex lock here
#
##############################################
avg_sample = np.array(diff_queue[0])

for x in range(1, len(diff_queue)):
    d1 = np.array(diff_queue[x])
    avg_sample = avg_sample + d1




avg = [x/len(diff_queue) for x in avg_sample]
#  x axis numbers 
dt = 12.0/len(avg) 
t= np.arange(0.0, 12.0,dt)


# show me diff avg plot
l=plt.figure(7)
plt.title('avg wav..')
plt.plot(t,avg)
l.show()


