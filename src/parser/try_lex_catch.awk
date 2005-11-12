#!/usr/bin/awk -f
# "try_lex_catch.awk"
#	$Id: try_lex_catch.awk,v 1.2.50.2 2005/11/12 01:52:46 fang Exp $
# wraps call to YYLEX in try-catch block for exception handling
# hint: this filter/patch should be applied BEFORE changing 
# names to a different prefix other than "yy".

# note: yyfreestacks is defined in our parser (.yy) files.  

/char.*=.*YYLEX/ || /char.*=.*yylex/ {
	print "\ttry {";
	print $0;
	print "\t} catch (...) {";
	print "\t\tstd::cerr << \"Caught exception from lexer, aborting parsing.\" << endl;";
	print "\t\tyyfreestacks(yyss, yyssp, yyvs, yyvsp, yylval, yychar, _lexer_state);";
	print "\t\tthrow;";	# re-throw
	print "\t}";
}

!/char.*=.*YYLEX/ && !/char.*=.*yylex/ { print; }

