/**
	\file "Object/lang/cflat_visitor.h"
	$Id: cflat_visitor.h,v 1.8 2008/10/11 06:35:13 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CFLAT_VISITOR_H__
#define	__HAC_OBJECT_LANG_CFLAT_VISITOR_H__

#include "util/NULL.h"
#include "util/size_t.h"
#include "Object/traits/classification_tags_fwd.h"
#include "Object/lang/PRS_footprint_expr_pool_fwd.h"

namespace HAC {
namespace entity {
class state_manager;
template <class Tag> class global_entry;
namespace SPEC {
	class footprint;
	class footprint_directive;
}
namespace PRS {
// forward declarations of all the visitable types in this hierarchy
class footprint;
class footprint_expr_node;
class footprint_rule;
class footprint_macro;

//=============================================================================
/**
	Base class from which other functional visitors are derived.  
	TODO: default visit behavior for non-terminal types.  
	No need to include state_manager -- its traversal is fixed.  
 */
class cflat_visitor {
protected:
	/**
		This needs to be set by the visit to the footprint.  
		Will initially be NULL, before the PRS footprint is entered. 
	 */
	const PRS_footprint_expr_pool_type*		expr_pool;

	/**
		helper class for maintaining expr_pool.
		We only made this public so other non-cflat functions could
		print better diagnostic messages cflat-style.  
		TODO: just use member_saver?
	 */
	class expr_pool_setter {
	private:
		cflat_visitor&                          cfv;
	public:
		expr_pool_setter(cflat_visitor&, const footprint&);
		expr_pool_setter(cflat_visitor&, const cflat_visitor&);
		~expr_pool_setter();
	};      // end struct expr_pool_setter
public:
	template <class Tag>
	struct instance_exception {
		/**
			Identifies which top-level process id caused exception.
		 */
		size_t				pid;
		explicit
		instance_exception(const size_t p) : pid(p) { }
	};
public:
	cflat_visitor() : expr_pool(NULL) { }
virtual	~cflat_visitor() { }

virtual	void
	visit(const global_entry<process_tag>&);
virtual	void
	visit(const global_entry<channel_tag>&);
virtual	void
	visit(const global_entry<enum_tag>&);
virtual	void
	visit(const global_entry<int_tag>&);
virtual	void
	visit(const global_entry<bool_tag>&);
virtual	void
	visit(const state_manager&);
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

private:
	template <class Tag>
	void __default_visit(const global_entry<Tag>&);

};	// end struct cflat_visitor

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CFLAT_VISITOR_H__

