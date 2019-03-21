MMU: MMU.cpp process
	g++ MMU.cpp -o MMU

process: process.cpp sched
	g++ process.cpp -o process

sched: sched.cpp Master
	g++ sched.cpp -o sched

Master: Master.cpp
	g++ -std=c++0x Master.cpp -o Master