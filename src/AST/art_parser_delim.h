/**
	\file "AST/art_parser_delim.h"
	Forward declarations for classes in HAC::parser.
	$Id: art_parser_delim.h,v 1.2.52.1 2005/12/11 00:45:04 fang Exp $
 */

#ifndef	__AST_HAC_PARSER_DELIM_H__
#define	__AST_HAC_PARSER_DELIM_H__

namespace HAC {
namespace parser {

//-----------------------------------------------------------------------------
// some constant delimiter strings, defined in art_parser.cc
// these are used as delimiter template arguments for node_list

extern	const char	none[];
extern	const char	comma[];
extern	const char	semicolon[];
extern	const char	scope[];
extern	const char	thickbar[];
extern	const char	colon[];
extern	const char	alias[];
extern	const char	pound[];        // calling it "hash" would be confusing

//-----------------------------------------------------------------------------
}	// end namespace parser
}	// end namespace HAC

#endif	// __AST_HAC_PARSER_DELIM_H__

