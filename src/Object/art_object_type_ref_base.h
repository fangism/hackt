/**
	\file "art_object_type_ref_base.h"
	Base classes for type objects.  
	$Id: art_object_type_ref_base.h,v 1.8 2005/05/08 20:50:44 fang Exp $
 */

#ifndef	__ART_OBJECT_TYPE_REF_BASE_H__
#define	__ART_OBJECT_TYPE_REF_BASE_H__

#include "macros.h"
#include "memory/pointer_classes.h"
#include "persistent.h"		// for persistent object interface

#include "art_object_definition_base.h"

namespace ART {
namespace entity {

using std::istream;
using util::persistent;
using util::persistent_object_manager;
using namespace util::memory;

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
protected:
	/**
		Optional set of template parameters with which a
		type is instantiated.  
		Types must match that of template signature.  
		Hmmm... maybe need a concrete_type class...
		distinguish type_reference from type_instance.  
		This is owned, and thus must be deleted.  
		Const?
	 */
	excl_ptr<const param_expr_list>		template_params;

protected:
	fundamental_type_reference();
public:
	explicit
	fundamental_type_reference(excl_ptr<const param_expr_list>& pl);

virtual	~fundamental_type_reference();

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o) const;

virtual never_ptr<const definition_base>
	get_base_def(void) const = 0;

	string
	template_param_string(void) const;

	string
	get_qualified_name(void) const;

	string
	hash_string(void) const;

	excl_ptr<param_expr_list>
	get_copy_template_params(void) const;

	/**
		Returns a shallow (pointer) copy or reference to
		the template parameter list.  
	 */
	never_ptr<const param_expr_list>
	get_template_params(void) const { return template_params; }

	// limits the extend to which it can be statically type-checked
	// i.e. whether parameter is resolved to a scope's formal
	bool
	is_dynamically_parameter_dependent(void) const;

	// later add dimensions and indices?

	excl_ptr<const fundamental_type_reference>
	resolve_canonical_type(void) const;

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
	may_be_equivalent(const fundamental_type_reference& t) const;

	bool
	must_be_equivalent(const fundamental_type_reference& t) const;

	// something for resolving typedefs
	// or return by value? statically would require copy constructor
	// wth, just allocate one...
	excl_ptr<const fundamental_type_reference>
	make_canonical_type_reference(void) const;

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

#endif	// __ART_OBJECT_TYPE_REF_BASE_H__

