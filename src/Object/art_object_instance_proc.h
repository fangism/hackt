/**
	\file "art_object_instance_proc.h"
	Class declarations for process instance and collections.  
	$Id: art_object_instance_proc.h,v 1.8.2.4.2.1 2005/02/20 09:08:16 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_PROC_H__
#define	__ART_OBJECT_INSTANCE_PROC_H__

#include "art_object_instance.h"
#include "memory/pointer_classes.h"

#include "multikey_fwd.h"
#include "multikey_qmap_fwd.h"


namespace ART {
namespace entity {
USING_LIST
using std::string;
using namespace util::memory;
using util::qmap;
using util::multikey_map;

//=============================================================================
// class process_instance_collection declared in "art_object_instance.h"

//=============================================================================
/**
	An actual instantiated instance of an enum.
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
	Needs to be pool allocated for efficient unique construction. 
	Derive from unique_instance_base.  
 */
class proc_instance : public persistent {
	typedef	proc_instance		this_type;
private:
	// need back-reference(s) to owner(s) or hierarchical keys?
	int		state;

	// TODO: contain a vector of pointers to sub-structures
	// concrete definition map will map member names to index/offsets
	//	per-class structure layout...
	// empty for now

public:
	proc_instance();
	~proc_instance();

	ostream&
	what(ostream&) const;

	PERSISTENT_METHODS_DECLARATIONS

};	// end class proc_instance

//-----------------------------------------------------------------------------
/**
	An uninitialized reference to an enum instance.  
	Only after references are connected, are the actual enum instances
	created.  
	Contains attribute fields.  
	Needs to become persistent because of alias pointers!
 */
class proc_instance_alias : public persistent {
public:
	typedef	never_ptr<const proc_instance_alias>	alias_ptr_type;
private:
	// consider alloc_ptr
	count_ptr<proc_instance>			instance;
	alias_ptr_type				alias;
	// validity fields?
	bool					instantiated;
public:

	proc_instance_alias();

	~proc_instance_alias();

	ostream&
	what(ostream&) const;

	bool
	valid(void) const { return instantiated; }

	void
	instantiate(void) { INVARIANT(!instantiated); instantiated = true; }

	const proc_instance_alias&
	canonical(void) const {
		alias_ptr_type ptr = alias;
		while (ptr) {
			ptr = ptr->alias;
		}
		return *ptr;
	}

	/// dereference, create
	proc_instance&
	operator * () const;

	/**
		Whether or not they refer to the same node.
		Check for instantiated?
	 */
	bool
	operator == (const proc_instance_alias& i) const {
		return &canonical() == &i.canonical();
	}

	friend
	ostream&
	operator << (ostream&, const proc_instance_alias&);

	// even though this is not persistent... yet
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC

};	// end class proc_instance_alias

ostream&
operator << (ostream&, const proc_instance_alias&);

//-----------------------------------------------------------------------------
// class process_instance_collection defined in "art_object_instance.h"

//-----------------------------------------------------------------------------
#define	PROC_ARRAY_TEMPLATE_SIGNATURE		template <size_t D>

PROC_ARRAY_TEMPLATE_SIGNATURE
class proc_array : public process_instance_collection {
private:
	typedef	process_instance_collection			parent_type;
friend class process_instance_collection;
public:
	typedef	parent_type::instance_ptr_type		instance_ptr_type;
	typedef	proc_instance_alias			element_type;
	typedef	multikey_map<D, pint_value_type, element_type, qmap>
							collection_type;
	typedef	typename collection_type::key_type	key_type;
private:
	collection_type					collection;
private:
	proc_array();
public:
	proc_array(const scopespace& o, const string& n);
	~proc_array();

	ostream&
	what(ostream& o) const;

	ostream&
	dump_unrolled_instances(ostream& o) const;

#if 0
	bool
	is_partially_unrolled(void) const;
#endif

	void
	instantiate_indices(const index_collection_item_ptr_type& i);

	const_index_list
	resolve_indices(const const_index_list& l) const;

	instance_ptr_type
	lookup_instance(const multikey_index_type& l) const;

	bool
	lookup_instance_collection(list<instance_ptr_type>& l, 
		const const_range_list& r) const;

	struct key_dumper {
		ostream& os;

		key_dumper(ostream& o) : os(o) { }

		ostream&
		operator () (const typename collection_type::value_type& );
	};	// end struct key_dumper

public:
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC
};	// end class proc_array

//-----------------------------------------------------------------------------
template <>
class proc_array<0> : public process_instance_collection {
private:
	typedef	process_instance_collection		parent_type;
friend class process_instance_collection;
public:
	typedef	parent_type::instance_ptr_type	instance_ptr_type;
private:
	proc_instance_alias			the_instance;

private:
	proc_array();

public:
	proc_array(const scopespace& o, const string& n);
	~proc_array();

	ostream&
	what(ostream&) const;

#if 0
	bool
	is_partially_unrolled(void) const;
#endif

	ostream&
	dump_unrolled_instances(ostream& o) const;

	void
	instantiate_indices(const index_collection_item_ptr_type& i);

	instance_ptr_type
	lookup_instance(const multikey_index_type& l) const;

	bool
	lookup_instance_collection(list<instance_ptr_type>& l, 
		const const_range_list& r) const;

	const_index_list
	resolve_indices(const const_index_list& l) const;

public:
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC
};	// end class proc_array (specialized)

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_PROC_H__

