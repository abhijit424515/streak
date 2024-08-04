FNAME = main

CPP = g++
TGT = server

CFLAGS = -g

$(TGT):
	$(CPP) --std=c++0x $(FNAME).cc -o $(TGT)

clean :
	rm -f *.o *.output
	rm -f $(TGT) 
	rm -rf parse.tab.c parse.tab.h scan.c 
