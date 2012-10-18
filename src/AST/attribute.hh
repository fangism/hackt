/**
	\file "AST/attribute.hh"
	Generic structure of attributes.  
	$Id: attribute.hh,v 1.2 2010/07/01 20:20:23 fang Exp $
 */

#ifndef	__HAC_AST_ATTRIBUTE_H__
#define	__HAC_AST_ATTRIBUTE_H__

#include <iosfwd>
#include "AST/AST_fwd.hh"
#include "lexer/hac_lex.hh"
#include "util/memory/count_ptr.hh"

namespace HAC {
namespace parser {
using std::ostream;
using lexer::line_position;
using util::memory::count_ptr;
class token_identifier;
class expr_list;

//=============================================================================
/**
	Generic attributes, structure, a key-value pair.
	Why not just use std::pair?
 */
struct generic_attribute {
	const count_ptr<const token_identifier>		key;
	const count_ptr<const expr_list>		values;
public:
	generic_attribute(const token_identifier*, const expr_list*);
	generic_attribute(const count_ptr<const token_identifier>&,
		const expr_list*);
	~generic_attribute();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

};	// end class attribute

//=============================================================================
}	// end namespace parser
}	// end namespace HAC
#endif	// __HAC_AST_ATTRIBUTE_H__

