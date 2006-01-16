/**
	\file "sim/prsim/Reference.cc"
	$Id: Reference.cc,v 1.1.2.1 2006/01/16 06:58:58 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <cstdio>
#include <string>
#include "sim/prsim/Reference.h"
#include "parser/instref-parse-real.h"	// for YYSTYPE
#include "AST/expr.h"
#include "util/stacktrace.h"
#include "util/libc.h"			// for tmpfile

extern	int instref_parse(void*, YYSTYPE&, FILE*);

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::string;
#include "util/using_ostream.h"
//=============================================================================

/**
	TODO: figure out a way to parse a string without
		going through a friggin' temp file.  
		This is really sad. 
 */
node_index_type
parse_node_to_index(const string& n) {
	FILE* temp = tmpfile();
	if (!temp) {
		// Woe is me!
		cerr << "Failed to create temporary file-buffer!" << endl;
		THROW_EXIT;
	}
	// fputs returns 0 on success, anything else on error
	if (fputs(n.c_str(), temp)) {
		cerr << "Error writing string to temporary file." << endl;
		fclose(temp);
		THROW_EXIT;
	} else {
		fflush(temp);		// flush it out, necessary?
		YYSTYPE lval;
		try {
			instref_parse(NULL, lval, temp);
		} catch (...) {
			cerr << "Error parsing instance name: " << n << endl;
			return INVALID_NODE_INDEX;
		}
		cerr << "parsed node name successfully... "
			"Fang finish the lookup!" << endl;
		return INVALID_NODE_INDEX;
	}
	fclose(temp);
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

