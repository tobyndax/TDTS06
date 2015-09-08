#Mac frameworks
LDFLAGS =
#Compiler flags, enable warnings
GCCFLAGS = -Wall
#Binaries that are needed for all sub-compilations
BINARIES = proxy.cpp comm.cpp main.cpp parser.cpp

all :  NetNinny

NetNinny : $(BINARIES)
	g++ $(GCCFLAGS) $(LDFLAGS) $(BINARIES) -o NetNinny

clean :
	rm NetNinny
	rm *.o
