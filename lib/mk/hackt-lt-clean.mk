# "Makefile[.in]"
#	$Id: hackt-lt-clean.mk,v 1.2 2007/07/31 23:22:35 fang Exp $
# vi: ft=automake
#
# This Makefile should not be invoked until AFTER hackt is installed!
# "hackt-lt-clean.mk"
#	$id: $

# should depend on $(OBJEXT)
clean-local: clean-obj

clean-obj:
	$(RM) *.o *.lo *.a *.la
	$(RM) -r .libs _libs

