// "art_object_module.h"

#ifndef	__ART_OBJECT_MODULE_H__
#define	__ART_OBJECT_MODULE_H__

#include <string>
#include "art_object_base.h"

namespace ART {
namespace entity {
//=============================================================================

using namespace std;

/**
	A module corresponds to a compilation unit, a file.  
	A module contains two parts:
	1) order-independent data
	2) source-order-dependent data
 */
class module : public sequential_scope, public object {
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

private:
	module();
public:
explicit	module(const string& s);
	~module();
	// operations: merge, diff?

	never_ptr<name_space>	get_global_namespace(void) const;
	void set_global_namespace(excl_ptr<name_space> n);

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;

public:
	ART_OBJECT_IO_METHODS

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
	bool	merge_module(module& m);

};	// end class super_module

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	//	__ART_OBJECT_MODULE_H__

