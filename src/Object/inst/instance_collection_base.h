/**
	\file "Object/inst/instance_collection_base.h"
	Base classes for instance and instance collection objects.  
	This file was "Object/art_object_instance_base.h"
		in a previous life.  
	$Id: instance_collection_base.h,v 1.8 2006/01/22 06:53:01 fang Exp $
 */

#ifndef	__OBJECT_INST_INSTANCE_COLLECTION_BASE_H__
#define	__OBJECT_INST_INSTANCE_COLLECTION_BASE_H__

#include <string>
#include <deque>
// #include <list>

#include "util/macros.h"
#include "util/boolean_types.h"
#include "Object/common/object_base.h"
#include "Object/common/util_types.h"
#include "Object/common/predicated_inst_stmt_ptr.h"
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
class param_expr;
class const_param_expr_list;
class physical_instance_collection;
class unroll_context;
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
 */
class instance_collection_base : public object, public persistent {
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
		This is a collection of instantiation statements 
		that, when unrolled, will instantiate instances
		at specified indices in the multidimensional collection, 
		implemented in the leaf children classes.  
		Can elements be NULL?
		TODO: subtype this!  don't use generic type common to all
		children.  Implement using class_traits policy.  
	 */
	index_collection_type		index_collection;

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
	/**
		Private, dimensions-specific construct, intended for
		childrens' use only.  
	 */
	explicit
	instance_collection_base(const size_t d) :
		object(), persistent(), owner(), key(), 
		index_collection(), dimensions(d), super_instance() { }

	/**
		Partial copy-constructor, copies everything 
		except the index collection (references to
		instantiation statements), and the super_instance pointer, 
		which should be NULL anyhow.  
	 */
	instance_collection_base(const this_type& t, const footprint&) :
		object(), persistent(), owner(t.owner), key(t.key), 
		index_collection(), dimensions(t.dimensions),
		super_instance() { }

public:
	// o should be reference, not pointer
	instance_collection_base(const scopespace& o, const string& n, 
		const size_t d);

virtual	~instance_collection_base();

/**
	Makes a deep copy of the instance collection for
	the sake of mapping in the footprint's instance collection map.
	This does not make a precise deep copy of every field, 
	but enough for the the unroller to work with.  
	The footprint argument is just for distiguishing constructors.  
 */
#define	MAKE_INSTANCE_COLLECTION_FOOTPRINT_COPY_PROTO			\
	instance_collection_base*					\
	make_instance_collection_footprint_copy(const footprint&) const

virtual	MAKE_INSTANCE_COLLECTION_FOOTPRINT_COPY_PROTO = 0;

	size_t
	get_dimensions(void) const { return dimensions; }

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

virtual	bool
	is_partially_unrolled(void) const = 0;

virtual	ostream&
	what(ostream&) const = 0;

// bas eimplementation built-upon
virtual	ostream&
	dump(ostream&) const;	// temporary

private:
	ostream&
	dump_collection_only(ostream&) const;

public:
	/**
		Depending on whether the collection is partially unrolled, 
		print the type.  
	 */
virtual	ostream&
	type_dump(ostream& o) const = 0;

	ostream&
	pair_dump(ostream& o) const;

	const string&
	get_name(void) const { return key; }

virtual	string
	get_qualified_name(void) const;

	ostream&
	dump_qualified_name(ostream&) const;

	ostream&
	dump_qualified_name(ostream&, const dump_flags&) const;

	ostream&
	dump_hierarchical_name(ostream&) const;

	ostream&
	dump_hierarchical_name(ostream&, const dump_flags&) const;

	size_t
	hierarchical_depth(void) const;

virtual	string
	hash_string(void) const { return key; }

/**
	Why is this a count_ptr?  because type_references can be reused
	all over the place, so we reference count all type references.  
	Unfortunately this forces us to do the same with static 
	built-in types.  

	Note: that this doesn't return the unrolled actual type, 
	need a different method for that.  
 */
	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;

	never_ptr<const definition_base>
	get_base_def(void) const;

	owner_ptr_type
	get_owner(void) const { return owner; }

	instantiation_state
	collection_state_end(void) const;

	instantiation_state
	current_collection_state(void) const;

	const_range_list
	detect_static_overlap(const index_collection_item_ptr_type& r) const;

	const_range_list
	add_instantiation_statement(const index_collection_type::value_type& r);

protected:
	// to grant access to param_value_collection
	bool
	formal_size_equivalent(const this_type& b) const;

public:
	size_t
	is_template_formal(void) const;

	bool
	is_relaxed_template_formal(void) const;

	size_t
	is_port_formal(void) const;

	size_t
	is_member_instance(void) const;

	bool
	is_local_to_definition(void) const;

	bool
	port_formal_equivalent(const this_type& b) const;

	bool
	is_template_dependent(void) const;

public:
virtual	count_ptr<meta_instance_reference_base>
	make_meta_instance_reference(void) const = 0;

virtual	count_ptr<nonmeta_instance_reference_base>
	make_nonmeta_instance_reference(void) const = 0;

// return type may become generic...
virtual	member_inst_ref_ptr_type
	make_member_meta_instance_reference(const inst_ref_ptr_type& b) const = 0;

private:
	// utility functions for handling index collection (inlined)
	void
	collect_index_collection_pointers(
		persistent_object_manager& m) const;

	void
	write_index_collection_pointers(
		const persistent_object_manager& m, ostream& ) const;

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
	static const never_ptr<const instance_collection_base>	null;
};	// end class instance_collection_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_INST_INSTANCE_COLLECTION_BASE_H__

