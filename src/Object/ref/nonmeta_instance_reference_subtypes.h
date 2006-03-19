/**
	\file "Object/ref/nonmeta_instance_reference_subtypes.h"
	Class family for instance references in HAC.  
	This file used to be "Object/art_object_nonmeta_inst_ref_subtypes.h"
		in a previous life.  
	$Id: nonmeta_instance_reference_subtypes.h,v 1.5.10.2 2006/03/19 06:14:13 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_NONMETA_INSTANCE_REFERENCE_SUBTYPES_H__
#define	__HAC_OBJECT_REF_NONMETA_INSTANCE_REFERENCE_SUBTYPES_H__

#include "Object/ref/nonmeta_instance_reference_base.h"
#if NEW_NONMETA_REFERENCE_HIERARCHY
#include "Object/ref/data_nonmeta_instance_reference.h"
#endif
#include "Object/traits/class_traits_fwd.h"
#include "Object/devel_switches.h"

namespace HAC {
namespace entity {
//=============================================================================
// classes pint_nonmeta_instance_reference and pbool_nonmeta_instance_reference
//	are in "art_object_expr_param_ref.*"

#if !SIMPLIFY_NONMETA_INSTANCE_REFERENCES
class channel_instance_reference_base :
		virtual public nonmeta_instance_reference_base {
	typedef	nonmeta_instance_reference_base		parent_type;
protected:
	channel_instance_reference_base() : parent_type() { }
public:
virtual	~channel_instance_reference_base() { }
};	// end class channel_instance_reference_base

//-----------------------------------------------------------------------------
class process_instance_reference_base :
		virtual public nonmeta_instance_reference_base {
	typedef	nonmeta_instance_reference_base		parent_type;
protected:
	process_instance_reference_base() : parent_type() { }
public:
virtual	~process_instance_reference_base() { }
};	// end class process_instance_reference_base

//-----------------------------------------------------------------------------
class datatype_instance_reference_base :
		virtual public nonmeta_instance_reference_base {
	typedef	nonmeta_instance_reference_base		parent_type;
protected:
	datatype_instance_reference_base() : parent_type() { }
public:
virtual	~datatype_instance_reference_base() { }
};	// end class datatype_instance_reference_base

//-----------------------------------------------------------------------------
class int_instance_reference_base :
		virtual public datatype_instance_reference_base {
	typedef	datatype_instance_reference_base	parent_type;
protected:
	int_instance_reference_base() : parent_type() { }
public:
virtual	~int_instance_reference_base() { }
};	// end class int_instance_reference_base

//-----------------------------------------------------------------------------
class bool_instance_reference_base :
		virtual public datatype_instance_reference_base {
	typedef	datatype_instance_reference_base	parent_type;
protected:
	bool_instance_reference_base() : parent_type() { }
public:
virtual	~bool_instance_reference_base() { }
};	// end class bool_instance_reference_base

//-----------------------------------------------------------------------------
class enum_instance_reference_base :
		virtual public datatype_instance_reference_base {
	typedef	datatype_instance_reference_base	parent_type;
protected:
	enum_instance_reference_base() : parent_type() { }
public:
virtual	~enum_instance_reference_base() { }
};	// end class enum_instance_reference_base

//-----------------------------------------------------------------------------
class struct_instance_reference_base :
		virtual public datatype_instance_reference_base {
	typedef	datatype_instance_reference_base	parent_type;
protected:
	struct_instance_reference_base() : parent_type() { }
public:
virtual	~struct_instance_reference_base() { }
};	// end class struct_instance_reference_base

//-----------------------------------------------------------------------------
class param_instance_reference_base :
		virtual public nonmeta_instance_reference_base {
	typedef	nonmeta_instance_reference_base		parent_type;
protected:
	param_instance_reference_base() : parent_type() { }
public:
virtual	~param_instance_reference_base() { }
};	// end class param_instance_reference_base

//-----------------------------------------------------------------------------
class pint_instance_reference_base :
		virtual public param_instance_reference_base {
	typedef	param_instance_reference_base		parent_type;
protected:
	pint_instance_reference_base() : parent_type() { }
public:
virtual	~pint_instance_reference_base() { }
};	// end class pint_instance_reference_base

//-----------------------------------------------------------------------------
class pbool_instance_reference_base :
		virtual public param_instance_reference_base {
	typedef	param_instance_reference_base		parent_type;
protected:
	pbool_instance_reference_base() : parent_type() { }
public:
virtual	~pbool_instance_reference_base() { }
};	// end class pbool_instance_reference_base

//-----------------------------------------------------------------------------
class preal_instance_reference_base :
		virtual public param_instance_reference_base {
	typedef	param_instance_reference_base		parent_type;
protected:
	preal_instance_reference_base() : parent_type() { }
public:
virtual	~preal_instance_reference_base() { }
};	// end class preal_instance_reference_base
#endif

//=============================================================================
#if NEW_NONMETA_REFERENCE_HIERARCHY

#define	NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE	template <class Tag>
#define	NONMETA_INSTANCE_REFERENCE_CLASS	nonmeta_instance_reference<Tag>
/**
	Meta-class specific base classes for nonmeta references.  
	From these classes, simply-indexed, and member references will sprout.  
	This concept is modeled after the meta_instance_reference hierarchy.  
	The base class specified by the traits_type must be either
	nonmeta_instance_reference_base or some derivative thereof.  
 */
NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
class nonmeta_instance_reference :
	public class_traits<Tag>::nonmeta_instance_reference_base_type {
public:
	typedef	class_traits<Tag>			traits_type;
	typedef	typename traits_type::nonmeta_instance_reference_base_type
							parent_type;

protected:
	nonmeta_instance_reference() : parent_type() { }

virtual	~nonmeta_instance_reference() { }

};	// end class nonmeta_instance_reference

#endif	// NEW_NONMETA_REFERENCE_HIERARCHY

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_NONMETA_INSTANCE_REFERENCE_SUBTYPES_H__

