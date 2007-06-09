# "lib/mk/Makefile.am"
#	$Id: Makefile.am,v 1.1 2007/06/09 01:56:30 fang Exp $
# Makefile for standard hackt library files.  
# All files managed herein should be architecture-independent, 
# and appropriate for installation in $(datadir).

include $(top_srcdir)/Make.stddef

makedir = $(pkgdatadir)/mk

# Makefile and automake templates
make_DATA = \
	hackt.mk \
	hackt-suffixes.am \
	hackt-rules.am

dist_noinst_DATA = \
	hackt-suffixes.mk \
	hackt-progs.mk \
	hackt-rules.mk \
	hackt-clean.mk

noinst_DATA = hackt-stddef.mk

CLEANFILES += $(noinst_DATA) $(make_DATA)

hackt-stddef.mk:
	{ $(ECHO) "" ; \
	$(ECHO) "$$""(SED) = $(SED)" ; \
	$(ECHO) "$$""(RM) = $(RM)" ; \
	$(ECHO) "$$""(MV) = $(MV)" ; \
	$(ECHO) "" ; \
	} > $@

hackt.mk: hackt-suffixes.mk hackt-rules.am
	{ $(ECHO) "# \"$@\"" ; \
	$(ECHO) "# generated Makefile" ; \
	$(CAT) hackt-stddef.mk ; \
	$(ECHO) ".SUFFIXES: \\" ; \
	$(CAT) $(srcdir)/hackt-suffixes.mk ; \
	$(CAT) hackt-rules.am ; \
	} > $@

# automake header (include first)
# note: SUFFIXES must already be defined to append
hackt-suffixes.am: hackt-suffixes.mk
	{ $(ECHO) "# \"$@\"" ; \
	$(ECHO) "# generated Makefile" ; \
	$(ECHO) "SUFFIXES += \\" ; \
	$(CAT) $< ; \
	} > $@

# automake footer (include last)
hackt-rules.am: hackt-progs.mk hackt-rules.mk hackt-clean.mk
	{ $(ECHO) "# \"$@\"" ; \
	$(ECHO) "# generated Makefile" ; \
	$(CAT) $(srcdir)/hackt-progs.mk ; \
	$(CAT) $(srcdir)/hackt-rules.mk ; \
	$(ECHO) "all:" ; \
	$(ECHO) "" ; \
	$(CAT) $(srcdir)/hackt-clean.mk ; \
	} > $@

local.cvsignore:
	$(TOUCH) $@

DISTCLEANFILES += local.cvsignore
# will automatically pickup in AUTOIGNORE

include $(top_srcdir)/Make.global

