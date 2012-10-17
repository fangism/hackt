/**
	\file "util/syntax_fwd.hh"
	Generic tree structure for syntax.
	Kind of reminiscent of LISP structures.  
	Uses visitor pattern.
	$Id: $
 */

#ifndef	__UTIL_SYNTAX_FWD_H__
#define	__UTIL_SYNTAX_FWD_H__

namespace util {
namespace syntax {

class tree_walker;
class node;
class terminal;
class string_token;
class node_list;

}	// end namespace syntax
}	// end namespace util

#endif	// __UTIL_SYNTAX_FWD_H__
