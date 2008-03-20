/**
	\file "AST/attribute.cc"
	$Id: attribute.cc,v 1.1 2008/03/20 00:03:14 fang Exp $
 */

#include "AST/attribute.h"
#include "AST/common.h"
#include "AST/node_list.tcc"
#include "AST/token_string.h"
#include "AST/expr_list.h"

namespace HAC {
namespace parser {

//=============================================================================
generic_attribute::generic_attribute(
		const token_identifier* i, const expr_list* e)
                : key(i), values(e) {
        NEVER_NULL(key); NEVER_NULL(values);
}

generic_attribute::~generic_attribute() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(generic_attribute)

line_position
generic_attribute::leftmost(void) const {
        return key->leftmost();
}

line_position
generic_attribute::rightmost(void) const {
        return values->rightmost();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// template class node_list<const attribute>;

template
node_list<const generic_attribute>::node_list(const generic_attribute*);

template
node_list<const generic_attribute>::~node_list();

template
ostream&
node_list<const generic_attribute>::what(ostream&) const;

template
line_position
node_list<const generic_attribute>::leftmost(void) const;

template
line_position
node_list<const generic_attribute>::rightmost(void) const;

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

