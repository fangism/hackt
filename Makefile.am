# "Makefile.am"
# Process this file with "automake" to generate "Makefile.in".

AUTOMAKE_OPTIONS = 1.9 subdir-objects dist-bzip2
# may work with older versions of automake, but not guaranteed

SUBDIRS = src test .

DOXYGEN_CONFIG = art++.doxygen.config

# When to update the libtool script:
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

CLEANFILES = art++.doxygen.config

DISTCLEANFILES = config.in.h~

DISTCLEANDIRS = autom4te.cache

EXTRA_DIST = Make.stddef readable_gcc_errors.sed $(DOXYGEN_CONFIG).in

if HAVE_DOT
$(DOXYGEN_CONFIG): $(DOXYGEN_CONFIG).in
	$(LN_S) $? $@
else
$(DOXYGEN_CONFIG): $(DOXYGEN_CONFIG).in
	sed '/^HAVE_DOT/s/YES/NO/g' $? > $@
endif

if HAVE_DOXYGEN
docs: $(DOXYGEN_CONFIG)
	$(DOXYGEN) $(DOXYGEN_CONFIG)
	(cd dox/latex; $(MAKE) < /dev/null )
else
docs:
	@echo No doxygen found in path.  
endif

clean-local:

distcleandirs:
	rm -rf $(DISTCLEANDIRS)

distclean-local: distcleandirs

# don't include CVS directories in distribution
dist-hook:
	rm -rf `find $(distdir) -name CVS`

cvsdiffs:
	-cvs diff > $@

commit: distcheck
	cvs commit

