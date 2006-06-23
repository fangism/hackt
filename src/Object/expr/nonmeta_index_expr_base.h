/**
	\file "Object/expr/nonmeta_index_expr_base.h"
	Base classes for data expressions.  
	NOTE: for revision history purposes, this file was copied from the
		last revision of "Object/art_object_data_expr_base.h"
		on the HACXX-00-01-04-main-00-48-connect-01 branch, 
		branch revision -11.
	TODO: future rename this file to nonmeta_expr_base.h
	$Id: nonmeta_index_expr_base.h,v 1.5.40.3 2006/06/23 21:08:14 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_NONMETA_INDEX_EXPR_BASE_H__
#define	__HAC_OBJECT_EXPR_NONMETA_INDEX_EXPR_BASE_H__

#include "util/persistent.h"
#include "util/memory/pointer_classes_fwd.h"

namespace HAC {
namespace entity {
struct expr_dump_context;
class unroll_context;
using std::ostream;
using util::persistent;
using util::memory::count_ptr;

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
	dump(ostream& o, const expr_dump_context&) const = 0;

#define	UNROLL_RESOLVE_COPY_NONMETA_INDEX_PROTO				\
	count_ptr<const nonmeta_index_expr_base>			\
	unroll_resolve_copy(const unroll_context&, 			\
		const count_ptr<const nonmeta_index_expr_base>&) const

virtual	UNROLL_RESOLVE_COPY_NONMETA_INDEX_PROTO = 0;

};	// end class nonmeta_index_expr_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_NONMETA_INDEX_EXPR_BASE_H__

