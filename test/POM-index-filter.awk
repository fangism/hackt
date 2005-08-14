#!/usr/bin/awk -f
# "POM-index-filter.awk"
#	$Id: POM-index-filter.awk,v 1.1.2.1 2005/08/14 20:09:05 fang Exp $
# filter for text dump of persistent_object_manager's object header
# the input to this file is the output of 
#	"POM-header-filter.awk"

{
if (match($0,"^Persistent Object Manager")) {
	print;		# echo title back out
	rewrite_header_indices();
} else {
	print;		# crashes???
}
}

function rewrite_header_indices() {
	getline;
	print;		# repeat the table header
	# 7 fields because we added the size field at the end
	while (getline && NF == 7) {
		# expect 6 fields
		rewrite_header_index($1,$2,$3,$4,$5,$6,$7);
	}
	# until empty line encountered
	print "";	# crashes???
}

# just replaces the index
function rewrite_header_index(ind,addr,type,arg,hd,tl,sz) {
	print "\t##ID##\t" addr "\t" type "\t" arg "\t" hd "\t" tl" \t" sz;
}

