// "art_parser.h"
// parser classes for ART only!

#ifndef __ART_PARSER_H__
#define __ART_PARSER_H__

// probably stick to lists, no hash_map's until the build() phase

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
/// This class extends functionality of the standard string for the parser.  

/**
	This class also keeps track of the line and column position in a 
	file for a given token.  
 */
class token_string : public string {
// will also contain a token_position

};

};
};

#endif	// __ART_PARSER_H__

