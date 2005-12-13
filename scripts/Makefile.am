# "scripts/Makefile.am"
#	$Id: Makefile.am,v 1.3.2.2 2005/12/13 02:57:21 fang Exp $

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
	exhaust_check.sh \
	tally_tests.awk \
	maintainerclobber

include $(top_srcdir)/Make.global

