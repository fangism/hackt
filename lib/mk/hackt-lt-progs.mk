# "hackt-lt-progs.mk"

# up to user to define the following, or let make pick defaults:
# CC = @CC@
# CXX = @CXX@

# expect these in path: a pre-configure and installed libtool script
LIBTOOL = $(HACKT_LIBTOOL)

CPPFLAGS = `$(HACKT_CONFIG) --cflags`

# COMPILE = $(CC) $(CPPFLAGS) $(CFLAGS)
CXXCOMPILE = $(CXX) $(CPPFLAGS) $(CXXFLAGS)
# LTCOMPILE = $(LIBTOOL) --tag=CC --mode=compile $(COMPILE)
LTCXXCOMPILE = $(LIBTOOL) --tag=CXX --mode=compile $(CXXCOMPILE)
# CCLD = $(CC)
CXXLD = $(CXX)
# LINK = $(LIBTOOL) --tag=CC --mode=link $(CCLD) $(CFLAGS) $(LDFLAGS) -o $@
CXXLINK = $(LIBTOOL) --tag=CXX --mode=link $(CXXLD) $(CXXFLAGS) $(LDFLAGS) -o $@

# libtool flags for creating shared, dlopen-able library
CHPSIM_MODULE_FLAGS = -module -rpath /bogus/lib

