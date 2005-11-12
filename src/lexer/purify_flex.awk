#!/usr/bin/awk -f
# "purify_flex.awk"
#	$Id: purify_flex.awk,v 1.1.2.2 2005/11/12 01:52:44 fang Exp $
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
	script_name = "\"lexer/purify_flex.awk\"";
	if (!length(name))
		name = "foo";
	# associative array with reduced member name
	# name of state struct member should be the same
	delete members;
	# dupe = name "\\." name;

	# some names are referenced in macro definitions before their 
	# declarations appear, so we have to preload a few member entries
	members["yy_more"];	# covers yy_more_len and yy_more_flag
	members["yy_n_chars"] = "\\.yy_n_chars";

	state_decl = "flex::lexer_stateREFERENCE " name;
	# also want to make sure it doesn't match any exceptions
	# local["yy_c_buf_p_offset"] = "";
}

function comment_out(str) {
	print "// " str " // removed by " script_name;
	print "/* now referenced as: " name "." extract_yy_identifier(str) " */";
}

# return substring starting with yy consisteing of identifier characters
function extract_yy_identifier(str, 
	# local vars
	ind) {
	ind = match(str, "yy[A-Za-z_0-9]+");
	return substr(str, ind, RLENGTH);
}

function substitute_members(str) {
	for (m in members) {
		except = members[m];
		if (!length(except) || !match(str, except)) {
			gsub(m, name ".&", str);
		}
	}
	return str;
}

{
	# NOTE: be careful to avoid the member members of yy_buffer_state.
	# typically, those are found indented, so we only match 
	# members that start at the beginning-of-line.  
	if (match($0, "^extern int yyleng;")) {
		# members[extract_yy_identifier($0)] = "";
		members["yyleng"] = "";
		comment_out($0);
	} else if (match($0, "FILE.*[*]yyin.*[*]yyout")) {
		members["yyin"] = "yyinp";	# must not match yyinput!
		members["yyout"] = "";
		comment_out($0);
	} else if (match($0, "^static YY_BUFFER_STATE yy_current_buffer.*;")) {
		# members[extract_yy_identifier($0)] = "";
		members["yy_current_buffer"] = "";
		comment_out($0);
		# this would be a good place to include the state file
		print "#include \"lexer/flex_lexer_state.h\"" \
			"  /* injected by " script_name " */";
	} else if (match($0, "^static char yy_hold_char;")) {
		# members[extract_yy_identifier($0)] = "";
		members["yy_hold_char"] = "";
		comment_out($0);
	} else if (match($0, "->yy_n_chars")) {
		# to disambiguate between member and non-members
		gsub("->yy_n_chars", "->YY_N_CHARS", $0);
		gsub("yy_n_chars", name ".&", $0);
		gsub("yy_current_buffer", name ".&", $0);
		# print substitute_members($0);
		print;
	} else if (match($0, "^static int yy_n_chars;")) {
		# don't match the members of yy_buffer_state
		# members[extract_yy_identifier($0)] = "";
		# members["yy_n_chars"] = "";
		comment_out($0);
	} else if (match($0, "int yy_n_chars;")) {
		# not the static declaration, but the struct member declaration
		gsub("yy_n_chars", "YY_N_CHARS", $0);
		print;
	} else if (match($0, "^int yyleng;")) {
		# members[extract_yy_identifier($0)] = "";
		members["yyleng"] = "";
		comment_out($0);
	} else if (match($0, "^static char [*]yy_c_buf_p")) {
		# but don't want to match yy_c_buf_p_offset
		# members[extract_yy_identifier($0)] = "";
		members["yy_c_buf_p"] = "yy_c_buf_p_";
		comment_out($0);
	} else if (match($0, "^static int yy_init")) {
		# members[extract_yy_identifier($0)] = "";
		members["yy_init"] = "yy_init_";
		comment_out($0);
	} else if (match($0, "^static int yy_start =")) {
		# members[extract_yy_identifier($0)] = "";
		members["yy_start"] = "";
		comment_out($0);
	} else if (match($0, "^static int yy_did_buffer_switch_on_eof;")) {
		# members[extract_yy_identifier($0)] = "";
		members["yy_did_buffer_switch_on_eof"] = "";
		comment_out($0);
	} else if (match($0, "char [*]yytext;")) {
		# members[extract_yy_identifier($0)] = "";
		members["yytext"] = "";
		comment_out($0);
	} else if (match($0, "^static yy_state_type yy_last_accepting_state")) {
		# members[extract_yy_identifier($0)] = "";
		members["yy_last_accepting_state"] = "";
		comment_out($0);
	} else if (match($0, "^static char [*]yy_last_accepting_cpos;")) {
		# members[extract_yy_identifier($0)] = "";
		members["yy_last_accepting_cpos"] = "";
		comment_out($0);
	} else if (match($0, "^static int yy_more_flag")) {
		# members[extract_yy_identifier($0)] = "";
		# members["yy_more_flag"] = "";		# covered by prefix
		comment_out($0);
	} else if (match($0, "^static int yy_more_len")) {
		# members[extract_yy_identifier($0)] = "";
		# members["yy_more_len"] = "";		# covered by prefix
		comment_out($0);
	# only if YY_STACK_USED
	} else if (match($0, "^static int yy_start_stack_ptr")) {
		# covered by "yy_start" already as a prefix
		# members["yy_start_stack_ptr"] = "";
		comment_out($0);
	} else if (match($0, "^static int yy_start_stack_depth")) {
		# members["yy_start_stack_depth"] = "";
		comment_out($0);
	} else if (match($0, "^static int [*]yy_start_stack")) {
		# members["yy_start_stack"] = "";
		comment_out($0);

	# bunch of special cases I can't otherwise catch easily :(
	# exception hack patches go here
	} else if (match($0, "define YY_CURRENT_BUFFER yy_current_buffer")) {
		gsub("yy", name ".&", $0);
		print;
	} else if (match($0, "define yymore")) {
		# macro definition
		gsub("yy_more", name ".&", $0);
		print;
	} else if (match($0, "define YY_MORE_ADJ")) {
		# macro definition
		gsub("yy_more_len", name ".&", $0);
		print;
	} else if (match($0, "define.*yy_start")) {
		# macro definition
		gsub("yy_start", name ".&", $0);
		print;
	} else if (match($0, "define.*ECHO")) {
		# macro definition
		gsub("yy", name ".&", $0);
		print;
	} else if (match($0, "define.*YY_NEW_FILE")) {
		# macro definition
		gsub("yyin", name ".&", $0);
		gsub("\\)", ", " name "&", $0);
		print;
	} else if (match($0, "yy_c_buf_p =.*_p_offset")) {
		# first instance only on this line
		sub("yy_c_buf_p", name ".&", $0);
		print;
	} else {
	# BIG ELSE

	if (match($0, "static int yy_get_next_buffer(.*)")) {
		# need to rewrite prototype of this function
		if (match($0, "PROTO")) {
			gsub("\\(.*\\)", "((" state_decl "))", $0);
		} else {
			gsub("\\(.*\\)", "(" state_decl ")", $0);
		}
	} else if (match($0, "yy_get_next_buffer()")) {
		# rewrite this function call
		gsub("buffer\\(.*\\)[^)]", "buffer(" name ")", $0);
	} else if (match($0, "static yy_state_type yy_get_previous_state(.*)")) {
		# need to rewrite prototype of this function
		if (match($0, "PROTO")) {
			gsub("\\(.*\\)", "((" state_decl "))", $0);
		} else {
			gsub("\\(.*\\)", "(" state_decl ")", $0);
		}
	} else if (match($0, "yy_get_previous_state()")) {
		# rewrite this function call
		gsub("\\(.*\\)", "(" name ")", $0);
	} else if (match($0, "static yy_state_type yy_try_NUL_trans(.*)")) {
		# don't worry about KNR prototypes
		if (match($0, "PROTO")) {
			gsub("\\)\\)", ", " state_decl "&", $0);
		} else {
			gsub("\\)", ", " state_decl "&", $0);
		}
	} else if (match($0, "yy_try_NUL_trans(.*)")) {
		# match function call
		gsub("\\)", ", " name "&", $0);
	} else if (match($0, "static void yyunput(.*)")) {
		if (match($0, "PROTO")) {
			gsub("\\)\\)", ", " state_decl "&", $0);
		} else {
			gsub("\\)", ", " state_decl "&", $0);
		}
	} else if (match($0, "yyunput(.*)")) {
		# match function call
		gsub("\\)", ", " name "&", $0);
	} else if (match($0, "static int yyinput(.*)")) {
		if (match($0, "PROTO")) {
			gsub("\\(.*\\)", "((" state_decl "))", $0);
		} else {
			gsub("\\)", state_decl "&", $0);
		}
	} else if (match($0, "yyinput(.*)")) {
		# match function call
		gsub("\\)", name "&", $0);
	} else if (match($0, "void yyrestart(.*)")) {
		if (match($0, "PROTO")) {
			gsub("\\)\\)", ", " state_decl "&", $0);
		} else {
			gsub("\\)", ", " state_decl "&", $0);
		}
	} else if (match($0, "yyrestart(.*)")) {
		# match function call
		gsub("\\)", ", " name "&", $0);
	} else if (match($0, "void yy_switch_to_buffer(.*)")) {
		if (match($0, "PROTO")) {
			gsub("\\)\\)", ", " state_decl "&", $0);
		} else {
			gsub("\\)", ", " state_decl "&", $0);
		}
	} else if (match($0, "yy_switch_to_buffer(.*)")) {
		# match function call
		gsub("\\)", ", " name "&", $0);
	} else if (match($0, "void yy_load_buffer_state(.*)")) {
		if (match($0, "PROTO")) {
			gsub("\\(.*\\)", "((" state_decl "))", $0);
		} else {
			gsub("\\(.*\\)", "(" state_decl ")", $0);
		}
	} else if (match($0, "yy_load_buffer_state(.*)")) {
		# match function call
		gsub("\\(.*\\)", "(" name ")", $0);
	} else if (match($0, "YY_BUFFER_STATE yy_create_buffer(.*)")) {
		if (match($0, "PROTO")) {
			gsub("\\)\\)", ", " state_decl "&", $0);
		} else {
			gsub("\\)", ", " state_decl "&", $0);
		}
	} else if (match($0, "yy_create_buffer(.*)") && !match($0, "ERROR")) {
		# careful not to match inside string...
		# match function call
		gsub("\\)", ", " name "&", $0);
	} else if (match($0, "void yy_delete_buffer(.*)")) {
		if (match($0, "PROTO")) {
			gsub("\\)\\)", ", " state_decl "&", $0);
		} else {
			gsub("\\)", ", " state_decl "&", $0);
		}
	} else if (match($0, "yy_delete_buffer(.*)")) {
		# match function call
		gsub("\\)", ", " name "&", $0);
	} else if (match($0, "void yy_init_buffer(.*)")) {
		if (match($0, "PROTO")) {
			gsub("\\)\\)", ", " state_decl "&", $0);
		} else {
			gsub("\\)", ", " state_decl "&", $0);
		}
	} else if (match($0, "yy_init_buffer(.*)")) {
		# match function call
		gsub("\\)", ", " name "&", $0);
	} else if (match($0, "void yy_flush_buffer(.*)")) {
		if (match($0, "PROTO")) {
			gsub("\\)\\)", ", " state_decl "&", $0);
		} else {
			gsub("\\)", ", " state_decl "&", $0);
		}
	} else if (match($0, "yy_flush_buffer(.*)")) {
		# match function call
		gsub("\\)", ", " name "&", $0);
	} else if (match($0, "YY_BUFFER_STATE yy_scan_buffer(.*)")) {
		if (match($0, "PROTO")) {
			gsub("\\)\\)", ", " state_decl "&", $0);
		} else {
			gsub("\\)", ", " state_decl "&", $0);
		}
	} else if (match($0, "yy_scan_buffer(.*)") && !match($0, "ERROR")) {
		# match function call
		gsub("\\)", ", " name "&", $0);
	} else if (match($0, "YY_BUFFER_STATE yy_scan_string(.*)")) {
		if (match($0, "PROTO")) {
			gsub("\\)\\)", ", " state_decl "&", $0);
		} else {
			gsub("\\)", ", " state_decl "&", $0);
		}
	} else if (match($0, "yy_scan_string(.*)") && !match($0, "ERROR")) {
		# match function call
		gsub("\\)", ", " name "&", $0);
	} else if (match($0, "YY_BUFFER_STATE yy_scan_bytes(.*)")) {
		if (match($0, "PROTO")) {
			gsub("\\)\\)", ", " state_decl "&", $0);
		} else {
			gsub("\\)", ", " state_decl "&", $0);
		}
	} else if (match($0, "yy_scan_bytes(.*)") && !match($0, "ERROR")) {
		# match function call
		gsub("\\)", ", " name "&", $0);
	} else if (match($0, "static void yy_push_state(.*)")) {
		if (match($0, "PROTO")) {
			gsub("\\)\\)", ", " state_decl "&", $0);
		} else {
			gsub("\\)", ", " state_decl "&", $0);
		}
	} else if (match($0, "yy_push_state(.*)")) {
		# match function call
		gsub("\\)", ", " name "&", $0);
	} else if (match($0, "static void yy_pop_state(.*)")) {
		if (match($0, "PROTO")) {
			gsub("\\)\\)", state_decl "&", $0);
		} else {
			gsub("\\)", state_decl "&", $0);
		}
	} else if (match($0, "yy_pop_state(.*)")) {
		# match function call
		gsub("\\)", name "&", $0);
	} else if (match($0, "static int yy_top_state(.*)")) {
		if (match($0, "PROTO")) {
			gsub("\\)\\)", state_decl "&", $0);
		} else {
			gsub("\\)", state_decl "&", $0);
		}
	} else if (match($0, "yy_top_state(.*)")) {
		# match function call
		gsub("\\)", name "&", $0);
	}
	str = $0;
	# ignore .yy, and ->yy
	if (match(str,"[^.>]yy")) {
	if (!match(str,"PROTO")) {
		if (!match(str,"define") && !match(str,"ifndef")) {
			str = substitute_members(str);
			# undo accidents
			# gsub("[.]" name, ".", str);
			# gsub("[-][>]" name, "->", str);
		} 
		# special case hack
#		if (match(str, "yy_n_chars")) {
#		if (match(str, "[=]")) {
#			sub(" yy_n_chars" , " " name ".yy_n_chars", str);
#		}
#		}
	}
	}
	print str;
	}
}

