#Custom Functions
rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

getobjs = $(patsubst %.cpp,%.cpp.o,$(call rwildcard,$1,*.cpp)) $(patsubst %.cc,%.cc.o,$(call rwildcard,$1,*.cc))

prepend_dir = $(patsubst %.$3,$2/%.$3,$1)


TARGET = jsonfs.out


OBJS = $(call getobjs,src)
DIR_OBJS = $(call prepend_dir,$(OBJS),objs,o)
DEPS = $(DIR_OBJS:.o=.d)






LIBS += -lfuse

INCLUDES = -I include -I lib/include -I lib/json/include




LIB_FOLDERS += lib



LIB_FOLDER_ARGS = $(patsubst %,-L %,$(LIB_FOLDERS))


DEFS += -DDEBUG
DEFS += -DDEBUG_FULL
#CXXFLAGS += -DFUSE_SIMULATION

FUSEFLAGS += -D_FILE_OFFSET_BITS=64
FUSEFLAGS += -DFUSE_USE_VERSION=32


WARN += -Wall
WARN += -fmax-errors=1
#WARN += -Wfatal-errors
WARN += -Wno-invalid-offsetof
WARN += -Wno-unused-local-typedefs
WARN += -Wno-class-memaccess
WARN += -Wno-unused-label
WARN += -Wno-unused-function
WARN += -Wno-warn=invalid-offsetof
WARN += -Werror


CXXFLAGS += -std=c++14
CXXFLAGS += -MMD 
CXXFLAGS += $(DEFS)
CXXFLAGS += $(WARN)
CXXFLAGS += $(INCLUDES)
CXXFLAGS += -O0
#CXXFLAGS += -O3
#CXXFLAGS += -ffast-math
CXXFLAGS += -funroll-loops
CXXFLAGS += $(LIB_FOLDER_ARGS)
# CXXFLAGS += -fno-exceptions
CXXFLAGS += -fno-rtti
CXXFLAGS += $(FUSEFLAGS)






all: $(TARGET)
	



$(TARGET): $(DIR_OBJS) Makefile
	echo $(DEPS)
	echo $(LIB_FOLDER_ARGS)
	$(CXX) $(CXXFLAGS) -o $@ $(DIR_OBJS) $(LIBS)

objs/%.cpp.o: %.cpp
	dirname $@ | xargs mkdir -p 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

objs/%.cc.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<
	

objs/%.cpp.ppp: %.cpp
	$(CXX) $(CXXFLAGS) -E -o $@ $<

clean:
	-rm -f $(DIR_OBJS)



-include $(DEPS)