/**
	\file "Object/expr/nonmeta_index_expr_base.h"
	Base classes for data expressions.  
	NOTE: for revision histry purposes, this file was copied from the
		last revision of "Object/art_object_data_expr_base.h"
		on the ARTXX-00-01-04-main-00-48-connect-01 branch, 
		branch revision -11.
	TODO: future rename this file to nonmeta_expr_base.h
	$Id: nonmeta_index_expr_base.h,v 1.2.22.1 2005/10/13 01:27:05 fang Exp $
 */

#ifndef	__OBJECT_EXPR_NONMETA_INDEX_EXPR_BASE_H__
#define	__OBJECT_EXPR_NONMETA_INDEX_EXPR_BASE_H__

#include "util/persistent.h"
#include "Object/devel_switches.h"

namespace ART {
namespace entity {
struct expr_dump_context;
using std::ostream;
using util::persistent;

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
#if USE_EXPR_DUMP_CONTEXT
	dump(ostream& o, const expr_dump_context&) const = 0;
#else
	dump(ostream&) const = 0;
#endif

};	// end class nonmeta_ndex_expr_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_EXPR_NONMETA_INDEX_EXPR_BASE_H__

