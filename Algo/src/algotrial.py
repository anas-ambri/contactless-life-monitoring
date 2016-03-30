import sys
from scipy.io import wavfile
#from matplotlib.pyplot
from matplotlib import pyplot as plt
import numpy as np
from collections import deque
import math
import scipy.fftpack
import time
from scipy import signal
from scipy.signal import find_peaks_cwt


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
print len(data)
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
    
    #print(len(dat_sample))
    # check data
    #print('trial: ',dat_sample)

    # convert values to phase
    hold = []
    for x in range (0,len(dat_sample)):
        # vital radio eq
        #value = float((2* math.pi* (dat_sample[x]/1000.0)))/wav_len
        # rounding values -- just for checking
        #val = float("{0:.6f}".format(value))
        #hold.append(val)
        hold.append(dat_sample[x])
    #samp_queue.append(dat_sample)
    hold2 =scipy.fftpack.fft(hold,len(hold))
    samp_queue.append(hold)
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

# vital radio modified data- stage 1
#f=plt.figure(5)
#plt.title('modified samp wav..')
#plt.plot(samp_queue[2])
#f.show()

###############################################
# Mutex lock diff_queue here
# get 5 diff matrices
diff_queue = deque([])

for x in range (0,len(samp_queue)-1):
    w1 = np.array(samp_queue[x])
    w2 = np.array(samp_queue[x+1])
    w3 = w1-w2
    diff_queue.append(w3)
    #print 'Diff[',x,']: ', diff_queue[x]

# vital radio modified data- stage 1- diff matrix
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

#  x axis numbers 
dt = 12.0/samp_slide
t= np.arange(0.0, 12.0,dt)


avg = [x/len(diff_queue) for x in avg_sample]

# show me diff avg plot
#l=plt.figure(7)
#plt.title('avg wav..')
#plt.plot(avg)
#l.show()

#x_avg = [(60* x)/ 2* math.pi for x in avg]

###########################################
#end diff_queue lock
# start test mutex lock

#########################################
# Testing Phase
# get fft of diff avg
# find if the peaks differ by 30%
# if yes dump data

#########################################
# test pendings starts
#print "x_avg length: ", len(x_avg)
#plt.plot(x_avg)
# test pendings ends

#########################################
fft_avg = scipy.fftpack.ifft(avg,len(avg))
freq = np.fft.fftfreq(len(avg), dt)
print "Avg length: ", len(fft_avg)
print "freq length: ",len(freq)


# show me fft_avg
#j=plt.figure(7)
#plt.title('fft_avg wav..')
#plt.plot(t,fft_avg)
#j.show()

test =abs(fft_avg[:len(fft_avg)/2])

freq2 = freq[:len(fft_avg)/2]
freq3 = [((x*60)/(2*math.pi)) for  x in freq2]


# show me test
m=plt.figure(8)
plt.title('test..')
plt.plot(freq3,test)
m.show()

# automated find of first max and preceeding max

#peak_value0 = np.amax(test)
#peak_index0 = np.argmax(test)

#if (peak_index0 == 0):
#   peak_value0 = np.amax(test[1:])
#   peak_index0 = np.argmax(test[1:])
peak_value0 = test[1]
peak_index0 =1
allow = True

if (test[0]<test[1]): #graph started on a low,now increasing
    peak_value0 = np.amax(test[1:])
    peak_index0 = np.argmax(test[1:])
    
else:   # graph decreasing
    # find low point first
    loc_min=1
    loc_ind= 1
    for x in range(1, len(test)):
        if (loc_min > test [x]):
            loc_min = test[x]
            loc_ind = x
        else:
            break;
    peak_value0 = np.amax(test[loc_ind+1:])
    peak_index0 = loc_ind+1+np.argmax(test[loc_ind+1:])# correct offset
        
print "Peak index" , peak_index0

peak_value1 = np.amax(test[peak_index0+1:])
peak_index1 = peak_index0+1+np.argmax(test[peak_index0+1:])# correct offset

#check if index1 at proper index
#print "test[",peak_index1 ,"]", test[peak_index1]
print "1st max: ", peak_value0
#print "1st max index: ", peak_index0
#print "2nd max: ", peak_value1
#print "2nd max index: ", peak_index1

margin = (peak_value0 - peak_value1)/peak_value0
#print "margin:" , margin
if (margin <0.3):
    #test failed
    # delete test 
    print "Test failed"

else:
    # pass test on to next function
    print "Test passed"

    
    final = scipy.fftpack.ifft(test,len(test))
    phase = scipy.angle(final)

    #bpm = (abs(phase[peak_index0])* 60)/(2* math.pi)
    #bpm = ((peak_value0)* 60)/(2* math.pi)
    bpm = peak_index0
    print "Bpm: ", bpm 
    # show me phase
    #n=plt.figure(8)
    #plt.title('phase...')
    #plt.plot(phase)
    #n.show()

    





#unlock test mutex lock 
########################################
#Extraction phase
# Use peaks from test
#
########################################


#fft_avg = scipy.fftpack.ifft(avg,len(avg))
#phase_avg = scipy.angle(fft_avg)
#x_avg = [(60* x)/ 2* math.pi for x in fft_avg]

#x_avg = [(60* x)/ 2* math.pi for x in phase_avg]

#freq = np.fft.fftfreq(len(fft_avg), dt)




# show me avg_fft 
#k=plt.figure(8)
#plt.title('avg fft wav..')
#plt.plot(t,avg)


#plt.plot(freq[:len(freq/2)],abs(x_avg[:len(x_avg)/2]))
#plt.plot(x_avg[:len(x_avg)/2],abs(fft_avg[:len(fft_avg)/2]))
#plt.xlabel('f (Hz)')
#plt.ylabel('|Avg_fft|')
#k.show()
#print fft_avg

# end of prog
end_time = time.time()

print "Exec:", end_time-start_time





# fig prints 
#f=plt.figure(1)
#plt.title('Static wav..')
#plt.plot(dat_diff)
#f.show()


#g=plt.figure(2)
#plt.title('Static mono wav..Dat_sample')
#plt.plot(dat_sample)
#g.show()

#h=plt.figure(3)
#plt.title('Static mono wav..Dat_sample3')
#plt.plot(dat_diff2)
#h.show()




#i=plt.figure(4)
#plt.title('Static mono wav..Dat_diff2')
#plt.plot(dat_diff2)
#i.show()



