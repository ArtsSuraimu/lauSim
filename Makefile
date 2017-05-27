CC = g++

LFLAGS = -lmosquittopp
# CSOURCES = $(wildcard com/**/*.c)
CXXSOURCES = $(wildcard com/**/*.cpp) $(wildcard algo/**.cpp) $(wildcard src/**.cpp) $(wildcard srv/**.cpp)
COBJS = $(patsubst %.c, build/%.o, $(CSOURCES))
CXXOBJS += $(patsubst %.cpp, build/%.o, $(CXXSOURCES))
INCL = $(wildcard include/*.h) $(wildcard include/**/*.h)
CLFAGS = -Wall -Wextra -pedantic -O0 -g

build/lauSim: $(COBJS) $(CXXOBJS)
#	$(info $$CXXOBJS = [${CXXOBJS}])
#	$(info $$INCL = [${INCL}])
	$(CC) -Wall -Wextra -pedantic -O0 -g $(LFLAGS) $(COBJS) $(CXXOBJS) -o build/lauSim

$(COBJS): build/%.o: %.c $(INCL)
	@mkdir -p $(@D)
	$(CC) -Wall -Wextra -pedantic -O0 -g -c $< -o $@

$(CXXOBJS): build/%.o: %.cpp $(INCL)
	@mkdir -p $(@D)
	$(CC) -Wall -Wextra -pedantic -O0 -g -c $< -o $@

clean:
	rm -rf build/

.PHONY: all clean
