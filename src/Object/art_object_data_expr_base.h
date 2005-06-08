/**
	\file "Object/art_object_data_expr_base.h"
	Base classes for data expressions.  
	TODO: future rename this file to nonmeta_expr_base.h
	$Id: art_object_data_expr_base.h,v 1.1.2.3 2005/06/08 04:03:44 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_DATA_EXPR_BASE_H__
#define	__OBJECT_ART_OBJECT_DATA_EXPR_BASE_H__

#include <iosfwd>
#include "util/persistent.h"
#include "util/memory/pointer_classes_fwd.h"

namespace ART {
namespace entity {
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

};	// end class bool_expr

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_DATA_EXPR_BASE_H__

