a.out: assn6_grp_14.cpp alt2.h libalt2.a
	g++ -g assn6_grp_14.cpp -L. -lalt2

libalt2.a: alt2.o
	ar -rcs libalt2.a alt2.o

alt2.o: alt2.cpp alt2.h
	g++ -c alt2.cpp

clean:
	rm alt2.o libalt1.a a.out
