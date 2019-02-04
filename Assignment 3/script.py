import numpy as np
import matplotlib.pyplot as plt
import subprocess
from subprocess import Popen, PIPE

print("Python script to run a3_14.cpp file")  #The main cpp file used by the script is a3_14.cpp

cmd = "a3_14.cpp"
subprocess.call(["g++","-std=c++11",cmd])

color = ["r","g","b","y","k"]                 #deciding the various colors, each for various scheduling algorithm
# args = ["10", "20", "30", "40", "50", "60", "70", "80", "90", "100"]#, "120", "140", "160", "180", "200"]
labels = ["FCFS", "Non-Preemptive SJF", "Preemptive SJF", "Round Robin", "HRN"]
args = ["10", "50", "100"]                    #args holds the number of processes for which simulations are done. The above line can be uncommented to increase the various number of processes for which the simulation is done
tn = []
atn = [[0.0 for i in range(len(args))] for j in range(5)]

for i in range(len(args)):                    #i stands for the no of jobs under considerations (is actually index for args array)
	for j in range(10):                   #for a given no of jobs, 10 simulations are done. j is the number of the simulation under work at present.
		result = subprocess.Popen(["./a.out", args[i]], stdout=subprocess.PIPE)
		out = result.stdout.read()    #out hold the result as printed on screen
		tn = out.split('\n')[:-1]     #tn is an array that hold the result in proper formatting
		tn = np.array(map(int, tn))   #tn holds the result for the particular simulation by typecasting to int
		for k in range(5):            #k stands for the various scheduling algorithms
			atn[k][i] += tn[k]    #atn hold the sum of turnaround time for all simulation
	for k in range(5):
		atn[k][i] /= 10.0             #atn now hold the average turnaround time because we have divided the total turnaround time by number of processes

plt.figure()
args = np.array(map(int, args))               #converting various elements of args from string to int
for k in range(5):
	# plt.figure()
	plt.scatter(args, atn[k], c=color[k]) #providing the information to be plotted
	plt.xlabel("Number of Jobs (N)")      #the label for x axis, which hold no of processes
	plt.ylabel("Average Turnaround Time") #the label for y axis, which hold average turnaround time
	plt.xticks(args)                      #label only the relevant number of jobs.
	# plt.title(labels[k])
	plt.legend(labels)                    #Displays the legend in the graph
	plt.savefig("Fig. "+str(k)+".png")

# plt.show()