import numpy as np
import subprocess
from subprocess import Popen, PIPE
 
print ("Hey this is Python Script Running\n")

cmd = "a3_14.cpp"
subprocess.call(["g++",cmd])

args = ["10", "50", "100"]
tn = []
tn = np.array(tn)
for i in range(len(args)):
	tn_fcfs = tn_npsjf = tn_psjf = tn_rr = tn_hrn = 0.0
	for j in range(10):
		result = subprocess.Popen(["./a.out",args[i]], stdout=subprocess.PIPE)
		out = result.stdout.read()
		tn = out.split('\n')[:-1]
		tn = np.array(map(int, tn))
		tn_fcfs += tn[0]
		tn_npsjf += tn[1]
		tn_rr += tn[2]
		# print(tn)
	
	print("FCFS ATN: " + str(float(tn_fcfs)/10))
	print("NPSJF ATN: " + str(float(tn_npsjf)/10))
	print("RR ATN: " + str(float(tn_rr)/10))
