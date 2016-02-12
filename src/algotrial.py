import sys
from scipy.io import wavfile
#from matplotlib.pyplot
from matplotlib import pyplot as plt
import numpy as np





testfile = '../wavs/static2mono.wav'

fs,data = wavfile.read(testfile)

#np.array(data[1],dtype = float)

dat_length = data.size
#dat_sample = data[6324:6424]
#dat_sample2 = data[6425:6525]
dat_sample3 = data[6324:7323]
dat_sample4 = data[7324:8323]
#dat_diff = dat_sample - dat_sample2
dat_diff2 = dat_sample3 - dat_sample4
#dat_Lchannel = data[:,0]



print('length:',dat_length);
print('rate:',fs);
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

h=plt.figure(3)
plt.title('Static mono wav..Dat_sample3')
plt.plot(dat_diff2)
h.show()




i=plt.figure(4)
plt.title('Static mono wav..Dat_diff2')
plt.plot(dat_diff2)
i.show()


#print ('first :',data[6324:6424])
#print ('first :',dat_Lchannel[6324:6424]);
#print ('Diff:', dat_diff);
