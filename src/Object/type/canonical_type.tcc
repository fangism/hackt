/**
	\file "Object/type/canonical_type.tcc"
	Implementation of canonical_type template class.  
	$Id: canonical_type.tcc,v 1.19 2011/03/23 00:36:18 fang Exp $
 */

#ifndef	__HAC_OBJECT_TYPE_CANONICAL_TYPE_TCC__
#define	__HAC_OBJECT_TYPE_CANONICAL_TYPE_TCC__

#include <iostream>
#include <vector>
#include "Object/type/canonical_type.hh"
#include "Object/traits/class_traits_fwd.hh"
#include "Object/def/typedef_base.hh"
#include "Object/def/template_formals_manager.hh"
#include "Object/type/template_actuals.hh"
#include "Object/expr/const_param_expr_list.hh"
#include "Object/unroll/unroll_context.hh"
#include "Object/global_entry.hh"
#include "util/persistent_object_manager.tcc"
#include "common/TODO.hh"
#include "util/stacktrace.hh"

namespace HAC {
namespace entity {
#include "util/using_ostream.hh"
//=============================================================================
// struct canonical_definition_load_policy method definitions

CANONICAL_TYPE_TEMPLATE_SIGNATURE
void
canonical_definition_load_policy<DefType>::operator () (
		const persistent_object_manager& m,
		never_ptr<const definition_type>& d) const {
	if (d) {
		m.load_object_once(&const_cast<definition_type&>(*d));
	}
}

//=============================================================================
// class canonical_type method definitions

CANONICAL_TYPE_TEMPLATE_SIGNATURE
CANONICAL_TYPE_CLASS::canonical_type() :
		base_type(), canonical_definition_ptr(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CANONICAL_TYPE_TEMPLATE_SIGNATURE
CANONICAL_TYPE_CLASS::canonical_type(const canonical_definition_ptr_type d) :
		base_type(), canonical_definition_ptr(d) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(canonical_definition_ptr);
	INVARIANT(!canonical_definition_ptr.template is_a<const typedef_base>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CANONICAL_TYPE_TEMPLATE_SIGNATURE
CANONICAL_TYPE_CLASS::canonical_type(const canonical_definition_ptr_type d,
		const param_list_ptr_type& p) :
		base_type(p), canonical_definition_ptr(d) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(canonical_definition_ptr);
	INVARIANT(!canonical_definition_ptr.template is_a<const typedef_base>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param p relaxed template arguments to append.  
	TODO: check correct number of parameters?
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
CANONICAL_TYPE_CLASS::canonical_type(const this_type& t,
		const const_param_list_ptr_type& p) :
		base_type(t.param_list_ptr, p),
		canonical_definition_ptr(t.canonical_definition_ptr) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(canonical_definition_ptr);
	INVARIANT(!canonical_definition_ptr.template is_a<const typedef_base>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CANONICAL_TYPE_TEMPLATE_SIGNATURE
CANONICAL_TYPE_CLASS::canonical_type(const canonical_definition_ptr_type d,
		const template_actuals& p) :
		base_type(p.make_const_param_list()),
		canonical_definition_ptr(d) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(canonical_definition_ptr);
	INVARIANT(!canonical_definition_ptr.template is_a<const typedef_base>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: use checked_cast<>
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
template <class DefType2>
CANONICAL_TYPE_CLASS::canonical_type(
		const canonical_type<DefType2>& c) :
		base_type(c.get_raw_template_params()), 
		canonical_definition_ptr(c.get_base_def()
			.template is_a<const canonical_definition_type>()) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(canonical_definition_ptr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CANONICAL_TYPE_TEMPLATE_SIGNATURE
CANONICAL_TYPE_CLASS::~canonical_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
// not needed yet
CANONICAL_TYPE_TEMPLATE_SIGNATURE
ostream&
CANONICAL_TYPE_CLASS::what(ostream& o) const {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Should print template actuals in the same manner as
	template_actuals::dump().
	NOTE: canonical_definition_ptr may be incomplete if
		we're still in the middle of instantiating it!
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
ostream&
CANONICAL_TYPE_CLASS::dump(ostream& o) const {
	// STACKTRACE_VERBOSE;
if (canonical_definition_ptr) {
	o << canonical_definition_ptr->get_qualified_name();	// get_name()
	return base_type::dump_template_args(o,
		canonical_definition_ptr->num_strict_formals());
} else {
	return o << "(incomplete type)";
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a generic fundamental type reference from a canonical one.  
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
count_ptr<const typename CANONICAL_TYPE_CLASS::type_reference_type>
CANONICAL_TYPE_CLASS::make_type_reference(void) const {
	typedef	count_ptr<const type_reference_type>	return_type;
	return canonical_definition_ptr->
		make_canonical_fundamental_type_reference(
			this->get_template_params())
			.template is_a<const type_reference_type>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is implemented for the sake of transition compatibility.  
	\return template params split into strict and relaxed lists.  
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
template_actuals
CANONICAL_TYPE_CLASS::get_template_params(void) const {
	if (!param_list_ptr)
		return template_actuals();
	const size_t num_strict =
		canonical_definition_ptr->num_strict_formals();
	return base_type::get_template_params(num_strict);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Whether or not this type reference is strict or relaxed.  
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
bool
CANONICAL_TYPE_CLASS::is_strict(void) const {
	NEVER_NULL(canonical_definition_ptr);
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
CANONICAL_TYPE_TEMPLATE_SIGNATURE
ostream&
CANONICAL_TYPE_CLASS::type_mismatch_error(ostream& o, 
		const this_type& l, const this_type& r) {
	o << "ERROR: container types mismatch!" << endl;
	l.dump(o << "\tgot: ") << endl;
	r.dump(o << "\tand: ") << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	This is only applicable to process_definitions.
	Don't delete this, good for reference.  
	This can't be instantiated with datatype_definition_base...
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
const footprint&
CANONICAL_TYPE_CLASS::get_definition_footprint(void) const {
	return canonical_definition_ptr->get_footprint(param_list_ptr);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates definition footprint (and those of dependent types, 
	recursively) from the bottom-up.  
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
good_bool
CANONICAL_TYPE_CLASS::create_definition_footprint(const footprint& top) const {
	STACKTRACE_VERBOSE;
	NEVER_NULL(canonical_definition_ptr);
	if (this->is_strict()) {
		canonical_definition_ptr->register_complete_type(
			param_list_ptr);
		return canonical_definition_ptr->create_complete_type(
			param_list_ptr, top);
	} else return good_bool(true);
	// delay the type-completion of relaxed types until
	// relaxed template parameters are given.  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	copy-modified from
	fundamental_type_reference::must_be_collectibly_type_equivalent
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
bool
CANONICAL_TYPE_CLASS::must_be_collectibly_type_equivalent(
		const this_type& t) const {
	if (this->canonical_definition_ptr != t.canonical_definition_ptr) {
		// type_mismatch_error(cerr, *this, t);
		return false;
	} else {
		if (this->param_list_ptr && t.param_list_ptr) {
			const size_t num_strict =
				canonical_definition_ptr->num_strict_formals();
			// only strict parameters must match
			const bool ret(this->param_list_ptr->
				must_be_equivalent(*t.param_list_ptr, 
					num_strict));
			if (!ret) {
				type_mismatch_error(cerr, *this, t);
			}
			return ret;
		} else {
			// not sure if this is true
			INVARIANT(!this->canonical_definition_ptr);
			INVARIANT(!t.canonical_definition_ptr);
			return true;
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	copy-modified from
	fundamental_type_reference::must_be_connectibly_type_equivalent
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
bool
CANONICAL_TYPE_CLASS::must_be_connectibly_type_equivalent(
		const this_type& t) const {
	if (this->canonical_definition_ptr != t.canonical_definition_ptr) {
		// type_mismatch_error(cerr, *this, t);
		return false;
	} else {
		if (this->param_list_ptr && t.param_list_ptr) {
			// all parameters must match
			const bool ret(this->param_list_ptr->
				must_be_equivalent(*t.param_list_ptr));
			if (!ret) {
				type_mismatch_error(cerr, *this, t);
			}
			return ret;
		} else {
			// not sure if this is true
			INVARIANT(!this->canonical_definition_ptr);
			INVARIANT(!t.canonical_definition_ptr);
			return true;
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CANONICAL_TYPE_TEMPLATE_SIGNATURE
good_bool
CANONICAL_TYPE_CLASS::unroll_port_instances(target_context& c, 
		subinstance_manager& sub) const {
	STACKTRACE_VERBOSE;
	return unroll_port_instances_policy<DefType>()(*this, c, sub);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NB: don't call this on a temporarily constructed canonical_type!
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
void
CANONICAL_TYPE_CLASS::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	if (canonical_definition_ptr) {
		canonical_definition_ptr->collect_transient_info(m);
	}
	if (param_list_ptr) {
		STACKTRACE_INDENT_PRINT("param_list_ptr = " <<
			&*param_list_ptr << endl);
		param_list_ptr->collect_transient_info(m);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CANONICAL_TYPE_TEMPLATE_SIGNATURE
void
CANONICAL_TYPE_CLASS::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	m.write_pointer(o, canonical_definition_ptr);
	m.write_pointer(o, param_list_ptr);
	const char d = direction;
	util::write_value(o, d);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	AAAARRRRGGG.... CALL THE HACK POLICE.
	data_type_reference::load_object employs a dirty hack for dealing
	with built-in definitions (until we implement them the right way).
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
void
CANONICAL_TYPE_CLASS::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	STACKTRACE_PERSISTENT_VERBOSE;
	m.read_pointer(i, canonical_definition_ptr);
	m.read_pointer(i, param_list_ptr);
	char d;
	util::read_value(i, d);
	direction = direction_type(d);
	canonical_definition_load_policy<DefType>()
		(m, canonical_definition_ptr);
{
	// TODO: also do this in canonical_generic_chan_type.  
	// need to load expression list because canonical type might
	// be used right away, and thus expressions cannot be deferred.  
	// FYI: this is called by
	// instance_alias_info_actuals::restore_canonical_footprint().
	if (param_list_ptr)
		m.load_object_once(&const_cast<param_list_type&>(
			*param_list_ptr));
}
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TYPE_CANONICAL_TYPE_TCC__

