#Anne Warden
#Makefile
CPPFLAGS=-pthread
LIBS=-lrt

all: master reverse upper

master:
	g++ $(CPPFLAGS) -o master master.cpp $(LIBS)

reverse:
	g++ $(CPPFLAGS) -o reverse reverse.cpp $(LIBS)

upper:
	g++ $(CPPFLAGS) -o upper upper.cpp $(LIBS)

clean:
	rm -f *~ master reverse upper /dev/shm/*cs375_*

submit.tar:
	tar -cvf aw308p6.tar shared.h master.cpp reverse.cpp upper.cpp Makefile
