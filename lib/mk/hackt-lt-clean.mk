# "Makefile[.in]"
#	$Id: hackt-lt-clean.mk,v 1.1.2.1 2007/07/27 21:03:42 fang Exp $
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

