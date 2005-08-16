/**
	\file "Object/art_object_module.h"
	Classes that represent a single compilation module, a file.  
	$Id: module.h,v 1.3.4.1 2005/08/16 03:47:14 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_MODULE_H__
#define	__OBJECT_ART_OBJECT_MODULE_H__

#include <string>
#include "Object/common/util_types.h"
#include "Object/unroll/sequential_scope.h"
#include "util/persistent.h"
#include "Object/devel_switches.h"

#if USE_MODULE_FOOTPRINT
#include "Object/def/footprint.h"
#endif

namespace ART {
namespace entity {

using std::string;
using std::ostream;
using util::persistent;
using util::persistent_object_manager;

//=============================================================================
/**
	A module corresponds to a compilation unit, a file.  
	A module contains two parts:
	1) order-independent data
	2) source-order-dependent data
 */
class module : public persistent, public sequential_scope {
friend class context;
	typedef	module				this_type;
protected:
	/**
		Name of the file.
		If blank, then was from -stdin- or -cin-.  
	 */
	string					name;

	/**
		The root namespace object contains information
		for definitions, and nested namespaces.  
	 */
	excl_ptr<name_space>			global_namespace;

#if USE_MODULE_FOOTPRINT
	footprint				_footprint;
#else
	/**
		Whether or not this entire module has been 
		successfully unrolled.
	 */
	bool					unrolled;
	/**
		Whether or not this object has had state space
		uniquely allocated.  
	 */
	bool					created;
#endif

private:
	module();
public:
	explicit
	module(const string& s);

	~module();
	// operations: merge, diff?

	never_ptr<name_space>
	get_global_namespace(void) const;

	void
	set_global_namespace(excl_ptr<name_space>& n);

	void
	collect_namespaces(namespace_collection_type&) const;

	template <class L>
	void
	collect(L&) const;

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	bool
	is_unrolled(void) const {
#if USE_MODULE_FOOTPRINT
		return _footprint.is_unrolled();
#else
		return unrolled;
#endif
	}

	bool
	is_created(void) const {
#if USE_MODULE_FOOTPRINT
		return _footprint.is_created();
#else
		return created;
#endif
	}

	/**
		Note: sequential scope has a const-version of this, 
		and is non-virtual.  
		Protected-ness keep user from accessing parent's unroll().
	 */
	good_bool
	unroll_module(void);

	good_bool
	create_unique(void);

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

};	// end class module

//=============================================================================

/**
	The final unit of compilation that links together
	one or more modules.  
	The difference between this and module is that
	this covers multiple compilation units.  
	Need a better name for this guy...
 */
class multi_module : public sequential_scope {
	typedef	multi_module		this_type;
protected:
	// set of file/module names?

	excl_ptr<name_space>		global_ns;

public:
	multi_module();
	~multi_module();

	// note: destructive merge (link-phase)
	bool
	merge_module(module& m);

};	// end class multi_module

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	//	__OBJECT_ART_OBJECT_MODULE_H__

