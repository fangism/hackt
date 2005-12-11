# "scripts/Makefile.am"
#	$Id: Makefile.am,v 1.3.2.1 2005/12/11 22:29:15 fang Exp $

include $(top_srcdir)/Make.stddef

# empty
local.cvsignore:
	$(TOUCH) $@

CLEANFILES +=
DISTCLEANFILES += local.cvsignore
AUTO_IGNORE += $(DISTCLEANFILES)
EXTRA_DIST += configure-gcc4.sh \
	backup_repository.sh \
	readable_gcc_errors.sed \
	balance_indent.awk \
	exhaust_check.sh \
	tally_tests.awk \
	maintainerclobber


include $(top_srcdir)/Make.global

