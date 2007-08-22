# "lib/mk/Makefile.am"
#	$Id: Makefile.am,v 1.3 2007/08/22 20:24:33 fang Exp $
# Makefile for standard hackt library files.  
# All files managed herein should be architecture-independent, 
# and appropriate for installation in $(datadir).

include $(top_srcdir)/Make.stddef

makedir = $(pkgdatadir)/mk

# Makefile and automake templates
make_DATA = \
	hackt.mk \
	hackt-suffixes.am \
	hackt-rules.am \
	hackt-lt.mk \
	hackt-lt-suffixes.am \
	hackt-lt-rules.am

# -lt are the libtoolized Makefile variants

dist_noinst_DATA = \
	hackt-suffixes.mk \
	hackt-progs.mk \
	hackt-rules.mk \
	hackt-clean.mk \
	hackt-lt-suffixes.mk \
	hackt-lt-progs.mk \
	hackt-lt-rules.mk \
	hackt-lt-clean.mk

noinst_DATA = hackt-stddef.mk

CLEANFILES += $(noinst_DATA) $(make_DATA)

hackt-stddef.mk:
	{ $(ECHO) "" && \
	$(ECHO) "SED = $(SED)" && \
	$(ECHO) "RM = $(RM)" && \
	$(ECHO) "MV = $(MV)" && \
	$(ECHO) "" ; \
	} > $@

# unified Makefile template, non-autotoolized
hackt.mk: hackt-stddef.mk hackt-suffixes.mk hackt-rules.am
	{ $(ECHO) "# \"$@\"" && \
	$(ECHO) "# generated Makefile" && \
	$(ECHO) "# vi: ft=make" && \
	$(CAT) hackt-stddef.mk && \
	$(ECHO) ".SUFFIXES: \\" && \
	$(CAT) $(srcdir)/hackt-suffixes.mk && \
	$(CAT) hackt-rules.am ; \
	} > $@

# automake header (include first)
# note: SUFFIXES must already be defined to append
hackt-suffixes.am: hackt-suffixes.mk
	{ $(ECHO) "# \"$@\"" ; \
	$(ECHO) "# generated Makefile" && \
	$(ECHO) "# vi: ft=make" && \
	$(ECHO) "SUFFIXES += \\" && \
	$(CAT) $< ; \
	} > $@

# automake footer (include last)
hackt-rules.am: hackt-progs.mk hackt-rules.mk hackt-clean.mk
	{ $(ECHO) "# \"$@\"" && \
	$(ECHO) "# generated Makefile" && \
	$(ECHO) "# vi: ft=make" && \
	$(ECHO) "all:" && \
	$(ECHO) "" && \
	for f in $^ ; do \
	  $(CAT) $$f ; \
	done ; \
	} > $@

#	$(CAT) $(srcdir)/hackt-progs.mk ;
#	$(CAT) $(srcdir)/hackt-rules.mk ;
#	$(CAT) $(srcdir)/hackt-clean.mk ;

# unified Makefile template, non-autotoolized
hackt-lt.mk: hackt-stddef.mk hackt-suffixes.mk hackt-lt-suffixes.mk hackt-lt-rules.am
	{ $(ECHO) "# \"$@\"" && \
	$(ECHO) "# generated Makefile" && \
	$(ECHO) "# vi: ft=make" && \
	$(CAT) hackt-stddef.mk && \
	$(ECHO) ".SUFFIXES: \\" && \
	$(CAT) $(srcdir)/hackt-suffixes.mk && \
	$(ECHO) ".SUFFIXES: \\" && \
	$(CAT) $(srcdir)/hackt-lt-suffixes.mk && \
	$(CAT) hackt-lt-rules.am ; \
	} > $@

# automake header (include first)
# note: SUFFIXES must already be defined to append
hackt-lt-suffixes.am: hackt-suffixes.mk hackt-lt-suffixes.mk
	{ $(ECHO) "# \"$@\"" && \
	$(ECHO) "# generated Makefile" && \
	$(ECHO) "# vi: ft=make" && \
	for f in $^ ; do \
	  $(ECHO) "SUFFIXES += \\" && \
	  $(CAT) $$f ; \
	done ; \
	} > $@

# automake footer (include last)
hackt-lt-rules.am: hackt-progs.mk hackt-lt-progs.mk \
		hackt-rules.mk hackt-lt-rules.mk \
		hackt-clean.mk hackt-lt-clean.mk
	{ $(ECHO) "# \"$@\"" && \
	$(ECHO) "# generated Makefile" && \
	$(ECHO) "# vi: ft=make" && \
	$(ECHO) "all:" && \
	$(ECHO) "" && \
	for f in $^ ; do \
	  $(CAT) $$f ; \
	done ; \
	} > $@

#	$(CAT) $(srcdir)/hackt-progs.mk ;
#	$(CAT) $(srcdir)/hackt-lt-progs.mk ;
#	$(CAT) $(srcdir)/hackt-rules.mk ;
#	$(CAT) $(srcdir)/hackt-lt-rules.mk ;
#	$(CAT) $(srcdir)/hackt-clean.mk ;
#	$(CAT) $(srcdir)/hackt-lt-clean.mk ;

local.cvsignore:
	$(TOUCH) $@

DISTCLEANFILES += local.cvsignore
# will automatically pickup in AUTOIGNORE

include $(top_srcdir)/Make.global

