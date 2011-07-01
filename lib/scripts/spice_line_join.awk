#!/usr/bin/awk -f
# joins line-continued lines into really long lines

BEGIN { buffer = ""; }

{
	if ($1 == "+") {
		buffer = buffer substr($0,2);
	} else {
		# print previous buffered line
		if (NR > 1)
			print buffer;
		# start new one
		buffer = $0;
	}
}

END { print buffer; }

