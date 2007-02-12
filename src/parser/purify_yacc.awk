#!/usr/bin/awk -f
# "purify_yacc.awk"
#	$Id: purify_yacc.awk,v 1.5 2007/02/12 06:54:41 fang Exp $
# helper script to transform yacc's generated parser into a pure-parser.
# one that is re-entrant.  

# We accomplish this by moving global variables to local variables.  
# the following variables are needed.  
# int yychar;
# short *yyssp;
# YYSTYPE *yyvsp;
# YYSTYPE yyval;
# YYSTYPE yylval; -- delete this but don't add locally, is passed in as param
# short *yyss;
# short *yysslim;
# YYSTYPE *yyvs;
# int yystacksize;

BEGIN {
	this_script = "\"$(srcdir)/parser/purify_yacc.awk\"";
	# associative array with (optional) initial values
	# delete declarations;	# this is not POSIX
	for (d in declarations) delete declarations[d];	# this IS
}

function comment_out(str) {
	print "/* " str " */ // removed by " this_script;
}

{
	if (match($0, "^int yychar;")) {
		declarations[$0] = "";
		comment_out($0);
	} else if (match($0, "^short [*]yyssp;")) {
		declarations[$0] = "NULL";
		comment_out($0);
	} else if (match($0, "^YYSTYPE [*]yyvsp;")) {
		declarations[$0] = "NULL";
		comment_out($0);
	} else if (match($0, "^YYSTYPE yyval;")) {
		declarations[$0] = "";
		comment_out($0);
	} else if (match($0, "^YYSTYPE yylval;")) {
		foo = 0;
		comment_out($0);
	} else if (match($0, "^short [*]yyss;")) {
		declarations[$0] = "NULL";
		comment_out($0);
	} else if (match($0, "^short yyss\\[.*\\];")) {
		# for byacc
		declarations[$0] = "";
		comment_out($0);
	} else if (match($0, "^short [*]yysslim;")) {
		declarations[$0] = "NULL";
		comment_out($0);
	} else if (match($0, "^YYSTYPE [*]yyvs;")) {
		declarations[$0] = "NULL";
		comment_out($0);
	} else if (match($0, "^YYSTYPE yyvs\\[.*\\];")) {
		# for byacc
		declarations[$0] = "";
		comment_out($0);
	} else if (match($0, "^int yystacksize;")) {
		declarations[$0] = "1";
		comment_out($0);
	} else if (match($0, "^yyparse[ ]*\\(.*\\)[^{]*$")) {	# no open brace
		print;
		do {
			getline;	# expecting open brace
			print;
		} while (!match($0, "\\{"));
		print "/* Injected by " this_script " */";
		for (d in declarations) {
			init = declarations[d];
			if (length(init)) {
				sub(";", " = " init ";", d);
				print d;
			} else {
				print d;
			}
		}
		print "/* end injection */";
	} else {
		print;
	}
}

