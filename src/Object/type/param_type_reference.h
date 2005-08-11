/**
	\file "Object/type/param_type_reference.h"
	Type-reference classes of the ART language.  
	TODO: must pool-allocate these, they're created frequently!
	This file originated from "Object/art_object_type_ref.h"
		in a previous life.  
 	$Id: param_type_reference.h,v 1.2.8.1 2005/08/11 03:40:55 fang Exp $
 */

#ifndef	__OBJECT_TYPE_PARAM_TYPE_REFERENCE_H__
#define	__OBJECT_TYPE_PARAM_TYPE_REFERENCE_H__

#include "Object/type/fundamental_type_reference.h"

namespace ART {
namespace entity {
class unroll_context;
class built_in_param_def;

//=============================================================================
/**
	Reference to a param definition.  
	This class really does nothing, but exists merely for consistency's
	sake.  Parameters are non-templatable, thus it suffices for 
	parameter instances to refer directly to the built-in definitions, 
	but we rather than make a special-case exception, we use this class.  
	The template_params member is inherited but not used.  
	Built-in implementation: only will ever be two instances of this class. 
 */
class param_type_reference : public fundamental_type_reference {
private:
	typedef	param_type_reference		this_type;
	typedef	fundamental_type_reference	parent_type;	// not used
protected:
	/**
		TODO: If this is never used, then phase it out.
	 */
	never_ptr<const built_in_param_def>	base_param_def;
public:
	explicit
	param_type_reference(const never_ptr<const built_in_param_def> td);

	// no need to virtual, unless something derives from this
	~param_type_reference();

	ostream&
	what(ostream& o) const;

	never_ptr<const definition_base>
	get_base_def(void) const;

	bool
	is_canonical(void) const;

	void
	commit_definition_arith(void) { }

	MAKE_CANONICAL_TYPE_REFERENCE_PROTO;

	unroll_context
	make_unroll_context(void) const;
private:
	MAKE_INSTANTIATION_STATEMENT_PRIVATE_PROTO;
			
	MAKE_INSTANCE_COLLECTION_PROTO;

	UNROLL_PORT_INSTANCES_PROTO;

private:
	// dummy implementation, never called
	PERSISTENT_METHODS_DECLARATIONS

};	// end class param_type_reference

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_TYPE_PARAM_TYPE_REFERENCE_H__

