import numpy as np
import matplotlib.pyplot as plt
import subprocess
from subprocess import Popen, PIPE

print("Python script to run a3_14.cpp file")

cmd = "a3_14.cpp"
subprocess.call(["g++","-std=c++11",cmd])

color = ["r","g","b","y","k"]
args = ["10", "20", "30", "40", "50", "60", "70", "80", "90", "100"]
tn = []
atn = [[0.0 for i in range(len(args))] for j in range(5)]

for i in range(len(args)):
	for j in range(10):
		result = subprocess.Popen(["./a.out",args[i]], stdout=subprocess.PIPE)
		out = result.stdout.read()
		tn = out.split('\n')[:-1]
		tn = np.array(map(int, tn))
		for k in range(5):
			atn[k][i] += tn[k]
	for k in range(5):
		atn[k][i] /= 10.0


args = np.array(map(int, args))
for k in range(5):
	plt.figure()
	plt.scatter(args, atn[k], c=color[k])

plt.show()