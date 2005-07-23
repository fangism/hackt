/**
	\file "Object/def/process_definition_base.h"
	Process-definition-related ART object classes.  
	This file came from "Object/art_object_definition_proc.h".
	$Id: process_definition_base.h,v 1.2 2005/07/23 06:52:28 fang Exp $
 */

#ifndef	__OBJECT_DEF_PROCESS_DEFINITION_BASE_H__
#define	__OBJECT_DEF_PROCESS_DEFINITION_BASE_H__

#include "Object/def/definition_base.h"

namespace ART {
namespace entity {
class process_type_reference;
//=============================================================================
/**
	Process definition base class.  From this, there will arise: true
	process definitions, and typedef process definitions. 
 */
class process_definition_base : virtual public definition_base {
private:
	typedef	definition_base		parent_type;
protected:
	// no new members?
protected:
//	process_definition_base(const string& n);
	process_definition_base() : definition_base() { }
public:
virtual	~process_definition_base() { }

	excl_ptr<definition_base>
	make_typedef(never_ptr<const scopespace> s, 
		const token_identifier& id) const;

#define	MAKE_CANONICAL_PROCESS_TYPE_REFERENCE_PROTO			\
	count_ptr<const process_type_reference>				\
	make_canonical_type_reference(const template_actuals&) const

virtual	MAKE_CANONICAL_PROCESS_TYPE_REFERENCE_PROTO = 0;

// inherited pure virtuals are still pure virtuals
protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class process_definition_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_DEF_PROCESS_DEFINITION_BASE_H__

