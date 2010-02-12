/**
	\file "Object/lang/SPEC_footprint.cc"
	$Id: SPEC_footprint.cc,v 1.5.88.4 2010/02/12 18:20:33 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <set>

#include "Object/def/footprint.h"
#include "Object/lang/SPEC_footprint.h"
#include "Object/lang/cflat_visitor.h"
#include "Object/inst/instance_pool.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/global_channel_entry.h"
#include "Object/inst/state_instance.h"
#include "Object/inst/instance_pool.h"
#include "Object/common/dump_flags.h"
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
#include "Object/global_entry_context.h"
#endif
#include "common/TODO.h"
#include "util/indent.h"
#include "util/IO_utils.h"
#include "util/persistent_object_manager.tcc"
#include "util/persistent_functor.tcc"
#include "util/stacktrace.h"

namespace util {
using HAC::entity::SPEC::footprint_directive;
//=============================================================================
// value_writer and value_reader specializations for footprint_directive
#if 0
template <>
struct value_writer<footprint_directive> {
	ostream& os;
	value_writer(ostream& o) : os(o) { }

	void
	operator () (const footprint_directive& d) const {
		d.write_object_base(os);
	}
};	// end struct value_writer

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct value_reader<footprint_directive> {
	istream& is;
	value_reader(istream& i) : is(i) { }

	void
	operator () (footprint_directive& d) const {
		d.load_object_base(is);
	}

};	// end struct value_reader
#endif

//=============================================================================
}	// end namespace util

namespace HAC {
namespace entity {
namespace SPEC {
#include "util/using_ostream.h"
#if 0
using util::write_value;
using util::read_value;
#endif
using util::auto_indent;
using PRS::cflat_visitor;

//=============================================================================
// class SPEC::footprint_directive method definitions

void
footprint_directive::accept(spec_visitor& v) const {
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	FINISH_ME(Fang);
#else
	v.visit(*this);
#endif
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
	o << d.name;
	directive_base::dump_params(d.params, o);
	o << '(';
	typedef	footprint_directive::nodes_type::const_iterator	const_iterator;
	const_iterator i(d.nodes.begin());
	const const_iterator e(d.nodes.end());
	INVARIANT(i!=e);
	directive_base::dump_node_group(*i, o, np);
	for (++i; i!=e; ++i) {
		directive_base::dump_node_group(*i, o << ',', np);
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
	const node_pool_type& bpool(f.get_instance_pool<bool_tag>());
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
	for_each(begin(), end(), util::persistent_collector_ref(m));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: persistent object manager not really needed.  
	But following convention is easy.  
	This implementation requires that value_writer and value_reader
		are specialized for footprint_directive.
 */
void
footprint::write_object_base(const persistent_object_manager& m,
		ostream& o) const {
	util::write_persistent_sequence(m, o,
		AS_A(const footprint_base_type&, *this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: persistent object manager not really needed.  
	But following convention is easy.  
	This implementation requires that value_writer and value_reader
		are specialized for footprint_directive.
 */
void
footprint::load_object_base(const persistent_object_manager& m,
		istream& i) {
	util::read_persistent_sequence_resize(m, i,
		AS_A(footprint_base_type&, *this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::accept(spec_visitor& v) const {
	v.visit(*this);
}

//=============================================================================
}	// end namespace SPEC
}	// end namespace entity
}	// end namespace HAC

