#!/usr/bin/awk -f
#	$Id: lexyacc-prefix-generator.awk,v 1.1.2.2 2006/01/16 06:58:59 fang Exp $
# "lexyacc-prefix-generator.awk"
#
# generates a header suitable for preprocessing and performing
# token substitution on lex/yacc-generated source files.  
# takes no input, pass < /dev/null

BEGIN {
	if (!length(PREFIX)) {
		print "Missing argument: -v prefix=<prefix>";
		exit 1;
	}
	header_str = "__LEXYACC_" toupper(PREFIX) "_PREFIX_H__";
	print "#ifndef " header_str;
	print "#define " header_str;

	# list of roots here
	roots["tokentype"];
#	roots["overflow"];			# is a macro? yes in bison
	roots["alloc"];
#	roots["translate"];			# static
#	roots["prhs"];				# static
#	roots["rhs"];				# static
#	roots["rline"];				# static
#	roots["tname"];				# static
#	roots["toknum"];			# static
#	roots["errstatus"];			# local
#	roots["token"];				# local
#	roots["_stack_print"];			# static
#	roots["_reduce_print"];			# static
#	roots["destruct"];			# static

	roots["maxdepth"];
	roots["parse"];
	roots["lex"];
	roots["error"];
	roots["lval"];
	roots["char"];
	roots["debug"];
	roots["defact"];
	roots["pact"];
	roots["r1"];
	roots["r2"];
	roots["def"];
	roots["chk"];
	roots["pgo"];
	roots["act"];
	roots["exca"];
	roots["errflag"];
	roots["nerrs"];

	roots["ps"];
	roots["pv"];
	roots["s"];
	roots["v"];

	roots["_yys"];
	roots["state"];
	roots["tmp"];
	roots["_yyv"];
	roots["val"];
	roots["lloc"];
	roots["reds"];
	roots["toks"];
	roots["lhs"];
	roots["len"];
	roots["defred"];
	roots["dgoto"];
	roots["sindex"];
	roots["rindex"];
	roots["gindex"];
	roots["table"];
	roots["check"];
	roots["name"];
	roots["rule"];

	roots["ss"];
	roots["vs"];
	roots["ssp"];
	roots["vsp"];

# more symbols from [f]lex generated lexer code
#	roots["_start"];		# macro to static int
	roots["restart"];		# void yyrestart(FILE*) (bison)
	roots["in"];			# FILE*
	roots["out"];			# FILE*
	roots["lineno"];		# int
#	roots["yyconst"];		# macro
#	roots["yyless"];		# macro
	roots["text"];			# char*
#	roots["_size_t"];		# fixed
#	roots["_buffer_state"];		# struct, fixed
	roots["_current_buffer"];	# static
	roots["_switch_to_buffer"];	# public
	roots["_create_buffer"];	# public (bison)
	roots["_delete_buffer"];	# public
	roots["_init_buffer"];		# public (bison)
	roots["_flush_buffer"];		# public (bison)
	roots["_load_buffer_state"];	# public (bison)
	roots["_scan_buffer"];		# public
	roots["_scan_string"];		# public (bison)
	roots["_scan_bytes"];		# public (bison)
#	roots["_flex_alloc"];		# static
#	roots["_flex_realloc"];		# static
#	roots["_flex_free"];		# static
#	roots["_new_buffer"];		# macro
#	roots["_set_interactive"];	# macro
#	roots["_set_bol"];		# macro
#	roots["_state_type"];		# fixed typedef
#	MANY more static variables and functions that need not be changed...

	roots["wrap"];			# per lexer
#	roots["unput"];			# static
#	roots["terminate"];		# macro

	for (r in roots) {
		print "#define yy" r "\t\t" PREFIX r;
	}

	print "#endif\t/* " header_str "*/";
}

