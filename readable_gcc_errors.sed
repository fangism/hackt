#!/usr/bin/sed -f
# "readable_gcc_errors.sed"

# inserts newlines between separate errors
# to use this, redirect the stderr of make into this script
# e.g make |& <script>

/^.*\.cc: In/ { i\
\
--------------------------------- new error -----------------------------------\

}

/^.*\.h: In/ { i\
\
--------------------------------- new error -----------------------------------\

}

/^.*\.cc: error: prototype.*/ { i\
\
--------------------------------- new error -----------------------------------\

}

