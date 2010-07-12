/**
	\file "Object/lang/cflat_context_visitor.h"
	$Id: cflat_context_visitor.h,v 1.8 2010/07/12 21:49:54 fang Exp $
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

	using cflat_context::lookup_global_id;

	template <class Tag>
	void
	__resolve_unique_literal_group(const directive_node_group_type&, 
		directive_node_group_type&) const;

	template <class Tag>
	ostream&
	__dump_resolved_canonical_literal(ostream&, const size_t, 
		const dump_flags&) const;

	template <class Tag>
	ostream&
	__dump_canonical_literal(ostream& o, const size_t lni,
			const dump_flags& df) const {
		return __dump_resolved_canonical_literal<Tag>(o,
			lookup_global_id<Tag>(lni), df);
	}

};	// end class cflat_context_visitor

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CFLAT_CONTEXT_VISITOR_H__
