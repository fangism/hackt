/**
	\file "Object/art_object_data_expr_base.h"
	Base classes for data expressions.  
	$Id: art_object_data_expr_base.h,v 1.1.2.1 2005/06/03 21:43:49 fang Exp $
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
 */
class data_expr : virtual public persistent {
protected:
	data_expr() : persistent() { }
virtual	~data_expr() { }

public:
virtual	ostream&
	what(ostream&) const = 0;

virtual	ostream&
	dump(ostream&) const = 0;

virtual	size_t
	dimensions(void) const = 0;
};	// end class data_expr

//=============================================================================
/**
	Abstract boolean data type expression.  
 */
class bool_expr : public data_expr {
	typedef	data_expr				parent_type;
public:
	bool_expr() : parent_type() { }
virtual	~bool_expr() { }
};	// end class bool_expr

//=============================================================================
/**
	Abstract boolean data type expression.  
 */
class int_expr : public data_expr {
	typedef	data_expr				parent_type;
public:
	int_expr() : parent_type() { }
virtual	~int_expr() { }
};	// end class bool_expr

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_DATA_EXPR_BASE_H__

