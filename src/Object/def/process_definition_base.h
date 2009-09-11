/**
	\file "Object/def/process_definition_base.h"
	Process-definition-related HAC object classes.  
	This file came from "Object/art_object_definition_proc.h".
	$Id: process_definition_base.h,v 1.6.86.1 2009/09/11 00:05:32 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_PROCESS_DEFINITION_BASE_H__
#define	__HAC_OBJECT_DEF_PROCESS_DEFINITION_BASE_H__

#include "Object/def/definition_base.h"
#include "Object/type/canonical_type_fwd.h"

namespace HAC {
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
public:
	typedef	process_type_reference	type_reference_type;
protected:
	// no new members?
protected:
//	process_definition_base(const string& n);
	process_definition_base() : definition_base() { }
public:
virtual	~process_definition_base() { }

	meta_type_tag_enum
	get_meta_type(void) const;

	excl_ptr<definition_base>
	make_typedef(never_ptr<const scopespace> s, 
		const token_identifier& id) const;

#define	MAKE_CANONICAL_PROCESS_TYPE_REFERENCE_PROTO			\
	count_ptr<const process_type_reference>				\
	make_canonical_fundamental_type_reference(const template_actuals&) const

virtual	MAKE_CANONICAL_PROCESS_TYPE_REFERENCE_PROTO = 0;

#define	MAKE_CANONICAL_PROCESS_TYPE_PROTO				\
	canonical_process_type						\
	make_canonical_type(const template_actuals&) const

virtual	MAKE_CANONICAL_PROCESS_TYPE_PROTO = 0;

// inherited pure virtuals are still pure virtuals
protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class process_definition_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_DEF_PROCESS_DEFINITION_BASE_H__

