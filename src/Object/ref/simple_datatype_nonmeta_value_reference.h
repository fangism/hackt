/**
	\file "Object/ref/simple_datatype_nonmeta_value_reference.h"
	Most general non-meta instance references.  
	This file was "Object/art_object_nonmeta_inst_ref_base.h"
		in its previous life.  
	$Id: simple_datatype_nonmeta_value_reference.h,v 1.5.2.2 2006/03/19 06:14:13 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_SIMPLE_DATATYPE_NONMETA_VALUE_REFERENCE_H__
#define	__HAC_OBJECT_REF_SIMPLE_DATATYPE_NONMETA_VALUE_REFERENCE_H__

#include "Object/devel_switches.h"
#include "Object/ref/simple_nonmeta_instance_reference_base.h"

#if !NEW_NONMETA_REFERENCE_HIERARCHY
#if NONMETA_TYPE_EQUIVALENCE
// then intermediate class is useless
#include "util/persistent.h"

namespace HAC {
namespace entity {
class data_type_reference;
class data_expr;
using util::memory::count_ptr;
using util::persistent;

//=============================================================================
/**
	A reference to a nonmeta data type.  
	Abstract base class.
	NOTE: both true datatypes and nonmeta-dependent 
		meta parameter references fall into this category.  
	This will be the base class interface for all
		simple_nonmeta_value_references.  
 */
class simple_datatype_nonmeta_value_reference :
	virtual public persistent, 
	public simple_nonmeta_instance_reference_base
{
protected:
	typedef	simple_nonmeta_instance_reference_base	parent_type;
protected:
	simple_datatype_nonmeta_value_reference() :
		persistent(), parent_type() { }
public:
virtual	~simple_datatype_nonmeta_value_reference() { }

#if NONMETA_TYPE_EQUIVALENCE
virtual	bool
	may_accept_expr_type(const data_expr&) const = 0;

#if 0
virtual	bool
	__may_be_nonmeta_type_equivalent(const data_type_reference&) const = 0;
#endif
// add	must-equivalence once we get to unrolling CHP
#else
virtual	count_ptr<const data_type_reference>
	get_data_type_ref(void) const = 0;
#endif

virtual	size_t
	dimensions(void) const = 0;

virtual	ostream&
	dump(ostream&, const expr_dump_context&) const = 0;

};	// end class simple_datatype_nonmeta_value_reference

//=============================================================================
}	// end namespace entity
}	// end namespace HAC
#endif	// NONMETA_TYPE_EQUIVALENCE
#endif	// NEW_NONMETA_REFERENCE_HIERARCHY

#endif	// __HAC_OBJECT_REF_SIMPLE_DATATYPE_NONMETA_VALUE_REFERENCE_H__

