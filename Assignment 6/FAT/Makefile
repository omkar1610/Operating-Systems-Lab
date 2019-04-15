a.out: assn6_grp_14.cpp alt1.h libalt1.a
	g++ assn6_grp_14.cpp -L. -lalt1

libalt1.a: alt1.o
	ar -rcs libalt1.a alt1.o

alt1.o: alt1.cpp alt1.h
	g++ -c alt1.cpp

clean:
	rm alt1.o libalt1.a a.out
