/**
	\file "art_object_instance_base.h"
	Base classes for instance and instance collection objects.  
	$Id: art_object_instance_base.h,v 1.7 2004/12/12 23:32:07 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_BASE_H__
#define	__ART_OBJECT_INSTANCE_BASE_H__

#include <deque>
#include "STL/list.h"

#include "macros.h"
#include "art_object_base.h"
#include "persistent.h"		// for persistent object interface
	// includes <iosfwd> <string>

#include "memory/pointer_classes.h"
	// need complete definition (never_ptr members)

namespace ART {
namespace entity {
//=============================================================================
USING_LIST
USING_DEQUE
using std::istream;
using namespace util;
using namespace util::memory;

//=============================================================================
/**
	The state of an instance collection, kept track by each 
	instance reference.  
	Since the iterators are list-like, they remain valid
	after sequence manipulation operations (like insert, erase).  
 */
typedef index_collection_type::const_iterator
		instantiation_state;


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
public:
	typedef	never_ptr<const scopespace>	owner_ptr_type;
protected:
	/**
		Back-pointer to the namespace to which this instantiation
		belongs.  
		Can be a namespace of definition's scopespace.  
		Is NEVER null, should be reference?
		Should never be a loop or conditional namespace.  
	 */
	const owner_ptr_type		owner;

	/**
		Name of instance.
	 */
	string				key;

	/**
		This is a collection of instantiation statements 
		that, when unrolled, will instantiate instances
		at specified indices in the multidimensional collection, 
		implemented in the leaf children classes.  
	 */
	index_collection_type			index_collection;

public:
	/**
		A somewhat redundant field for the dimensionality of the
		collection.  Really, this is a per-class compile-time
		property, but rather than calling dimensions() through
		a massive virtual function, we cache the value once
		per-object.  
		We allow this to be public because it is constant.  
	 */
	const size_t	dimensions;

	// children will implement unrolled collection of instances?
	// but only instances that are not found in definitions?
protected:
	/**
		Private, dimensions-specific construct, intended for
		childrens' use only.  
	 */
	explicit
	instance_collection_base(const size_t d) :
		object(), persistent(), owner(), key(), 
		index_collection(), dimensions(d) { }

public:
	// o should be reference, not pointer
	instance_collection_base(const scopespace& o, const string& n, 
		const size_t d);

virtual	~instance_collection_base();

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o) const;	// temporary

	ostream&
	pair_dump(ostream& o) const;

	string
	get_name(void) const { return key; }

virtual	string
	get_qualified_name(void) const;

virtual	string
	hash_string(void) const { return key; }

/**
	Why is this a count_ptr?  because type_references can be reused
	all over the place, so we reference count all type references.  
	Unfortunately this forces us to do the same with static 
	built-in types.  
 */
virtual	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const = 0;

	never_ptr<const definition_base>
	get_base_def(void) const;

	owner_ptr_type
	get_owner(void) const { return owner; }

	instantiation_state
	collection_state_end(void) const;

	instantiation_state
	current_collection_state(void) const;

	const_range_list
	detect_static_overlap(index_collection_item_ptr_type r) const;

	const_range_list
	add_instantiation_statement(index_collection_type::value_type r);

private:
	bool
	formal_size_equivalent(
		never_ptr<const instance_collection_base> b) const;
public:
	bool
	is_template_formal(void) const;

	bool
	is_port_formal(void) const;

	bool
	template_formal_equivalent(
		never_ptr<const instance_collection_base> b) const;

	bool
	port_formal_equivalent(
		never_ptr<const instance_collection_base> b) const;

protected:
	bool
	check_expression_dimensions(const param_expr& pr) const;

public:
/**
	always pushes onto context's object_stack, 
	currently always returns NULL, useless
 */
virtual	count_ptr<instance_reference_base>
	make_instance_reference(void) const = 0;

virtual	count_ptr<member_instance_reference_base>
	make_member_instance_reference(
		count_ptr<const simple_instance_reference> b) const = 0;
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
		persistent_object_manager& m, istream&);
protected:
	// wrappers to provide consistent interface to children
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(persistent_object_manager&, istream&);

public:
	/** just for convenience */
	static const never_ptr<const instance_collection_base>	null;
};	// end class instance_collection_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_BASE_H__

