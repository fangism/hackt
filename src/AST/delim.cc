/**
	\file "AST/delim.cc"
	Delimiter strings.  
	Can't find a better place to stash these...
	$Id: delim.cc,v 1.2 2005/12/13 04:15:07 fang Exp $
 */

#ifndef	__HAC_AST_DELIM_CC__
#define	__HAC_AST_DELIM_CC__

#define	ENABLE_STACKTRACE		0

#include "AST/delim.hh"

//=============================================================================
namespace HAC {
namespace parser {

//=============================================================================
// global constants
// need to be kept here for explicit template instantiations (?)
const char none[] = "";		///< delimiter for node_list template argument
const char comma[] = ",";	///< delimiter for node_list template argument
const char semicolon[] = ";";	///< delimiter for node_list template argument
const char scope[] = "::";	///< delimiter for node_list template argument
const char thickbar[] = "[]";	///< delimiter for node_list template argument
const char colon[] = ":";	///< delimiter for node_list template argument
const char alias[] = "=";	///< delimiter for node_list template argument
const char pound[] = "#";	///< delimiter for node_list template argument

// eventually token keywords here too? or "AST/token.cc"
//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_DELIM_CC__

