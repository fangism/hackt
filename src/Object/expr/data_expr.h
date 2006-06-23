/**
	\file "Object/expr/data_expr.h"
	Base classes for data expressions.  
	NOTE: for revision histry purposes, this file was copied from the
		last revision of "Object/art_object_data_expr_base.h"
		on the HACXX-00-01-04-main-00-48-connect-01 branch, 
		branch revision -11.
	TODO: future rename this file to nonmeta_expr_base.h
	$Id: data_expr.h,v 1.6.16.2 2006/06/23 21:08:12 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_DATA_EXPR_H__
#define	__HAC_OBJECT_EXPR_DATA_EXPR_H__

#include "util/persistent.h"
#include "util/memory/pointer_classes_fwd.h"
// unroll_resolve_copy is only needed because CHP assignments
// are not (yet) subtyped, if we ever update this, we can trim here

#define	USE_DATA_EXPR_EQUIVALENCE	0

namespace HAC {
namespace entity {
class data_type_reference;
class unroll_context;
struct expr_dump_context;
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
	dump(ostream&, const expr_dump_context&) const = 0;

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

#define	UNROLL_RESOLVE_COPY_DATA_PROTO					\
	count_ptr<const data_expr>					\
	unroll_resolve_copy(const unroll_context&, 			\
		const count_ptr<const data_expr>&) const

virtual	UNROLL_RESOLVE_COPY_DATA_PROTO = 0;

};	// end class data_expr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_DATA_EXPR_H__

