/**
	\file "Object/type/canonical_type.cc"
	Explicit template instantiation of canonical type classes.  
	Probably better to include the .tcc where needed, 
	as this is just temporary and convenient.  
	$Id: canonical_type.cc,v 1.6 2006/02/06 01:30:51 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#define	STACKTRACE_PERSISTENTS			0 && ENABLE_STACKTRACE

#include "Object/type/canonical_type.tcc"
#include "Object/def/user_def_datatype.h"
#include "Object/def/user_def_chan.h"
#include "Object/def/process_definition.h"
#include "Object/type/data_type_reference.h"
#include "Object/type/channel_type_reference.h"
#include "Object/type/process_type_reference.h"
#include "Object/inst/subinstance_manager.h"
#include "Object/traits/proc_traits.h"
#include "common/TODO.h"

namespace HAC {
namespace entity {
//=============================================================================
// specialized definitions

/**
	This specialization should be enough to cover all datatypes, 
	built-in or user-defined.  
 */
void
canonical_definition_load_policy<datatype_definition_base>::operator () (
		const persistent_object_manager& m,
		never_ptr<const definition_type>& d) const {
	STACKTRACE_VERBOSE;
	data_type_reference::intercept_builtin_definition_hack(m, d);
	const never_ptr<const user_def_datatype>
		uddef(d.is_a<const user_def_datatype>());
	if (uddef) {
		m.load_object_once(&const_cast<user_def_datatype&>(*uddef));
	}
	// else don't bother with built-in types or parameterless types.  
}

//-----------------------------------------------------------------------------
template <>
struct unroll_port_instances_policy<datatype_definition_base> {
	void
	operator () (const canonical_generic_datatype& d, 
			const unroll_context& c,
			subinstance_manager& sub) const {
		// temporary
		// eventually will need template arguments
		data_type_reference::unroll_port_instances(
			d.get_base_def(), d.get_template_params(), c, sub);
	}
};	// end struct unroll_port_instances_policy

//-----------------------------------------------------------------------------
template <>
struct unroll_port_instances_policy<user_def_datatype> {
	void
	operator () (const canonical_user_def_data_type& d, 
			const unroll_context& c,
			subinstance_manager& sub) const {
		// temporary
		data_type_reference::unroll_port_instances(
			d.get_base_def(), d.get_template_params(), c, sub);
		// FINISH_ME(Fang);
	}
};	// end struct unroll_port_instances_policy

//-----------------------------------------------------------------------------
template <>
struct unroll_port_instances_policy<user_def_chan> {
	void
	operator () (const canonical_user_def_chan_type& d, 
			const unroll_context& c,
			subinstance_manager& sub) const {
		// temporary
		FINISH_ME(Fang);
	}
};	// end struct unroll_port_instances_policy

//-----------------------------------------------------------------------------
template <>
struct unroll_port_instances_policy<process_definition> {
	void
	operator () (const canonical_process_type& p, 
			const unroll_context& c,
			subinstance_manager& sub) const {
		// modeled after process_type_reference::unroll_port_instances()
		const port_formals_manager&
			pf(p.canonical_definition_ptr->get_port_formals());
		const unroll_context cc(p.make_unroll_context());
		pf.unroll_ports(cc, sub);
	}
};	// end struct unroll_port_instances_policy

//=============================================================================
/**
	First, this recursively assigns the subinstances inherited from
	the external context through public ports.  
	Then, it allocates the remaining (unaassigned) private subinstances.  
	\param ind the globally assigned index of this instance.  
	\pre ff is not yet initialized or assigned.  
 */
good_bool
canonical_type_footprint_frame_policy<process_definition>::
		initialize_and_assign(const canonical_process_type& cpt,
		footprint_frame& ff, state_manager& sm, 
		const port_member_context& pmc, const size_t ind) {
	STACKTRACE_VERBOSE;
	const footprint&
		f(cpt.get_base_def()->get_footprint(
			cpt.get_raw_template_params()));
	new (&ff) footprint_frame(f);	// placement construct
	f.assign_footprint_frame(ff, pmc);
	// allocate the rest with the state_manager
	ff.allocate_remaining_subinstances(f, sm,
		parent_tag_enum(class_traits<process_tag>::type_tag_enum_value),
		ind);
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
canonical_type_footprint_frame_policy<process_definition>::
		initialize_frame_pointer_only(
		const canonical_process_type& cpt, const footprint*& f) {
	const footprint&
		_f(cpt.get_base_def()->get_footprint(
			cpt.get_raw_template_params()));
	f = &_f;
}

//=============================================================================
/**
	\param cpt the canonical_process_type whose footprint is checked.  
	\param f the footprint pointer to be checked against. 
	\pre the footprint argument pointer should point to the
		looked-up footprint, according to the complete type.  
 */
void
check_footprint_policy<process_definition>::operator () (
		const canonical_process_type& cpt,
		const footprint* const f) const {
	const footprint&
		_f(cpt.get_base_def()->get_footprint(
			cpt.get_raw_template_params()));
	INVARIANT(f == &_f);
}

//=============================================================================
/**
	Copy-modified from process_definition's specialization above.  
	First, this recursively assigns the subinstances inherited from
	the external context through public ports.  
	Then, it allocates the remaining (unaassigned) private subinstances.  
	\param ind the globally assigned index of this instance.  
	\pre ff is not yet initialized or assigned.  
 */
good_bool
canonical_type_footprint_frame_policy<user_def_datatype>::
		initialize_and_assign(const canonical_user_def_data_type& cpt,
		footprint_frame& ff, state_manager& sm, 
		const port_member_context& pmc, const size_t ind) {
	STACKTRACE_VERBOSE;
	const footprint&
		f(cpt.get_base_def()->get_footprint(
			cpt.get_raw_template_params()));
	new (&ff) footprint_frame(f);	// placement construct
	f.assign_footprint_frame(ff, pmc);
	// allocate the rest with the state_manager
	ff.allocate_remaining_subinstances(f, sm,
		parent_tag_enum(
			class_traits<datastruct_tag>::type_tag_enum_value),
		ind);
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
canonical_type_footprint_frame_policy<user_def_datatype>::
		initialize_frame_pointer_only(
		const canonical_user_def_data_type& cpt, const footprint*& f) {
	const footprint&
		_f(cpt.get_base_def()->get_footprint(
			cpt.get_raw_template_params()));
	f = &_f;
}

//=============================================================================
/**
	\param cpt the canonical_process_type whose footprint is checked.  
	\param f the footprint pointer to be checked against. 
	\pre the footprint argument pointer should point to the
		looked-up footprint, according to the complete type.  
 */
void
check_footprint_policy<user_def_datatype>::operator () (
		const canonical_user_def_data_type& cpt,
		const footprint* const f) const {
	const footprint&
		_f(cpt.get_base_def()->get_footprint(
			cpt.get_raw_template_params()));
	INVARIANT(f == &_f);
}

//=============================================================================
template
canonical_user_def_data_type::canonical_type(const canonical_generic_datatype&);

template class canonical_type<datatype_definition_base>;
template class canonical_type<user_def_datatype>;
template class canonical_type<user_def_chan>;
template class canonical_type<process_definition>;
// specialized, defined in "Object/type/canonical_generic_chan_type.cc"
// template class canonical_type<channel_definition_base>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

