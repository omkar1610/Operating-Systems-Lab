Master: Master.cpp sched
	g++ -g -std=c++11 Master.cpp -o Master

sched: sched.cpp process
	g++ -g -std=c++11 sched.cpp -o sched

process: process.cpp MMU
	g++ -g -std=c++11 process.cpp -o process

MMU: MMU.cpp
	g++ -g -std=c++11 MMU.cpp -o MMU


clean: 
	rm Master sched process MMU
