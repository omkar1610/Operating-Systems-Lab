import numpy as np
import matplotlib.pyplot as plt
import subprocess
from subprocess import Popen, PIPE

print("Python script to run a3_14.cpp file")

cmd = "a3_14.cpp"
subprocess.call(["g++","-std=c++11",cmd])

[[[1,2,3,4],[10,20,30,40]],
 [[1,2,3,4],[10,20,30,40]]]

# args = ["10", "50", "100"]
args = ["4"]
tn = []
tn = np.array(tn)
atn_fcfs = atn_npsjf = atn_psjf = atn_rr = atn_hrn = [[],[]]
atn = [[[],[]]]
for i in range(len(args)):
	tn_fcfs = tn_npsjf = tn_psjf = tn_rr = tn_hrn = 0.0
	ttn = []
	ttn = np.array(ttn)
	np.zeros(ttn)
	for j in range(1):
		result = subprocess.Popen(["./a.out",args[i]], stdout=subprocess.PIPE)
		out = result.stdout.read()
		tn = out.split('\n')[:-1]
		tn = np.array(map(int, tn))
		tn_fcfs += tn[0]
		tn_npsjf += tn[1]
		tn_psjf += tn[2]
		tn_rr += tn[3]
		tn_hrn += tn[4]
		print(tn)
	
	atn_fcfs[0].append(int(args[i]))
	atn_fcfs[1].append(float(tn_fcfs)/10)

	atn_npsjf[0].append(int(args[i]))
	atn_npsjf[1].append(float(tn_npsjf)/10)

	atn_psjf[0].append(int(args[i]))
	atn_psjf[1].append(float(tn_psjf)/10)

	atn_rr[0].append(int(args[i]))
	atn_rr[1].append(float(tn_rr)/10)

	atn_hrn[0].append(int(args[i]))
	atn_hrn[1].append(float(tn_hrn)/10)


plt.figure()
plt.plot(atn_fcfs[0], atn_fcfs[1])
plt.plot(atn_npsjf[0], atn_npsjf[1], 'g')
plt.show()