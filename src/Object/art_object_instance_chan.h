/**
	\file "art_object_instance_chan.h"
	Class declarations for channel instance and collections.  
	$Id: art_object_instance_chan.h,v 1.8.8.1 2005/02/02 19:08:17 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_CHAN_H__
#define	__ART_OBJECT_INSTANCE_CHAN_H__

#include "art_object_instance.h"
#include "memory/pointer_classes.h"

#include "multikey_fwd.h"
#include "multikey_qmap_fwd.h"


namespace ART {
namespace entity {
USING_LIST
using std::string;
using namespace util::memory;
using QMAP_NAMESPACE::qmap;
using MULTIKEY_NAMESPACE::multikey_base;
using MULTIKEY_MAP_NAMESPACE::multikey_map;

//=============================================================================
// class channel_instance_collection declared in "art_object_instance.h"

//=============================================================================
/**
	An actual instantiated instance of an enum.
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
 */
struct chan_instance {
	// need back-reference(s) to owner(s) or hierarchical keys?
	int		state;

public:
	chan_instance();

};	// end class chan_instance

//-----------------------------------------------------------------------------
/**
	An uninitialized reference to an enum instance.  
	Only after references are connected, are the actual enum instances
	created.  
	Contains attribute fields.  
 */
class chan_instance_alias {
public:
	typedef	never_ptr<const chan_instance_alias>	alias_ptr_type;
private:
	// consider alloc_ptr
	count_ptr<chan_instance>			instance;
	alias_ptr_type				alias;
	// validity fields?
	bool					instantiated;
public:

	bool
	valid(void) const { return instantiated; }

	void
	instantiate(void) { INVARIANT(!instantiated); instantiated = true; }

	const chan_instance_alias&
	canonical(void) const {
		alias_ptr_type ptr = alias;
		while (ptr) {
			ptr = ptr->alias;
		}
		return *ptr;
	}

	/// dereference, create
	chan_instance&
	operator * () const;

#if 0
	/**
		Alias connection.  
	 */
	chan_instance_alias&
	operator = (const chan_instance_alias& b) {
		alias = alias_ptr_type(&b);
		return *this;
	}
#endif

	/**
		Whether or not they refer to the same node.
		Check for instantiated?
	 */
	bool
	operator == (const chan_instance_alias& i) const {
		return &canonical() == &i.canonical();
	}

	friend
	ostream&
	operator << (ostream&, const chan_instance_alias&);

};	// end class chan_instance_alias

ostream&
operator << (ostream&, const chan_instance_alias&);

//-----------------------------------------------------------------------------
// class channel_instance_collection defined in "art_object_instance.h"

//-----------------------------------------------------------------------------
#define	CHAN_ARRAY_TEMPLATE_SIGNATURE		template <size_t D>

CHAN_ARRAY_TEMPLATE_SIGNATURE
class chan_array : public channel_instance_collection {
private:
	typedef	channel_instance_collection			parent_type;
friend class channel_instance_collection;
public:
	typedef	parent_type::instance_ptr_type		instance_ptr_type;
	typedef	chan_instance_alias			element_type;
	typedef	multikey_map<D, pint_value_type, element_type, qmap>
							collection_type;

private:
	collection_type					collection;
private:
	chan_array();
public:
	chan_array(const scopespace& o, const string& n);
	~chan_array();

	ostream&
	what(ostream& o) const;

	bool
	is_partially_unrolled(void) const;

	ostream&
	dump_unrolled_instances(ostream& o) const;

	void
	instantiate_indices(const index_collection_item_ptr_type& i);

	const_index_list
	resolve_indices(const const_index_list& l) const;

	instance_ptr_type
	lookup_instance(const unroll_index_type& l) const;

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
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC_NO_POINTERS
};	// end class chan_array

//-----------------------------------------------------------------------------
template <>
class chan_array<0> : public channel_instance_collection {
private:
	typedef	channel_instance_collection		parent_type;
friend class channel_instance_collection;
public:
	typedef	parent_type::instance_ptr_type	instance_ptr_type;
private:
	chan_instance_alias			the_instance;

private:
	chan_array();

public:
	chan_array(const scopespace& o, const string& n);
	~chan_array();

	ostream&
	what(ostream&) const;

	bool
	is_partially_unrolled(void) const;

	ostream&
	dump_unrolled_instances(ostream& o) const;

	void
	instantiate_indices(const index_collection_item_ptr_type& i);

	instance_ptr_type
	lookup_instance(const unroll_index_type& l) const;

	bool
	lookup_instance_collection(list<instance_ptr_type>& l, 
		const const_range_list& r) const;

	const_index_list
	resolve_indices(const const_index_list& l) const;

public:
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC_NO_POINTERS
};	// end class chan_array (specialized)

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_CHAN_H__

