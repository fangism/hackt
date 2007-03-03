#/usr/bin/awk -f
# "yacc_const_char.awk"
#	$Id: yacc_const_char.awk,v 1.1 2007/03/03 04:12:14 fang Exp $
# patch to replace char* with const char* in yacc skeletons
# because assigning string literals to them is deprecated and rejected
# known bad skeleton: byacc 1.14 (1997)

BEGIN { r=0; }

# can't just use 2-address range match, don't know how to negate it
/#if.*YYDEBUG/ { r=1; }
/#endif/ { r=0; }

/char[ ]?\*/ {
	if (r && !(match($0, "const char"))) {
		print "/* replaced char* with const char* */";
		sub("char[ ]?[*]", "const char *", $0);
	}
	print;
}
# else
!/char[ ]?\*/ {
	print;
}

