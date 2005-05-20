#!/usr/bin/awk -f

{
if (match($0,"^Persistent Object Manager")) {
	rewrite_header();
} else if (match($0,"^In module created from")) {
	gsub(": [^ ]*[ ]", ": ##FILE## ",$0);
	print $0;
} else {
	print;
}
}

function rewrite_header() {
	print;		# echo title back out
	getline;
	print $0 "\tsize";	# echo table header back out
	while (getline && NF == 6) {
		# expect 6 fields
		rewrite_header_entry($1,$2,$3,$4,$5,$6);
	}
	# until empty line encountered
	print "";
}

function rewrite_header_entry(ind,addr,type,arg,hd,tl) {
	printf("\t" ind "\t##ADDR##\t" type "\t" arg "\t");
	printf("#HEAD#\t#TAIL#\t");
	if (match(type,"module"))
		print "##SIZE##";
	else	print tl-hd;
}

