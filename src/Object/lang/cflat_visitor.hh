/**
	\file "Object/lang/cflat_visitor.hh"
	$Id: cflat_visitor.hh,v 1.11 2010/04/07 00:12:50 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CFLAT_VISITOR_HH__
#define	__HAC_OBJECT_LANG_CFLAT_VISITOR_HH__

#include "util/NULL.h"
#include "util/size_t.h"
#include "Object/traits/classification_tags_fwd.hh"
#include "Object/lang/PRS_footprint_expr_pool_fwd.hh"

namespace HAC {
namespace entity {
class footprint;
template <class Tag> class state_instance;
namespace SPEC {
	class footprint;
	class footprint_directive;
}

template <class Tag>
struct instance_exception {
	/**
		Identifies which top-level process id caused exception.
	 */
	size_t				pid;
	explicit
	instance_exception(const size_t p) : pid(p) { }
};

namespace RTE {
class footprint;
class footprint_expr_node;
struct footprint_assignment;
}
namespace PRS {
// forward declarations of all the visitable types in this hierarchy
class footprint;
class footprint_expr_node;
struct footprint_rule;
class footprint_macro;


//=============================================================================
/**
	Base class from which other functional visitors are derived.  
	TODO: default visit behavior for non-terminal types.  
	No need to include state_manager -- its traversal is fixed.  
	Intended to be a visitor of unrolled PRS and SPEC directives.
 */
class cflat_visitor {
	typedef	cflat_visitor				this_type;
public:
virtual	~cflat_visitor() { }

virtual	void
	visit(const entity::footprint&);
virtual	void
	visit(const footprint&);
virtual	void
	visit(const footprint_rule&) = 0;
virtual	void
	visit(const footprint_expr_node&) = 0;
virtual	void
	visit(const footprint_macro&) = 0;
virtual	void
	visit(const SPEC::footprint&);
virtual	void
	visit(const SPEC::footprint_directive&) = 0;
virtual	void
	visit(const RTE::footprint&);
virtual	void
	visit(const RTE::footprint_assignment&) = 0;
virtual	void
	visit(const RTE::footprint_expr_node&) = 0;
private:
	template <class Tag>
	void __default_visit(const state_instance<Tag>&);

};	// end struct cflat_visitor

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CFLAT_VISITOR_HH__

