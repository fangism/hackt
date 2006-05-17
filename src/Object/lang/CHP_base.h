/**
	\file "Object/lang/CHP_base.h"
	Class definitions for CHP-related objects.  
	$Id: CHP_base.h,v 1.6.18.1 2006/05/17 02:22:51 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CHP_BASE_H__
#define	__HAC_OBJECT_LANG_CHP_BASE_H__

#include "util/persistent.h"
#include "Object/devel_switches.h"
#if ENABLE_CHP_FOOTPRINT
#include "util/memory/count_ptr.h"
#endif

namespace HAC {
namespace entity {
struct expr_dump_context;
class unroll_context;
/**
	Namespace for CHP object classes.  
 */
namespace CHP {
using std::ostream;
using util::persistent;
using util::persistent_object_manager;
#if ENABLE_CHP_FOOTPRINT
class action;
using util::memory::count_ptr;
typedef	count_ptr<action>			action_ptr_type;
#endif

//=============================================================================
#if ENABLE_CHP_FOOTPRINT
/**
	Interpretation of members:
	~changed : no change (no error), can use shallow copy,
		copy can be valid (unused) or NULL.  
	changed && ~copy : there was an error creating copy.  
	changed && copy : copy points to newly allocated copy.  
 */
struct unroll_action_return_type {
	bool					changed;
	action_ptr_type				copy;

	/**
		Default constructor signals an error.  
	 */
	unroll_action_return_type() : changed(true), copy(NULL) { }

	unroll_action_return_type(const bool b, const action_ptr_type& c) :
		changed(b), copy(c) { }

	// default destructor

};      // end struct unroll_action_return_type
#endif

//=============================================================================
/**
	Abstract base class for type-checked CHP statements.  
 */
class action : public persistent {
public:
	typedef	unroll_action_return_type	unroll_return_type;

	action() { }
virtual	~action() { }

virtual	ostream&
	dump(ostream&, const expr_dump_context&) const = 0;

#if ENABLE_CHP_FOOTPRINT
	/**
		unroll_context-binding functor.  
	 */
	struct transformer {
		const unroll_context&		_context;

		explicit
		transformer(const unroll_context& c) : _context(c) { }

		action_ptr_type
		operator () (const action_ptr_type&) const;
	};

#define	CHP_UNROLL_ACTION_PROTO						\
	unroll_action_return_type					\
	unroll(const unroll_context&) const

virtual	CHP_UNROLL_ACTION_PROTO = 0;
#endif

};	// end class action

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CHP_BASE_H__

