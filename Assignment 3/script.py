import numpy as np
import matplotlib.pyplot as plt
import subprocess
from subprocess import Popen, PIPE

print("Python script to run a3_14.cpp file")

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
		tn_psjf += tn[2]
		tn_rr += tn[3]
		tn_hrn += tn[4]
		# print(tn)
	
	print("FCFS ATN: " + str(float(tn_fcfs)/10))
	print("NPSJF ATN: " + str(float(tn_npsjf)/10))
	print("PSJF ATN: " + str(float(tn_psjf)/10))
	print("RR ATN: " + str(float(tn_rr)/10))
	print("HRN ATN: " + str(float(tn_hrn)/10))
	print("\n")
