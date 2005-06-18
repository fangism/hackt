/**
	\file "Object/art_object_data_expr_base.h"
	Base classes for data expressions.  
	TODO: future rename this file to nonmeta_expr_base.h
	$Id: art_object_data_expr_base.h,v 1.1.4.5 2005/06/18 23:34:42 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_DATA_EXPR_BASE_H__
#define	__OBJECT_ART_OBJECT_DATA_EXPR_BASE_H__

#include <iosfwd>
#include "util/persistent.h"
#include "util/memory/pointer_classes_fwd.h"

#define	USE_DATA_EXPR_EQUIVALENCE	0

namespace ART {
namespace entity {
class data_type_reference;
using std::ostream;
using util::persistent;
using util::memory::count_ptr;
//=============================================================================
/**
	Base class for all datatype expressions.  
	Datatype expressions are classified as non-meta expressions, 
	i.e. their values are only determined at run-time.  
	It is always safe to accept a meta expression in place of a 
	nonmeta expression.  
	TODO: perhaps call this nonmeta_data_expr for clarity?
 */
class data_expr : virtual public persistent {
protected:
	data_expr() : persistent() { }
public:
virtual	~data_expr() { }

virtual	ostream&
	what(ostream&) const = 0;

virtual	ostream&
	dump(ostream&) const = 0;

virtual	size_t
	dimensions(void) const = 0;

#define	GET_DATA_TYPE_REF_PROTO						\
	count_ptr<const data_type_reference>				\
	get_data_type_ref(void) const

virtual	GET_DATA_TYPE_REF_PROTO = 0;

#if USE_DATA_EXPR_EQUIVALENCE
#define	DATA_EXPR_MAY_EQUIVALENCE_PROTO					\
	bool may_be_type_equivalent(const data_expr&) const

virtual	DATA_EXPR_MAY_EQUIVALENCE_PROTO = 0;
#endif

};	// end class data_expr

//=============================================================================
/**
	base class for generic non-meta index expressions.  
	meta_index_expr will derive from this.  
 */
class nonmeta_index_expr_base : virtual public persistent {
protected:
	nonmeta_index_expr_base() : persistent() { }
public:
virtual	~nonmeta_index_expr_base() { }

virtual	ostream&
	dump(ostream&) const = 0;

};	// end class nonmeta_ndex_expr_base

//=============================================================================
/**
	Base class for general range expressions, meta or nonmeta.  
 */
class nonmeta_range_expr_base : virtual public nonmeta_index_expr_base {
	typedef	nonmeta_index_expr_base		parent_type;
protected:
	nonmeta_range_expr_base() : parent_type() { }
public:
virtual	~nonmeta_range_expr_base() { }

};	// end clas nonmeta_range_expr_base

//=============================================================================
/**
	Abstract boolean data type expression.  
 */
class bool_expr : public data_expr {
	typedef	data_expr			parent_type;
protected:
	bool_expr() : parent_type() { }
public:
virtual	~bool_expr() { }

virtual	ostream&
	dump_brief(ostream&) const = 0;

};	// end class bool_expr

//=============================================================================
/**
	Abstract boolean data type expression.  
 */
class int_expr : virtual public nonmeta_index_expr_base, public data_expr {
	typedef	data_expr			parent_type;
protected:
	int_expr() : nonmeta_index_expr_base(), parent_type() { }
public:
virtual	~int_expr() { }

virtual	ostream&
	dump(ostream&) const = 0;

virtual	ostream&
	dump_brief(ostream&) const = 0;

};	// end class bool_expr

//=============================================================================
/**
	Expressions that represent enumerated values.  
	Pretty much limited to enum_nonmeta_instance_references.  
 */
class enum_expr : public data_expr {
	typedef	data_expr			parent_type;
protected:
	enum_expr() : parent_type() { }
public:
virtual	~enum_expr() { }

};	// end class enum_expr

//=============================================================================
/**
	Expressions that represent structured values.  
	Pretty much limited to struct_nonmeta_instance_references.  
 */
class struct_expr : public data_expr {
	typedef	data_expr			parent_type;
protected:
	struct_expr() : parent_type() { }
public:
virtual	~struct_expr() { }

};	// end class struct_expr

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_DATA_EXPR_BASE_H__

