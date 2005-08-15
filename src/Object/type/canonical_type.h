/**
	\file "Object/type/canonical_type.h"
	$Id: canonical_type.h,v 1.1.2.3.2.3 2005/08/15 05:39:26 fang Exp $
 */

#ifndef	__OBJECT_TYPE_CANONICAL_TYPE_H__
#define	__OBJECT_TYPE_CANONICAL_TYPE_H__

#include "Object/type/canonical_type_fwd.h"
#include "Object/type/canonical_type_base.h"
#include "util/memory/excl_ptr.h"

namespace ART {
namespace entity {
class unroll_context;
class definition_base;
class template_actuals;
class subinstance_manager;
using util::memory::never_ptr;

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

	unroll_context
	make_unroll_context(void) const;

	void
	unroll_port_instances(const unroll_context&,
		subinstance_manager&) const;

	// like fundamental_type_reference::unroll_register_complete_type()
	void
	register_definition_footprint(void) const;

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
		never_ptr<const definition_type>&) const { }
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

//=============================================================================
// possilbly specialize built-in data types, but require same interface

// possilbly specialize built-in channel type, but require same interface

//=============================================================================
}	// end namespace entity
}	// end namespace ART


#endif	// __OBJECT_TYPE_CANONICAL_TYPE_H__

