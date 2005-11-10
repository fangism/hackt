#! `which awk` -f
# "bison-output-to-C.awk"
# by Fang
#	$Id: bison-output-to-C.awk,v 1.3.50.1 2005/11/10 23:11:53 fang Exp $

# takes the y.output file from bison -v and converts it into C strings
# for use in error reporting

# CO-DEPENDENT ON:
# parser/common-output-to-C.awk
# CONFLICTS WITH:
# parser/yacc-output-to-C.awk

# chicken-before-the-egg

function main_program_name() {
	return "bison-output-to-C.awk";
}

function transform_rule(str,
# local vars
	head, tail, ret) {
	# we know this match is at the beginning of the line
	if (match(str,"[ ]*[0-9]* ") != 1)
		exit(1);		# sanity check
	head = substr(str,1,RLENGTH);
	ret = substr(str,RLENGTH+1);
	gsub(" ","",head);		# delete whitespaces
#	if (length(head))
		return "\t" ret "  (" head ")";
	# else more work to do extracting text
#	match(ret);
}

# main pattern matching body
/^state [0-9]*/ {
	print "/************************** " $0 " **************************/";
# in bison's y.output, state section ends with two blank lines
	rc = 0;
	print "static const char* const yy_rule_string_set_" \
		state_count "[] = {";
	first_blank = 0;			# reset
	while (getline) {
		if (!length($0)) {
			if (first_blank)	break;
				# second blank line in a row
			else			first_blank = 1;
		} else {
			first_blank = 0;	# reset
#		if (!match($0, "go to state") && !match($0, "\\$default"))
		if (!match($0, "go to state"))
		{
#			print "\t" string_to_Cstring($0) ",";
			print "\t" string_to_Cstring(transform_rule($0)) ",";
			rc++;
		}
		}
		# else ignore other shift and goto lines
	}
# some versions of bison (1.35) generate some empty goto states
# that just trampoline to other states
# in this case, we emit a bogus string as a placeholder
# to keep from emitting an empty aggregate initializer.
	if (!rc) {
		print "\"(goto ?)\"";
		rc++;
	}
	rule_count[state_count] = rc;
	print "};";		# end string array
	state_count++;
}

