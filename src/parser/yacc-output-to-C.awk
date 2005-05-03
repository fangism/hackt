#! `which awk` -f
# "yacc-output-to-C.awk"
# by Fang
#	$Id: yacc-output-to-C.awk,v 1.2.66.1 2005/05/03 19:51:13 fang Exp $

# takes the y.output file from yacc -v and converts it into C strings
# for use in error reporting

# CO-DEPENDENT ON:
# parser/common-output-to-C.awk
# CONFLICTS WITH:
# parser/bison-output-to-C.awk

# chicken-before-the-egg

function main_program_name() {
	return "yacc-output-to-C.awk";
}

# main pattern matching body

/^state [0-9]*/ {
	print "/************************** " $0 " **************************/";
	rc = 0;
	print "static const char* const yy_rule_string_set_" \
		state_count "[] = {";
	while (getline && length($0)) {
		print "\t" string_to_Cstring($0) ",";
		rc++;
	}
	rule_count[state_count] = rc;
	print "};";		# end string array
	state_count++;
}

