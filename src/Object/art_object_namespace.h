/**
	\file "art_object_namespace.h"
	Classes for scoped objects including namespaces.  
	$Id: art_object_namespace.h,v 1.7.2.1.6.1 2005/02/15 07:32:05 fang Exp $
 */

#ifndef	__ART_OBJECT_NAMESPACE_H__
#define	__ART_OBJECT_NAMESPACE_H__

#include "STL/list.h"

#include "art_object_base.h"
#include "persistent.h"		// for persistent object interface
	// includes <iosfwd> <string>

#include "qmap.h"		// need complete definition
#include "hash_qmap.h"		// need complete definition
#include "memory/excl_ptr.h"	// need complete definition (never_ptr members)
#include "memory/list_vector_pool_fwd.h"

namespace ART {
//=============================================================================
// forward declarations from outside namespaces
namespace parser {
	// note: methods may specify string as formal types, 
	// but you can still pass token_identifiers and token_strings
	// because they are derived from string.
	class token_identifier;
	class qualified_id_slice;
	class qualified_id;
}

//=============================================================================
/**
	The namespace of objects that will be returned by the type-checker, 
	and includes the various hierarchical symbol tables in their 
	respective scopes.  
 */
namespace entity {
//=============================================================================
USING_LIST
USING_CONSTRUCT
using std::string;
using std::istream;
using util::hash_qmap;
using util::persistent;
using util::persistent_object_manager;
using parser::token_identifier;
using parser::qualified_id_slice;
using parser::qualified_id;
using namespace util::memory;
using util::qmap;

//=============================================================================
/**
	Generic base class for scoped entities such as namespaces, 
	process definitions, and other definition bodies.  
	To restrict the type of things that can be added to a specific
	scope-class, derive from this class privately.  
	e.g. process definitions cannot contain other namespaces or other 
	process definitions, and namespaces do not contain formals or
	naked language bodies.  
 */
class scopespace : virtual public object, virtual public persistent {
protected:	// typedefs -- keep these here for re-use

	/**
		Aliased namespaces, which are not owned, 
		cannot be modified.  
	 */
	typedef	qmap<string, never_ptr<const name_space> >	alias_map_type;

	/**
		Container for open namespaces with optional aliases.  
		Doesn't have to be a map because never any need to search
		by key.  List implementation is sufficient, because
		whole list will always be searched, if it is searched at all.  
		These pointers are read-only, and thus not
		owned by this namespace.  
	 */
	typedef list<never_ptr<const name_space> >	namespace_list;

	/**
		This set contains the list of identifiers for this namespace
		that have been mapped to some class: either another namespace, 
		a process/data-type/channel definitions/instantiation.  
		The language currently forbids reuse of identifiers within
		this namespace, so one cannot say namespace x {}; followed
		by int x, even though the syntax tree is sufficient to 
		disambiguate between the uses of x, based on context.  
		We do, however, allow the overshadowing of names from
		other namespaces, say that of the parent, or of an 
		imported or sub-space.  
		This type is needed to rapid lookup of identifiers in a body
		that can refer to a symbol declared in the template formals, 
		port parameters, or local definitions and instantiations.  
		The stored value is a generic polymorphic object pointer 
		whose type is deduced in the grammar.  
		EVERY addition to this namespace must register
		through this hash_map.  
		Again, these pointers are not owned.  
		These (redundantly) stored copies of pointers are read-only.  
		To get the modifiable pointers, you'll need to look them up 
		in the corresponding type-specific map.  
	 */
	typedef	hash_qmap<string, some_ptr<object> >	used_id_map_type;

	// new idea: use used_id_map as cache for type references and 
	// parameters expressions.  

	/** convenience struct for dumping */
	class bin_sort {
	// public unary_function<const used_id_map_type::const_iterator&, void>
	public:
		typedef qmap<string, never_ptr<name_space> >
							ns_bin_type;
		typedef qmap<string, never_ptr<definition_base> >
							def_bin_type;
		typedef qmap<string, never_ptr<typedef_base> >
							alias_bin_type;
		typedef qmap<string, never_ptr<instance_collection_base> >
							inst_bin_type;
		typedef qmap<string, never_ptr<param_instance_collection> >
							param_bin_type;

		ns_bin_type		ns_bin;
		def_bin_type		def_bin;
		alias_bin_type		alias_bin;
		inst_bin_type		inst_bin;
		param_bin_type		param_bin;

		// only default constructor
	private:
		// prevent accidental copying
		explicit bin_sort(const bin_sort&);
	public:
		bin_sort();
		void operator() (const used_id_map_type::value_type& i);
	};	// end class bin_sort

	/** read-only version of sorted bins */
	class const_bin_sort {
	// public unary_function<const used_id_map_type::const_iterator&, void>
	public:
		typedef qmap<string, never_ptr<const name_space> >
							ns_bin_type;
		typedef qmap<string, never_ptr<const definition_base> >
							def_bin_type;
		typedef qmap<string, never_ptr<const typedef_base> >
							alias_bin_type;
		typedef qmap<string, never_ptr<const instance_collection_base> >
							inst_bin_type;
		typedef qmap<string, never_ptr<const param_instance_collection> >
							param_bin_type;

		ns_bin_type		ns_bin;
		def_bin_type		def_bin;
		alias_bin_type		alias_bin;
		inst_bin_type		inst_bin;
		param_bin_type		param_bin;

		// only default constructor
	private:
		// prevent accidental copying
		explicit const_bin_sort(const const_bin_sort&);
	public:
		const_bin_sort();
		void operator() (const used_id_map_type::value_type& i);
		void stats(ostream& o) const;
	};	// end class const_bin_sort

protected:	// members
	// should really only contain instantiations? no definitions?
	// what should a generic scopespace contain?
	/**
		Before mapping a new symbol to a symbolic object, 
		it must not already be mapped to an existing object.  
		All additions to the current scope must register
		through this map.  
		Would be *nice* if code was structured to do this 
		automatically, expecially in sub-classes of scopespace.  
		Making this private forces access through the [] operators.  
	 */
	used_id_map_type	used_id_map;

protected:
	scopespace();
public:
virtual	~scopespace();

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o) const = 0;

virtual	const string&
	get_key(void) const = 0;

virtual	string
	get_qualified_name(void) const = 0;

virtual never_ptr<const scopespace>
	get_parent(void) const = 0;

virtual	never_ptr<const object>
	lookup_object_here(const string& id) const;

virtual	never_ptr<object>
	lookup_object_here_with_modify(const string& id) const;

virtual	never_ptr<const object>
	lookup_object(const string& id) const;

virtual	never_ptr<const object>
	lookup_object(const qualified_id_slice& id) const;

virtual	never_ptr<const scopespace>
	lookup_namespace(const qualified_id_slice& id) const;

protected:
	never_ptr<const instance_collection_base>
	add_instance(excl_ptr<instance_collection_base>& i);
public:
	// need id because instantiation statement won't be named yet!
	never_ptr<const instance_collection_base>
	add_instance(never_ptr<instantiation_statement> i, 
		const token_identifier& id);

	bool
	add_definition_alias(const never_ptr<const definition_base> d, 
		const string& a);

	size_t
	exclude_population(void) const;

virtual	bool
	exclude_object(const used_id_map_type::value_type& i) const;

	bool
	exclude_object_val(const used_id_map_type::value_type i) const;

// helper functions for object IO
private:
	// for used_id_map
	void
	collect_used_id_map_pointers(persistent_object_manager& m) const;

	void
	write_object_used_id_map(const persistent_object_manager& m, 
		ostream&) const;

	void
	load_object_used_id_map(const persistent_object_manager& m, 
		istream&);

protected:
	void
	collect_transient_info_base(persistent_object_manager& m) const;

	void
	write_object_base(const persistent_object_manager& m, ostream&) const;

	static
	void
	write_object_base_fake(const persistent_object_manager& m, ostream&);

	void
	load_object_base(const persistent_object_manager& m, istream&);

private:
// no concrete method for loading -- that remains derived-class specific
// so each sub-class may impose its own restrictions
virtual	void
	load_used_id_map_object(excl_ptr<persistent>& o) = 0;
};	// end class scopespace

//=============================================================================
/**
	Namespace container class.  
 */
class name_space : public scopespace {
private:
	typedef	name_space			this_type;
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
	PERSISTENT_METHODS_DECLARATIONS

/** helper method for adding a variety of objects */
	void
	load_used_id_map_object(excl_ptr<persistent>& o);
public:
	static const never_ptr<const name_space>	null;

	LIST_VECTOR_POOL_ESSENTIAL_FRIENDS
	LIST_VECTOR_POOL_STATIC_DECLARATIONS

};	// end class name_space

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_NAMESPACE_H__

