# "Makefile.am"
#
# Process this file with "automake" to generate "Makefile.in".
#
#	$Id: Makefile.am,v 1.9 2004/11/02 07:51:26 fang Exp $

AUTOMAKE_OPTIONS = 1.9 subdir-objects dist-bzip2
# may work with older versions of automake, but not guaranteed

SUBDIRS = src test . dox/lang

include $(top_srcdir)/Make.stddef



DOXYGEN_CONFIG = art++.doxygen.config

# When to update the libtool script:
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

# see configure.in for adding libtool flags

CLEANFILES = art++.doxygen.config art++.doxygen.log

DISTCLEANFILES = config.in.h~

DISTCLEANDIRS = autom4te.cache

EXTRA_DIST = Make.stddef \
	bootstrap maintainerclobber \
	$(DOXYGEN_CONFIG).in \
	readable_gcc_errors.sed \
	balance_indent.awk

if HAVE_DOT
$(DOXYGEN_CONFIG): $(DOXYGEN_CONFIG).in
	$(LN_S) $? $@
else
$(DOXYGEN_CONFIG): $(DOXYGEN_CONFIG).in
	$(SED) '/^HAVE_DOT/s/YES/NO/g' $? > $@
endif

if HAVE_DOXYGEN
docs: $(DOXYGEN_CONFIG)
	$(DOXYGEN) $(DOXYGEN_CONFIG)
	(cd dox/latex; $(MAKE) < /dev/null )
else
docs:
	@$(ECHO) No doxygen found in path.  
endif

clean-local:

distcleandirs:
	$(RM) -r $(DISTCLEANDIRS)

distclean-local: distcleandirs

# don't include CVS directories in distribution
dist-hook:
	$(RM) -r `find $(distdir) -name CVS`

cvsdiffs: force
	-$(CVS) diff > $@

commit: cvsdiffs distcheck
	$(CVS) commit

force:

