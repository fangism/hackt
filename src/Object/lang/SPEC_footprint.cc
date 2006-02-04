/**
	\file "Object/lang/SPEC_footprint.cc"
	$Id: SPEC_footprint.cc,v 1.1.2.1 2006/02/04 01:33:11 fang Exp $
 */

#include <iostream>
#include "Object/def/footprint.h"
#include "Object/lang/SPEC_footprint.h"
#include "Object/lang/cflat_visitor.h"
#include "Object/inst/instance_pool.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/common/dump_flags.h"
#include "util/indent.h"
#include "util/IO_utils.h"
#include "util/persistent_object_manager.tcc"
#include "util/persistent_functor.tcc"

namespace util {
using HAC::entity::SPEC::footprint_directive;
//=============================================================================
// value_writer and value_reader specializations for footprint_directive
template <>
struct value_writer<footprint_directive> {
	ostream& os;
	value_writer(ostream& o) : os(o) { }

	void
	operator () (const footprint_directive& d) const {
		d.write_object(os);
	}
};	// end struct value_writer

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct value_reader<footprint_directive> {
	istream& is;
	value_reader(istream& i) : is(i) { }

	void
	operator () (footprint_directive& d) const {
		d.load_object(is);
	}

};	// end struct value_reader

//=============================================================================
}	// end namespace util

namespace HAC {
namespace entity {
namespace SPEC {
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;
using util::auto_indent;
using PRS::cflat_visitor;

//=============================================================================
// class SPEC::footprint_directive method definitions

/**
	\return 1-indexed offset of first error if found, else 0.  
 */
size_t
footprint_directive::first_error(void) const {
	// TODO: use std::find! and std::distance.
	const size_t s = args.size();
	if (s) {
		size_t i = 0;
		for ( ; i<s; i++) {
			if (!args[i]) {
				cerr << "Error resolving literal " << i <<
					"." << endl;
				return i+1;
			}
		}
	}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_directive::write_object(ostream& o) const {
	write_value(o, key);
	util::write_sequence(o, args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_directive::load_object(istream& o) {
	read_value(o, key);
	util::read_sequence_resize(o, args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_directive::accept(cflat_visitor& v) const {
	v.visit(*this);
}

//=============================================================================
// class SPEC::footprint method definitions

footprint::footprint() : footprint_base_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint::~footprint() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
footprint::dump_directive(const footprint_directive& d, ostream& o, 
		const node_pool_type& np) {
	o << d.key << '(';
	typedef	footprint_directive::const_iterator	const_iterator;
	const_iterator i(d.begin());
	const const_iterator e(d.end());
	INVARIANT(i!=e);
	np[*i].get_back_ref()->dump_hierarchical_name(o, 
		dump_flags::no_definition_owner);
	for (++i; i!=e; ++i) {
		np[*i].get_back_ref()->dump_hierarchical_name(o << ',', 
			dump_flags::no_definition_owner);
	}
	return o << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: indent?
 */
ostream&
footprint::dump(ostream& o, const entity::footprint& f) const {
if (size()) {
	const node_pool_type& bpool(f.get_pool<bool_tag>());
	o << auto_indent << "resolved specs:" << endl;
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; ++i) {
		dump_directive(*i, o << auto_indent, bpool) << endl;
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param k the name of the directive.
 */
footprint_directive&
footprint::push_back_directive(const string& k) {
	push_back(footprint_directive(k));
	footprint_directive& ret(back());
	// ret.resize(s);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Currently has no persistent information to track.  
 */
void
footprint::collect_transient_info_base(persistent_object_manager& m) const {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: persistent object manager not really needed.  
	But following convention is easy.  
 */
void
footprint::write_object_base(const persistent_object_manager& m,
		ostream& o) const {
	util::write_sequence(o, AS_A(const footprint_base_type&, *this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: persistent object manager not really needed.  
	But following convention is easy.  
 */
void
footprint::load_object_base(const persistent_object_manager& m,
		istream& i) {
	util::read_sequence_resize(i, AS_A(footprint_base_type&, *this));
}

//=============================================================================
}	// end namespace SPEC
}	// end namespace entity
}	// end namespace HAC

