// "art_utils.h"

#ifndef __ART_UTILS_H__
#define __ART_UTILS_H__

#include <assert.h>
#include <iostream>

#include "art_lex.h"

using namespace std;

namespace ART {

//=============================================================================
/// just a line number and column position
class line_position {
public:
	long		line;
	long		col;
public:
	line_position() : line(0), col(0) { }
	line_position(const long l, const long c) : line(l), col(c)
		{ assert(line>0); assert(col>0); }
	line_position(const line_position& lp) : 
		line(lp.line), col(lp.col) { }		// no assertion yet
	line_position(const token_position& tp) : 
		line(tp.line), col(tp.col)
		{ assert(line>0); assert(col>0); }
// default destructor

};

//=============================================================================
class line_range {
public:
	line_position	start;
	line_position	end;
public:
	line_range() : start(), end() { }
	line_range(const line_position& l, const line_position& r) :
		start(l), end(r) { }
// default destructor

friend ostream& operator << (ostream& o, const line_range& l);
};

//=============================================================================
};

#endif	// __ART_UTILS_H__

