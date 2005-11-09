#!/usr/bin/awk -f
# "bison-union-type.awk"
# David Fang, 2004
#	$Id: bison-union-type.awk,v 1.5.48.1 2005/11/09 08:24:01 fang Exp $

# CO-DEPENDENT ON:
# parser/common-union-type.awk
# CONFLICTS WITH:
# parser/yacc-union-type.awk

# DISCLAIMER:
# Not guaranteed to work on traditional yacc output, 
# only tested for bison -y output.  
# See "yacc-union-type.awk".  

BEGIN {
	# user-defined token symbols are enumerater from 258 and higher
	# NOTE: this is different from yacc, which starts at 257...
	# Only God knows why.  
	# To make matters worse, old enough versions of bison start at 257...
	# We need to auto-detect...
	# define a MINIMUM_BOGOSITY token *first* in yacc file, get its num.  
	# e.g. %token MINIMUM_BOGOSITY	/* fake token */
	# grep MINIMUM_BOGOSITY y.tab.h | head -n1 | cut -d\  -f 3
	# pass it into this script as -v token_enum to override guess

	shift_regex = "shift, and go";
}

function main_program_name() {
	return "bison-union-type.awk";
}

function handle_default_token_enum() {
	if (!length(token_enum))
		token_enum = 256 +2;	# default
}

function register_end_symbols() {
	# need a special case for the "$end" symbol (bison-1.875)
	# need a special case for the "$" symbol (bison-1.35)
	symbol_type["$end"] = "TOTALLY_BOGUS";		# bogus union member
	symbol_type["$"] = "TOTALLY_BOGUS";		# bogus union member
	enum_of["TOTALLY_BOGUS"] = -1;			# end condition
}

# main pattern matching body
/^state [0-9]*/ {
	print "/************************** " $0 " **************************/";
	# state count
	sc = 0;
	first_blank = 0;

	while (getline) {
	if (!length($0)) {
		if (first_blank) break;
			# second blank line in a row
		else	first_blank = 1;
	} else {
		first_blank = 0;	# reset
		if (match($0, "go to state")) {
			match_shift = match($0, shift_regex);
# struct-style assignment is deprecated, use constructor instead.
			printf("static const yy_state_map_link yysml_" \
				state_count "_" sc " = { ");
			if (match_shift) {
				printf($7);
			} else {	# is just a goto
				printf($5);
			}
			# work around for goto actions in the middle of a rule
			enum_sym = enum_of[symbol_type[$1]];
			if (!length(enum_sym)) {
				# is blank, b/c no type associated
				enum_sym = "-1";
			}
			printf(", " enum_sym ", ");

			if (sc) printf("&yysml_" state_count "_" sc-1);
			else	printf("NULL");		# or 0

			printf(" }; ");
			if (match_shift) {
				print "/* shift */";
			} else {
				print "/* goto */";
			}
			sc++;
		}
		# else just continue
	}	# end if
	}	# end while

	print "";
	shift_count[state_count] = sc;
	state_count++;
}

