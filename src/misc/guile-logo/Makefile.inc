# "misc/guile-logo/Makefile.inc"
#	$Id: Makefile.inc,v 1.1 2007/01/27 05:50:54 fang Exp $
#	vim: ft=automake
# play-testing guile functionality

GUILE_LOGO_DIR = misc/guile-logo

if WANT_LIBMISCFUN
FUNPROGRAMS += misc/guile-logo/demo1 misc/guile-logo/guile-logo
FUNLIBS += misc/guile-logo/libtortoise.la
endif

misc_guile_logo_libtortoise_la_SOURCES = \
	misc/guile-logo/tortoise.cc \
	misc/guile-logo/tortoise.h

misc_guile_logo_libtortoise_la_CPPFLAGS = \
	-I$(srcdir)/$(GUILE_LOGO_DIR) @X11_INCLUDE@
misc_guile_logo_libtortoise_la_CXXFLAGS = $(AM_CXXFLAGS) -Wno-old-style-cast
misc_guile_logo_libtortoise_la_LDFLAGS = @X11_LDPATH@
misc_guile_logo_libtortoise_la_LIBADD = -lX11

misc_guile_logo_demo1_SOURCES = misc/guile-logo/demo1.cc
misc_guile_logo_demo1_CPPFLAGS = -I$(srcdir)/$(GUILE_LOGO_DIR)
misc_guile_logo_demo1_LDADD = misc/guile-logo/libtortoise.la
# misc_guile_logo_demo1_LDFLAGS = @X11_LDPATH@

misc_guile_logo_guile_logo_SOURCES = \
	misc/guile-logo/tortoise_wrap.cxx \
	misc/guile-logo/logo_main.cc

# yes, distribute this built-source too
BUILT_SOURCES += misc/guile-logo/tortoise_wrap.cxx

misc_guile_logo_guile_logo_CPPFLAGS = \
	-I$(srcdir)/$(GUILE_LOGO_DIR) @GUILE_CPPFLAGS@
misc_guile_logo_guile_logo_CXXFLAGS = \
	$(AM_CXXFLAGS) -Wno-long-long -Wno-unused -Wno-error
misc_guile_logo_guile_logo_LDFLAGS = @GUILE_LDFLAGS@
misc_guile_logo_guile_logo_LDADD = misc/guile-logo/libtortoise.la -lguile

# alternative is to distribute without requiring user to have SWIG
misc/guile-logo/tortoise_wrap.cxx: misc/guile-logo/tortoise.swig
if HAVE_SWIG
	$(SWIG) -Wall -Werror -c++ -guile -scm -o $@ $<
else
	@$(ECHO) "No swig found in path, cannot regenerate $@"
endif

EXTRA_DIST += misc/guile-logo/README \
	misc/guile-logo/hex-test.scm \
	misc/guile-logo/tortoise_wrap.cxx \
	misc/guile-logo/tortoise.swig

