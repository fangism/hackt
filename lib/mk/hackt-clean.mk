# "mk/hackt-clean.mk"
#	$Id: hackt-clean.mk,v 1.2 2007/06/12 20:59:27 fang Exp $

clean: clean-local

clean-local:
	-$(RM) *.haco
#	-$(RM) *.haco-u *.unrolldump
	-$(RM) *.haco-c *.createdump
	-$(RM) *.haco-a *.allocdump
	-$(RM) *.prs
	-$(RM) *.lvsprs
	-$(RM) *.lvssprs
	-$(RM) *.prsimexpr*
	-$(RM) *.prs-dot* *.prs-*-ps
	-$(RM) *.prs-*-fig *.prs-*-pdf
	-$(RM) *.sprs
	-$(RM) *.chpsim-event-*

