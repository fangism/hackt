#! /usr/bin/awk -f
#	$Id: state_enum_filter.awk,v 1.5 2005/11/10 02:13:13 fang Exp $
# "test/state_enum_filter.awk"

# This script has been converted into a library function, to be called
# by other awk modules.  It no longer stands alone.  

# Whenever the grammar changes, the enumerations of the parser states
# will change, which will cause a change in the error reporting
# from the artc compiler.  
# This filter script appropriately replaces the state numbers in 
# appropriate contexts:

# 1) When artc reports possible production rules in the event of a syntax
#	error, it also gives the state number in parenthesis at the 
#	end of each production line.  
# 2) "In state XXX, possible rules are:"

BEGIN {
	state_str = "#STATE#";
	rule_str = "#RULE#";

	# strings to match
	case1 = ".*[:].*[.].*[(][0-9]+[)]$";
	case1b = "\\|.*[.].*[(][0-9]+[)]$";
	case2 = "in state [0-9]+, possible rules are:";
	case3 = "^[0-9]+\t";
	case4 = " \\(rule [0-9]+\\)";	# note extra first space
}

# for all lines
function filter_state_enums(str) {
	# assertion: in all below cases, the regexp [0-9]+ matches only once
	if (match(str, case1)) {
		gsub("[0-9]+", rule_str, str);
	} else if (match(str, case1b)) {
		gsub("[0-9]+", rule_str, str);
	} else if (match(str, case2)) {
		gsub("[0-9]+", state_str, str);
	} else if (match(str, case3)) {
		# need to qualify context a bit more here...
		gsub("[0-9]+\t", state_str "\t", str);

		# old version of bison uses "$" instead of "$end"
		gsub("\\$$", "$end", str);
	} else if (match(str, case4)) {
		gsub(case4, "(" rule_str ")", str);
	}
	# else keep str as-is

# further canonicalization of parser output:
# because bison's .output strings differ in formatting between versions :S

	# error message changed...
	gsub(": parse error", ": syntax error", str);

	# the production delimiter changed in some old version...
	gsub("  ->  ", ": ", str);

	# extraneous empty parens in output
	gsub("[ \\t]*\\(\\).*$", "", str);

	return str;
}

