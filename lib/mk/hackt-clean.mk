# "mk/hackt-clean.mk"
#	$Id: hackt-clean.mk,v 1.2.52.1 2009/08/27 20:38:32 fang Exp $

clean: clean-local

clean-local:
	-$(RM) *.haco
#	-$(RM) *.haco-u *.unrolldump
	-$(RM) *.haco-c *.createdump
	-$(RM) *.haco-a *.allocdump
#	-$(RM) *.spice
	-$(RM) *.prs
	-$(RM) *.lvsprs
	-$(RM) *.lvssprs
	-$(RM) *.prsimexpr*
	-$(RM) *.prs-dot* *.prs-*-ps
	-$(RM) *.prs-*-fig *.prs-*-pdf
	-$(RM) *.sprs
	-$(RM) *.chpsim-event-*

