/**
	\file "art_object_instance_management_base.h"
	Base class for any sequential instantiation or manupulation.  
	$Id: art_object_instance_management_base.h,v 1.6.2.1 2005/02/03 03:34:53 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_MANAGEMENT_BASE_H__
#define	__ART_OBJECT_INSTANCE_MANAGEMENT_BASE_H__

#include <iosfwd>
#include "STL/list.h"
#include "persistent.h"
#include "memory/pointer_classes.h"
#include "persistent_object_manager.h"

namespace ART {
namespace parser {
class context;
}

namespace entity {
USING_LIST
using std::istream;
using std::ostream;
using util::persistent;
using util::persistent_object_manager;
using util::memory::excl_ptr;
using util::memory::sticky_ptr;
using parser::context;
class unroll_context;

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
			explicit
			dumper(ostream& o);

			template <template <class> class P>
			ostream&
			operator () (const P<const instance_management_base>& i) const;
	};      // end class dumper

public:
virtual ostream&
	dump(ostream& o) const = 0;

	// need pure virtual unrolling methods
	// argument should contain some stack of expression values
virtual void
	unroll(unroll_context& ) const = 0;
};	// end class instance_management_base

//=============================================================================
/**
	Abstract interface for scopes with sequential 
	instance management actions.
 */
class sequential_scope {
// allow context to reference instance_management_list directly
friend class context;
public:
	/**
		By using sticky_ptr, this is not copy-constructible.
	 */
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

private:
	/// Intentionally undefined private copy-constructor
	sequential_scope(const sequential_scope&);

public:
	ostream&
	dump(ostream& o) const;

	void
	append_instance_management(
		excl_ptr<const instance_management_base>& i);

private:
	void
	collect_object_pointer_list(persistent_object_manager& m) const;


	// why even have these at all?
	void
	write_object_pointer_list(const persistent_object_manager& m, 
		ostream&) const;

	void
	load_object_pointer_list(const persistent_object_manager& m, 
		istream&);

protected:
	void
	collect_transient_info_base(persistent_object_manager& m) const;

	void
	write_object_base(const persistent_object_manager& m, ostream&) const;

	void
	load_object_base(const persistent_object_manager& m, istream&);

public:
	static
	void
	write_object_base_fake(const persistent_object_manager& m, ostream&);

// need not be virtual?
// may need context later...
	void
	unroll(unroll_context& ) const;

};      // end class sequential_scope

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_MANAGEMENT_BASE_H__

