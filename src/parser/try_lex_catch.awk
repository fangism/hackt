#!/usr/bin/awk -f
# "try_lex_catch.awk"
# wraps call to YYLEX in try-catch block for exception handling
# hint: this filter/patch should be applied BEFORE changing 
# names to a different prefix other than "yy".

/char.*YYLEX/ {
	print "\ttry {";
	print $0;
	print "\t} catch (...) {";
	print "\t\tstd::cerr << \"Caught exception from lexer, aborting parsing.\" << endl;";
	print "\t\tyyfreestacks(yyss, yyssp, yyvs, yyvsp, yylval);";
	print "\t\tthrow;";	# re-throw
	print "\t}";
}

!/char.*YYLEX/ { print; }

