/**
	\file "art_lex.cc"
	Utility lexer function definitions.
	$Id: art_lex.cc,v 1.3 2005/02/27 22:12:00 fang Exp $
 */

#include <ostream>
#include "art_lex.h"

namespace ART {
namespace lexer {

// inline
ostream& operator << (ostream& o, const line_range& l) {
	if (l.start.line == l.end.line) {
		o << "[" << l.start.line << ":" << l.start.col;
		if (l.start.col != l.end.col)
			o << ".." << l.end.col;
		o << "]";
	} else {
		o << "[" << l.start.line << ":" << l.start.col << "--"
			<< l.end.line << ":" << l.end.col << "]";
	}
	return o;
}

}	// end namespace lexer
}	// end namespace ART

