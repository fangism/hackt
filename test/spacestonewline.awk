#! /usr/bin/awk -f
# "spacestonewline.awk"

{
	num = split($0,tokens);
	i = 1;
	for ( ; i<=num; i++)
		print tokens[i];
}

