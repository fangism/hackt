/**
	\file "art_object_instance_management_base.h"
	Base class for any sequential instantiation or manupulation.  
	$Id: art_object_instance_management_base.h,v 1.1 2004/12/06 07:11:20 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_MANAGEMENT_BASE_H__
#define	__ART_OBJECT_INSTANCE_MANAGEMENT_BASE_H__

#include <iosfwd>
#include "STL/list.h"
#include "persistent.h"
#include "memory/pointer_classes.h"
#include "persistent_object_manager.h"

namespace ART {
namespace entity {
USING_LIST
using std::ostream;
using util::persistent;
using util::persistent_object_manager;
using util::memory::excl_ptr;
using util::memory::sticky_ptr;

//=============================================================================
/**
	Abstract base class for sequential instantiation management objects, 
	including instantiations, parameters, assignments, connections.  
	Don't bother deriving from object, unless it is necessary.  
 */
class instance_management_base : public persistent {
protected:
	// none
public:
	/**
		Helper functor for adding a dereference before dumping, 
		since the majority of objects are pointer-classed.
		Consider using this in object as well.  
	 */
	class dumper {
		private:
			ostream& os;
		public:
			explicit dumper(ostream& o);

			template <template <class> class P>
			ostream&
			operator () (const P<const instance_management_base>& i) const;
	};      // end class dumper

public:
virtual ostream& dump(ostream& o) const = 0;

#if 0
virtual void collect_transient_info(persistent_object_manager& m) const = 0;
virtual void write_object(const persistent_object_manager& m) const = 0;
virtual void load_object(persistent_object_manager& m) = 0;
#endif

	// need pure virtual unrolling methods
	// argument should contain some stack of expression values
virtual void unroll(void) const = 0;
};	// end class instance_management_base

//=============================================================================
/**
	Abstract interface for scopes with sequential 
	instance management actions.
 */
class sequential_scope {
public:
	typedef list<sticky_ptr<const instance_management_base> >
					instance_management_list_type;
protected:
	/**
		The unified list of sequential instance management actions, 
		including parameters, instantiations, assignments, 
		and connections.  
		Used for maintaining actions in source order.  
	 */
	instance_management_list_type   instance_management_list;
public:
	sequential_scope();
virtual ~sequential_scope();

	ostream& dump(ostream& o) const;
	void append_instance_management(
		excl_ptr<const instance_management_base> i);

	void collect_object_pointer_list(persistent_object_manager& m) const;
	void write_object_pointer_list(const persistent_object_manager& m) const;
	void load_object_pointer_list(const persistent_object_manager& m);

// need not be virtual?
// may need context later...
	void unroll(void) const;

};      // end class sequential_scope

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_MANAGEMENT_BASE_H__

