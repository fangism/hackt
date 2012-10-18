/**
	\file "Object/type/canonical_type.hh"
	$Id: canonical_type.hh,v 1.12 2010/04/07 00:12:57 fang Exp $
 */

#ifndef	__HAC_OBJECT_TYPE_CANONICAL_TYPE_H__
#define	__HAC_OBJECT_TYPE_CANONICAL_TYPE_H__

#include "Object/type/canonical_type_fwd.hh"
#include "Object/type/canonical_type_base.hh"
#include "util/boolean_types.hh"
#include "util/memory/excl_ptr.hh"

namespace HAC {
namespace entity {
class unroll_context;
class definition_base;
class template_actuals;
class subinstance_manager;
class footprint;
class footprint_frame;
class state_manager;
using util::memory::never_ptr;
using util::good_bool;

//=============================================================================

#define	CANONICAL_TYPE_TEMPLATE_SIGNATURE	template <class DefType>
#define	CANONICAL_TYPE_CLASS			canonical_type<DefType>

/**
	A canonical type references a non-typedef definition
	with only constant template parameters, if applicable.  
	SHould be implemented with same interface as 
		fundamental_type_reference.  
	Q: use const_param_expr_list or pointer?
	A: depends on whether or not this is expected to be copied much.  
		Since it will be used with unroll context actuals, 
		it should probably be a pointer.  
		This will also be used in footprint lookups.  
	TODO: import more standard interfaces from fundamental_type_reference
	TODO: use defintion class traits to determine 
		conversion policy and whether or not definition needs 
		assertion check.  
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
class canonical_type : public canonical_type_base {
	typedef	CANONICAL_TYPE_CLASS		this_type;
	typedef	canonical_type_base		base_type;
public:
	/**
		This must be a non-typedef definition.  
	 */
	typedef	DefType				canonical_definition_type;
	/**
		Generic fundamental type reference derivative.  
	 */
	typedef	typename canonical_definition_type::type_reference_type
						type_reference_type;
	typedef	never_ptr<const canonical_definition_type>
						canonical_definition_ptr_type;
private:
	canonical_definition_ptr_type		canonical_definition_ptr;
public:
	canonical_type();

	explicit
	canonical_type(const canonical_definition_ptr_type);

	canonical_type(const canonical_definition_ptr_type, 
		const param_list_ptr_type&);

	// used to append relaxed template arguments
	canonical_type(const this_type&, const const_param_list_ptr_type&);

	canonical_type(const canonical_definition_ptr_type, 
		const template_actuals&);

	template <class DefType2>
	canonical_type(const canonical_type<DefType2>&);

	// default copy-constructor suffices

	~canonical_type();

	// do we need a generic version?
	canonical_definition_ptr_type
	get_base_def(void) const { return this->canonical_definition_ptr; }

	template_actuals
	get_template_params(void) const;

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	count_ptr<const type_reference_type>
	make_type_reference(void) const;

	bool
	is_strict(void) const;

	bool
	is_relaxed(void) const { return !this->is_strict(); }

	operator bool () const { return this->canonical_definition_ptr; }

	bool
	must_be_collectibly_type_equivalent(const this_type&) const;

	bool
	must_be_connectibly_type_equivalent(const this_type&) const;

	static
	ostream&
	type_mismatch_error(ostream&, const this_type&, const this_type&);

	good_bool
	unroll_port_instances(const unroll_context&,
		subinstance_manager&) const;

#if 0
	// don't delete, useful for copying
	const footprint&
	get_definition_footprint(void) const;
#endif

	good_bool
	create_definition_footprint(const footprint&) const;

	using base_type::combine_relaxed_actuals;
	using base_type::match_relaxed_actuals;

public:
// object persistence
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

private:
	friend struct unroll_port_instances_policy<DefType>;

};	// end class canonical_type

//=============================================================================
/**
	Default action: no-op
 */
template <class DefType>
struct canonical_definition_load_policy {
	typedef	DefType			definition_type;
	void
	operator () (const persistent_object_manager&, 
		never_ptr<const definition_type>&) const;
};	// end struct canonical_definition_load_policy

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Specialization defined "Object/type/canonical_type.cc"
 */
template <>
struct canonical_definition_load_policy<datatype_definition_base> {
	typedef	datatype_definition_base		definition_type;
	void
	operator () (const persistent_object_manager&, 
		never_ptr<const definition_type>&) const;
};	// end struct canonical_definition_load_policy

//-----------------------------------------------------------------------------
template <class DefType>
struct canonical_type_footprint_frame_policy {

	static
	good_bool
	initialize_and_assign(const canonical_type<DefType>&,
			const footprint_frame&) {
		return good_bool(true);
	}
};      // end struct initialize_footprint_frame_policy

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct canonical_type_footprint_frame_policy<process_definition> {

	static
	good_bool
	initialize_and_assign(const canonical_process_type&, footprint_frame&);
};      // end struct initialize_footprint_frame_policy

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct canonical_type_footprint_frame_policy<user_def_datatype> {

	static
	good_bool
	initialize_and_assign(const canonical_user_def_data_type&,
		footprint_frame&);
};      // end struct initialize_footprint_frame_policy

//-----------------------------------------------------------------------------
template <class DefType>
struct check_footprint_policy {
	void
	operator () (const canonical_type<DefType>&,
		const footprint* const) const {
	// no-op
	}
};	// end struct check_footprint_policy

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct check_footprint_policy<process_definition> {
	void
	operator () (const canonical_process_type&,
		const footprint* const) const;
};	// end struct check_footprint_policy

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct check_footprint_policy<user_def_datatype> {
	void
	operator () (const canonical_user_def_data_type&,
		const footprint* const) const;
};	// end struct check_footprint_policy

//=============================================================================
// possilbly specialize built-in data types, but require same interface

// possilbly specialize built-in channel type, but require same interface

//=============================================================================
}	// end namespace entity
}	// end namespace HAC


#endif	// __HAC_OBJECT_TYPE_CANONICAL_TYPE_H__

