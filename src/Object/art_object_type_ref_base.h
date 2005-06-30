/**
	\file "Object/art_object_type_ref_base.h"
	Base classes for type objects.  
	$Id: art_object_type_ref_base.h,v 1.14.4.1 2005/06/30 23:22:26 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_TYPE_REF_BASE_H__
#define	__OBJECT_ART_OBJECT_TYPE_REF_BASE_H__

#include "util/macros.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "util/persistent.h"		// for persistent object interface
#include "util/boolean_types.h"

#include "Object/art_object_util_types.h"
#include "Object/art_object_template_actuals.h"

namespace ART {
namespace parser {
class token_identifier;
}

namespace entity {
class definition_base;
class instance_collection_base;
class scopespace;
using parser::token_identifier;
using std::istream;
using util::persistent;
using util::persistent_object_manager;
using util::memory::excl_ptr;
using util::memory::never_ptr;
using util::memory::count_ptr;
using util::good_bool;

//=============================================================================
/**
	Ultimate base class of all type-references, don't know if this
	is actually useful...
 */
class type_reference_base : public persistent {
protected:
public:
	type_reference_base() : persistent() { }
virtual	~type_reference_base() { }

};	// end class type_reference_base

//=============================================================================
/**
	This class is a reference to a type (datatype, process, channel), 
	and contains optional template argments.  
	Instantiations should contain wrapper-references of this type, 
	and not direct references to type definitions.  
	This level of indirection...
	Sub-classes thereof should contain const pointers to 
	definitions of the specific classes.  

	This object is a pre-unroll type-reference, which corresponds to
	an appearance of a type reference in the source code.  
	We need a separate unroll-time type-reference, one that is actually
	created from unrolling.  
	These template parameters contained herein merely contain expressions.  
	The final unrolled template parameters must be resolved constants.  
	We only need actual type references for non-primary (not built-in)
	types.  
	Perhaps the unrolled actuals should be contained in the
	instance_collection family of classes...
 */
class fundamental_type_reference : public type_reference_base {
public:
	typedef	template_actuals::arg_list_ptr_type	template_args_ptr_type;
protected:
	/// set of template parameters passed to this type
	template_actuals			template_args;

protected:
	fundamental_type_reference();

	explicit
	fundamental_type_reference(const template_actuals&);

public:
virtual	~fundamental_type_reference();

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o) const;

virtual never_ptr<const definition_base>
	get_base_def(void) const = 0;

	string
	template_param_string(void) const;

	/**
		Returns a shallow (pointer) copy or reference to
		the template parameter list.  
	 */
	const template_actuals&
	get_template_params(void) const;

	// limits the extend to which it can be statically type-checked
	// i.e. whether parameter is resolved to a scope's formal
	bool
	is_dynamically_parameter_dependent(void) const;

	// later add dimensions and indices?

#if 0
	excl_ptr<const fundamental_type_reference>
	resolve_canonical_type(void) const;
#endif

	static	
	excl_ptr<instantiation_statement_base>
	make_instantiation_statement(
		const count_ptr<const fundamental_type_reference>& t, 
		const index_collection_item_ptr_type& d);

private:
virtual	excl_ptr<instantiation_statement_base>
	make_instantiation_statement_private(
		const count_ptr<const fundamental_type_reference>& t, 
		const index_collection_item_ptr_type& d) const = 0;

public:
virtual	excl_ptr<instance_collection_base>
	make_instance_collection(const never_ptr<const scopespace> s, 
		const token_identifier& id, const size_t d) const = 0;

public:
	bool
	may_be_collectibly_type_equivalent(
		const fundamental_type_reference& t) const;

	bool
	must_be_collectibly_type_equivalent(
		const fundamental_type_reference& t) const;

	bool
	may_be_connectibly_type_equivalent(
		const fundamental_type_reference& t) const;

	bool
	must_be_connectibly_type_equivalent(
		const fundamental_type_reference& t) const;

	// something for resolving typedefs
	// or return by value? statically would require copy constructor
	// wth, just allocate one...
/**
	NOTE: this requires that template arguments have been resolved
	to constants!!!
 */
#define	MAKE_CANONICAL_TYPE_REFERENCE_PROTO				\
	count_ptr<const fundamental_type_reference>			\
	make_canonical_type_reference(void) const

protected:
virtual	MAKE_CANONICAL_TYPE_REFERENCE_PROTO = 0;

public:
virtual	good_bool
	unroll_register_complete_type(void) const;

protected:
	void
	collect_transient_info_base(persistent_object_manager& m) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class fundamental_type_reference

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_TYPE_REF_BASE_H__

