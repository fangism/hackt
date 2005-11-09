#!/usr/bin/awk -f
# "yacc-union-type.awk"
# David Fang, 2004
#	$Id: yacc-union-type.awk,v 1.5.48.1 2005/11/09 08:24:01 fang Exp $

# CO-DEPENDENT ON:
# parser/common-union-type.awk
# CONFLICTS WITH:
# parser/bison-union-type.awk

# DISCLAIMER:
# Not guaranteed to work on 'bison -y' output, 
# only tested for traditional yacc.
# See "bison-union-type.awk"

function main_program_name() {
	return "yacc-union-type.awk";
}

function handle_default_token_enum() {
	# no-op
}

function register_end_symbols() {
	# no-op
}

# main pattern matching body
/^state [0-9]*/ {
	print "/************************** " $0 " **************************/";
	# state count
	sc = 0;
	while (getline && length($0)) {
		sc = 0;
		# do nothing, ignore production rule states
	}
	while (getline) {
		# collect shift actions
	if (NF == 3) {
	if ($2 == "shift") {
# struct-style assignment is deprecated, use constructor instead.
		printf("static const yy_state_map_link yysml_" state_count \
			"_" sc " = { " $3 ", " enum_of[symbol_type[$1]] ", ");
		if (sc) printf("&yysml_" state_count "_" sc-1);
		else	printf("NULL");		# or 0
		print " }; /* shift */";
		sc++;
	}	# else ignore reduce
	} else if (NF == 2) {
		if ($2 != "error" && $2 != "accept") {		# sanity check
			print "expected \"error\" or \"accept\", aborting";
			exit 1;
		}
		else {
			getline;		# munch blank line
			break;
		}
	} else {
		# probably blank line
		break;
	}
	}	# end while
	while (getline && NF == 3 && $2 == "goto") {
		# collect goto actions
# struct-style assignment is deprecated, use constructor instead.
		enum_sym = enum_of[symbol_type[$1]];
		if (!length(enum_sym)) {
			enum_sym = "-1";
		}
		printf("static const yy_state_map_link yysml_" state_count \
			"_" sc " = { " $3 ", " enum_sym ", ");
		if (sc) printf("&yysml_" state_count "_" sc-1);
		else	printf("NULL");		# or 0
		print " }; /* goto */";
		sc++;
	}
	print "";
	shift_count[state_count] = sc;
	state_count++;
}

