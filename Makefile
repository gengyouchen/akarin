dramsim_path = $(HOME)/DRAMSim2/
CXXFLAGS = -I$(dramsim_path)
LDFLAGS = -L$(dramsim_path) -Wl,-rpath=$(dramsim_path)
LIBS = -ldramsim

all:
	$(CXX) -o Akarin main.cpp Akarin.cpp $(CXXFLAGS) $(LDFLAGS) $(LIBS)

exec:
	./Akarin

clean:
	rm Akarin -f
	rm dramsim.log -f
	rm dramsim.*.log -f
	rm results -rf
