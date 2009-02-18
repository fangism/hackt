/**
	\file "Object/common/namespace.h"
	Classes for scoped objects including namespaces.  
	This file came from "Object/common/namespace.h"
		in its previous life.  
	$Id: namespace.h,v 1.8 2009/02/18 00:22:29 fang Exp $
 */

#ifndef	__HAC_OBJECT_COMMON_NAMESPACE_H__
#define	__HAC_OBJECT_COMMON_NAMESPACE_H__

#include "Object/common/scopespace.h"
#include "Object/common/object_base.h"
#include "Object/common/util_types.h"

#include "util/memory/list_vector_pool_fwd.h"

//=============================================================================
namespace HAC {
namespace entity {
USING_CONSTRUCT

/**
	Namespace container class.  
 */
class name_space : public object, public scopespace {
private:
	typedef	name_space			this_type;
	typedef	scopespace			parent_type;
	// list of pointers to other opened namespaces (and their aliases)
	// table of type definitions (user-defined data types)
	// table of process definitions
	// table of real instantiations (outside of definitions)

	// determine for which elements the order matters:

	// does order of type definitions matter?
	// for now yes, because there is no support for prototype declarations
	// yet, which enforces an ordering on the declarations.  
	// or this may just be a temporary restriction on the ordering of defs

	// order of instantiations? shouldn't matter.

protected:
	/**
		The (short) name of the namespace.  
		Should be const, but pool requires assignability.  
	 */
	string					key;

	/**
		The parent namespace of this namespace.  
		Should be const.  
	 */
	never_ptr<const name_space>		parent;

	/**
		The set of namespaces which are open to search within
		this namespace's scope.  The imported namespaces may be 
		re-opened and renamed.  When this namespace closes, however, 
		the list of open spaces and aliases will be purged.  
		You'll have to add them each time you re-open this
		namespace if you want them, otherwise, it is a convenient
		way to reset the namespace alias list.  
		Remember: not owned.  
		CONSIDER: possible cycles in searching?
			hash pointers of searched, to prevent re-visiting?
	 */
	namespace_list		open_spaces;

	/**
		This is the set of open namespaces that are aliased
		locally under a different sub-namespaces, 
		a pseudo subspace.  These, too, are cleared upon leaving
		this namespace.  These will always be searched just like
		any other sub-namespace.  The difference is that 
		these namespace pointers are not owned, and thus are
		not deleted at destruction time.  
	 */
	alias_map_type		open_aliases;

	// need some structure for unrolled_instances

	// later introduce single symbol imports?
	// i.e. using A::my_type;
private:
	name_space();

public:
	explicit
	name_space(const string& n);

	name_space(const string& n, never_ptr<const name_space>);

	~name_space();

	const string&
	get_key(void) const;

	never_ptr<const scopespace>
	get_parent(void) const;

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	ostream&
	pair_dump(ostream& o) const;

	string
	get_qualified_name(void) const;

#if 0
	ostream&
	dump_qualified_name(ostream&) const;
#endif
	ostream&
	dump_qualified_name(ostream&, const dump_flags&) const;

	// horrible hack, overriding scopespace::'s
	bool
	is_global_namespace(void) const;

	never_ptr<const name_space>
	get_global_namespace(void) const;

// update these return types later
	never_ptr<name_space>
	add_open_namespace(const string& n);

	never_ptr<const name_space>
	leave_namespace(void);	// or close_namespace

	never_ptr<const name_space>
	add_using_directive(const qualified_id& n);

	never_ptr<const name_space>
	add_using_alias(const qualified_id& n, const string& a);

	// overrides default
	never_ptr<const object>
	lookup_member(const string&) const;

private:
	never_ptr<name_space>
	add_namespace(excl_ptr<name_space>& ns);

public:

	// do we really need to specialize adding definitions by class?
	// to be used ONLY by the global namespace (???)
	never_ptr<definition_base>
	add_definition(excl_ptr<definition_base>& db);

// returns type if unique match found, else NULL
	never_ptr<const scopespace>
	lookup_namespace(const qualified_id_slice& id) const;

	never_ptr<const name_space>
	lookup_open_alias(const string& id) const;

	void
	collect_namespaces(namespace_collection_type&) const;

// type-specific counterparts, obsolete

// some private utility functions (may become public later)
// add versions for querying for types, instantiations, etc...
private:
	never_ptr<const name_space>
	query_namespace_match(const qualified_id_slice& id) const;

	never_ptr<const name_space>
	query_subnamespace_match(const qualified_id_slice& id) const;

	void	
	query_import_namespace_match(namespace_list& m, 
		const qualified_id& id) const;

	// these will not be recursive, but iteratively invoked by
	// add_blah_inst/def();

	// the following are not used... yet
	void
	find_namespace_ending_with(namespace_list& m, 
		const qualified_id& id) const;

	void
	find_namespace_starting_with(namespace_list& m, 
		const qualified_id& id) const;

// void	inherit_built_in_types(void);		// OBSOLETE

// will we need generalized versions of queries that return object*
// if we don't know a priori what an identifier's class is?
// single symbol table or separate?

	bool
	exclude_object(const used_id_map_type::value_type& i) const;

public:
	void
	unroll_params(void);

	void
	unroll_instances(void);

	void
	unroll_connections(void);

// methods for object file I/O
public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

/** helper method for adding a variety of objects */
	void
	load_used_id_map_object(excl_ptr<persistent>& o);
public:
	static const never_ptr<const name_space>	null;

	LIST_VECTOR_POOL_ESSENTIAL_FRIENDS
	LIST_VECTOR_POOL_DEFAULT_STATIC_DECLARATIONS

};	// end class name_space

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

//=============================================================================
// specializations in other namespaces

#if 0
namespace util {
	SPECIALIZE_UTIL_WHAT_DECLARATION(HAC::entity::name_space)
}	// end namespace util
#endif

//=============================================================================
#endif	// __HAC_OBJECT_COMMON_NAMESPACE_H__

