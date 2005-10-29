# "scripts/Makefile.am"
#	$Id: Makefile.am,v 1.1.10.1 2005/10/29 21:43:44 fang Exp $

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
	tally_tests.awk


include $(top_srcdir)/Make.global

