# "yacc-union-type.awk"

# the goal of this script is to produce:
# a C function that takes as arguments:
#	a union object
#	a pair of states (enumerated) representing a transition
#		between states of the parser
# and returns a pointer to the correct member
#	as a pointer to a super-class of all members

# suggetion:
# grep -v "#include" <yacc-file> | cpp -P | awk -f <this file> -v type=<base type>

# To accomplish this, we need to know what state the parser was in
# when a particular symbol was shifted onto the stack.  
# What we have available when the parser encounters a syntax error
# are the entire state stack and value stack.  
# Walking the state stack from bottom to top tells us the precise
# state stransitions.  
# Where do we look up union-member type given the state transition?
# It's not in y.tab.c because it has no need to go figure out what
# the type IS, only that types match where they are expected.  
# We leverage the "human-readable" output of yacc, namely the -v option
# which produces "y.output".  
# y.output conveniently contains the information about what symbols caused
# transitions from one state to the next.  
# We just have to translate them into a form that can be used by C.  
# more on this later...

# input variables:
# type = the super-base class of all union members (without pointer)
#	the yacc file alone doesn't know anything about classes
# yaccfile = the yacc input file, preferably preprocessed with the command:
#	grep -v "#include" <yacc-file> | cpp -P |
#		sed -e "/^%start/,$$$$d" -e "/%{/,/%}/d"
# include (optional)
# namespace (optional)

# criteria for the union type for yylval:
# all members of the union must be pointers to classes with a common
# parent class.  

BEGIN {
	go_union = 0;
	member_count = 0;
	state_count = 0;

	if (yaccfile == "") {
		print "\"yaccfile\" must be defined as an argument!";
		exit;
	} 
	if (type == "") {
		print "\"type\" must be defined as an argument!";
		exit;
	}

	nincs = split(include, inc_array);
	for (i=1; i<= nincs; i++) {
		print "#include \"" inc_array[i] "\"";
	}
	if (namespace) {
		print "using namespace " namespace ";";
	}
	print "#include \"y.tab.h\"";	# for YYSTYPE

	process_union(yaccfile);
	process_symbol_types(yaccfile);

	print "#ifndef NULL";
	print "#define NULL\t0";
	print "#endif";
	print "";

	print "typedef struct _yy_state_map_link_ yy_state_map_link;";
	print "struct _yy_state_map_link_ {";
        print "\tint state;\t\t/* state number to match */";
        print "\tint type_enum;\t\t/* enumerated type */";
        print "\tyy_state_map_link* next;";
	print "};";

	print type "* yy_union_resolve(YYSTYPE&, const short, const short);";

}

function process_union(file) {
if (!got_union) {
	do { getline < file; } while(!match($0, "union.*{"));
	# then we know we're in a union
	while (getline < file) {
	if (match($0, "[}]"))
		break;
	else if (NF >= 2) {
		member_type = $1;
		member_id = $2;
		gsub(";", "", member_id);
		# pointer handling, depends on here pointer is placed
		# only handles single pointers for now
		if (match(member_id,"[*]")) {
			gsub("[*]", "", member_id);
			member_type += "*";
		} else if (!match(member_type, "[*]")) {
			printf "union member must be a pointer!";
			exit;
		}
		type_of[member_id] = member_type;
#		print "union member " member_id " has type " member_type;
		enum_of[member_type] = member_count;
		member_id_array[member_count] = member_id;
		member_count++;
	}
	# else ignore line
	}	# end while
	got_union = 1;
}}

function process_symbol_types(file) {
if (got_union) {
	while(getline < file) {
	argc = split($0, type_args);
	if (match($1,"%token") || match($1,"%type")) {
	if (argc >= 3) {
		member_id = type_args[2];
		gsub("[<>]","", member_id);
		member_type = type_of[member_id];
#		print "found union member " member_id " with type " member_type;
		if (member_type == "") {
			print "union member \"" member_id "\" not found!";
			exit;
		}
		for (i=3; i<= argc; i++) {
			symbol_id = type_args[i];
			symbol_type[symbol_id] = member_type;
#			print "symbol " symbol_id " returns member " \
#				member_id " with type " member_type;
		}
	} # end if argc
	} # end if match(...)
	# just read to end-of-file
} # end while
} else {	# !got_union
	print "need to have completed process_union() before " \
		"invoking process_symbol_types().";
	exit;
}
}

/^state [0-9]*/ {
	print "/************************** " $0 " **************************/";
	sc = 0;
	while (getline && length($0)) {
		sc = 0;
		# do nothing, ignore production rule states
	}
	while (getline && NF == 3 && $2 == "shift") {
		# collect shift actions
		printf("static yy_state_map_link yysml_" state_count \
			"_" sc " = { " $3 ", " enum_of[symbol_type[$1]] ", ");
		if (sc) printf("&yysml_" state_count "_" sc-1);
		else	printf("NULL");		# or 0
		print "};";
		sc++;
	}
	while (getline && NF == 3 && $2 == "goto") {
		# collect goto actions
		printf("static yy_state_map_link yysml_" state_count \
			"_" sc " = { " $3 ", " enum_of[symbol_type[$1]] ", ");
		if (sc) printf("&yysml_" state_count "_" sc-1);
		else	printf("NULL");		# or 0
		print "};";
		sc++;
	}
	shift_count[state_count] = sc;
	state_count++;
}


END {
	for (mid in type_of) {
		type_str = type_of[mid];
		gsub("[*]", "", type_str);
		gsub("::", "_", type_str);
		print "static " type "* yy_union_get_" type_str \
			"(YYSTYPE& u) {";
		print "\treturn u." mid ";";
		print "}";
	}

	print type "* yy_union_resolve(YYSTYPE& u, const short i, const short j) {";
	print "static yy_state_map_link* yysma[" state_count "] = {";
	for (i=0; i<state_count; i++) {
		if (shift_count[i])
			print "\t&yysml_" i "_" shift_count[i] -1 ", ";
		else print "NULL, ";
	}
	print "};";
	print "static " type "* (*yy_union_get[" member_count "])(YYSTYPE&) = {"
	for (i=0; i<member_count; i++) {
		type_str = type_of[member_id_array[i]];
		gsub("[*]", "", type_str);
		gsub("::", "_", type_str);
		print "\t&yy_union_get_" type_str ", ";
	}
	print "};";	# end of array of function pointers
	print "yy_state_map_link* iter = yysma[i];";
	print "while (iter && iter->state != j) {";
	print "\titer = iter->next;";
	print "} /* end while */";
	print "if (iter) {";
		print "\treturn (*yy_union_get[iter->type_enum])(u);";
	print "} else {";
	print "\treturn NULL;";
	print "}";
	print "}";

}

