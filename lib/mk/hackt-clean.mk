# "mk/hackt-clean.mk"
#	$Id: hackt-clean.mk,v 1.1 2007/06/09 01:56:30 fang Exp $

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

