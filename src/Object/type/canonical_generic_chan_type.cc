/**
	\file "Object/type/canonical_generic_type.tcc"
	Implementation of canonical_type template class.  
	$Id: canonical_generic_chan_type.cc,v 1.13 2011/04/02 01:46:11 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#define	STACKTRACE_CONSTRUCTORS			(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_DUMPS			(0 && ENABLE_STACKTRACE)

#include "Object/type/canonical_generic_chan_type.hh"

#include <iostream>
#include "Object/type/canonical_type.hh"
#include "Object/type/builtin_channel_type_reference.hh"
#include "Object/traits/class_traits_fwd.hh"
#include "Object/def/typedef_base.hh"
#include "Object/def/datatype_definition_base.hh"
#include "Object/def/template_formals_manager.hh"
#include "Object/type/template_actuals.hh"
#include "Object/expr/const_param_expr_list.hh"
#include "Object/unroll/unroll_context.hh"
#include "util/persistent_object_manager.tcc"
#include "util/indent.hh"
#include "common/TODO.hh"
#include "common/ICE.hh"
#include "util/stacktrace.hh"

namespace HAC {
namespace entity {
#include "util/using_ostream.hh"
using util::write_value;
using util::read_value;

//=============================================================================
// class canonical_type method definitions

canonical_generic_chan_type::canonical_type() :
		base_type(), canonical_definition_ptr(NULL), 
		datatype_list() {
	// what is direction? don't care
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
canonical_generic_chan_type::canonical_type(
		const canonical_definition_ptr_type d, 
		const direction_type dir) :
		base_type(), canonical_definition_ptr(d), datatype_list() {
	NEVER_NULL(canonical_definition_ptr);
	INVARIANT(!canonical_definition_ptr.is_a<const typedef_base>());
	set_direction(dir);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
canonical_generic_chan_type::canonical_type(
		const canonical_definition_ptr_type d,
		const param_list_ptr_type& p, 
		const direction_type dir) :
		base_type(p), canonical_definition_ptr(d), datatype_list() {
	NEVER_NULL(canonical_definition_ptr);
	INVARIANT(!canonical_definition_ptr.is_a<const typedef_base>());
	set_direction(dir);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
canonical_generic_chan_type::canonical_type(
		const this_type& d,
		const const_param_list_ptr_type& p) :
		base_type(), canonical_definition_ptr(), datatype_list() {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
canonical_generic_chan_type::canonical_type(
		const canonical_definition_ptr_type d,
		const template_actuals& p,
		const direction_type dir) :
		base_type(p.make_const_param_list()),
		canonical_definition_ptr(d),
		datatype_list() {
	NEVER_NULL(canonical_definition_ptr);
	INVARIANT(!canonical_definition_ptr.is_a<const typedef_base>());
	set_direction(dir);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	TODO: use checked_cast<>
 */
template <class DefType2>
canonical_generic_chan_type::canonical_type(
		const canonical_type<DefType2>& c) :
		base_type(c.get_raw_template_params()), 
		canonical_definition_ptr(c.get_base_def()
			.is_a<const canonical_definition_type>()), 
		datatype_list() {
	NEVER_NULL(canonical_definition_ptr);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
canonical_generic_chan_type::~canonical_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
// not needed yet

ostream&
canonical_generic_chan_type::what(ostream& o) const {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: what if built-in channel type has void data type!?!?
 */
canonical_generic_chan_type::operator bool () const {
	return canonical_definition_ptr || datatype_list.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Should print template actuals in the same manner as
	template_actuals::dump().
	In the built-in channel type case, model after
		builtin_channel_type_reference::dump().
 */
ostream&
canonical_generic_chan_type::dump(ostream& o) const {
#if STACKTRACE_DUMPS
	STACKTRACE_VERBOSE;
#endif
	if (canonical_definition_ptr) {
#if STACKTRACE_DUMPS
		STACKTRACE_INDENT_PRINT("user-def-chan" << endl);
#endif
		o << canonical_definition_ptr->get_qualified_name();
		base_type::dump_template_args(o,
			canonical_definition_ptr->num_strict_formals());
		return channel_type_reference_base::dump_direction(o,direction);
	} else {
		typedef	datatype_list_type::const_iterator	const_iterator;
#if STACKTRACE_DUMPS
		STACKTRACE_INDENT_PRINT("builtin-def-chan" << endl);
#endif
		o << "chan";
		channel_type_reference_base::dump_direction(o, direction);
		o << '(';
		INVARIANT(datatype_list.size());
		const_iterator i(datatype_list.begin());
		const const_iterator e(datatype_list.end());
		i->dump(o);
		for (i++; i!=e; i++) {
			i->dump(o << ", ");
		}
		return o << ')';
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a generic fundamental type reference from a canonical one.  
 */
count_ptr<const canonical_generic_chan_type::type_reference_type>
canonical_generic_chan_type::make_type_reference(void) const {
	if (canonical_definition_ptr) {
		return canonical_definition_ptr->
			make_canonical_fundamental_type_reference(
				get_template_params())
				.is_a<const type_reference_type>();
	} else {
		typedef	count_ptr<builtin_channel_type_reference> return_type;
		const return_type ret(new builtin_channel_type_reference);
		typedef	datatype_list_type::const_iterator	const_iterator;
		const_iterator i(datatype_list.begin());
		const const_iterator e(datatype_list.begin());
		for ( ; i!=e; i++) {
			ret->add_datatype(i->make_type_reference());
		}
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is implemented for the sake of transition compatibility.  
	\return template params split into strict and relaxed lists.  
 */
template_actuals
canonical_generic_chan_type::get_template_params(void) const {
	if (!canonical_definition_ptr || !param_list_ptr)
		return template_actuals();
	const size_t num_strict =
		canonical_definition_ptr->num_strict_formals();
	return base_type::get_template_params(num_strict);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Whether or not this type reference is strict or relaxed.  
 */
bool
canonical_generic_chan_type::is_strict(void) const {
	if (!canonical_definition_ptr) {
		return true;
	}
	const template_formals_manager&
		tfm(canonical_definition_ptr->get_template_formals_manager());
	const bool expects = tfm.has_relaxed_formals();
	const bool has = (param_list_ptr) ?
		param_list_ptr->size() > tfm.num_strict_formals() : false;
	if (has) {
		INVARIANT(expects);
		return true;
	} else {
		return !expects;
	}
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Standard error message for mismatched types.  
 */
ostream&
canonical_generic_chan_type::type_mismatch_error(ostream& o, 
		const this_type& l, const this_type& r) {
	o << "ERROR: types mismatch!" << endl;
	l.dump(o << "\tgot: ") << endl;
	r.dump(o << "\tand: ") << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	copy-modified from fundamental_type_reference::must_be_collectibly_type_equivalent
 */
bool
canonical_generic_chan_type::must_be_collectibly_type_equivalent(
		const this_type& t) const {
if (canonical_definition_ptr) {
	if (canonical_definition_ptr != t.canonical_definition_ptr) {
		// type_mismatch_error(cerr, *this, t);
		return false;
	} else {
		if (param_list_ptr && t.param_list_ptr) {
			const size_t num_strict =
				canonical_definition_ptr->num_strict_formals();
			// only strict parameters must match
			const bool ret(param_list_ptr->
				must_be_equivalent(*t.param_list_ptr, 
					num_strict));
			if (!ret) {
				type_mismatch_error(cerr, *this, t);
			}
			return ret;
		} else if (!param_list_ptr && !t.param_list_ptr) {
			return true;
		} else {
			FINISH_ME(Fang);
			return false;
		}
	}
} else {
	typedef	datatype_list_type::const_iterator	const_iterator;
	if (t.canonical_definition_ptr)
		return false;
	if (datatype_list.size() != t.datatype_list.size())
		return false;
	const_iterator i(datatype_list.begin());
	const_iterator j(t.datatype_list.begin());
	const const_iterator e(datatype_list.end());
	for ( ; i!=e; i++, j++) {
		if (!i->must_be_collectibly_type_equivalent(*j))
			return false;
	}
	// else everything matches
	return true;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	copy-modified from fundamental_type_reference::must_be_connectibly_type_equivalent
 */
bool
canonical_generic_chan_type::must_be_connectibly_type_equivalent(
		const this_type& t) const {
if (canonical_definition_ptr) {
	if (canonical_definition_ptr != t.canonical_definition_ptr) {
		// type_mismatch_error(cerr, *this, t);
		return false;
	} else {
		if (param_list_ptr && t.param_list_ptr) {
			// all parameters must match
			const bool ret(param_list_ptr->
				must_be_equivalent(*t.param_list_ptr));
			if (!ret) {
				type_mismatch_error(cerr, *this, t);
			}
			return ret;
		} else if (!param_list_ptr && !t.param_list_ptr) {
			return true;
		} else {
			return false;
		}
	}
} else {
	typedef	datatype_list_type::const_iterator	const_iterator;
	if (t.canonical_definition_ptr)
		return false;
	if (datatype_list.size() != t.datatype_list.size())
		return false;
	const_iterator i(datatype_list.begin());
	const_iterator j(t.datatype_list.begin());
	const const_iterator e(datatype_list.end());
	for ( ; i!=e; i++, j++) {
		if (!i->must_be_connectibly_type_equivalent(*j))
			return false;
	}
	// else everything matches
	return true;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Are channel members considered ports?
	They cannot be referenced as subinstances, only as fields 
	in CHP communcation statements.  
	For now, just no-op.  
	TODO: finalize the behavior of this.  
 */
good_bool
canonical_generic_chan_type::unroll_port_instances(
		const unroll_context& c, subinstance_manager& sub) const {
#if 0
	FINISH_ME(Fang);
#else
if (canonical_definition_ptr) {
} else {
}
#endif
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Channels don't have footprints yet...
	but when they do... TODO!
 */
good_bool
canonical_generic_chan_type::create_definition_footprint(
		const footprint& top) const {
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
canonical_generic_chan_type::collect_transient_info_base(
		persistent_object_manager& m) const {
	if (canonical_definition_ptr)
		canonical_definition_ptr->collect_transient_info(m);
	if (param_list_ptr)
		param_list_ptr->collect_transient_info(m);
{
	typedef	datatype_list_type::const_iterator	const_iterator;
	const_iterator i(datatype_list.begin());
	const const_iterator e(datatype_list.end());
	for ( ; i!=e; i++) {
		i->collect_transient_info_base(m);
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
canonical_generic_chan_type::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, canonical_definition_ptr);
	m.write_pointer(o, param_list_ptr);
{
	const char d = direction;
	write_value(o, d);
	typedef	datatype_list_type::const_iterator	const_iterator;
	write_value(o, datatype_list.size());
	const_iterator i(datatype_list.begin());
	const const_iterator e(datatype_list.end());
	for ( ; i!=e; i++) {
		i->write_object_base(m, o);
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
canonical_generic_chan_type::load_object_base(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, canonical_definition_ptr);
	m.read_pointer(i, param_list_ptr);
{
	char d;
	read_value(i, d);
	direction = direction_type(d);
	typedef	datatype_list_type::iterator	iterator;
	size_t size;
	read_value(i, size);
	datatype_list.resize(size);	// vector only
	iterator iter(datatype_list.begin());
	size_t j = 0;
	for ( ; j<size; j++, iter++) {
		iter->load_object_base(m, i);
	}
}
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

