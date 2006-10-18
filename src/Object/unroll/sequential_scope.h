/**
	\file "Object/unroll/sequential_scope.h"
	Base class for any sequential instantiation or manupulation.  
	This file came from "Object/art_object_instance_management_base.h"
		in prehistoric revisions.  
	$Id: sequential_scope.h,v 1.11 2006/10/18 01:20:08 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_SEQUENTIAL_SCOPE_H__
#define	__HAC_OBJECT_UNROLL_SEQUENTIAL_SCOPE_H__

#include <iosfwd>
#include <list>
#include "util/persistent_fwd.h"
#include "util/boolean_types.h"
#include "Object/unroll/instance_management_base.h"
	// for prototype macros
#include "Object/devel_switches.h"
#if REF_COUNT_INSTANCE_MANAGEMENT
#include "util/memory/count_ptr.h"
#else
#include "util/memory/excl_ptr.h"	// contains sticky_ptr
#endif

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
#if REF_COUNT_INSTANCE_MANAGEMENT
using util::memory::count_ptr;
#else
using util::memory::excl_ptr;
using util::memory::sticky_ptr;
#endif
using util::good_bool;
using parser::context;
class unroll_context;
class instance_management_base;

//=============================================================================
/**
	Abstract interface for scopes with sequential 
	instance management actions.
 */
class sequential_scope {
public:
	/**
		By using sticky_ptr, this is not copy-constructible.
	 */
#if REF_COUNT_INSTANCE_MANAGEMENT
	typedef list<count_ptr<const instance_management_base> >
#else
	typedef list<sticky_ptr<const instance_management_base> >
#endif
					instance_management_list_type;
	typedef	instance_management_list_type::const_iterator
							const_iterator;
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
	dump(ostream& o, const expr_dump_context&) const;

	void
	append_instance_management(
#if REF_COUNT_INSTANCE_MANAGEMENT
		const instance_management_list_type::value_type&
#else
		excl_ptr<const instance_management_base>&
#endif
		);

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

protected:
// need not be virtual?
	good_bool
	unroll(const unroll_context& ) const;

};      // end class sequential_scope

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_SEQUENTIAL_SCOPE_H__

