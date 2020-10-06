CXX=g++
CXXOPTIMIZE= -O2
CXXFLAGS= -g -Wall -pthread -std=c++11 $(CXXOPTIMIZE)
USERID=Adrisson_Gianluigi_LucasAlberto
#CLASSES=SUA_LIB_COMUM
all: showip web-server
showip:
	$(CXX) -o $@ $^ $(CXXFLAGS) $@.cpp
web-server:
	$(CXX) -o $@ $^ $(CXXFLAGS) $@.cpp
clean:
	rm -rf *.o *~ *.gch *.swp *.dSYM multi-thread showip web-server *.tar.gz
tarball: clean
	tar -cvf lab02-$(USERID).tar.gz *
