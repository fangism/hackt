/**
	\file "art_object_instance_enum.h"
	Class declarations for built-in and user-defined data instances
	and instance collections.  
	$Id: art_object_instance_enum.h,v 1.9.2.3.2.4.2.3 2005/02/26 04:56:44 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_ENUM_H__
#define	__ART_OBJECT_INSTANCE_ENUM_H__

#include "art_object_instance.h"
#include "memory/pointer_classes.h"
#include "art_object_classification_details.h"

#if USE_INSTANCE_COLLECTION_TEMPLATE
#include "art_object_instance_collection.h"
#include "art_object_instance_alias.h"
#else
#include "multikey_fwd.h"
#include "multikey_qmap_fwd.h"
#endif


namespace ART {
namespace entity {
#if !USE_INSTANCE_COLLECTION_TEMPLATE
USING_LIST
using std::string;
using namespace util::memory;
using util::qmap;
using util::multikey_map;
#endif

//=============================================================================
// class datatype_instance_collection declared in "art_object_instance.h"

//=============================================================================
/**
	An actual instantiated instance of an enum.
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
 */
class enum_instance : public persistent {
	// need back-reference(s) to owner(s) or hierarchical keys?
	never_ptr<const enum_instance_alias_base>	back_ref;
public:
	enum_instance();

	PERSISTENT_METHODS_DECLARATIONS
};	// end class enum_instance

//-----------------------------------------------------------------------------
#if !USE_INSTANCE_COLLECTION_TEMPLATE
/**
	An uninitialized reference to an enum instance.  
	Only after references are connected, are the actual enum instances
	created.  
	Contains attribute fields.  
 */
class enum_instance_alias {
public:
	typedef	never_ptr<const enum_instance_alias>	alias_ptr_type;
private:
	// consider alloc_ptr
	count_ptr<enum_instance>			instance;
	alias_ptr_type				alias;
	// validity fields?
	bool					instantiated;
public:

	bool
	valid(void) const { return instantiated; }

	void
	instantiate(void) { INVARIANT(!instantiated); instantiated = true; }

	const enum_instance_alias&
	canonical(void) const {
		alias_ptr_type ptr = alias;
		while (ptr) {
			ptr = ptr->alias;
		}
		return *ptr;
	}

	/// dereference, create
	enum_instance&
	operator * () const;

#if 0
	/**
		Alias connection.  
	 */
	enum_instance_alias&
	operator = (const enum_instance_alias& b) {
		alias = alias_ptr_type(&b);
		return *this;
	}
#endif

	/**
		Whether or not they refer to the same node.
		Check for instantiated?
	 */
	bool
	operator == (const enum_instance_alias& i) const {
		return &canonical() == &i.canonical();
	}

	friend
	ostream&
	operator << (ostream&, const enum_instance_alias&);

};	// end class enum_instance_alias

ostream&
operator << (ostream&, const enum_instance_alias&);

//-----------------------------------------------------------------------------
/**
	Interface to collection of data-enum instance aliases.  
	TODO: need to add reference to definition for layout/enum map.  
 */
class enum_instance_collection : public datatype_instance_collection {
private:
	typedef	datatype_instance_collection		parent_type;
	typedef	enum_instance_collection		this_type;
public:
	typedef	enum_instance_alias			instance_alias_base_type;
	typedef	enum_alias_connection			alias_connection_type;
	typedef	parent_type::type_ref_ptr_type		type_ref_ptr_type;
	typedef	never_ptr<instance_alias_base_type>		instance_ptr_type;
	typedef	packed_array_generic<pint_value_type, instance_ptr_type>
							alias_collection_type;
	typedef	parent_type::inst_ref_ptr_type		inst_ref_ptr_type;
	typedef	parent_type::member_inst_ref_ptr_type	member_inst_ref_ptr_type;
protected:
	explicit
	enum_instance_collection(const size_t d) : parent_type(d) { }
public:
	enum_instance_collection(const scopespace& o, const string& n, 
		const size_t d);

virtual	~enum_instance_collection();

virtual	ostream&
	what(ostream&) const = 0;

	ostream&
	type_dump(ostream&) const;

virtual	bool
	is_partially_unrolled(void) const = 0;

#if 0
	// this could just return hard-coded built-in type...
	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;
#endif

	bool
	commit_type(const type_ref_ptr_type& );

	count_ptr<instance_reference_base>
	make_instance_reference(void) const;

	member_inst_ref_ptr_type
	make_member_instance_reference(const inst_ref_ptr_type& ) const;

virtual	void
	instantiate_indices(const index_collection_item_ptr_type& i) = 0;

virtual instance_ptr_type
	lookup_instance(const multikey_index_type& i) const = 0;

virtual	bool
	lookup_instance_collection(list<instance_ptr_type>& l, 
		const const_range_list& r) const = 0;

virtual	const_index_list
	resolve_indices(const const_index_list& l) const = 0;

#if 0
virtual bool
	connect(const multikey_index_type& k, const enum_instance_alias& b) = 0;
#endif

virtual	bool
	unroll_aliases(const multikey_index_type&, const multikey_index_type&,
		alias_collection_type&) const = 0;

public:

	static
	enum_instance_collection*
	make_enum_array(const scopespace& o, const string& n, const size_t d);

	static
	persistent*
	construct_empty(const int);

	void
	collect_transient_info(persistent_object_manager& m) const;

protected:
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class enum_instance_collection

//-----------------------------------------------------------------------------
#define	ENUM_ARRAY_TEMPLATE_SIGNATURE		template <size_t D>

ENUM_ARRAY_TEMPLATE_SIGNATURE
class enum_array : public enum_instance_collection {
private:
	typedef	enum_array<D>				this_type;
	typedef	enum_instance_collection		parent_type;
friend class enum_instance_collection;
public:
	typedef	parent_type::instance_ptr_type		instance_ptr_type;
	typedef	enum_instance_alias			element_type;
	typedef	multikey_map<D, pint_value_type, element_type, qmap>
							collection_type;
	typedef	typename collection_type::key_type	key_type;
private:
	typedef	typename util::multikey<D,pint_value_type>::generator_type
							key_generator_type;
	typedef	typename collection_type::iterator	iterator;
	typedef	typename collection_type::const_iterator
							const_iterator;
private:
	collection_type					collection;
private:
	enum_array();
public:
	enum_array(const scopespace& o, const string& n);
	~enum_array();

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

	bool
	unroll_aliases(const multikey_index_type&, const multikey_index_type&,
		alias_collection_type&) const;

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
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC_NO_POINTERS
};	// end class enum_array

//-----------------------------------------------------------------------------
template <>
class enum_array<0> : public enum_instance_collection {
private:
	typedef	enum_instance_collection		parent_type;
friend class enum_instance_collection;
public:
	typedef	parent_type::instance_ptr_type	instance_ptr_type;
private:
	enum_instance_alias			the_instance;

private:
	enum_array();

public:
	enum_array(const scopespace& o, const string& n);
	~enum_array();

	ostream&
	what(ostream&) const;

	bool
	is_partially_unrolled(void) const;

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

	bool
	unroll_aliases(const multikey_index_type&, const multikey_index_type&,
		alias_collection_type&) const;

public:
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC_NO_POINTERS
};	// end class enum_array (specialized)
#endif	// USE_INSTANCE_COLLECTION_TEMPLATE

//=============================================================================
typedef	instance_array<enum_tag, 0>	enum_scalar;
typedef	instance_array<enum_tag, 1>	enum_array_1D;
typedef	instance_array<enum_tag, 2>	enum_array_2D;
typedef	instance_array<enum_tag, 3>	enum_array_3D;
typedef	instance_array<enum_tag, 4>	enum_array_4D;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_ENUM_H__

