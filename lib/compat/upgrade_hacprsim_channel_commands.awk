#!/usr/bin/awk -f
#	$Id: upgrade_hacprsim_channel_commands.awk,v 1.1 2010/05/19 23:09:31 fang Exp $
# "upgrade_hacprsim_channel_commands.awk"
# convert old prsim channel commands to new hacprsim channel commands

# optional variable:
#	-v ack_init= initial state of .a channels (sinks)
#	-v enable_init= initial state of .e channels (sinks)

BEGIN {
# process variables
	if (!length(ack_init)) { ack_init = 1; }
	else { ack_init = strtonum(ack_init); }

	if (!length(enable_init)) { enable_init = 0; }
	else { enable_init = strtonum(enable_init); }

	channel_type_map["e1ofN"] = "e";
	channel_type_map["ev1ofN"] = "ev";
	channel_type_map["a1ofN"] = "a";
	channel_type_map["av1ofN"] = "av";
	channel_type_map["eMx1of2"] = "e"; 	
	channel_type_map["evMx1of2"] = "ev"; 	
}

# match all lines by hand, exclusive cases
{
if (match($0, "^channel[ \t]+")) {
	ack_type = channel_type_map[$2];
	if (match(ack_type, "e")) {
		init = enable_init;
	} else {
		init = ack_init;
	}
	if (match($2, "Mx1of2")) {
		print $1 " " $4 " " ack_type ":" init " b:" $3 " d:2";
	} else {
		print $1 " " $4 " " ack_type ":" init " :0 d:" $3;
	}

} else if (match($0, "^injectfile[ \t]+")) {
	print "channel-source-file " $2 " " $3;
} else if (match($0, "^loop-injectfile[ \t]+")) {
	print "channel-source-file-loop " $2 " " $3;
} else if (match($0, "^expectfile[ \t]+")) {
	print "channel-expect-file " $2 " " $3;
} else if (match($0, "^loop-expectfile[ \t]+")) {
	print "channel-expect-file-loop " $2 " " $3;
} else if (match($0, "^dumpfile[ \t]+")) {
	print "channel-log " $2 " " $3;
} else if (match($0,"^set _.*Reset")) {
	print;
	if ($3) {
		print "channel-release-all";
	} else {
		print "channel-reset-all";
	}
} else {
	# default: no modification
	print;
}
}

