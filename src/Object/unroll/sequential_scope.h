/**
	\file "Object/unroll/sequential_scope.h"
	Base class for any sequential instantiation or manupulation.  
	This file came from "Object/art_object_instance_management_base.h"
		in prehistoric revisions.  
	$Id: sequential_scope.h,v 1.13.2.1 2008/03/15 03:33:54 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_SEQUENTIAL_SCOPE_H__
#define	__HAC_OBJECT_UNROLL_SEQUENTIAL_SCOPE_H__

#include <iosfwd>
#include <list>
#include "util/persistent_fwd.h"
#include "util/boolean_types.h"
#include "Object/unroll/instance_management_base.h"
	// for prototype macros
#include "util/memory/count_ptr.h"

namespace HAC {
namespace parser {
class context;
}

namespace entity {
using std::list;
using std::istream;
using std::ostream;
using util::persistent;
using util::persistent_object_manager;
using util::memory::count_ptr;
using util::good_bool;
using parser::context;
class unroll_context;
class instance_management_base;

/**
	The unified list of sequential instance management actions, 
	including parameters, instantiations, assignments, 
	and connections.  
	Used for maintaining actions in source order.  
	NOTE: production rules are are maintained separately
	in Object/lang/PRS.h:rule_set.
	We removed const because conditional_scopes need to 
	be extended during parse-tree walking.
	(Really, all other uses should be const!)
	Screw it, it's worth const_cast-ing in one place...
 */
typedef list<count_ptr<const instance_management_base> >
				instance_management_list_type;
//=============================================================================
/**
	Abstract interface for scopes with sequential 
	instance management actions.
	Defined in "Object/unroll/instance_management_base.cc".
 */
class sequential_scope : public instance_management_list_type {
	typedef	instance_management_list_type		parent_type;
public:
	sequential_scope();

// does this really need to be virtual? shouldn't be...
virtual ~sequential_scope();

public:
	ostream&
	dump(ostream& o, const expr_dump_context&) const;

#if 0
	// should be alias to push_back
	void
	append_instance_management(
		const instance_management_list_type::value_type&);
#endif

	using parent_type::empty;
	using parent_type::pop_back;
	using parent_type::back;

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

	good_bool
	unroll(const unroll_context& ) const;

	// predicated unroll
	good_bool
	unroll_if(const unroll_context&,
		bool (*)(const instance_management_base*)) const;

};      // end class sequential_scope

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_SEQUENTIAL_SCOPE_H__

