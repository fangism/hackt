# "Makefile.am"
#
# Process this file with "automake" to generate "Makefile.in".
#
#	$Id: Makefile.am,v 1.15.2.1 2005/07/21 01:06:18 fang Exp $

include $(top_srcdir)/Make.stddef

AUTOMAKE_OPTIONS += subdir-objects dist-bzip2
# may work with older versions of automake, but not guaranteed

SUBDIRS = src test . dox

DOXYGEN_CONFIG = art++.doxygen.config

# When to update the libtool script:
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

# see configure.in for adding libtool flags

CLEANFILES += $(DOXYGEN_CONFIG) art++.doxygen.log DevelopmentPlan.ps

DISTCLEANFILES += config.in.h~

DISTCLEANDIRS += autom4te.cache

AUTO_IGNORE += aclocal.m4 libtool stamp-h1 \
	config.h.in config.h config.log config.status config.cache \
	configure configure.scan configure.lineno \
	autoscan.log autom4te.cache \
	$(CLEANFILES) \
	distcheck.log distcheck.log.tmp

EXTRA_DIST += Make.stddef Make.global local.cvsignore \
	bootstrap maintainerclobber \
	$(DOXYGEN_CONFIG).in \
	DevelopmentPlan.dot \
	readable_gcc_errors.sed \
	balance_indent.awk

# TODO: use configure for this
if HAVE_DOT
$(DOXYGEN_CONFIG): $(DOXYGEN_CONFIG).in
	$(LN_S) $? $@
else
$(DOXYGEN_CONFIG): $(DOXYGEN_CONFIG).in
	$(SED) '/^HAVE_DOT/s/YES/NO/g' $? > $@
endif

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

clean-local:

distcleandirs:
	$(RM) -r $(DISTCLEANDIRS)

distclean-local: distcleandirs

# distcheck is a top-level only target
# .PRECIOUS: distcheck.log
# doesn't work, tries to override previous definition without accumulating
distcheck.log: force
	@if [ -f $@ ] ; then $(MV) $@ $@~ ; fi
	@$(ECHO) "Logging \"make distcheck\" to $@ ..."
	@$(ECHO) Tests started `$(DATE)` > $@.tmp
	$(MAKE) distcheck 2>&1 | cat >> $@.tmp
	-$(MV) $@.tmp $@
	$(TAIL) -n 5 $@

# don't include CVS directories in distribution
dist-hook:
	$(RM) -r `find $(distdir) -name CVS`

.NOTPARALLEL: commit
commit: cvsdiffs distcheck
	$(CVS) commit

include $(top_srcdir)/Make.global

