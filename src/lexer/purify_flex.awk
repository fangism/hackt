#!/usr/bin/awk -f
# "purify_flex.awk"
#	$Id: purify_flex.awk,v 1.1.2.1 2005/11/11 08:20:46 fang Exp $
# helper script to transform flex's generated scanner into a pure-scanner.
# one that is re-entrant.  
# This script was copy-inspired from "parser/purify_yacc.awk"

# pass -v name=... to override the name of the struct state object.  

# It is a known fact that the standard C flex-scanners are not reentrant like
# the original lex-scanners may have been.  The C++ scanners generated
# by flex are, however re-entrant.  If that is not an option, 
# then use this script to hack the generated file.  

# We accomplish this by moving global variables to local variables.  
# the following variables are needed.  

BEGIN {
	if (!length(name))
		name = "foo";
	# associative array with reduced member name
	# name of state struct member should be the same
	delete members;
	dupe = name "\\." name;
}

# return substring starting with yy consisteing of identifier characters
function extract_yy_identifier(str) {
}

{
	# NOTE: be careful to avoid the member members of yy_buffer_state.
	# typically, those are found indented, so we only match 
	# members that start at the beginning-of-line.  
	if (match($0, "^extern int yyleng;")) {
		members["yyleng"] = "";
	} else if (match($0, "FILE.*[*]yyin.*[*]yyout")) {
		members["yyin"] = "";
		members["yyout"] = "";
	} else if (match($0, "^static YY_BUFFER_STATE yy_current_buffer.*;")) {
		members["yy_current_buffer"] = "";
		# this would be a good place to include the state file
		print "#include \"lexer/flex_lexer_state.h\"";
	} else if (match($0, "^static char yy_hold_char;")) {
		members["yy_hold_char"] = "";
	} else if (match($0, "^static int yy_n_chars;")) {
		members["yy_n_chars"] = "";
	} else if (match($0, "^int yyleng;")) {
		members["yyleng"] = "";
	} else if (match($0, "^static char [*]yy_c_buf_p")) {
		members["yy_c_buf_p"] = "";
	} else if (match($0, "^static int yy_init")) {
		members["yy_init"] = "";
	} else if (match($0, "^static int yy_start =")) {
		members["yy_start"] = "";
	} else if (match($0, "^static int yy_did_buffer_switch_on_eof;")) {
		members["yy_did_buffer_switch_on_eof"] = "";
	} else if (match($0, "char [*]yytext;")) {
		members["yytext"] = "";
	} else if (match($0, "^static yy_state_type yy_last_accepting_state")) {
		members["yy_last_accepting_state"] = "";
	} else if (match($0, "^static char [*]yy_last_accepting_cpos;")) {
		members["yy_last_accepting_cpos"] = "";
	} else if (match($0, "^static int yy_more_flag")) {
		members["yy_more_flag"] = "";
	} else if (match($0, "^static int yy_more_len")) {
		members["yy_more_len"] = "";
	# only if YY_STACK_USED
	} else if (match($0, "^static int yy_start_stack_ptr")) {
		# covered by "yy_start" already
		# members["yy_start_stack_ptr"] = "";
	} else if (match($0, "^static int yy_start_stack_depth")) {
		# members["yy_start_stack_depth"] = "";
	} else if (match($0, "^static int [*]yy_start_stack")) {
		# members["yy_start_stack"] = "";
	} else {
		str = $0;
		if (match(str,"[^.]yy") && !match(str,"PROTO") &&
				!match(str,"define")) {
			for (m in members) {
				gsub(m, name ".&", str);
			}
			# shit happens...
			# while (match(str, dupe)) {
			#	gsub(dupe, name, str);
			# }
		}
		print str;
	}
}

