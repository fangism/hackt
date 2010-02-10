/**
	\file "Object/lang/cflat_visitor.h"
	$Id: cflat_visitor.h,v 1.9.24.3 2010/02/10 06:43:08 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CFLAT_VISITOR_H__
#define	__HAC_OBJECT_LANG_CFLAT_VISITOR_H__

#include "util/NULL.h"
#include "util/size_t.h"
#include "Object/traits/classification_tags_fwd.h"
#include "Object/lang/PRS_footprint_expr_pool_fwd.h"
#include "Object/devel_switches.h"

// TEMPORARY
namespace HAC {
namespace entity {
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
#define	GLOBAL_ENTRY		state_instance
class footprint;
#else
#define	GLOBAL_ENTRY		global_entry
class state_manager;
#endif
template <class Tag> class GLOBAL_ENTRY;
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

namespace PRS {
// forward declarations of all the visitable types in this hierarchy
class footprint;
class footprint_expr_node;
class footprint_rule;
class footprint_macro;


//=============================================================================
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
/**
	Base class from which other functional visitors are derived.  
	TODO: default visit behavior for non-terminal types.  
	No need to include state_manager -- its traversal is fixed.  
 */
class cflat_visitor
#if 0 && MEMORY_MAPPED_GLOBAL_ALLOCATION
	: public global_entry_context
	// or dumper? no need
#endif
{
	typedef	cflat_visitor				this_type;
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
		expr_pool_setter(cflat_visitor&,
			const PRS_footprint_expr_pool_type&);
		expr_pool_setter(cflat_visitor&, const footprint&);
		expr_pool_setter(cflat_visitor&, const cflat_visitor&);
		~expr_pool_setter();
	};      // end struct expr_pool_setter
public:
	cflat_visitor() : expr_pool(NULL) { }
virtual	~cflat_visitor() { }

virtual	void
	visit(const GLOBAL_ENTRY<process_tag>&);
virtual	void
	visit(const GLOBAL_ENTRY<channel_tag>&);
virtual	void
	visit(const GLOBAL_ENTRY<enum_tag>&);
virtual	void
	visit(const GLOBAL_ENTRY<int_tag>&);
virtual	void
	visit(const GLOBAL_ENTRY<bool_tag>&);
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
virtual	void
	visit(const entity::footprint&);
#else
virtual	void
	visit(const state_manager&);
#endif
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
	void __default_visit(const GLOBAL_ENTRY<Tag>&);

};	// end struct cflat_visitor

//=============================================================================
#endif
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CFLAT_VISITOR_H__

