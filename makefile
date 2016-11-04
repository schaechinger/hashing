#
#  makefile
#  hashing
#
#  Created by Manuel Schächinger on 06.05.2013.
#  Copyright (c) 2013 Manuel Schächinger
#

CXXFLAGS := -g -Wall -c -std=c++11

# Set directory names
SRCDIR := src
OBJDIR := obj
BINDIR := bin
APP := hashing

cppfiles := $(wildcard $(SRCDIR)/*.cpp)
objects := $(subst $(SRCDIR)/, $(OBJDIR)/, $(cppfiles:.cpp=.o))
deps := $(objects:.o=.d)

.PHONY: all clean

all:  $(BINDIR)/$(APP)
-include $(deps)

$(OBJDIR)/%.d: $(SRCDIR)/%.cpp 
	mkdir -p $(@D)
	$(CXX) -MM -MT "$@ $(patsubst %.d,%.o,$@)" -MF $@ $<

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo compiling  $< ...
	$(CXX) $(CXXFLAGS) $(INCLUDE) $< -o $@

$(BINDIR)/$(APP): $(objects)
	mkdir -p $(BINDIR)
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) -r  $(OBJDIR)
	$(RM) -r  $(BINDIR)
