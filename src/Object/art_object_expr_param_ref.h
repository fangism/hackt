/**
	\file "Object/art_object_expr_param_ref.h"
	Classes related to parameter instance reference expressions. 
	$Id: art_object_expr_param_ref.h,v 1.10 2005/05/10 04:51:13 fang Exp $
 */

#ifndef __OBJECT_ART_OBJECT_EXPR_PARAM_REF_H__
#define __OBJECT_ART_OBJECT_EXPR_PARAM_REF_H__

#include "util/boolean_types.h"
#include "Object/art_object_expr_const.h"	// for const_index_list
#include "Object/art_object_inst_ref_base.h"	// includes "art_object_base.h"

//=============================================================================
namespace ART {
namespace entity {
USING_LIST
using std::string;
using std::ostream;

// Is this file now obsolete?

//=============================================================================
/**
	NOT SURE THIS CLASS IS USEFUL... eventually...
	For arrays of expressions.
	Statically type-checked.  
	Should sub-type...
	Consider multidimensional_qmap for unroll time.
class param_expr_collective : public param_expr {
protected:
	list<excl_ptr<param_expr> >	elist;
public:
	param_expr_collective();
	~param_expr_collective();

	ostream& what(ostream& o) const;
	string hash_string(void) const;
	size_t dimensions(void) const;
	bool may_be_equivalent(const param_expr& p) const;
	bool must_be_equivalent(const param_expr& p) const;
virtual	bool has_static_constant_dimensions(void) const = 0;
virtual	const_range_list static_constant_dimensions(void) const = 0;
};	// end class param_expr_collective
**/

//=============================================================================
}	// end namespace ART
}	// end namespace entity

#endif	// __OBJECT_ART_OBJECT_EXPR_PARAM_REF_H__

