/**
	\file "Object/art_object_instance_collection.h"
	Class declarations for scalar instances and instance collections.  
	$Id: art_object_instance_collection.h,v 1.9.2.3 2005/06/14 05:38:32 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INSTANCE_COLLECTION_H__
#define	__OBJECT_ART_OBJECT_INSTANCE_COLLECTION_H__

#include <iosfwd>
#include <set>

#include "Object/art_object_classification_fwd.h"
#include "Object/art_object_index.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "util/persistent.h"
#include "util/multikey_set.h"
#include "util/boolean_types.h"

namespace ART {
namespace entity {
USING_LIST
using std::istream;
using std::ostream;
using std::set;
using std::string;
using util::memory::count_ptr;
using util::memory::never_ptr;
using util::bad_bool;
using util::multikey_set;
using util::multikey_set_element_derived;
using util::persistent;
using util::persistent_object_manager;

class scopespace;
class meta_instance_reference_base;
class nonmeta_instance_reference_base;
class const_index_list;
class const_range_list;
class const_param_expr_list;

//=============================================================================
/**
	This is a functor for specializing the formatting of printed types.  
	We provide a default implementation.  
	Specializations should follow the same pattern.  
	(Why not use plain static functions?)
 */
template <class Tag>
struct collection_type_manager {
	typedef	typename class_traits<Tag>::instance_collection_parameter_type
					instance_collection_parameter_type;
	typedef	typename class_traits<Tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef	typename class_traits<Tag>::type_ref_ptr_type
					type_ref_ptr_type;

	/// was separate type-dumper functor
	struct dumper {
		ostream& os;
		dumper(ostream& o) : os(o) { }

		// intentionally undefined
		ostream&
		operator () (const instance_collection_generic_type&);
	};	// end struct dumper

	static
	void
	collect(persistent_object_manager&, 
		const instance_collection_generic_type&);

	static
	void
	write(const persistent_object_manager&, ostream&, 
		const instance_collection_generic_type&);

	static
	void
	load(const persistent_object_manager&, istream&, 
		instance_collection_generic_type&);

	static
	type_ref_ptr_type
	get_type(const instance_collection_generic_type&);

	/**
		NOTE: Was separate type-dumper functor.
		\return true on error, false on success.
	 */
	static
	bad_bool
	commit_type(instance_collection_generic_type&, 
		const type_ref_ptr_type&);

};	// end struct type_manager

//-----------------------------------------------------------------------------
//=============================================================================
#define	INSTANCE_COLLECTION_TEMPLATE_SIGNATURE				\
template <class Tag>

#define	INSTANCE_COLLECTION_CLASS					\
instance_collection<Tag>

/**
	Interface to collection of instance aliases.  
	This abstract base class is dimension-generic.  
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
class instance_collection :
	public class_traits<Tag>::instance_collection_parent_type {
friend struct collection_type_manager<Tag>;
private:
	typedef	Tag					category_type;
	typedef	typename class_traits<Tag>::instance_collection_parent_type
							parent_type;
	typedef	INSTANCE_COLLECTION_CLASS		this_type;
public:
	typedef	typename class_traits<Tag>::type_ref_type
							type_ref_type;
	typedef	typename class_traits<Tag>::type_ref_ptr_type
							type_ref_ptr_type;
	typedef	typename class_traits<Tag>::instance_alias_base_type
							instance_alias_base_type;
	typedef	never_ptr<instance_alias_base_type>	instance_alias_base_ptr_type;
	typedef	typename class_traits<Tag>::alias_collection_type
							alias_collection_type;
	typedef	typename class_traits<Tag>::instance_collection_parameter_type
					instance_collection_parameter_type;
	typedef	typename class_traits<Tag>::simple_meta_instance_reference_type
					simple_meta_instance_reference_type;
	typedef	typename class_traits<Tag>::simple_nonmeta_instance_reference_type
					simple_nonmeta_instance_reference_type;
	typedef	typename class_traits<Tag>::member_simple_meta_instance_reference_type
				member_simple_meta_instance_reference_type;
//	typedef	meta_instance_reference_base		meta_instance_reference_base_type;
// public:
protected:
	typedef	typename parent_type::inst_ref_ptr_type	inst_ref_ptr_type;
	typedef	typename parent_type::member_inst_ref_ptr_type
							member_inst_ref_ptr_type;
private:
	/**
		General parameter object for type checking.  
	 */
	instance_collection_parameter_type		type_parameter;
protected:
	explicit
	instance_collection(const size_t d) :
		parent_type(d), type_parameter() { }
public:
	instance_collection(const scopespace& o, const string& n, 
		const size_t d);

virtual	~instance_collection();

	const instance_collection_parameter_type&
	get_type_parameter(void) const { return type_parameter; }

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

	type_ref_ptr_type
	get_type_ref_subtype(void) const;

	bad_bool
	commit_type(const type_ref_ptr_type& );

	count_ptr<meta_instance_reference_base>
	make_meta_instance_reference(void) const;

	count_ptr<nonmeta_instance_reference_base>
	make_nonmeta_instance_reference(void) const;

	member_inst_ref_ptr_type
	make_member_meta_instance_reference(const inst_ref_ptr_type&) const;

virtual	void
	instantiate_indices(const const_range_list& i) = 0;

	never_ptr<const const_param_expr_list>
	get_actual_param_list(void) const;

virtual instance_alias_base_ptr_type
	lookup_instance(const multikey_index_type& i) const = 0;

virtual	bool
	lookup_instance_collection(list<instance_alias_base_ptr_type>& l, 
		const const_range_list& r) const = 0;

virtual	const_index_list
	resolve_indices(const const_index_list& l) const = 0;

#if 0
virtual int
	connect(const multikey_index_type& k, const instance_alias& b) = 0;
#endif
virtual	bool
	unroll_aliases(const multikey_index_type&, const multikey_index_type&, 
		alias_collection_type&) const = 0;

public:
virtual	instance_alias_base_type&
	load_reference(istream& i) const = 0;

	static
	this_type*
	make_array(const scopespace& o, const string& n, const size_t d);

	static
	persistent*
	construct_empty(const int);

protected:
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class instance_collection

//-----------------------------------------------------------------------------
#define	INSTANCE_ARRAY_TEMPLATE_SIGNATURE				\
template <class Tag, size_t D>

#define	INSTANCE_SCALAR_TEMPLATE_SIGNATURE				\
template <class Tag>


#define	INSTANCE_ARRAY_CLASS						\
instance_array<Tag,D>

#define	INSTANCE_SCALAR_CLASS						\
instance_array<Tag,0>

/**
	Multidimensional collection of int instance aliases.  
	\param D the number of dimensions (max. 4).  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
class instance_array :
	public class_traits<Tag>::instance_collection_generic_type {
friend class instance_collection<Tag>;
	typedef	instance_array<Tag,D>			this_type;
	typedef	typename class_traits<Tag>::instance_collection_generic_type
							parent_type;
public:
	typedef	typename class_traits<Tag>::instance_alias_base_type
							instance_alias_base_type;
//	typedef	typename parent_type::instance_alias_base_ptr_type
	typedef	typename class_traits<Tag>::instance_alias_base_ptr_type
							instance_alias_base_ptr_type;
	typedef	typename class_traits<Tag>::alias_collection_type
							alias_collection_type;
	// template explicitly required by g++-4.0
	typedef	typename class_traits<Tag>::template instance_alias<D>::type
							element_type;
	/**
		This is the data structure used to implement the collection.  
	 */
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
	instance_array();
public:
	instance_array(const scopespace& o, const string& n);
	~instance_array();

	ostream&
	what(ostream& o) const;

	bool
	is_partially_unrolled(void) const;

	ostream&
	dump_unrolled_instances(ostream& o) const;

	void
	instantiate_indices(const const_range_list& i);

	const_index_list
	resolve_indices(const const_index_list& l) const;

	instance_alias_base_ptr_type
	lookup_instance(const multikey_index_type& l) const;

	// is this used? or can it be replaced by unroll_aliases?
	bool
	lookup_instance_collection(list<instance_alias_base_ptr_type>& l, 
		const const_range_list& r) const;

	bool
	unroll_aliases(const multikey_index_type&, const multikey_index_type&, 
		alias_collection_type&) const;

	instance_alias_base_type&
	load_reference(istream& i) const;

	class element_writer {
		ostream& os;
		const persistent_object_manager& pom;
	public:
		element_writer(const persistent_object_manager& m, ostream& o)
			: os(o), pom(m) { }

		void
		operator () (const element_type& ) const;
	};      // end struct element_writer

	class element_loader {
		istream& is;
		const persistent_object_manager& pom;
		collection_type& coll;
	public:
		element_loader(const persistent_object_manager& m,
			istream& i, collection_type& c) :
			is(i), pom(m), coll(c) { }

		void
		operator () (void);
	};      // end class element_loader

	class connection_writer {
		ostream& os;
		const persistent_object_manager& pom;
	public:
		connection_writer(const persistent_object_manager& m,
			ostream& o) : os(o), pom(m) { }

		void
		operator () (const element_type& ) const;
	};      // end struct connection_writer

	class connection_loader {
		istream& is;
		const persistent_object_manager& pom;
	public:
		connection_loader(const persistent_object_manager& m,
			istream& i) : is(i), pom(m) { }

		void
		operator () (const element_type& );
	};      // end class connection_loader

	struct key_dumper {
		ostream& os;

		key_dumper(ostream& o) : os(o) { }

		ostream&
		operator () (const value_type& );
	};	// end struct key_dumper

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC
};	// end class instance_array

//-----------------------------------------------------------------------------
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
class instance_array<Tag,0> :
		public class_traits<Tag>::instance_collection_generic_type {
friend class instance_collection<Tag>;
	typedef	typename class_traits<Tag>::instance_collection_generic_type
							parent_type;
	typedef	INSTANCE_SCALAR_CLASS			this_type;
public:
	typedef	typename class_traits<Tag>::instance_alias_base_type
						instance_alias_base_type;
	typedef	typename class_traits<Tag>::instance_alias_base_ptr_type
						instance_alias_base_ptr_type;
	typedef	typename class_traits<Tag>::alias_collection_type
							alias_collection_type;
	// template explicitly required by g++-4.0
	typedef	typename class_traits<Tag>::template instance_alias<0>::type
							instance_type;
private:
	instance_type					the_instance;

private:
	instance_array();

public:
	instance_array(const scopespace& o, const string& n);

	~instance_array();

	ostream&
	what(ostream&) const;

	bool
	is_partially_unrolled(void) const;

	ostream&
	dump_unrolled_instances(ostream& o) const;

	void
	instantiate_indices(const const_range_list& i);

	instance_alias_base_ptr_type
	lookup_instance(const multikey_index_type& l) const;

	bool
	lookup_instance_collection(list<instance_alias_base_ptr_type>& l, 
		const const_range_list& r) const;

	bool
	unroll_aliases(const multikey_index_type&, const multikey_index_type&, 
		alias_collection_type&) const;

	instance_alias_base_type&
	load_reference(istream& i) const;

	const_index_list
	resolve_indices(const const_index_list& l) const;


public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS_NO_ALLOC
#if 0
	// soon...
	LIST_VECTOR_POOL_ESSENTIAL_FRIENDS
	LIST_VECTOR_POOL_DEFAULT_STATIC_DECLARATIONS
#endif
};	// end class instance_array (specialized)

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INSTANCE_COLLECTION_H__

