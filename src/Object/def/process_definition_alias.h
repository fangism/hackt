/**
	\file "Object/def/process_definition_alias.h"
	Process-definition-related HAC object classes.  
	This file came from "Object/art_object_definition_proc.h".
	$Id: process_definition_alias.h,v 1.5 2006/01/22 06:52:59 fang Exp $
 */

#ifndef	__OBJECT_DEF_PROCESS_DEFINITION_ALIAS_H__
#define	__OBJECT_DEF_PROCESS_DEFINITION_ALIAS_H__

#include "Object/def/process_definition_base.h"
#include "Object/def/typedef_base.h"

namespace HAC {
namespace entity {
//=============================================================================
/**
	A process-class typedef.  
	Is usable as a process_definition_base for complete types, 
	but not definable.  
	Has no contents, just a level of indirection to the encapsulated
	process type. 
	May be templated for partial type specifications.  
 */
class process_definition_alias : public process_definition_base, 
		public typedef_base {
private:
	typedef	process_definition_alias		this_type;
protected:
	const string					key;
	/** parent can be namespace or definition */
	never_ptr<const scopespace>			parent;
	excl_ptr<const process_type_reference>		base;
private:
	process_definition_alias();
public:
	process_definition_alias(const string& n, 
		const never_ptr<const scopespace> p);

	~process_definition_alias();

	ostream&
	what(ostream& o) const;

	const string&
	get_key(void) const;

	never_ptr<const scopespace>
	get_parent(void) const;

	never_ptr<const scopespace>
	get_scopespace(void) const;

	never_ptr<const fundamental_type_reference>
	get_base_type_ref(void) const;

	never_ptr<const port_formals_manager>
	get_port_formals_manager(void) const;

	bool
	assign_typedef(excl_ptr<const fundamental_type_reference>& f);

	MAKE_FUNDAMENTAL_TYPE_REFERENCE_PROTO;

	MAKE_CANONICAL_PROCESS_TYPE_REFERENCE_PROTO;

	MAKE_CANONICAL_PROCESS_TYPE_PROTO;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

private:
	void
	load_used_id_map_object(excl_ptr<persistent>& o);
};	// end class process_definition_alias

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_DEF_PROCESS_DEFINITION_ALIAS_H__

