# "Makefile.am"
#
# Process this file with "automake" to generate "Makefile.in".
#
#	$Id: Makefile.am,v 1.19.6.2 2005/11/07 08:54:16 fang Exp $

include $(top_srcdir)/Make.stddef

AUTOMAKE_OPTIONS += subdir-objects dist-bzip2
# may work with older versions of automake, but not guaranteed

SUBDIRS = scripts src test . dox

DOXYGEN_CONFIG = hackt.doxygen.config

# When to update the libtool script:
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

# see configure.in for adding libtool flags

CLEANFILES += $(DOXYGEN_CONFIG) hackt.doxygen.log DevelopmentPlan.ps

DISTCLEANFILES += config.in.h~ distcheck.log

DISTCLEANDIRS += autom4te.cache

AUTO_IGNORE += aclocal.m4 libtool stamp-h1 \
	config.h.in \
	configure configure.scan \
	autoscan.log autom4te.cache \
	$(am__CONFIG_DISTCLEAN_FILES) \
	$(CONFIG_CLEAN_FILES) \
	$(CONFIG_HEADER) \
	distcheck.log distcheck.log.tmp

# Many of these are already in DIST_COMMON
EXTRA_DIST += Make.stddef Make.global local.cvsignore \
	bootstrap maintainerclobber \
	BOUNTY \
	$(DOXYGEN_CONFIG).in \
	config/macro-value.h.in \
	config/config_subst.awk \
	DevelopmentPlan.dot


if HAVE_DOT
DevelopmentPlan.ps: DevelopmentPlan.dot
	$(DOT) -Tps $? -o$@
else
DevelopmentPlan.ps: DevelopmentPlan.dot
	@$(ECHO) No dot found in path.  
endif

if HAVE_DOXYGEN
docs: $(DOXYGEN_CONFIG)
	$(DOXYGEN) $(DOXYGEN_CONFIG)
if HAVE_LATEX
	(cd dox/latex; $(MAKE) < /dev/null )
endif
else
docs:
	@$(ECHO) No doxygen found in path.  
endif

# distcheck is a top-level only target
# .PRECIOUS: distcheck.log
# doesn't work, tries to override previous definition without accumulating
distcheck.log: force
	@if [ -f $@ ] ; then $(MV) $@ $@~ ; fi
	@$(ECHO) "Logging \"make distcheck\" to $@ ..."
	@$(ECHO) Tests started `$(DATE)` > $@.tmp
	-$(MAKE) distcheck >> $@.tmp 2>&1
	-$(MV) $@.tmp $@
	$(TAIL) -n 5 $@

# don't include CVS directories in distribution
dist-hook:
	$(RM) -r `find $(distdir) -name CVS`

.NOTPARALLEL: commit
commit: cvsdiffs distcheck
	$(CVS) commit

include $(top_srcdir)/Make.global

