#! /usr/bin/awk
# "test/state_enum_filter.awk"

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
	case1 = ".*[:].*[.].*[(][0-9]+[)]$";
	case2 = "in state [0-9]+, possible rules are:";
	case3 = "^[0-9]+\t";
}

# for all lines
{
	str = $0;
	# assertion: in all below cases, the regexp [0-9]+ matches only once
	if (match(str, case1)) {
		gsub("[0-9]+", rule_str, str);
	} else if (match(str, case2)) {
		gsub("[0-9]+", state_str, str);
	} else if (match(str, case3)) {
		# need to qualify context a bit more here...
		gsub("[0-9]+\t", state_str "\t", str);
	}
	# else keep str as-is
	print str;
}

END {
}

