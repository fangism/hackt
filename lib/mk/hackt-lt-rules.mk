# "hackt-lt-rules.mk"
#	$Id: hackt-lt-rules.mk,v 1.1.2.1 2007/07/27 21:03:43 fang Exp $
# counterpart to "hackt-lt-suffixes.mk" and "hackt-lt-progs.mk"
# not bothering with automatic dependency tracking here

.cc.lo:
	$(LTCXXCOMPILE) $< -c -o $@

.cc.o:
	$(CXXCOMPILE) $< -c -o $@

# .c.lo:
# 	$(LTCOMPILE) $< -c -o $@

# .c.o:
#	$(COMPILE) $< -c -o $@

