/**
	\file "Object/type/canonical_type.cc"
	Explicit template instantiation of canonical type classes.  
	Probably better to include the .tcc where needed, 
	as this is just temporary and convenient.  
	$Id: canonical_type.cc,v 1.20 2010/04/07 00:12:56 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#define	STACKTRACE_PERSISTENTS			(0 && ENABLE_STACKTRACE)

#include "Object/type/canonical_type.tcc"
#include "Object/def/user_def_datatype.hh"
#include "Object/def/user_def_chan.hh"
#include "Object/def/process_definition.hh"
#include "Object/type/data_type_reference.hh"
#include "Object/type/channel_type_reference.hh"
#include "Object/type/process_type_reference.hh"
#include "Object/inst/subinstance_manager.hh"
#include "Object/traits/proc_traits.hh"
#include "Object/traits/struct_traits.hh"
#include "Object/def/footprint.hh"
#include "Object/devel_switches.hh"
#include "common/TODO.hh"
#include "common/ICE.hh"

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
	good_bool
	operator () (const canonical_generic_datatype& d, 
			target_context& c,
			subinstance_manager& sub) const {
		// temporary
		// eventually will need template arguments
		data_type_reference::unroll_port_instances(
			d.get_base_def(), d.get_raw_template_params(), c, sub);
		return good_bool(true);
	}
};	// end struct unroll_port_instances_policy

//-----------------------------------------------------------------------------
template <>
struct unroll_port_instances_policy<user_def_datatype> {
	good_bool
	operator () (const canonical_user_def_data_type& d, 
			target_context& c,
			subinstance_manager& sub) const {
		// temporary
		data_type_reference::unroll_port_instances(
			d.get_base_def(), d.get_raw_template_params(), c, sub);
		// FINISH_ME(Fang);
		return good_bool(true);
	}
};	// end struct unroll_port_instances_policy

//-----------------------------------------------------------------------------
template <>
struct unroll_port_instances_policy<user_def_chan> {
	good_bool
	operator () (const canonical_user_def_chan_type& d, 
			target_context& c,
			subinstance_manager& sub) const {
		// temporary
		FINISH_ME(Fang);
		return good_bool(false);
	}
};	// end struct unroll_port_instances_policy

//-----------------------------------------------------------------------------
/**
	Define to 1 to do a deep-copy of cached subinstance structures,
	instead of re-unrolling ports (by port_formals_manager).  
 */
template <>
struct unroll_port_instances_policy<process_definition> {
	/**
		Is this missing the top-level const footprint&?
		Ans: is now referenced inside the unroll_context.
		NOTE: (2006-12-01) now we require a dependent type to be
			complete created (not just unrolled), 
			hence the call to create_definition_footprint.  
	 */
	good_bool
	operator () (const canonical_process_type& p, 
			target_context& c,
			subinstance_manager& sub) const {
		STACKTRACE_VERBOSE;
		// template formals/actuals included in footprint already
		if (!p.create_definition_footprint(
				c.get_top_footprint()).good) {
			cerr << "Error instantiating process footprint: "
				<< endl;
			p.dump(cerr << "From canonical type: ") << endl;
			return good_bool(false);
		}
/***
	Problem: when type is incomplete, we can't access a footprint
	because relaxed actuals are missing but required.
	Solution: since read-only footprint is only needed for
		the purpose of unrolling ports.  
	Q: After implementing relaxed-template parameters, do we guarantee
		that this is only called on strict types?
	A: tests show: still needed
***/
	if (p.is_strict()) {
		STACKTRACE_INDENT_PRINT("have strict type." << endl);
		const footprint&
			f(p.canonical_definition_ptr->get_footprint(
				p.param_list_ptr));
		sub.deep_copy(f.get_port_template(), c);
		return good_bool(true);
	} else {
		STACKTRACE_INDENT_PRINT("have relaxed type." << endl);
		// unroll temporary footprint using partial template params
		NEVER_NULL(p.param_list_ptr);
		typedef	count_ptr<const dynamic_param_expr_list>
			params_ptr_type;
		const params_ptr_type d(p.param_list_ptr->to_dynamic_list());
		const template_actuals a(d, params_ptr_type(NULL));
		DECLARE_TEMPORARY_FOOTPRINT(f);
		const unroll_context cc(&f, &c);
		if (p.canonical_definition_ptr->get_template_formals_manager()
				.unroll_formal_parameters(cc, a).good) {
			sub.deep_copy(f.get_port_template(), c);	// ???
			return good_bool(true);
		} else {
			cerr << "FATAL: unexpected error unrolling temporary "
				"parameter-only footprint." << endl;
			p.dump(cerr << "with canonical type: ") << endl;
			return good_bool(false);
		}
	}
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
		footprint_frame& ff) {
	STACKTRACE_VERBOSE;
	const footprint&
		f(cpt.get_base_def()->get_footprint(
			cpt.get_raw_template_params()));
	new (&ff) footprint_frame(f);	// placement construct
	return good_bool(true);
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
	const footprint& _f
		__ATTRIBUTE_UNUSED_CTOR__((cpt.get_base_def()->get_footprint(
			cpt.get_raw_template_params()))) ;
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
		footprint_frame& ff) {
	STACKTRACE_VERBOSE;
	const footprint&
		f(cpt.get_base_def()->get_footprint(
			cpt.get_raw_template_params()));
	new (&ff) footprint_frame(f);	// placement construct
	return good_bool(true);
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
	const footprint& _f
		__ATTRIBUTE_UNUSED_CTOR__((cpt.get_base_def()->get_footprint(
			cpt.get_raw_template_params())));
	INVARIANT(f == &_f);
}

//=============================================================================
/**
	Ordering definition for built-in data types.  
 */
bool
operator < (const canonical_generic_datatype& l,
		const canonical_generic_datatype& r) {
	const never_ptr<const datatype_definition_base>
		ld(l.get_base_def()), 
		rd(r.get_base_def());
	NEVER_NULL(ld);
	NEVER_NULL(rd);
	if (ld != rd) {
		return ld->less_ordering(*rd);
	} else {
		typedef	canonical_type_base::const_param_list_ptr_type
						params_ptr_type;
		const params_ptr_type& lp(l.get_raw_template_params());
		const params_ptr_type& rp(r.get_raw_template_params());
		// NOTE: empty param lists are considered equivalent to NULL
		if (lp) {
			if (rp) {
				// compare const_param_expr_lists
				return *lp < *rp;
			} else {
				// non-NULL lp cannot possibly be < rp
				return false;
			}
		} else {
			if (rp) {
				// if rp is also empty, then lp == rp
				return !rp->size();
			} else {
				return false;
			}
		}
	}
}

//=============================================================================
template
canonical_user_def_data_type::canonical_type(const canonical_generic_datatype&);
#if ENABLE_DATASTRUCTS
template
canonical_generic_datatype::canonical_type(const canonical_user_def_data_type&);
#endif

template class canonical_type<datatype_definition_base>;
#if ENABLE_DATASTRUCTS
template class canonical_type<user_def_datatype>;
#endif
template class canonical_type<user_def_chan>;
template class canonical_type<process_definition>;
// specialized, defined in "Object/type/canonical_generic_chan_type.cc"
// template class canonical_type<channel_definition_base>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

