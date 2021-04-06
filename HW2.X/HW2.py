import matplotlib.pyplot as plt
import numpy as np
import csv
import statistics
import numpy as np

t = [] # column 0
data1 = [] # column 1
data2 = [] # column 2

with open('sigD.csv') as f:
    # open the csv file
    reader = csv.reader(f)
    for row in reader:
        # read the rows 1 one by one
        t.append(float(row[0])) # leftmost column
        data1.append(float(row[1])) # second column
        
dt = t[1]
s = data1
Ts = dt
Fs = 1/Ts

# Moving Average
# X = 50
# ss = []
# j = range(X,len(s))
# for i in j:
#     ss.append(statistics.mean(s[i-X:i]))
# jj = np.linspace(dt*X,t[-1], num=len(ss))

# IIR Average
# X = 50
# A = .8
# B = 1-A
# sss = []
# newaverage = A*s[0]+B*s[1]
# sss.append(newaverage)
# for i in range(1,len(s)):
#     newaverage = A*sss[i-1]+B*s[i]
#     sss.append(newaverage)
# jjj = np.linspace(dt*X,t[-1], num=len(sss))

#FIR Average
X = 15
ssss = []
# copied from website
h = [
    0.016475920282948690,
    0.017054204224753452,
    0.017617052667658336,
    0.018163183173519573,
    0.018691346589453775,
    0.019200330501115824,
    0.019688962591300284,
    0.020156113893671546,
    0.020600701931734123,
    0.021021693733492330,
    0.021418108712617286,
    0.021789021407337010,
    0.022133564068691849,
    0.022450929090250545,
    0.022740371271860769,
    0.023001209910510589,
    0.023232830711902033,
    0.023434687516883282,
    0.023606303837450455,
    0.023747274197611152,
    0.023857265274998492,
    0.023936016839734148,
    0.023983342487660070,
    0.023999130165688807,
    0.023983342487660070,
    0.023936016839734148,
    0.023857265274998492,
    0.023747274197611152,
    0.023606303837450455,
    0.023434687516883282,
    0.023232830711902033,
    0.023001209910510589,
    0.022740371271860769,
    0.022450929090250545,
    0.022133564068691849,
    0.021789021407337010,
    0.021418108712617286,
    0.021021693733492330,
    0.020600701931734123,
    0.020156113893671546,
    0.019688962591300284,
    0.019200330501115824,
    0.018691346589453775,
    0.018163183173519573,
    0.017617052667658336,
    0.017054204224753452,
    0.016475920282948690,
]
j = range(X,len(s))
weight = range(0,len(h))
FIRsum = 0
for i in j:
    for ii in weight:
        FIRsum += s[i-ii]*h[ii]
    ssss.append(FIRsum)
    FIRsum = 0
jjjj = np.linspace(dt*X,t[-1], num=len(ssss))

ts = np.arange(0,t[-1],Ts) # time vector
y = s # the data to make the fft from
n = len(y) # length of the signal
k = np.arange(n)
T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y = np.fft.fft(y)/n # fft computing and normalization
Y = Y[range(int(n/2))]

# tss = np.arange(0,t[-1],Ts) # time vector
# yy = ss # the data to make the fft from
# nn = len(y) # length of the signal
# kk = np.arange(n)
# TT = n/Fs
# frq2 = kk/TT # two sides frequency range
# frq2 = frq2[range(int(n/2))] # one side frequency range
# YY = np.fft.fft(y)/n # fft computing and normalization
# YY = YY[range(int(n/2))]

plt.plot(t,y,'black')
plt.plot(jjjj,ssss,'r')
plt.xlabel('Time')
plt.ylabel('Amplitude')
# ax1.loglog(frq,abs(Y),'b') # plotting the fft
# ax2.set_xlabel('Freq (Hz)')
# ax2.set_ylabel('|Y(freq)|')
# ax2.loglog(frq,abs(YY),'b') # plotting the fft
# plt.title('N = '+str(X))
plt.show()