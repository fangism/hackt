# "mk/hackt-clean.mk"
#	$Id: hackt-clean.mk,v 1.3 2009/08/28 20:44:45 fang Exp $

clean: clean-local

clean-local:
	-$(RM) *.haco
#	-$(RM) *.haco-u *.unrolldump
	-$(RM) *.haco-c *.createdump
	-$(RM) *.haco-a *.allocdump
	-$(RM) *.spice
	-$(RM) *.prs
	-$(RM) *.lvsprs
	-$(RM) *.lvssprs
	-$(RM) *.prsimexpr*
	-$(RM) *.prs-dot* *.prs-*-ps
	-$(RM) *.prs-*-fig *.prs-*-pdf
	-$(RM) *.sprs
	-$(RM) *.chpsim-event-*

