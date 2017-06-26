CC = g++

LFLAGS = -lmosquittopp
# CSOURCES = $(wildcard com/**/*.c)
CXXSOURCES = $(wildcard com/**/*.cpp) $(wildcard algo/**.cpp) $(wildcard src/**.cpp) $(wildcard srv/**.cpp)
COBJS = $(patsubst %.c, build/%.o, $(CSOURCES))
CXXOBJS += $(patsubst %.cpp, build/%.o, $(CXXSOURCES))
INCL = $(wildcard include/*.h) $(wildcard include/**/*.h)
COMFLAGS = -Wall -Wextra -pedantic -O0 -g
CFLAGS = $(COMFLAGS) -std=c99
CXXFLAGS = $(COMFLAGS) -std=c++11

build/lauSim: $(COBJS) $(CXXOBJS)
#	$(info $$CXXOBJS = [${CXXOBJS}])
#	$(info $$INCL = [${INCL}])
	$(CC) -Wall -Wextra -pedantic -O0 -g $(LFLAGS) $(COBJS) $(CXXOBJS) -o build/lauSim

$(COBJS): build/%.o: %.c $(INCL)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(CXXOBJS): build/%.o: %.cpp $(INCL)
	@mkdir -p $(@D)
	$(CC) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build/

.PHONY: all clean
