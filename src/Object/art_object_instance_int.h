/**
	\file "art_object_instance_int.h"
	Class declarations for built-in and user-defined data instances
	and instance collections.  
	$Id: art_object_instance_int.h,v 1.9.2.3.2.6.2.1 2005/02/24 19:34:39 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_INT_H__
#define	__ART_OBJECT_INSTANCE_INT_H__

#include "art_object_instance.h"
#include "memory/pointer_classes.h"

#include <set>
#include "multikey_set.h"
#include "ring_node.h"
#include "packed_array_fwd.h"


namespace ART {
namespace entity {
USING_LIST
using std::set;
using std::string;
using namespace util::memory;
using util::ring_node_derived;
using util::multikey_set;
using util::multikey_set_element_derived;
using util::packed_array_generic;

//=============================================================================
// class datatype_instance_collection declared in "art_object_instance.h"

class int_instance;

template <size_t>
class int_array;

//=============================================================================
class int_instance_alias_info {
	typedef	int_instance_alias_info			this_type;
public:
	typedef	count_ptr<int_instance>			instance_ptr_type;
	typedef	int_instance_collection			container_type;
public:
	instance_ptr_type				instance;
	never_ptr<const container_type>			container;
protected:
	int_instance_alias_info() : instance(NULL), container(NULL) { }

public:
	int_instance_alias_info(const never_ptr<const container_type> m) :
		instance(NULL), container(m) { }

virtual	~int_instance_alias_info();

	bool
	valid(void) const { return container; }

	void
	instantiate(const never_ptr<const container_type> p) {
		NEVER_NULL(p);
		INVARIANT(!container);
		container = p;
	}

	bool
	operator == (const this_type& i) const {
		return instance == i.instance && container == i.container;
	}

virtual	void
	dump_alias(ostream& o) const;

virtual	void
	write_next_connection(const persistent_object_manager& m, 
		ostream& o) const;

public:
	void
	collect_transient_info_base(persistent_object_manager& m) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

	class transient_info_collector {
		persistent_object_manager&      manager;
	public:
		explicit
		transient_info_collector(persistent_object_manager& m) :
			manager(m) { }

		void
		operator () (const int_instance_alias_info&);
	};	// end class transient_info_collector

};	// end class int_instance_alias_info

//-----------------------------------------------------------------------------
typedef	ring_node_derived<int_instance_alias_info>
						int_instance_alias_base;

ostream&
operator << (ostream&, const int_instance_alias_base&);

//-----------------------------------------------------------------------------
class int_instance : public persistent {
	never_ptr<int_instance_alias_base>	back_ref;
public:
	int_instance();

	PERSISTENT_METHODS_DECLARATIONS

};	// end class int_instance

//-----------------------------------------------------------------------------
/**
	An uninitialized reference to a int instance.  
	Only after references are connected, are the actual int instances
	created.  
	Contains attribute fields.  
 */
template <size_t D>
class int_instance_alias :
		public multikey_set_element_derived<
			D, pint_value_type, int_instance_alias_base> {
	typedef	int_instance_alias<D>			this_type;
public:
	typedef	multikey_set_element_derived<
			D, pint_value_type, int_instance_alias_base>
							parent_type;

private:
	/**
		grandparent_type is maplikeset_element_derived.
	 */
	typedef	typename parent_type::value_type	grandparent_type;
	/**
		great_grandparent_type is ring_node_derived
	 */
	typedef	typename grandparent_type::parent_type	great_grandparent_type;
public:
	typedef	typename parent_type::key_type		key_type;
	// or simple_type?

public:
	int_instance_alias() : parent_type() { }

	int_instance_alias(const key_type& k) : parent_type(k) { }

	int_instance_alias(const key_type& k, 
		const never_ptr<const int_instance_collection> p) :
			parent_type(k, grandparent_type(
				great_grandparent_type(p))) { }
	~int_instance_alias();

	/**
		Implicit conversion needed for some multikey_assoc
		algorithms.  
	 */
	operator const key_type& () const { return key; }

	void
	write_next_connection(const persistent_object_manager&, 
		ostream&) const;

	void
	dump_alias(ostream&) const;

	/**
		Whether or not they refer to the same node.
		Check for instantiated?
	 */
	bool
	operator == (const parent_type& i) const {
		// return &canonical() == &i.canonical();
		return this->contains(p);
	}

#if 0
	friend
	ostream&
	operator << (ostream&, const int_instance_alias&);
#endif
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC

};	// end class int_instance_alias

template <size_t D>
ostream&
operator << (ostream&, const int_instance_alias<D>&);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
class int_instance_alias<0> : public int_instance_alias_base {
private:
	typedef	int_instance_alias<0>		this_type;
	typedef	int_instance_alias_base		parent_type;
public:
	~int_instance_alias();

	void
	dump_alias(ostream& o) const;

	void
	write_next_connection(const persistent_object_manager& m,
		ostream& o) const;

public:
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC
};	// end class int_instance_alias<0>

//-----------------------------------------------------------------------------
/**
	Interface to collection of data-int instance aliases.  
	TODO: need to add a width parameter, or at least reference
		the fully-specified type.
 */
class int_instance_collection : public datatype_instance_collection {
private:
	typedef	datatype_instance_collection		parent_type;
	typedef	int_instance_collection			this_type;
public:
	typedef	parent_type::type_ref_ptr_type		type_ref_ptr_type;
	typedef	int_instance_alias_base			instance_alias_base_type;
	typedef	never_ptr<instance_alias_base_type>		instance_ptr_type;
	// duplicated in instance_reference, by the way...
	typedef	packed_array_generic<pint_value_type, instance_ptr_type>
							alias_collection_type;
	typedef	int_alias_connection			alias_connection_type;
	typedef	pint_value_type				param_type;
	typedef	parent_type::inst_ref_ptr_type		inst_ref_ptr_type;
	typedef	parent_type::member_inst_ref_ptr_type	member_inst_ref_ptr_type;
private:
	/**
		The bit-width of the integers in this collection.  
		Per collection, this is set once at unroll-time.
	 */
	param_type					int_width;
protected:
	explicit
	int_instance_collection(const size_t d) :
		parent_type(d), int_width(0) { }
public:
	int_instance_collection(const scopespace& o, const string& n, 
		const size_t d);

virtual	~int_instance_collection();

	size_t
	width(void) const { return int_width; }

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
	make_member_instance_reference(const inst_ref_ptr_type&) const;

virtual	void
	instantiate_indices(const index_collection_item_ptr_type& i) = 0;

	never_ptr<const const_param_expr_list>
	get_actual_param_list(void) const;

virtual instance_ptr_type
	lookup_instance(const multikey_index_type& i) const = 0;

virtual	bool
	lookup_instance_collection(list<instance_ptr_type>& l, 
		const const_range_list& r) const = 0;

virtual	const_index_list
	resolve_indices(const const_index_list& l) const = 0;

#if 0
virtual int
	connect(const multikey_index_type& k, const int_instance_alias& b) = 0;
#endif
virtual	bool
	unroll_aliases(const multikey_index_type&, const multikey_index_type&, 
		alias_collection_type&) const = 0;

public:

	static
	int_instance_collection*
	make_int_array(const scopespace& o, const string& n, const size_t d);

	static
	persistent*
	construct_empty(const int);

protected:
	void
	write_object_base(const persistent_object_manager& m, ostream& ) const;

	void
	load_object_base(const persistent_object_manager& m, istream& );

};	// end class int_instance_collection

//-----------------------------------------------------------------------------
#define	INT_ARRAY_TEMPLATE_SIGNATURE		template <size_t D>

/**
	Multidimensional collection of int instance aliases.  
	\param D the number of dimensions (max. 4).  
 */
INT_ARRAY_TEMPLATE_SIGNATURE
class int_array : public int_instance_collection {
friend class int_instance_collection;
	typedef	int_array<D>				this_type;
	typedef	int_instance_collection			parent_type;
public:
	typedef	parent_type::instance_ptr_type		instance_ptr_type;
	typedef	parent_type::alias_collection_type	alias_collection_type;
	typedef	int_instance_alias<D>			element_type;
	typedef	multikey_set<D, element_type>		collection_type;
	typedef	typename element_type::key_type		key_type;
	typedef	typename collection_type::value_type	value_type;
private:
	typedef	typename util::multikey<D, pint_value_type>::generator_type
							key_generator_type;
	typedef	element_type&				reference;
	typedef	typename collection_type::iterator	iterator;
	typedef	typename collection_type::const_iterator
							const_iterator;
private:
	collection_type					collection;
private:
	int_array();
public:
	int_array(const scopespace& o, const string& n);
	~int_array();

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
	lookup_instance(const multikey_index_type& l) const;

	// is this used? or can it be replaced by unroll_aliases?
	bool
	lookup_instance_collection(list<instance_ptr_type>& l, 
		const const_range_list& r) const;

	bool
	unroll_aliases(const multikey_index_type&, const multikey_index_type&, 
		alias_collection_type&) const;

	class element_writer {
		ostream& os;
		const persistent_object_manager& pom;
	public:
		element_writer(const persistent_object_manager& m, ostream& o)
			: os(o), pom(m) { }

		void
		operator () (const element_type& ) const;
	};      // end struct element_writer

	class connection_writer {
		ostream& os;
		const persistent_object_manager& pom;
	public:
		connection_writer(const persistent_object_manager& m,
			ostream& o) : os(o), pom(m) { }

		void
		operator () (const element_type& ) const;
	};      // end struct connection_writer


	struct key_dumper {
		ostream& os;

		key_dumper(ostream& o) : os(o) { }

		ostream&
		operator () (const value_type& );
	};	// end struct key_dumper

public:
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC
};	// end class int_array

//-----------------------------------------------------------------------------
template <>
class int_array<0> : public int_instance_collection {
friend class int_instance_collection;
	typedef	int_instance_collection		parent_type;
	typedef	int_array<0>			this_type;
public:
	typedef	parent_type::instance_ptr_type	instance_ptr_type;
	typedef	parent_type::alias_collection_type
						alias_collection_type;
	typedef	int_instance_alias<0>		instance_type;
private:
	instance_type				the_instance;

private:
	int_array();

public:
	int_array(const scopespace& o, const string& n);
	~int_array();

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

	bool
	unroll_aliases(const multikey_index_type&, const multikey_index_type&, 
		alias_collection_type&) const;

	const_index_list
	resolve_indices(const const_index_list& l) const;


public:
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC
};	// end class int_array (specialized)

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_INT_H__

