/**
	\file "AST/attribute.cc"
	$Id: attribute.cc,v 1.5 2010/07/01 20:20:22 fang Exp $
 */

#include "AST/attribute.hh"
#include "AST/common.hh"
#include "AST/node_list.tcc"
#include "AST/token_string.hh"
#include "AST/expr_list.hh"
#include "util/memory/count_ptr.tcc"

namespace util {
namespace memory {
template class count_ptr<const HAC::parser::generic_attribute>;
}	// end namespace memory
}	// end namespace util

namespace HAC {
namespace parser {

//=============================================================================
generic_attribute::generic_attribute(
		const token_identifier* i, const expr_list* e)
		: key(i), values(e) {
	INVARIANT(key || values);
//	NEVER_NULL(key);
//	NEVER_NULL(values);
}

generic_attribute::generic_attribute(
		const count_ptr<const token_identifier>& i,
		const expr_list* e) :
		key(i), values(e) {
	INVARIANT(key || values);
//	NEVER_NULL(key);
//	NEVER_NULL(values);
}

generic_attribute::~generic_attribute() { }

PARSER_WHAT_DEFAULT_IMPLEMENTATION(generic_attribute)

line_position
generic_attribute::leftmost(void) const {
	if (key)
		return key->leftmost();
	else	return values->leftmost();
}

line_position
generic_attribute::rightmost(void) const {
	if (values)
		return values->rightmost();
	else	return key->rightmost();
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

