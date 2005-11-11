#!/usr/bin/awk -f
# "purify_yacc.awk"
#	$Id: purify_yacc.awk,v 1.1.2.2 2005/11/11 08:20:47 fang Exp $
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
	# line count
	# associaltive array with (optional) initial values
	delete declarations;
}

{
	if (match($0, "^int yychar;")) {
		declarations[$0] = "";
	} else if (match($0, "^short [*]yyssp;")) {
		declarations[$0] = "NULL";
	} else if (match($0, "^YYSTYPE [*]yyvsp;")) {
		declarations[$0] = "NULL";
	} else if (match($0, "^YYSTYPE yyval;")) {
		declarations[$0] = "";
	} else if (match($0, "^YYSTYPE yylval;")) {
		foo = 0;
	} else if (match($0, "^short [*]yyss;")) {
		declarations[$0] = "NULL";
	} else if (match($0, "^short [*]yysslim;")) {
		declarations[$0] = "NULL";
	} else if (match($0, "^YYSTYPE [*]yyvs;")) {
		declarations[$0] = "NULL";
	} else if (match($0, "^int yystacksize;")) {
		declarations[$0] = "1";
	} else if (match($0, "^yyparse\\(.*\\)")) {
		print;
		getline;	# expecting open brace
		print;
		for (d in declarations) {
			init = declarations[d];
			if (length(init)) {
				sub(";", " = " init ";", d);
				print d;
			} else {
				print d;
			}
		}
	} else {
		print;
	}
}

