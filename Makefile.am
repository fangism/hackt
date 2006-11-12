# "Makefile.am"
#
# Process this file with "automake" to generate "Makefile.in".
#
#	$Id: Makefile.am,v 1.38 2006/11/12 05:57:40 fang Exp $

include $(top_srcdir)/Make.stddef

AUTOMAKE_OPTIONS += subdir-objects dist-bzip2
# may work with older versions of automake, but not guaranteed

SUBDIRS = scripts src lib test . dox
# temporary, for debugging make distcheck in dox, 
# so we don't have to wait for the entire src tree to build first
# SUBDIRS = scripts dox src lib test .

docdir = $(pkgdatadir)/doc

DOXYGEN_CONFIG = hackt.doxygen.config

# When to update the libtool script:
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

# don't need $(srcdir) because cd $(top_srcdir) is prior to exec. aclocal
ACLOCAL_AMFLAGS = -I config

# see configure.in for adding libtool flags

CLEANFILES += $(DOXYGEN_CONFIG) hackt.doxygen.log DevelopmentPlan.ps

DISTCLEANFILES += config.in.h~ distcheck.log

DISTCLEANDIRS += autom4te.cache

AUTO_IGNORE += aclocal.m4 libtool \
	config.h.in \
	configure configure.scan \
	autoscan.log autom4te.cache \
	$(am__CONFIG_DISTCLEAN_FILES) \
	$(CONFIG_CLEAN_FILES) \
	$(CONFIG_HEADER) \
	distcheck.log distcheck.log.tmp

doc_DATA = \
	AUTHORS \
	BOUNTY \
	CONTRIBUTORS \
	COPYING \
	NEWS \
	README \
	TODO

noinst_SCRIPTS = install_transform.sh

# Many of these are already in DIST_COMMON
EXTRA_DIST += Make.stddef Make.global local.cvsignore \
	bootstrap \
	BOUNTY \
	BRANCHES \
	BUILDS \
	CONTRIBUTORS \
	README.CVS \
	README.maintainer \
	$(DOXYGEN_CONFIG).in \
	config/macro-value.h.in \
	config/config_subst.awk \
	config/ylwrap \
	config/am__remove_distdir.diff \
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
	@if test -f $@ ; then $(MV) $@ $@~ ; fi
	@$(ECHO) "Logging \"make distcheck\" to $@ ..."
	@$(ECHO) Tests started `$(DATE)` > $@.tmp
	-$(MAKE) DISTCHECK_CONFIGURE_FLAGS="$(DISTCHECK_CONFIGURE_FLAGS)" distcheck >> $@.tmp 2>&1
	-$(MV) $@.tmp $@
	@$(ECHO) Tests finished `$(DATE)` >> $@
	$(TAIL) -n 6 $@

# don't include CVS directories in distribution
dist-hook:
	$(RM) -r `$(FIND) $(distdir) -name CVS`

# for recursive make to not parallelize these
.NOTPARALLEL: pdf ps
# .NOTPARALLEL: install
.NOTPARALLEL: commit
if HAVE_CVS
commit: cvsdiffs distcheck
	$(CVS) commit
else
commit:
	@$(ECHO) "Sorry, no 'cvs' found in path."
endif

help-local:
	@$(ECHO) "Local make targets:" ; \
	$(ECHO) "    distcheck: checks self-containedness of source tree (important)" ; \
	$(ECHO) "    distcheck.log: logs distcheck" ; \
	$(ECHO) "    install: installs built targets (recursive)" ; \
	$(ECHO) "    install-check: checks install (recursive)" ; \
	$(ECHO) "    uninstall: uninstalls this package (recursive)" ; \
	$(ECHO) "    distclean: removes configure targets and cleans (recursive)" ; \
	$(ECHO) "    maintainer-clean: removes bootstrap targets and distcleans" ; \
	$(ECHO) "    libtool: regenerates the configured libtool script" ; \
	$(ECHO) "    DevelopmentPlan.ps: visualize development plan using dot" ; \
	$(ECHO) "    commit: distcheck followed by CVS commit" ; \
	$(ECHO) "    docs: builds Doxygen source documentation (SLOW)"

include $(top_srcdir)/Make.global

