#!/usr/bin/awk -f
# "yacc-prefix.h"

#	To get the effect of -p prefix with yacc or bison, 
#	When compiling the source, pass -v PREFIX=name 
#	to change the names.  
#	The effect of this is symbol-renaming without file-renaming.  
#	This should be a near-comprehensive list of public symbols
#	that appear in both lex and yacc generated sources.
#	Note that static variables and functions and macros are not
#	part of the external interface of a module and need not be
#	transformed, although there is no harm in doing so.  

BEGIN {
	if (!length(PREFIX)) {
		op = 0;				# don't do anything
	} else {
		op = 1;
	}

	roots["tokentype"] = 1;
	roots["overflow"] = 1;			# is a macro?
	roots["alloc"] = 1;
#	roots["translate"] = 1;			# static
#	roots["prhs"] = 1;			# static
#	roots["rhs"] = 1;			# static
#	roots["rline"] = 1;			# static
#	roots["tname"] = 1;			# static
#	roots["toknum"] = 1;			# static
#	roots["errstatus"] = 1;			# local
#	roots["token"] = 1;			# local
#	roots["_stack_print"] = 1;		# static
#	roots["_reduce_print"] = 1;		# static
#	roots["destruct"] = 1;			# static

	roots["maxdepth"] = 1;
	roots["parse"] = 1;
	roots["lex"] = 1;
	roots["error"] = 1;
	roots["lval"] = 1;
	roots["char"] = 1;
	roots["debug"] = 1;
	roots["pact"] = 1;
	roots["r1"] = 1;
	roots["r2"] = 1;
	roots["def"] = 1;
	roots["chk"] = 1;
	roots["pgo"] = 1;
	roots["act"] = 1;
	roots["exca"] = 1;
	roots["errflag"] = 1;
	roots["nerrs"] = 1;

	roots["ps"] = 1;
	roots["pv"] = 1;
	roots["s"] = 1;
	roots["v"] = 1;

	roots["yys"] = 1;
	roots["state"] = 1;
	roots["tmp"] = 1;
	roots["_yyv"] = 1;
	roots["val"] = 1;
	roots["lloc"] = 1;
	roots["reds"] = 1;
	roots["toks"] = 1;
	roots["lhs"] = 1;
	roots["len"] = 1;
	roots["defred"] = 1;
	roots["dgoto"] = 1;
	roots["sindex"] = 1;
	roots["rindex"] = 1;
	roots["gindex"] = 1;
	roots["table"] = 1;
	roots["check"] = 1;
	roots["name"] = 1;
	roots["rule"] = 1;

	roots["ss"] = 1;
	roots["vs"] = 1;
	roots["ssp"] = 1;
	roots["vsp"] = 1;

# more symbols from [f]lex generated lexer code
#	roots["_start"] = 1;		# macro to static int
	roots["restart"] = 1;		# void yyrestart(FILE*)
	roots["in"] = 1;		# FILE*
	roots["out"] = 1;		# FILE*
#	roots["yyconst"] = 1;		# macro
#	roots["yyless"] = 1;		# macro
	roots["text"] = 1;		# char*
#	roots["_size_t"] = 1;		# fixed
#	roots["_buffer_state"] = 1;	# struct, fixed
#	roots["_current_buffer"] = 1;	# static
#	roots["_switch_to_buffer"] = 1;	# fixed
#	roots["_create_buffer"] = 1;	# fixed
#	roots["_init_buffer"] = 1;	# fixed
#	roots["_flush_buffer"] = 1;	# fixed
#	roots["_scan_buffer"] = 1;	# fixed
#	roots["_scan_string"] = 1;	# fixed
#	roots["_scan_bytes"] = 1;	# fixed
#	roots["_flex_alloc"] = 1;	# static
#	roots["_flex_realloc"] = 1;	# static
#	roots["_flex_free"] = 1;	# static
#	roots["_new_buffer"] = 1;	# macro
#	roots["_set_interactive"] = 1;	# macro
#	roots["_set_bol"] = 1;		# macro
#	roots["_state_type"] = 1;	# fixed typedef
#	MANY more static variables and functions that need not be changed...

	roots["wrap"] = 1;		# per lexer
#	roots["unput"] = 1;		# static
#	roots["terminate"] = 1;		# macro

}

/yy/ && op {
	str = $0;
	l = 9;		# length of longest root token
	# go from longest to shortest
	for ( ; l > 0; l--) {
		for (r in roots) {
			if (length(r) == l)
				str = yacc_prefix_sub(str, r);
			# else skip
		}
	}
	print str;
}

!op || !/yy/ {
	print;
}

function yacc_prefix_sub(str, root) {
	gsub("yy" root, PREFIX root, str);
	return str;
}

# END { }

