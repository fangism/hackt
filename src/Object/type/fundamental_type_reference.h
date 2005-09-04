/**
	\file "Object/type/fundamental_type_reference.h"
	Base classes for type objects.  
	This file originated from "Object/art_object_type_ref_base.h".
	$Id: fundamental_type_reference.h,v 1.3 2005/09/04 21:14:59 fang Exp $
 */

#ifndef	__OBJECT_TYPE_FUNDAMENTAL_TYPE_REFERENCE_H__
#define	__OBJECT_TYPE_FUNDAMENTAL_TYPE_REFERENCE_H__

#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "util/boolean_types.h"

#include "Object/type/type_reference_base.h"
#include "Object/common/util_types.h"
#include "Object/type/template_actuals.h"

namespace ART {
namespace parser {
class token_identifier;
}

namespace entity {
class definition_base;
class instance_collection_base;
class scopespace;
class subinstance_manager;	// from "Object/inst/subinstance_manager.h"
using parser::token_identifier;
using std::istream;
using util::persistent_object_manager;
using util::memory::excl_ptr;
using util::memory::never_ptr;
using util::memory::count_ptr;
using util::good_bool;

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
	typedef	fundamental_type_reference		this_type;
public:
	typedef	template_actuals::arg_list_ptr_type	template_args_ptr_type;
	typedef	template_actuals::const_arg_list_ptr_type
						const_template_args_ptr_type;
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

#if 0
	// limits the extend to which it can be statically type-checked
	// i.e. whether parameter is resolved to a scope's formal
	bool
	is_dynamically_parameter_dependent(void) const;
#endif

	bool
	is_strict(void) const;

	bool
	is_relaxed(void) const { return !is_strict(); }

	// whether or not base definition is canonical
virtual	bool
	is_canonical(void) const = 0;

	// whether or not actuals are constants
	bool
	is_resolved(void) const;

#define	MERGE_RELAXED_ACTUALS_PROTO					\
	count_ptr<const this_type>					\
	merge_relaxed_actuals(const const_template_args_ptr_type&) const

	static	
	excl_ptr<instantiation_statement_base>
	make_instantiation_statement(
		const count_ptr<const fundamental_type_reference>& t, 
		const index_collection_item_ptr_type& d, 
		const const_template_args_ptr_type&);

	static	
	excl_ptr<instantiation_statement_base>
	make_instantiation_statement(
		const count_ptr<const fundamental_type_reference>& t, 
		const index_collection_item_ptr_type& d);

private:
#define	MAKE_INSTANTIATION_STATEMENT_PRIVATE_PROTO			\
	excl_ptr<instantiation_statement_base>				\
	make_instantiation_statement_private(				\
		const count_ptr<const fundamental_type_reference>& t, 	\
		const index_collection_item_ptr_type& d, 		\
		const const_template_args_ptr_type&) const

virtual	MAKE_INSTANTIATION_STATEMENT_PRIVATE_PROTO = 0;

public:

#define	MAKE_INSTANCE_COLLECTION_PROTO					\
	excl_ptr<instance_collection_base>				\
	make_instance_collection(const never_ptr<const scopespace> s, 	\
		const token_identifier& id, const size_t d) const

virtual	MAKE_INSTANCE_COLLECTION_PROTO = 0;

#define	UNROLL_PORT_INSTANCES_PROTO					\
	void								\
	unroll_port_instances(const unroll_context&, 			\
		subinstance_manager&) const

public:
virtual	bool
	may_be_collectibly_type_equivalent(
		const fundamental_type_reference& t) const = 0;

virtual	bool
	may_be_connectibly_type_equivalent(
		const fundamental_type_reference& t) const = 0;

#define	TYPE_EQUIVALENT_PROTOS						\
	bool								\
	may_be_collectibly_type_equivalent(				\
		const fundamental_type_reference& t) const;		\
	bool								\
	may_be_connectibly_type_equivalent(				\
		const fundamental_type_reference& t) const;

	static
	ostream&
	type_mismatch_error(ostream&, const this_type&, const this_type&);

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

#endif	// __OBJECT_TYPE_FUNDAMENTAL_TYPE_REFERENCE_H__

