/**
	\file "Object/lang/cflat_context_visitor.h"
	$Id: cflat_context_visitor.h,v 1.7 2010/05/11 00:18:09 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CFLAT_CONTEXT_VISITOR_H__
#define	__HAC_OBJECT_LANG_CFLAT_CONTEXT_VISITOR_H__

#include "Object/cflat_context.h"
#include "Object/lang/SPEC_fwd.h"

namespace HAC {
namespace entity {
struct dump_flags;		// from "Object/common/dump_flags.h"

//=============================================================================
/**
	TODO: is this class actually useful?
	Can member functions be pushed into parent class?  Probably.

	Refined visitor class, equipped with the essential structures
	needed to effectively traversal an allocated instance hierarchy.  
	It's named so, to attribute its origins to the cflat pass.  
	Practically every such visitor will need to derive from this.  
	This is still an abstract base class, because visit is unimplemented.
	Or should we provide a default visitation pattern?
 */
class cflat_context_visitor :
		public cflat_context {
public:

	cflat_context_visitor(
		const footprint_frame& ff, const global_offset& g) :
		cflat_context(ff, g) { }

	~cflat_context_visitor() { }

	size_t
	__lookup_global_bool_id(const size_t) const;

	void
	__resolve_unique_literal_group(const directive_node_group_type&, 
		directive_node_group_type&) const;

	ostream&
	__dump_canonical_literal(ostream&, const size_t,
		const dump_flags&) const;

	ostream&
	__dump_resolved_canonical_literal(ostream&, const size_t, 
		const dump_flags&) const;

};	// end class cflat_context_visitor

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CFLAT_CONTEXT_VISITOR_H__
