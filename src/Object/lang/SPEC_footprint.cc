/**
	\file "Object/lang/SPEC_footprint.cc"
	$Id: SPEC_footprint.cc,v 1.10 2010/08/24 21:05:44 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <set>

#include "Object/def/footprint.hh"
#include "Object/lang/SPEC_footprint.hh"
#include "Object/lang/cflat_visitor.hh"
#include "Object/lang/SPEC_registry.hh"
#include "Object/inst/instance_pool.hh"
#include "Object/inst/alias_empty.hh"
#include "Object/inst/instance_alias_info.hh"
#include "Object/global_channel_entry.hh"
#include "Object/inst/state_instance.hh"
#include "Object/inst/instance_pool.hh"
#include "Object/common/dump_flags.hh"
#include "Object/global_entry_context.hh"
#include "common/TODO.hh"
#include "util/indent.hh"
#include "util/IO_utils.hh"
#include "util/persistent_object_manager.tcc"
#include "util/persistent_functor.tcc"
#include "util/stacktrace.hh"

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
#include "util/using_ostream.hh"
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
		const entity::footprint& f) {
	o << d.name;
	directive_base::dump_params(d.params, o);
	cflat_spec_registry_type::const_iterator
		df(cflat_spec_registry.find(d.name));
	INVARIANT(df != cflat_spec_registry.end());
	o << '(';
switch (df->second.type_enum) {
case META_TYPE_BOOL:
#if PRIVATE_MEMBER_REFERENCES
	d.dump_groups<bool_tag>(o, f);
#else
	d.dump_groups(o, f.get_instance_pool<bool_tag>());
#endif
	break;
case META_TYPE_PROCESS:
#if PRIVATE_MEMBER_REFERENCES
	d.dump_groups<process_tag>(o, f);
#else
	d.dump_groups(o, f.get_instance_pool<process_tag>());
#endif
	break;
default:
	DIE;
}
	return o << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints all unrolled spec directives.
 */
ostream&
footprint::dump(ostream& o, const entity::footprint& f) const {
if (size()) {
	o << auto_indent << "resolved specs:" << endl;
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; ++i) {
		dump_directive(*i, o << auto_indent, f) << endl;
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

