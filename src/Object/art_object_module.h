/**
	\file "art_object_module.h"
	Classes that represent a single compilation module, a file.  
	$Id: art_object_module.h,v 1.11.2.1 2005/02/03 03:34:54 fang Exp $
 */

#ifndef	__ART_OBJECT_MODULE_H__
#define	__ART_OBJECT_MODULE_H__

#include <string>
#include "art_object_base.h"
#include "art_object_instance_management_base.h"
#include "persistent.h"

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
class module :
	public object, public persistent
#if 0
	// changing this to private or protected crashes on darwin-gcc-3.3!?
	, private sequential_scope
#else
	, public sequential_scope
#endif
	{
friend class context;
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

	/**
		Whether or not this entire module has been 
		successfully unrolled.
	 */
	bool					unrolled;

private:
	module();
public:
explicit	module(const string& s);
	~module();
	// operations: merge, diff?

	never_ptr<name_space>
	get_global_namespace(void) const;

	void
	set_global_namespace(excl_ptr<name_space>& n);

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	bool
	is_unrolled(void) const {	return unrolled;	}

	/**
		Note: sequential scope has a const-version of this, 
		and is non-virtual.  
		Protected-ness keep user from accessing parent's unroll().
	 */
	void
	unroll_module(void);

public:
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

#endif	//	__ART_OBJECT_MODULE_H__

