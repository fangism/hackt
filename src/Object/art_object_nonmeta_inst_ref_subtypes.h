/**
	\file "Object/art_object_nonmeta_inst_ref_subtypes.h"
	Class family for instance references in ART.  
	$Id: art_object_nonmeta_inst_ref_subtypes.h,v 1.1.2.2 2005/06/08 04:03:49 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_NONMETA_INST_REF_SUBTYPES_H__
#define	__OBJECT_ART_OBJECT_NONMETA_INST_REF_SUBTYPES_H__

#include "Object/art_object_nonmeta_inst_ref_base.h"

namespace ART {
namespace entity {
template <class> struct class_traits;

using std::ostream;
using std::istream;

//=============================================================================
#if 0
#define	SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE		\
template <class Tag>

#define SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS				\
simple_nonmeta_instance_reference<Tag>

/**
	Final class, need not be virtual.  
 */
SIMPLE_NONMETA_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
class simple_nonmeta_instance_reference :
		public simple_nonmeta_instance_reference_base, 
		public class_traits<Tag>::nonmeta_instance_reference_base_type {
	typedef	SIMPLE_NONMETA_INSTANCE_REFERENCE_CLASS	this_type;
	// one of the *_instance_reference_base classes listed below
	typedef	typename class_traits<Tag>::nonmeta_instance_reference_base_type
							parent_type;
	typedef	simple_nonmeta_instance_reference_base	common_base_type;
	// see Object/art_object_inst_ref.h for more inspiring typedefs...
protected:
	simple_nonmeta_instance_reference();
public:
	explicit
	simple_nonmeta_instance_reference(excl_ptr<index_list_type>&);

virtual	~simple_nonmeta_instance_reference();

	ostream&
	dump(ostream&) const;

	ostream&
	what(ostream&) const;

	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

};	// end class simple_nonmeta_instance_reference
#endif

//=============================================================================
// classes pint_nonmeta_instance_reference and pbool_nonmeta_instance_reference
//	are in "art_object_expr_param_ref.*"

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
};	// end clas bool_instance_reference_base

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
};	// end clas param_instance_reference_base

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

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_NONMETA_INST_REF_SUBTYPES_H__

