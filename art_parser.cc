// "art_parser.cc"
// class method definitions for ART::parser

// template instantiations are at the end of the file

#include "art_parser.h"

namespace ART {
namespace parser {

//=============================================================================
// global constants
const char none[] = "";		///< delimiter for node_list template argument
const char comma[] = ",";	///< delimiter for node_list template argument
const char semicolon[] = ";";	///< delimiter for node_list template argument
const char scope[] = "::";	///< delimiter for node_list template argument
const char thickbar[] = "[]";	///< delimiter for node_list template argument
const char colon[] = ":";	///< delimiter for node_list template argument

//=============================================================================
// method definitions for node_list<>
// had trouble finding reference to template functions defined here...?

/*** attempt this again later
NODE_LIST_TEMPLATE_SPEC_EXPORT
node_list<T,D>*
node_list<T,D>::wrap(node* b, node* e) {
	open = dynamic_cast<terminal*>(b);
	assert(open);
	assert(dynamic_cast<token_char*>(open) ||
		dynamic_cast<token_string*>(open));
	close = dynamic_cast<terminal*>(e);
	assert(close);
	assert(dynamic_cast<token_char*>(close) ||
		dynamic_cast<token_string*>(close));
	return this;
}
***/

//=============================================================================
// chan_type methods
/**
	Associates a channel or port with a data type, such as a list of 
	ints and bools.  
 */
chan_type*
chan_type::attach_data_types(node* t) {
	assert(t); assert(!dtypes);     // sanity check    
	dtypes = dynamic_cast<base_data_type_list*>(t);
	assert(dtypes);
	return this;
}

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS

};
};

