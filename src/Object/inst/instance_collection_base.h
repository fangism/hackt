/**
	\file "Object/inst/instance_collection_base.h"
	Base classes for instance and instance collection objects.  
	This file was "Object/art_object_instance_base.h"
		in a previous life.  
	$Id: instance_collection_base.h,v 1.14.4.1 2006/11/01 07:52:30 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_COLLECTION_BASE_H__
#define	__HAC_OBJECT_INST_INSTANCE_COLLECTION_BASE_H__

#include <string>
#include "util/macros.h"
#include "util/boolean_types.h"
#include "Object/common/object_base.h"
#include "Object/common/util_types.h"
#include "Object/inst/substructure_alias_fwd.h"
#include "Object/devel_switches.h"
#if !POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
#include "util/persistent.h"		// for persistent object interface
#endif
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
//=============================================================================
struct dump_flags;
class definition_base;
class footprint;
class scopespace;
class meta_instance_reference_base;
class nonmeta_instance_reference_base;
class fundamental_type_reference;
class const_range_list;
class const_index_list;
class param_expr;
class const_param_expr_list;
class physical_instance_collection;
class unroll_context;
class instance_placeholder_base;
using std::list;
using std::istream;
using std::string;
using util::bad_bool;
using util::good_bool;
#if !POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
using util::persistent;
using util::persistent_object_manager;
#endif
using util::memory::never_ptr;
using util::memory::count_ptr;

//=============================================================================
/**
	Base class for instantiation objects.  Recall that instantiations
	may appear both inside and outside definitions.  Instantiations
	inside definitions will build as part of the definitions, 			whereas those outside definitions will register as actual 
	instantiations in the object file.  
	Instantiations may be simple or complex-aggregate.  
	Rules for collective instantiations (two types):
	1) dense...
	2) sparse...
	Sub-classes must contain const pointers to fundamental_type_reference
	sub-classes.  

	REWORK:
	No type_ref member, acquire that from instantiation_statement.  
	Instead of list of indices in index_collection, 
	use list of statements that contain indices.  

	NOTE: this used to have index_collection for tracking instantiation
		statements per collection, but they were removed on the
		HACKT-00-01-04-main-00-77-8-aggregate-01-02-ref branch.
 */
class instance_collection_base
#if !POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
		: public persistent
#endif
		{
	typedef	instance_collection_base	this_type;
public:
	typedef	never_ptr<const scopespace>	owner_ptr_type;
	// should be consistent with 
	//	member_meta_instance_reference_base::base_inst_ptr_type
	typedef	count_ptr<const meta_instance_reference_base>
						inst_ref_ptr_type;
	// needs to be of a type that can be pushed onto object stack
	typedef	count_ptr<meta_instance_reference_base>
						member_inst_ref_ptr_type;
	typedef	count_ptr<const const_param_expr_list>
						instance_relaxed_actuals_type;
	typedef	never_ptr<const substructure_alias>
						super_instance_ptr_type;
protected:
	// children will implement unrolled collection of instances?
	// but only instances that are not found in definitions?

	/**
		Pointer to parent super instance.  
		Added 2005-07-10.
		If top-level instance item, then this is NULL.  
		TODO: decide what to do about persistence.  
		Does this only belong in physical_instance_collection?
		No, EVERYTHING may have a super, even parameters.  

		2005-07-11:
		NOTE: this field is maintained persistently in a
		less-than-usual fashion.  It is the responsibility of the
		super instance (parent) alias to write and restore its
		back-link (to itself) to its children.  
		Thus, the persistent object manager will not touch
		this field, however, it should be noted that
		restoration will have to be well ordered between
		parent and children.  
	 */
	super_instance_ptr_type		super_instance;
protected:
	instance_collection_base() : 
#if !POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
		persistent(), 
#endif
		super_instance() { }

public:

virtual	~instance_collection_base();

private:
// this was added just to support formal_size_equivalent()...
virtual	never_ptr<const instance_placeholder_base>
	__get_placeholder_base(void) const = 0;

public:
virtual	size_t
	get_dimensions(void) const = 0;

	bool
	is_subinstance(void) const { return super_instance; }

	const super_instance_ptr_type&
	get_super_instance(void) const { return super_instance; }

	void
	relink_super_instance(const substructure_alias& a) {
		INVARIANT(!super_instance);
		super_instance = super_instance_ptr_type(&a);
	}

#if 0
	good_bool
	create_super_instance(footprint&);
#endif

virtual	ostream&
	what(ostream&) const = 0;

// base implementation built-upon
protected:
	ostream&
	dump_base(ostream&) const;

	ostream&
	dump_base(ostream&, const dump_flags&) const;

public:
virtual	ostream&
	dump(ostream&, const dump_flags&) const = 0;	// temporary

protected:
	// just to satisfy object_base forwards to public dump()
	ostream&
	dump(ostream&) const;

	ostream&
	dump_collection_only(ostream&) const;

public:
	/**
		Depending on whether the collection is partially unrolled, 
		print the type.  
	 */
virtual	ostream&
	type_dump(ostream& o) const = 0;

virtual	const string&
	get_name(void) const = 0;

	string
	get_footprint_key(void) const;

	ostream&
	dump_hierarchical_name(ostream&) const;

	ostream&
	dump_hierarchical_name(ostream&, const dump_flags&) const;

	size_t
	hierarchical_depth(void) const;

/**
	Why is this a count_ptr?  because type_references can be reused
	all over the place, so we reference count all type references.  
	Unfortunately this forces us to do the same with static 
	built-in types.  

	Note: that this doesn't return the unrolled actual type, 
	need a different method for that.  

	This only returns the type given by the first (possibly predicated)
	instantiation statement.  
 */
virtual	count_ptr<const fundamental_type_reference>
	get_unresolved_type_ref(void) const = 0;

virtual	owner_ptr_type
	get_owner(void) const = 0;

	bool
	is_local_to_definition(void) const;

	bool
	is_template_dependent(void) const;

virtual	const_index_list
	resolve_indices(const const_index_list&) const = 0;

#if 0
// been obsolete for a long time
private:
	// utility functions for handling index collection (inlined)
	void
	collect_index_collection_pointers(persistent_object_manager& m) const;

	void
	write_index_collection_pointers(
		const persistent_object_manager& m, ostream&) const;

	void
	load_index_collection_pointers(
		const persistent_object_manager& m, istream&);
#endif
#if !POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
protected:
	// wrappers to provide consistent interface to children
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);
#endif

public:
	/** just for convenience */
	static const never_ptr<const instance_collection_base>	null;
};	// end class instance_collection_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_COLLECTION_BASE_H__

