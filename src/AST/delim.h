/**
	\file "AST/delim.h"
	Forward declarations for classes in HAC::parser.
	$Id: delim.h,v 1.2 2005/12/13 04:15:07 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_delim.h,v 1.2.52.1 2005/12/11 00:45:04 fang Exp
 */

#ifndef	__HAC_AST_DELIM_H__
#define	__HAC_AST_DELIM_H__

namespace HAC {
namespace parser {

//-----------------------------------------------------------------------------
// some constant delimiter strings
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

#endif	// __HAC_AST_DELIM_H__

