// "art_parser.h"
// parser classes for ART only!

#ifndef __ART_PARSER_H__
#define __ART_PARSER_H__

// probably stick to lists, no hash_map's until the build() phase
// if this file gets too honkin' big, I'll split it up...

#include <iostream>
#include <string>

#ifdef	__APPLE__		// different location with gcc-3.3 on OS X
#include <ext/hash_map>
using namespace __gnu_cxx;
#else
#include <hash_map>
#endif

#include "art_switches.h"
#include "list_of_ptr.h"	// includes <list>

using namespace std;

/// This is the general namespace for all ART-related classes.  
namespace ART {

/// This namespace is reserved for ART's parser-related classes.  
/**
	This contains all of the classes for the abstract syntax tree (AST).  
	Each class should implement recursive methods of traversal.  
	The created AST will only reflect a legal instance of the grammar, 
	therefore, one should use the type-check and build phase to 
	return a more useful manipulate ART object.  
 */
namespace parser {
//=============================================================================
/// the abstract base class for parser nodes, universal return type
/**
	Rather than have to rely on unions in the lexer and parser, 
	we prefer to use a single type of node for manipulation.  
	This will be useful when we wish to dump the parser's value stack
	in error reporting and debugging.  
 */
class node {
public:
///	standard virtual destructor
virtual	~node() { }

/**
	Shows representation without recursive descent.  
	Derived classes of non-terminals should just print their type name.  
 */
virtual	ostream& what(ostream& o) const = 0;

};

//=============================================================================
/// abstract base class for terminal tokens, mainly to be used by lexer
class terminal : public node {
protected:
/// the position in the file where token was found, pos.off is unused
	token_position	pos;
public:
///	base constructor always records the current position of the token
//	"current" is defined in art_switches.h
	terminal() { pos.line = current.line; pos.leng = current.leng;
		pos.col = current.col; }

///	standard virtual destructor
virtual	~terminal() { }

virtual	ostream& what(ostream& o) const { return o << "(node)"; }
};

//=============================================================================
/// abstract base class for non-terminal symbols, mainly to be used by parser
class nonterminal : public node {
public:
///	standard virtual destructor
virtual ~nonterminal() { }

virtual	ostream& what(ostream& o) const { return o << "(non-terminal)"; }
// virtual	ostream& where(ostream& o) const = 0;
	// includes token position information
};

//=============================================================================
class token_char : public terminal {
protected:
/// the character
	int c;
public:
	token_char(const int i) : terminal(), c(i) { }
virtual	~token_char() { }

virtual	ostream& what(ostream& o) const { return o << (char) c; }
};

//=============================================================================
/// stores an integer (long) in native form and retains position information
class token_int : public terminal {
protected:
/// the value
	long val;
public:
/// standard constructor
	token_int(const long v) : terminal(), val(v) { }
/// standard virtual destructor
virtual	~token_int() { }

virtual	ostream& what(ostream& o) const { return o << val; }
};

//=============================================================================
/// stores an float (double) in native form and retains position information
class token_float : public terminal {
protected:
/// the value
	double val;
public:
/// standard constructor
	token_float(const double v) : terminal(), val(v) { }
/// standard virtual destructor
virtual	~token_float() { }

virtual	ostream& what(ostream& o) const { return o << val; }
};

//=============================================================================
/// This class extends functionality of the standard string for the parser.  
/**
	This class also keeps track of the line and column position in a 
	file for a given token.  
	For now, this class is used for identifiers.  
	For quoted strings, use token_quoted_string.  
 */
class token_string : public string, public terminal {
public:
/// uses base class' constructors to copy text and record position
	token_string(const char* s) : string(s), terminal() { }
virtual	~token_string() { }

virtual	ostream& what(ostream& o) const 
		{ return o << (const string&) (*this); }
};

//=============================================================================
/// keyword version of token_string class
class token_keyword : public token_string {
public:
	token_keyword(const char* s) : token_string(s) { }
virtual	~token_keyword() { }

virtual	ostream& what(ostream& o) const
		{ return o << "keyword: " << *((const string*) this); }
};

//=============================================================================
/// quoted-string version of token_string class
class token_quoted_string : public token_string {
public:
	token_quoted_string(const char* s) : token_string(s) { }
virtual	~token_quoted_string() { }

virtual	ostream& what(ostream& o) const { 
		// punt: handle special characters later...
		return ((const token_string*) this)->what(o << "\"") << "\"";
	}
};

//=============================================================================
};	// end namespace parser
};	// end namespace ART

#endif	// __ART_PARSER_H__

