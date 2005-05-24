#!/usr/bin/awk -f
# "POM-header-filter.awk"
# filter for text dump of persistent_object_manager's object header

# NOTE: when used as a pipe redirected from stdout combined with stderr,
#	it may crash on particular print statements.  
#	This is probably some problem with an implementation of awk itself.  

{
if (match($0,"^Persistent Object Manager")) {
	print;		# echo title back out
	rewrite_header();
} else if (match($0,"^In module created from")) {
	gsub(": [^ ]*[ ]", ": ##FILE## ",$0);
	print $0;
} else {
	print;		# crashes???
}
}

function rewrite_header() {
	getline;
	print $0 "\tsize";	# echo table header back out
	while (getline && NF == 6) {
		# expect 6 fields
		rewrite_header_entry($1,$2,$3,$4,$5,$6);
	}
	# until empty line encountered
	print "";	# crashes???
}

function rewrite_header_entry(ind,addr,type,arg,hd,tl) {
	printf("\t" ind "\t##ADDR##\t" type "\t" arg "\t");
	printf("#HEAD#\t#TAIL#\t");
	if (match(type,"module"))
		print "##SIZE##";
	else	print tl-hd;
}

