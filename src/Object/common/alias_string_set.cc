/**
	\file "Object/common/alia_string_set.cc"
	$Id: alias_string_set.cc,v 1.1.2.2 2005/09/17 04:48:54 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include "Object/common/alias_string_set.h"
#include "main/cflat_options.h"
#include "util/macros.h"
#include "util/stacktrace.h"
#include <iostream>

namespace ART {
namespace entity {
#include "util/using_ostream.h"
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints node aliases.  
 */
ostream&
alias_string_set::dump_aliases(ostream& o, const string& canonical,
		const cflat_options& cf) const {
	STACKTRACE("alias_string_set::dump_aliases()");
	const_iterator n(begin());
	const_iterator oi(n++);
	const const_iterator oe(end());
	INVARIANT(oi != oe);
	typedef	const_iterator::value_type::const_iterator
					const_inner_iterator;
	const_inner_iterator i(oi->begin());
	const const_inner_iterator e(oi->end());
	for ( ; i!=e; i++) {
		__dump_aliases(o, canonical, cf, *i, n, oe);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private recursive printing of all hierarchical aliases.  
 */
ostream&
alias_string_set::__dump_aliases(ostream& o, const string& canonical, 
		const cflat_options& cf, const string& prefix, 
		const const_iterator ob, const const_iterator oe) {
	STACKTRACE("alias_string_set::__dump_aliases()");
if (ob == oe) {
	if (cf.dump_self_connect || prefix != canonical) {
		// TODO: factor this into some method of cflat_options.  
		// print connection, check options
		switch (cf.connect_style) {
			case cflat_options::CONNECT_STYLE_CONNECT:
				o << "connect ";
				break;
			case cflat_options::CONNECT_STYLE_EQUAL:
				o << "= ";
				break;
			case cflat_options::CONNECT_STYLE_WIRE:
				o << "wire ";
				break;
			default:
				o << "alias ";
				break;
		}
		if (cf.enquote_names) {
			o << "\"" << canonical << "\" \"" << prefix << "\"";
		} else {
			o << canonical << ' ' << prefix;
		}
		o << endl;
	}
} else {
	typedef	const_iterator::value_type::const_iterator
					const_inner_iterator;
	const_iterator n(ob);
	n++;
	const_inner_iterator i(ob->begin());
	const const_inner_iterator e(ob->end());
	for ( ; i!=e; i++) {
		const string new_prefix(prefix +"." +*i);
		__dump_aliases(o, canonical, cf, new_prefix, n, oe);
	}
}
	return o;
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

