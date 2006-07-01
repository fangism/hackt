#!/usr/bin/awk -f
#	$Id: nls_bison_hack.awk,v 1.1 2006/07/01 21:00:35 fang Exp $
# "parser/nls_bison_hack.awk"
# Filters annoying YY_(...) wrapped arguments passed to yyerror
# which disrupts the yyerror bison hack.  

/YY_\(.*\)/ {
	if (!match($0, "define")) {
		while (match($0, "YY_([^)]+)")) {
			# strip away "YY_(" ")"
			rpl = substr($0, RSTART+4, RLENGTH-4);
			# print "RPL = " rpl;
			sub("YY_[(][^)]+[)]", rpl, $0);
		}
		# print $0;
	} else {
		print $0;
	}
}

!/YY_\(.*\)/ {
	print $0;
}

