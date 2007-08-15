# "scripts/Makefile.am"
#	$Id: Makefile.am,v 1.7 2007/08/15 02:48:37 fang Exp $

include $(top_srcdir)/Make.stddef

# empty
local.cvsignore:
	$(TOUCH) $@

PRECONFIGURED_TESTSCRIPTS = \
	replace_if_changed.sh.in

POSTCONFIGURED_TESTSCRIPTS = \
	$(PRECONFIGURED_TESTSCRIPTS:.in=)

CLEANFILES +=
DISTCLEANFILES += local.cvsignore $(POSTCONFIGURED_TESTSCRIPTS)
AUTO_IGNORE += $(DISTCLEANFILES)
EXTRA_DIST += $(PRECONFIGURED_TESTSCRIPTS) \
	configure-gcc4.sh \
	backup_repository.sh \
	readable_gcc_errors.sed \
	balance_indent.awk \
	extract_texinfo.awk \
	exhaust_check.sh \
	rm_pattern.sh \
	tally_tests.awk \
	bootstrap.freebsd \
	maintainerclobber

include $(top_srcdir)/Make.global

