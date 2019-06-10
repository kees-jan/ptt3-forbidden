# In this directory, type one of the following:
#   make
#   make all

# Name of your library. It is always called libsomething.a, otherwise
# the linker won't find it.
LIB = libforbidden.a

# -I directives for the directories of your include files
INCLUDE_LOCATIONS = -I../inc

# Name of all source files in your library
SOURCES_LIB = $(wildcard *.cpp)

#Don't change these
CXX = g++
CXXFLAGS += -ggdb3 -Wall -Wextra -Wshadow -Werror -pedantic -std=c++1z
CPPFLAGS += -D_REENTRANT -DDEBUG $(INCLUDE_LOCATIONS)
OBJECTS_LIB = $(SOURCES_LIB:.cpp=.o)
DEPENDENCY_FILES = $(addprefix .,$(OBJECTS_LIB:.o=.d))

all: $(LIB)

# Tell make to include the dependency information for your cpp files,
# if they exist, and if we are not executing "clean".
#
# Using dependency information, if you edit a .h file, all relevant
# .cpp files will be rebuilt.
ifneq "$(MAKECMDGOALS)" "clean"
-include $(DEPENDENCY_FILES)
endif

# Tells make how to compile a single source file, and to generate
# dependency information for it.
%.o:%.cpp Makefile
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@ -MD -MP -MF .$*.d

# Create the lib needed by your exe
$(LIB): $(OBJECTS_LIB)
	ar rcs $@ $^

# To remove generated files
clean:
	rm -f $(LIB) $(OBJECTS_LIB) $(DEPENDENCY_FILES)

.PHONY: clean all
