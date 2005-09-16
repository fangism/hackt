/**
	\file "Object/common/alia_string_set.cc"
	$Id: alias_string_set.cc,v 1.1.2.1 2005/09/16 07:19:36 fang Exp $
 */

#include "Object/common/alias_string_set.h"
#include <iostream>

namespace ART {
namespace entity {
//=============================================================================
// class alias_string_set method definitions

alias_string_set::alias_string_set() : parent_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
alias_string_set::~alias_string_set() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
alias_string_set::dump(ostream& o) const {
	const_iterator si(begin());
	const const_iterator se(end());
	for ( ; si!=se; si++) {
		typedef	const_iterator::value_type::const_iterator
						const_inner_iterator;
		o << '{';
		const_inner_iterator ii(si->begin());
		const const_inner_iterator ie(si->end());
		for ( ; ii!=ie; ii++) {
			o << *ii << ',';
		}
		o << "}x";
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
alias_string_set::push_back(void) {
	parent_type::push_back(value_type());
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

