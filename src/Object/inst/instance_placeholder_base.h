/**
	\file "Object/inst/instance_placeholder_base.h"
	Base classes for instance and instance collection objects.  
	This file was "Object/art_object_instance_base.h"
		in a previous life.  
	$Id: instance_placeholder_base.h,v 1.4.4.3 2006/11/07 01:07:57 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_PLACEHOLDER_BASE_H__
#define	__HAC_OBJECT_INST_INSTANCE_PLACEHOLDER_BASE_H__

#include <string>
#include "util/macros.h"
#include "util/boolean_types.h"
#include "Object/common/object_base.h"
#include "Object/common/util_types.h"
#include "Object/inst/substructure_alias_fwd.h"
#include "util/persistent.h"		// for persistent object interface
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
class instance_collection_base;
class physical_instance_placeholder;
class unroll_context;
class instantiation_statement_base;
using std::list;
using std::istream;
using std::string;
using util::bad_bool;
using util::good_bool;
using util::persistent;
using util::persistent_object_manager;
using util::memory::never_ptr;
using util::memory::count_ptr;

//=============================================================================
/**
	This class was originally instance_collection_base, 
	but it has been deemed appropriate to separate placeholders
	(which belong in scopes) from their actual collection (in footprints).

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
class instance_placeholder_base : public object, public persistent {
	typedef	instance_placeholder_base	this_type;
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
	/**
		Back-pointer to the namespace to which this instantiation
		belongs.  
		Can be a namespace of definition's scopespace.  
		Is NEVER null, should be reference?
		Should never be a loop or conditional namespace.  

		Wants to be const, but because at least one child class
		is pool-allocated (requiring assignability), 
		we have to make it normal...
	 */
	owner_ptr_type			owner;

	/**
		Name of instance.
	 */
	string				key;

	/**
		A somewhat redundant field for the dimensionality of the
		collection.  Really, this is a per-class compile-time
		property, but rather than calling dimensions() through
		a massive virtual function, we cache the value once
		per-object.  
		We allow this to be public because it is constant.  

		Wants to be const.  
	 */
	size_t				dimensions;

	// children will implement unrolled collection of instances?
	// but only instances that are not found in definitions?

protected:
	// dimensions unknown
	instance_placeholder_base() :
		object(), persistent(), owner(), key() { }
	/**
		Private, dimensions-specific construct, intended for
		childrens' use only.  
	 */
	explicit
	instance_placeholder_base(const size_t d) :
		object(), persistent(), owner(), key(), 
		dimensions(d) { }

	/**
		Partial copy-constructor, copies everything 
		except the index collection (references to
		instantiation statements), and the super_instance pointer, 
		which should be NULL anyhow.  
	 */
	instance_placeholder_base(const this_type& t, const footprint&) :
		object(), persistent(), owner(t.owner), key(t.key), 
		dimensions(t.dimensions) { }

	// o should be reference, not pointer
	instance_placeholder_base(const scopespace& o, const string& n, 
		const size_t d);

public:
virtual	~instance_placeholder_base();

/**
	TODO: rename this after reworking instances.
	Makes a deep copy of the instance collection for
	the sake of mapping in the footprint's instance collection map.
	This does not make a precise deep copy of every field, 
	but enough for the the unroller to work with.  
	The footprint argument WAS just for distiguishing constructors.  
	NOTE: not any more! TODO: delete comment
 */
#define	MAKE_INSTANCE_COLLECTION_FOOTPRINT_COPY_PROTO			\
	instance_collection_base*					\
	make_instance_collection_footprint_copy(footprint&) const

virtual	MAKE_INSTANCE_COLLECTION_FOOTPRINT_COPY_PROTO = 0;

	size_t
	get_dimensions(void) const { return dimensions; }

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

public:
virtual	void
	attach_initial_instantiation_statement(
		const count_ptr<const instantiation_statement_base>&) = 0;

	/**
		Depending on whether the collection is partially unrolled, 
		print the type.  
	 */
virtual	ostream&
	type_dump(ostream& o) const = 0;

	ostream&
	pair_dump(ostream& o) const;

	// provides a fixed dump_flags
	ostream&
	pair_dump_top_level(ostream& o) const;

	const string&
	get_name(void) const { return key; }

// virtual
	string
	get_qualified_name(void) const;

	ostream&
	dump_qualified_name(ostream&, const dump_flags&) const;

	string
	get_footprint_key(void) const;

#if 0
	ostream&
	dump_hierarchical_name(ostream&) const;

	ostream&
	dump_hierarchical_name(ostream&, const dump_flags&) const;
#endif

virtual	string
	hash_string(void) const { return key; }

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

	never_ptr<const definition_base>
	get_base_def(void) const;

	owner_ptr_type
	get_owner(void) const { return owner; }

	// to grant access to param_value_collection
	bool
	formal_size_equivalent(const this_type& b) const;

// protected:
public:
virtual	index_collection_item_ptr_type
	get_initial_instantiation_indices(void) const = 0;


virtual	count_ptr<nonmeta_instance_reference_base>
	make_nonmeta_instance_reference(void) const = 0;

// return type may become generic...
virtual	member_inst_ref_ptr_type
	make_member_meta_instance_reference(
		const inst_ref_ptr_type& b) const = 0;

private:
	// utility functions for handling index collection (inlined)
	void
	collect_index_collection_pointers(
		persistent_object_manager& m) const;

	void
	write_index_collection_pointers(
		const persistent_object_manager& m, ostream&) const;

	void
	load_index_collection_pointers(
		const persistent_object_manager& m, istream&);

protected:
	// wrappers to provide consistent interface to children
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

public:
	/** just for convenience */
	static const never_ptr<const instance_placeholder_base>	null;
};	// end class instance_placeholder_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_PLACEHOLDER_BASE_H__

