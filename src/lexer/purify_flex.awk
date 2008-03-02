#!/usr/bin/awk -f
# "purify_flex.awk"
#	$Id: purify_flex.awk,v 1.9.10.1 2008/03/02 22:38:28 fang Exp $
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
	# NOTE: delete <array> is a gawk extension, and not POSIX
	# delete members;
	for (m in members) delete members[m];
	
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

function substitute_members(str,
	# local vars
	except) {
	for (m in members) {
		except = members[m];
		if (!length(except) || !match(str, except)) {
			gsub(m, name ".&", str);
		}
	}
	return str;
}


function replace_proto_params(str, decl) {
	if (match(str, "PROTO")) {
		gsub("\\(.*\\)", "((" decl "))", str);
	} else {
		gsub("\\(.*\\)", "(" decl ")", str);
	}
	return str;
}

function append_proto_params(str, decl) {
	if (match(str, "PROTO")) {
		gsub("\\)\\)", ", " decl "&", str);
	} else {
		gsub("\\)", ", " decl "&", str);
	}
	return str;
}

# to work, there must be only one set or parentheses on this line.  
# useful for replacing a 'void' parameter
function replace_call_args(str, arg) {
	gsub("\\(.*\\)", "(" arg ")", str);
	return str;
}

# to work, there must be only one set or parentheses on this line.  
function append_call_args(str, arg) {
	gsub("\\)", ", " arg "&", str);
	return str;
}

# for all lines...
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
		# print "#include \"lexer/flex_lexer_state.h\"" \
		# instead, we print a forward declaration now
		print "namespace flex { class lexer_state; }";
		print "\t/* injected by " script_name " */";
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
	} else if (match($0, "^static int yy_init[^()]+$")) {
		# careful not to catch yy_init_globals() from flex 2.5.33!
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
	} else if (match($0, "int yylineno")) {
		# members[extract_yy_identifier($0)] = "";
		members["yylineno"] = "";
		comment_out($0);
	} else if (match($0, "^static size_t yy_buffer_stack_top")) {
		# flex 2.5.31
		members["yy_buffer_stack"] = "";
		comment_out($0);
	} else if (match($0, "^static size_t yy_buffer_stack_max")) {
		# flex 2.5.31
		members["yy_buffer_stack"] = "";
		comment_out($0);
	} else if (match($0, "^static YY_BUFFER_STATE [*] yy_buffer_stack")) {
		# flex 2.5.31
		members["yy_buffer_stack"] = "";
		comment_out($0);

	# bunch of special cases I can't otherwise catch easily :(
	# exception hack patches go here
	} else if (match($0, "define YY_CURRENT_BUFFER yy_current_buffer")) {
		# flex 2.5.4
		gsub("yy", name ".&", $0);
		print;
	} else if (match($0, "define YY_AT_BOL.*yy_current_buffer")) {
		# flex 2.5.4
		gsub("yy_current_buffer", name ".&", $0);
		# to avoid substituting the ->yy_at_bol
		print;
	} else if (match($0, "define YY_CURRENT_BUFFER.*yy_buffer_stack")) {
		# flex 2.5.31
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
	} else if (match($0, "yy_c_buf_p =.*_p_offset") ||
			# flex-2.5.31 parenthesizes :S
			match($0, "\\(yy_c_buf_p\\) =.*_p_offset")) {
		# first instance only on this line
		sub("yy_c_buf_p", name ".&", $0);
		print;
	} else {
	# BIG ELSE

	if (match($0, "int yy_get_next_buffer(.*)")) {
		# is originally static linkage
		# need to rewrite prototype of this function
		$0 = replace_proto_params($0, state_decl);
	} else if (match($0, "yy_get_next_buffer\\([ ]*\\)")) {
		# rewrite this function call
		gsub("buffer\\(.*\\)[^)]", "buffer(" name ")", $0);
	} else if (match($0, "static yy_state_type yy_get_previous_state(.*)")) {
		# need to rewrite prototype of this function
		$0 = replace_proto_params($0, state_decl);
	} else if (match($0, "yy_get_previous_state\\([ ]*\\)")) {
		# rewrite this function call
		$0 = replace_call_args($0, name);
	} else if (match($0, "static yy_state_type yy_try_NUL_trans(.*)")) {
		# don't worry about KNR prototypes
		$0 = append_proto_params($0, state_decl);
	} else if (match($0, "yy_try_NUL_trans(.*)")) {
		# match function call
		gsub("\\)", ", " name "&", $0);
	} else if (match($0, "static void yyunput(.*)")) {
		# $0 = replace_proto_params($0, state_decl);
		$0 = append_proto_params($0, state_decl);	# 2.5.31
	} else if (match($0, "yyunput(.*)")) {
		# match function call
		$0 = append_call_args($0, name);
	} else if (match($0, "static int yyinput(.*)")) {
		$0 = replace_proto_params($0, state_decl);
		# $0 = append_proto_params($0, state_decl);
	} else if (match($0, "yyinput(.*)")) {
		$0 = replace_call_args($0, name);
	} else if (match($0, "void yyrestart(.*)")) {
		$0 = append_proto_params($0, state_decl);
	} else if (match($0, "yyrestart(.*)")) {
		$0 = append_call_args($0, name);
	} else if (match($0, "void yy_switch_to_buffer(.*)")) {
		$0 = append_proto_params($0, state_decl);
	} else if (match($0, "yy_switch_to_buffer(.*)")) {
		$0 = append_call_args($0, name);
	} else if (match($0, "void yy_load_buffer_state(.*)")) {
		$0 = replace_proto_params($0, state_decl);
	} else if (match($0, "yy_load_buffer_state(.*)")) {
		$0 = replace_call_args($0, name);
	} else if (match($0, "YY_BUFFER_STATE yy_create_buffer(.*)")) {
		$0 = append_proto_params($0, state_decl);
	} else if (match($0, "yy_create_buffer(.*)") && !match($0, "ERROR")) {
		# careful not to match inside string...
		$0 = append_call_args($0, name);
	} else if (match($0, "void yy_delete_buffer(.*)")) {
		$0 = append_proto_params($0, state_decl);
	} else if (match($0, "yy_delete_buffer(.*)")) {
		$0 = append_call_args($0, name);
	} else if (match($0, "void yy_init_buffer(.*)")) {
		$0 = append_proto_params($0, state_decl);
	} else if (match($0, "yy_init_buffer(.*)")) {
		$0 = append_call_args($0, name);
	} else if (match($0, "void yy_flush_buffer(.*)")) {
		$0 = append_proto_params($0, state_decl);
	} else if (match($0, "yy_flush_buffer(.*)")) {
		$0 = append_call_args($0, name);
	} else if (match($0, "YY_BUFFER_STATE yy_scan_buffer(.*)")) {
		$0 = append_proto_params($0, state_decl);
	} else if (match($0, "yy_scan_buffer(.*)") && !match($0, "ERROR")) {
		$0 = append_call_args($0, name);
	} else if (match($0, "YY_BUFFER_STATE yy_scan_string(.*)")) {
		$0 = append_proto_params($0, state_decl);
	} else if (match($0, "yy_scan_string(.*)") && !match($0, "ERROR")) {
		$0 = append_call_args($0, name);
	} else if (match($0, "YY_BUFFER_STATE yy_scan_bytes(.*)")) {
		$0 = append_proto_params($0, state_decl);
	} else if (match($0, "yy_scan_bytes(.*)") && !match($0, "ERROR")) {
		# This is why we need real parsing and syntax tree rewriting.
		# match function call
		if (match($0, "strlen")) {
			# beware of embedded strlen in flex 2.5.31
			regex = "strlen\\([^()]+\\)";
			# goddamn mawk rejects: lvalue = match(...);
			# forcing us to use RSTART
			match($0, regex);
			# print "ind = " ind ", RL = " RLENGTH;
			# swapout = substr($0, ind, RLENGTH -1);
			swapout = substr ($0, RSTART, RLENGTH);
			# flex 2.5.31 only
			# sub(regex, "XXX )", $0);
			sub(regex, "XXX", $0);
			$0 = append_call_args($0, name);
			sub("XXX", swapout, $0);
		} else {
			$0 = append_call_args($0, name);
		}
		# gsub("\\)", ", " name "&", $0);
	} else if (match($0, "static void yy_push_state(.*)")) {
		$0 = append_proto_params($0, state_decl);
	} else if (match($0, "yy_push_state(.*)")) {
		$0 = append_call_args($0, name);
	} else if (match($0, "static void yy_pop_state(.*)")) {
		$0 = append_proto_params($0, state_decl);
	} else if (match($0, "yy_pop_state(.*)")) {
		$0 = append_call_args($0, name);
	} else if (match($0, "static int yy_top_state(.*)")) {
		$0 = append_proto_params($0, state_decl);
	} else if (match($0, "yy_top_state(.*)")) {
		$0 = append_call_args($0, name);
	} else if (match($0, "void yyensure_buffer_stack(.*)")) {
		# (2.5.31 only)
		$0 = replace_proto_params($0, state_decl);
	} else if (match($0, "yyensure_buffer_stack(.*)")) {
		# (2.5.31 only)
		$0 = replace_call_args($0, name);
	} else if (match($0, "void yypush_buffer_state(.*)")) {
		# (2.5.31 only)
		$0 = append_proto_params($0, state_decl);
	} else if (match($0, "yypush_buffer_state(.*)")) {
		# (2.5.31 only)
		$0 = append_call_args($0, name);
	} else if (match($0, "void yypop_buffer_state(.*)")) {
		# (2.5.31 only)
		$0 = replace_proto_params($0, state_decl);
	} else if (match($0, "yypop_buffer_state(.*)")) {
		# (2.5.31 only)
		$0 = replace_call_args($0, name);
	} else if (match($0, "int yyget_lineno(.*)")) {
		# (2.5.31 only)
		$0 = replace_proto_params($0, state_decl);
	} else if (match($0, "yyget_lineno(.*)")) {
		# (2.5.31 only)
		$0 = replace_call_args($0, name);
	} else if (match($0, "int yyget_leng(.*)")) {
		# (2.5.31 only)
		$0 = replace_proto_params($0, state_decl);
	} else if (match($0, "yyget_leng(.*)")) {
		# (2.5.31 only)
		$0 = replace_call_args($0, name);
	} else if (match($0, "int yyget_debug(.*)")) {
		# (2.5.31 only)
		$0 = replace_proto_params($0, state_decl);
	} else if (match($0, "yyget_debug(.*)")) {
		# (2.5.31 only)
		$0 = replace_call_args($0, name);
	} else if (match($0, "FILE [*]yyget_in(.*)")) {
		# (2.5.31 only)
		$0 = replace_proto_params($0, state_decl);
	} else if (match($0, "yyget_in(.*)")) {
		# (2.5.31 only)
		$0 = replace_call_args($0, name);
	} else if (match($0, "FILE [*]yyget_out(.*)")) {
		# (2.5.31 only)
		$0 = replace_proto_params($0, state_decl);
	} else if (match($0, "yyget_out(.*)")) {
		# (2.5.31 only)
		$0 = replace_call_args($0, name);
	} else if (match($0, "char [*]yyget_text(.*)")) {
		# (2.5.31 only)
		$0 = replace_proto_params($0, state_decl);
	} else if (match($0, "yyget_text(.*)")) {
		# (2.5.31 only)
		$0 = replace_call_args($0, name);
	} else if (match($0, "void yyset_in(.*)")) {
		# (2.5.31 only)
		$0 = append_proto_params($0, state_decl);
	} else if (match($0, "yyset_in(.*)")) {
		# (2.5.31 only)
		$0 = append_call_args($0, name);
	} else if (match($0, "void yyset_out(.*)")) {
		# (2.5.31 only)
		$0 = append_proto_params($0, state_decl);
	} else if (match($0, "yyset_out(.*)")) {
		# (2.5.31 only)
		$0 = append_call_args($0, name);
	} else if (match($0, "void yyset_lineno(.*)")) {
		# (2.5.31 only)
		$0 = append_proto_params($0, state_decl);
	} else if (match($0, "yyset_lineno(.*)")) {
		# (2.5.31 only)
		$0 = append_call_args($0, name);
	} else if (match($0, "void yyset_debug(.*)")) {
		# (2.5.31 only)
		$0 = append_proto_params($0, state_decl);
	} else if (match($0, "yyset_debug(.*)")) {
		# (2.5.31 only)
		$0 = append_call_args($0, name);
	} else if (match($0, "int yylex_destroy(.*)")) {
		# (2.5.31 only)
		$0 = replace_proto_params($0, state_decl);
	} else if (match($0, "yylex_destroy(.*)")) {
		# (2.5.31 only)
		$0 = replace_call_args($0, name);
	} else if (match($0, "static int yy_init_globals(.*)")) {
		# (2.5.33 only)
		$0 = replace_proto_params($0, state_decl);
	} else if (match($0, "yy_init_globals(.*)")) {
		# (2.5.33 only)
		$0 = replace_call_args($0, name);
	}
	str = $0;
	# ignore .yy, and ->yy
	if (match(str,"[^.>]yy")) {
	if (!match(str,"PROTO")) {
		if (!match(str,"define") && !match(str,"ifndef") && \
				!match(str,"undef")) {
			str = substitute_members(str);
		} 
	}
	}
	print str;
	}
}

