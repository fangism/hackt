#!/usr/bin/sed -f
# "readable_gcc_errors.sed"
#	$Id: readable_gcc_errors.sed,v 1.1 2005/09/04 22:13:49 fang Exp $

# inserts newlines between separate errors
# to use this, redirect the stderr of make into this script
# e.g make |& <script>

/^.*\..*cc: In/ { i\
\
--------------------------------- new error -----------------------------------\

}

/^.*\.h: In/ { i\
\
--------------------------------- new error -----------------------------------\

}

/^.*\.cc: error: prototype/ { i\
\
--------------------------------- new error -----------------------------------\

}

/^.*\..*cc:[0-9]+: error: no matching function for call/ { i\
\
--------------------------------- new error -----------------------------------\

}

/^In file included from/ { i\
\
--------------------------------- new error -----------------------------------\

}

/^.*\.o:.*undefined reference to/ { i\
\
--------------------------------- link error ----------------------------------\

}

