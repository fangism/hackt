/**
	\file "Object/art_object_module.h"
	Classes that represent a single compilation module, a file.  
	$Id: module.h,v 1.16 2008/03/17 23:02:19 fang Exp $
 */

#ifndef	__HAC_OBJECT_MODULE_H__
#define	__HAC_OBJECT_MODULE_H__

#include <string>
#include "Object/common/util_types.h"
#include "Object/unroll/sequential_scope.h"
#include "util/persistent.h"
#include "Object/def/footprint.h"
#include "Object/state_manager.h"
#include "Object/def/process_definition.h"
#include "util/tokenize_fwd.h"
#include "util/attributes.h"
#include "util/STL/vector_fwd.h"

namespace HAC {
class cflat_options;

namespace parser {
class context;
}

namespace entity {
class process_type_reference;
using std::default_vector;
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
	NOTE: this is not used polymorphically with its process_definition
		parent_type.
 */
class module : public process_definition {
friend class parser::context;
	typedef	module				this_type;
private:
	typedef	process_definition		parent_type;
protected:
	/**
		The root namespace object contains information
		for definitions, and nested namespaces.  
	 */
	excl_ptr<name_space>			global_namespace;
	/**
		Whether or not the global instances have been allocated
		in the state_manager.  
	 */
	bool					allocated;
	/**
		Global unique instance manager.  
	 */
	state_manager				global_state;

private:
	module();

	// private, empty, undefined copy-constructor
	module(const module&);
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
	is_unrolled(void) const;

	bool
	is_created(void) const;

	bool
	is_allocated(void) const {
		return allocated;
	}

	const state_manager&
	get_state_manager(void) const { return global_state; }

	const footprint&
	get_footprint(void) const;
private:
	footprint&
	get_footprint(void);

public:
	/**
		Note: sequential scope has a const-version of this, 
		and is non-virtual.  
		Protected-ness keep user from accessing parent's unroll().
	 */
	good_bool
	unroll_module(void);

	good_bool
	create_unique(void);

	good_bool
	allocate_unique(void);

	good_bool
	cflat(ostream&, const cflat_options&) const;

	good_bool
	cflat(ostream&, const cflat_options&);

	template <class Tag>
	void
	match_aliases(util::string_list&, const size_t) const;

private:
	good_bool
	create_dependent_types(void);

	good_bool
	__cflat(ostream&, const cflat_options&) const;

	good_bool
	__cflat_rules(ostream&, const cflat_options&) const;

	good_bool
	__cflat_aliases(ostream&, const cflat_options&) const;

	good_bool
	__allocate_unique(void);

public:
	good_bool
	allocate_unique_process_type(const process_type_reference&, 
		const module&);
public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

};	// end class module

//=============================================================================
#if 0
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
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	//	__HAC_OBJECT_MODULE_H__

