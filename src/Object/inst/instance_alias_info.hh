/**
	\file "Object/inst/instance_alias_info.hh"
	Class declarations for aliases.
	Definition of implementation is in "art_object_instance_collection.tcc"
	This file came from "Object/art_object_instance_alias.h"
		in a previous life.  
	$Id: instance_alias_info.hh,v 1.30 2010/04/30 23:58:44 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_ALIAS_INFO_H__
#define	__HAC_OBJECT_INST_INSTANCE_ALIAS_INFO_H__

#include "util/memory/excl_ptr.hh"
#include "util/memory/count_ptr.hh"
#include "util/persistent_fwd.hh"
#include "common/status.hh"
#include "Object/inst/substructure_alias_base.hh"
#include "Object/traits/class_traits_fwd.hh"
#include "Object/inst/internal_aliases_policy_fwd.hh"
#include "Object/inst/connection_policy.hh"
#include "Object/devel_switches.hh"

namespace HAC {
namespace entity {
struct dump_flags;
class footprint;
class instance_alias_info_actuals;
class alias_visitor;
struct alias_printer;
template <class> struct alias_matcher;
template <class> class collection_interface;
template <class> struct instance_collection_pool_bundle;
template <class> class port_actual_collection;
using std::ostream;
using std::istream;
using util::memory::never_ptr;
using util::memory::count_ptr;
using util::persistent_object_manager;

//=============================================================================
#define	INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE				\
template <class Tag>

#define	INSTANCE_ALIAS_INFO_CLASS					\
instance_alias_info<Tag>

/**
	Information structure for a heirarchical name for a bool.  
	This may be extended arbitrarily to contain attributes.  
	Each object of this type represents a unique qualified name.  
	TODO: parent type that determines whether or not
	this contains relaxed actual parameters, and how they are stored.  
	TODO: factor out the template-independent members/methods of this class.
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
class instance_alias_info :
		public substructure_alias_base<
			class_traits<Tag>::has_substructure>, 
		// was protected
		public class_traits<Tag>::instance_alias_relaxed_actuals_type, 
		public class_traits<Tag>::connection_policy {
	typedef	INSTANCE_ALIAS_INFO_CLASS	this_type;
friend class instance_alias_info_actuals;
public:
	typedef	class_traits<Tag>		traits_type;
	typedef	typename traits_type::instance_alias_relaxed_actuals_type
						actuals_parent_type;
	typedef	substructure_alias_base<traits_type::has_substructure>
						substructure_parent_type;
	typedef	internal_aliases_policy<traits_type::can_internally_alias>
						internal_alias_policy;
	typedef	typename traits_type::connection_policy
						direction_connection_policy;
	typedef	instance_collection_pool_bundle<Tag>
						collection_pool_bundle_type;
	typedef	typename traits_type::instance_type
						instance_type;
	/**
		The reference count pointer type to the underlying
		uniquely created instance.  
	 */
	typedef	count_ptr<instance_type>	instance_ptr_type;

	/**
		Container type.
	 */
	typedef	typename traits_type::instance_collection_generic_type
					instance_collection_generic_type;
	typedef	port_actual_collection<Tag>	port_actuals_type;
	typedef	never_ptr<const port_actuals_type>	port_actuals_ptr_type;
	typedef	collection_interface<Tag>		container_type;
	typedef	instance_collection_generic_type	canonical_container_type;
	typedef	never_ptr<const container_type>	container_ptr_type;

	template <class T>
	class _iterator {
	public:
		typedef	T&		reference;
		typedef	T*		pointer;
		typedef	T		value_type;
	private:
		T*	ptr;
	public:
		explicit
		_iterator(T* s) : ptr(s) { }

		template <class S>
		_iterator(const _iterator<S>& i) : ptr(i.ptr) { }

		operator bool () const { return this->ptr; }

		reference
		operator * () const { return *this->ptr; }

		pointer
		operator -> () const { return this->ptr; }

		this_type&
		operator ++ () {
			this->ptr = this->ptr->next;
			return *this;
		}

		this_type
		operator ++ (int) {
			const _iterator temp(*this);
			this->ptr = this->ptr->next;
			return temp;
		}
	};	// end struct _iterator

	typedef	_iterator<this_type>		pseudo_iterator;
	typedef	_iterator<const this_type>	pseudo_const_iterator;

	typedef	typename actuals_parent_type::alias_actuals_type
						relaxed_actuals_type;
protected:
	this_type*					next;
public:
	// size_t instance_index; // was moved to substructure_base class
	/**
		Back-reference to the mother container.
		Consider using this to determine "instantiated" state.  
		NOTE: this could be pushed to children, and replaced
			with virtual interface.  
		It is through this pointer that an instance is able to
		deduce its position and key within a collection without
		containing the actual key!
	 */
	container_ptr_type				container;

	// cannot use default copy-ctor
	// explicit		// need implicit for vector use
	instance_alias_info(const this_type&);

	// constructors only intended for children classes
	instance_alias_info() : 
		substructure_parent_type(), 
		actuals_parent_type(), 
		direction_connection_policy(),
		next(this), 
		container(NULL) { }

	/**
		Plain constructor initializing with container back-ptr.
	 */
	explicit
	instance_alias_info(const container_ptr_type m) :
		substructure_parent_type(), 
		actuals_parent_type(), 
		direction_connection_policy(),
		next(this), 
		container(m) {
	}

	// default copy-constructor

	// default destructor, non-virtual? yes, but protected.
	~instance_alias_info();

	// default assignment

public:
	/**
		container is inherited from instance_alias_info
	 */
	bool
	valid(void) const { return this->container; }

	// don't inline this virtual
	never_ptr<const physical_instance_collection>
	get_container_base(void) const;

	never_ptr<const physical_instance_collection>
	get_supermost_collection(void) const;

	never_ptr<const substructure_alias>
	get_supermost_substructure(void) const;

	bool
	is_subinstance(void) const;

	void
	check(const container_type* p) const;

	const this_type*
	peek(void) const { return this->next; }

protected:
	good_bool
	unite(this_type&, const unroll_context&);

public:
	pseudo_const_iterator
	find(void) const;


	pseudo_iterator
	find(const unroll_context&);

	void
	finalize_find(const unroll_context&);

	pseudo_iterator
	find(void);

public:
	ostream&
	dump_key(ostream&) const;

public:
	/**
		Instantiates officially by linking to parent collection.  
		FYI: This is only called by instance_array<0> (scalar)
			in instantiate_indices().
	 */
	void
	instantiate_actuals_only(const unroll_context&);

	void
	instantiate(const container_ptr_type p, const unroll_context&);

	void
	instantiate_actual_from_formal(const port_actuals_ptr_type, 
		const unroll_context&, const this_type&);

	void
	import_properties(const this_type&);

	error_count
	check_connection(void) const;

	void
	update_direction_flags(void);

	void
	finalize_canonicalize(this_type&);

	size_t
	assign_local_instance_id(footprint&);

	/**
		Attaches actual parameters to this alias.  
		TODO: make this policy-specific, of course.  
		NOTE: constness is merely for convenience, promising not
			to modify the key.  
		\return true if successful, didn't collide.
	 */
	using actuals_parent_type::get_relaxed_actuals;
	using actuals_parent_type::dump_actuals;
	using actuals_parent_type::dump_complete_type;
	using actuals_parent_type::attach_actuals;
	using actuals_parent_type::compare_actuals;
	using actuals_parent_type::create_dependent_types;

	bool
	has_complete_type(void) const;

	void
	assert_complete_type(void) const;

	pseudo_const_iterator
	find_complete_type_alias(void) const;

private:
	using actuals_parent_type::__initialize_assign_footprint_frame;

public:	// assignment needs to be accessible to std::vector
	// for push_back -- but we guarantee that it is never called at
	// run-time, because this class is not assignable.  
	/**
		Make this unassignable.  Assignment is an error, 
		because this has alias semantics.  
	 */
	this_type&
	operator = (const this_type&);

protected:
	physical_instance_collection&
	trace_collection(const substructure_alias&) const;

#if 0
	physical_instance_collection&
	retrace_collection(const substructure_alias&) const;
#endif

public:

#define	TRACE_ALIAS_BASE_PROTO						\
	typename instance_alias_info<Tag>::substructure_parent_type&	\
	__trace_alias_base(const substructure_alias&) const

#define	TRACE_ALIAS_PROTO						\
	instance_alias_info<Tag>&					\
	trace_alias(const substructure_alias&) const

	TRACE_ALIAS_BASE_PROTO;
	TRACE_ALIAS_PROTO;

public:
	bool
	must_match_type(const this_type&) const;

	// consider: pure virtual multikey_generic<K>

	/**
		Equality comparison is required to determine
		whether or not an object of this type has default value.
	 */
	bool
	operator == (const this_type& i) const {
		return (this->instance_index == i.instance_index)
			&& (this->container == i.container);
	}

	bool
	is_port_alias(void) const;

	ostream&
	dump_alias(ostream& o, const dump_flags&) const;

	ostream&
	dump_aliases(ostream& o) const;

	ostream&
	dump_leaf_name(ostream&) const;

	ostream&
	dump_hierarchical_name(ostream&) const;

	ostream&
	dump_hierarchical_name(ostream&, const dump_flags&) const;

	void
	accept(alias_visitor&) const;

	size_t
	hierarchical_depth(void) const;

	using substructure_parent_type::dump_ports;
#if RECURSE_COLLECT_ALIASES
	using substructure_parent_type::collect_port_aliases;
#endif
	using substructure_parent_type::connect_ports;
	// using substructure_parent_type::lookup_port_instance;

	/// called by top-level
	good_bool
	allocate_assign_subinstance_footprint_frame(footprint_frame&) const;

private:
	using substructure_parent_type::__allocate_subinstance_footprint;

public:
	static
	good_bool
	replay_connect_port(this_type&, this_type&);

	static
	good_bool
	checked_connect_port(this_type&, this_type&, const unroll_context&);

#if INSTANCE_SUPPLY_OVERRIDES
	void
	connect_implicit_port(this_type&);

#if INSTANCE_SUPPLY_DISCONNECT
	void
	disconnect_implicit_port(void);
#endif
#endif

	/// counterpart to load_next_connection
	void
	write_next_connection(const collection_pool_bundle_type&, 
		ostream& o) const;

	void
	load_next_connection(const collection_pool_bundle_type&, istream& i);

	/// static version of load_next_connection
	static
	this_type&
	load_alias_reference(const collection_pool_bundle_type&, istream& i);

public:
	void
	collect_transient_info_base(persistent_object_manager& m) const;

	void
	write_object_base(const footprint&, 
		const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const footprint&, 
		const persistent_object_manager&, istream&);

	void
	collect_transient_info(persistent_object_manager& m) const {
		this->collect_transient_info_base(m);
	}

	void
	write_object(const footprint& f, 
			const persistent_object_manager& m, ostream& o) const {
		this->write_object_base(f, m, o);
	}

	void
	load_object(const footprint& f, 
			const persistent_object_manager& m, istream& i) {
		this->load_object_base(f, m, i);
	}

	class transient_info_collector {
		persistent_object_manager&	manager;
	public:
		explicit
		transient_info_collector(persistent_object_manager& m) :
			manager(m) { }

		void
		operator () (const INSTANCE_ALIAS_INFO_CLASS&);
	};	// end class transient_info_collector

};	// end class instance_alias_info

//-----------------------------------------------------------------------------

template <class Tag>
ostream&
operator << (ostream&, const instance_alias_info<Tag>&);

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_ALIAS_INFO_H__

