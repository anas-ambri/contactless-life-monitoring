import sys
from scipy.io import wavfile
#from matplotlib.pyplot
from matplotlib import pyplot as plt
import numpy as np
from collections import deque
import math





testfile = '../wavs/static2monoshort.wav'

fs,data = wavfile.read(testfile)

#np.array(data[1],dtype = float)

dat_length = data.size     #
#dat_sample = data[6324:6424]
#dat_sample2 = data[6425:6525]
#dat_sample3 = data[6324:7323]
#dat_sample4 = data[7324:8323]
#dat_diff = dat_sample - dat_sample2
#dat_diff2 = dat_sample3 - dat_sample4
#dat_Lchannel = data[:,0]



print('length:',dat_length);
print('rate:',fs);

# slide variables
samp_slide = 1000
samp_min =6324
count = 0
#samp_queue = []
samp_queue = deque([])

# read 6 sample batches
# convert to phase 

wav_len =300000000/float(fs)# fs corresponds to signal frq

while len(samp_queue)<3:
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
        val = float("{0:.4f}".format(value))
        hold.append(val)
    #samp_queue.append(dat_sample)
    samp_queue.append(hold)
    print 'queue size:',len(samp_queue)
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

# TODO -- fix "not responding" matplotlib
#plt.ion()
#plt.draw()
#plt.show()
#plt.ioff()
    
    
# average out diffs

avg_sample = np.array(diff_queue[0])

for x in range(1, len(diff_queue)):
    d1 = np.array(diff_queue[x])
    avg_sample = avg_sample + d1


avg = [x/len(diff_queue) for x in avg_sample]
    
# vital radio modified data- stage 1- diff matrix avg
#f=plt.figure(7)
#plt.title('avg wav..')
#plt.plot(avg)
#f.show()




#count =0
#while (count<dat_length):
    #print ('#',count, '   :', data[count])
    #count+=1
    
#print (count);

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



#print ('first :',data[6324:6424])
#print ('first :',dat_Lchannel[6324:6424]);
#print ('Diff:', dat_diff);
