#!/usr/bin/awk -f
# "chpsim-binary-size-filter.awk"
# for the purpose of filtering out architecture-dependent trace size info
# happens to be reusable for prsim trace dumps too

{
if (match($0, "^[\t]+start[\t]start[\t]file[\t]chunk")) {
	print;
	getline;
	# check for table of contents header
	if (match($0, "^[\t]+epoch[\t]index[\t]time[\t]offset[\t]size")) {
		print;
		# then filter out binary-sensitive info: offsets, sizes
		while (getline && (NF==5) &&
			match($1, "[0-9]+") &&
			match($2, "[0-9]+") &&
#			match($3, "[0-9]+") &&	# floating point
			match($4, "[0-9]+") &&
			match($5, "[0-9]+")) {
			print "\t" $1 "\t" $2 "\t" $3 "\t#SIZE#\t#SIZE#";
		}
	}
}
	print;
}

