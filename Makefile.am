# "Makefile.am"
#
# Process this file with "automake" to generate "Makefile.in".
#
#	$Id: Makefile.am,v 1.15 2005/07/20 20:59:47 fang Exp $

AUTOMAKE_OPTIONS = 1.9 subdir-objects dist-bzip2
# may work with older versions of automake, but not guaranteed

SUBDIRS = src test . dox

include $(top_srcdir)/Make.stddef



DOXYGEN_CONFIG = art++.doxygen.config

# When to update the libtool script:
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

# see configure.in for adding libtool flags

CLEANFILES = art++.doxygen.config art++.doxygen.log DevelopmentPlan.ps

DISTCLEANFILES = config.in.h~

DISTCLEANDIRS = autom4te.cache

EXTRA_DIST = Make.stddef \
	bootstrap maintainerclobber \
	$(DOXYGEN_CONFIG).in \
	DevelopmentPlan.dot \
	readable_gcc_errors.sed \
	balance_indent.awk

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

# .PRECIOUS: distcheck.log
# doesn't work, tries to override previous definition without accumulating
distcheck.log: force
	@if [ -f $@ ] ; then $(MV) $@ $@~ ; fi
	@$(ECHO) "Logging \"make distcheck\" to $@ ..."
	@$(ECHO) Tests started `$(DATE)` > $@.tmp
	$(MAKE) distcheck 2>&1 | cat >> $@.tmp
	-$(MV) $@.tmp $@
	$(TAIL) -n 5 $@
# grep not working as expected?
#	@if $(GREP) -q "archives ready for distribution" $@ ; then \
#		$(TAIL) -n 5 $@ ; \
#		$(ECHO) "Congratulations!" ; \
#	else \
#		$(ECHO) "Sorry, check $@ for distcheck errors." ; \
#		exit 1 ; \
#	fi

# don't include CVS directories in distribution
dist-hook:
	$(RM) -r `find $(distdir) -name CVS`

cvsdiffs: force
	-$(CVS) diff -u > $@
	@$(ECHO) `$(GREP) "^-" $@ | $(GREP) -v "^---" | wc -l` "lines removed"
	@$(ECHO) `$(GREP) "^+" $@ | $(GREP) -v "^+++" | wc -l` "lines added"

.NOTPARALLEL: commit
commit: cvsdiffs distcheck
	$(CVS) commit

force:

