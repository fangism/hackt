/**
	\file "art_object_base.h"
	Base classes for semantic objects.  
 */

#ifndef	__ART_OBJECT_BASE_H__
#define	__ART_OBJECT_BASE_H__

#include <iosfwd>
#include <string>
#include <list>
#include <deque>

#include "art_macros.h"
#include "persistent.h"		// for persistent object interface

#include "qmap.h"		// need complete definition
#include "hash_qmap.h"		// need complete definition
#include "ptrs.h"		// need complete definition (never_ptr members)
#include "count_ptr_fwd.h"

// for convenience
#include "art_object_type_hash.h"

//=============================================================================
// macros

#if 0
DEFINITIONS OF THESE MACROS HAVE MOVED
/***
	Standard set of prototypes for persistent object IO-related
	methods.  
	I got sick of typing and pasting them over and over...
	Should be in classes' public sections.  
	Note: this macro should be used only in final concrete classes, 
	because they are all non-virtual.  
	Don't stick a semicolon after this.  
***/
#define	ART_OBJECT_IO_METHODS_NO_POINTERS				\
	void write_object(const persistent_object_manager& m) const;	\
static	object* construct_empty(void);					\
	void load_object(persistent_object_manager& m);

#define	ART_OBJECT_IO_METHODS						\
	ART_OBJECT_IO_METHODS_NO_POINTERS				\
	void collect_transient_info(persistent_object_manager& m) const;

//-----------------------------------------------------------------------------
// macros for use in write_object and load_object
// just sanity-check extraneous information, later enable or disable
// with another switch.

// sanity check switch is overrideable by the includer
#ifndef	NO_OBJECT_SANITY
#define	NO_OBJECT_SANITY	0	// default 0, keep sanity checks
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if NO_OBJECT_SANITY
#define	WRITE_POINTER_INDEX(f, m)
#else
#define	WRITE_POINTER_INDEX(f, m)					\
	write_value(f, m.lookup_ptr_index(this))
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if NO_OBJECT_SANITY
#define	STRIP_POINTER_INDEX(f, m)
#else
#define	STRIP_POINTER_INDEX(f, m) 					\
	{								\
	long index;							\
	read_value(f, index);						\
	if (index != m.lookup_ptr_index(this)) {			\
		long hohum = m.lookup_ptr_index(this);			\
		cerr << "<object>::load_object(): " << endl		\
			<< "\tthis = " << this << ", index = " << index	\
			<< ", expected: " << hohum << endl;		\
		assert(index == m.lookup_ptr_index(this));		\
	}								\
	}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// more sanity-check code, make this switchable
#if NO_OBJECT_SANITY
#define	WRITE_OBJECT_FOOTER(f)
#else
#define	WRITE_OBJECT_FOOTER(f)						\
	write_value(f, -1L)
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if NO_OBJECT_SANITY
#define	STRIP_OBJECT_FOOTER(f)
#else
#define	STRIP_OBJECT_FOOTER(f)						\
	{								\
	long neg_one;							\
	read_value(f, neg_one);						\
	assert(neg_one == -1L);						\
	}
#endif

#endif

//=============================================================================
// temporary switches

//=============================================================================
// forward declarations

namespace ART {
//=============================================================================
// forward declarations from outside namespaces
namespace parser {
	// note: methods may specify string as formal types, 
	// but you can still pass token_identifiers and token_strings
	// because they are derived from string.
	class token_string;
	class token_identifier;
	class qualified_id_slice;
	class qualified_id;
	class context;
}
using namespace parser;

//=============================================================================
/**
	The namespace of objects that will be returned by the type-checker, 
	and includes the various hierarchical symbol tables in their 
	respective scopes.  
 */
namespace entity {
//=============================================================================
	using namespace std;
	using namespace util;
	using namespace PTRS_NAMESPACE;	// for experimental pointer classes
	using namespace COUNT_PTR_NAMESPACE;
	using namespace QMAP_NAMESPACE;
	using namespace HASH_QMAP_NAMESPACE;

//=============================================================================
// forward declarations

	class scopespace;
	class name_space;
	class loop_scope;
	class conditional_scope;

	class definition_base;
	class typedef_base;
	class channel_definition_base;
	class datatype_definition_base;
	class process_definition_base;
	class process_definition;
	class enum_datatype_def;
	class built_in_datatype_def;
	class built_in_param_def;

	class fundamental_type_reference;
	class collective_type_reference;
	class data_type_reference;
	class channel_type_reference;
	class process_type_reference;
	class param_type_reference;		// redundant

	class instance_collection_base;
	class channel_instance_collection;
	class datatype_instance_collection;
	class process_instance_collection;
	class param_instance_collection;
	class pint_instance_collection;
	class pbool_instance_collection;

	class instantiation_statement;

	class instance_reference_base;
	class simple_instance_reference;
	class member_instance_reference_base;
	class datatype_instance_reference;
	class channel_instance_reference;
	class process_instance_reference;
	class param_instance_reference;
	class pint_instance_reference;		// relocated "art_object_expr"
	class pbool_instance_reference;		// relocated "art_object_expr"

	class instance_management_base;
//	class connection_assignment_base;
	class param_expression_assignment;
	class instance_reference_connection;
	class aliases_connection;
	class port_connection;

// declarations from "art_object_expr.h"
	class param_expr;
	class param_expr_list;
	class const_param_expr_list;
	class dynamic_param_expr_list;
	class pint_expr;
	class pbool_expr;
	class range_expr;
	class pint_range;
	class const_range;
	class range_expr_list;
	class const_range_list;
	class dynamic_range_list;
	class index_list;			// not ART::parser::index_list
	class const_index_list;

	/**
		Value type of this needs to be more general
		to accommodate loop and conditional scopes?
	 */
	typedef	count_const_ptr<range_expr_list>
					index_collection_item_ptr_type;
	/**
		UPDATE: now contains reference to instantiation_statements, 
		which *contain* the index/range expressions.  

		We keep track of the state of instance collections at
		various program points with this container.
	 */
	typedef	deque<never_const_ptr<instantiation_statement> >
					index_collection_type;

	/** the state of an instance collection, kept track by each 
		instance reference */
	typedef	index_collection_type::const_iterator
					instantiation_state;

//=============================================================================
// general non-member functions

//=============================================================================
/**
	The mother-root object type.
	Any class that will persist in the form of an object file
	should ultimately derive from this class.  
 */
class object {
public:
virtual ~object() { }

/**
	What eeeeez it man?
 */
virtual	ostream& what(ostream& o) const = 0;

/**
	Detailed hierarchical dump of contents.  
	Later: add detail-level argument.  
 */
virtual	ostream& dump(ostream& o) const = 0;

#if 0
/**	prints out what it (may have) looked like in source */
virtual	ostream& source(ostream& o) const = 0;
#endif

/**
	Use this method to automatically dereference object handles.  
 */
virtual const object& self(void) const { return *this; }

/**
	Consider an interface to hierarchical errors embedded in the objects.  
	Mmmm... fancy.
**/

#if 0
FACTORED OUT INTO class persistent;
/** walks object hierarchy and registers reachable pointers with manager */
virtual	void collect_transient_info(persistent_object_manager& m) const;
/** Writes the object out to a managed buffer */
virtual	void write_object(const persistent_object_manager& m) const;
/** Loads the object from a managed buffer */
virtual	void load_object(persistent_object_manager& m);

public:
	static bool			warn_unimplemented;
#endif
};	// end class object

//=============================================================================
/**
	Need handle class of object to add one level of indirection.  
	When handle is destroyed, the dereferenced member is not de-allocated. 
	This is particularly useful for aggregate types whose elements
	are non-const and owned by the container, but you want to 
	add a const (non-owned) member.  
	Using this will ensure that when the handle is deleted the 
	referenced object is left intact.  
	Used in class name_space to manage open namespaces.  
 */
class object_handle : public object {
protected:
	/**
		The actual object referenced.  
		Never delete.  
		Is it ever NULL?
		Should be reference instead of pointer?
	 */
	const object&			obj;
public:
	/**
		Constructor to wrap around an object reference.  
		\param o may not be a handle.  
			Someone else should have responsibility for deleting.  
	 */
	object_handle(never_const_ptr<object> o);

	/**
		No intention to de-allocate reference object.  
	 */
	~object_handle() { }

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	const object& self(void) const { return obj; }
};	// end class object_handle

// after note: if we need object_handle (non-const) implement later...

//=============================================================================
/**
	List of objects.
	Will potentially want to make the following sub-typed lists:
	(some cases will have special implicit conversions)
	list<pint_range> for array declarations/instantiations,
	list<index_expr> for array indexing and dimension collapsing, 
	list<param_expr> for param_expression_assignments, 
	list<instance_reference_base> for alias_connections.  
 */
class object_list : public object, public list<count_ptr<object> > {
public:
	typedef	count_ptr<object>			value_type;
protected:
	typedef	list<value_type>			parent;
public:
	object_list();
	~object_list();
	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
// using parent's list interface

	// could use excl_ptr, but then would need to release to count_ptr...
	count_ptr<range_expr_list>
		make_formal_dense_range_list(void) const;
	count_ptr<range_expr_list>
		make_sparse_range_list(void) const;
	excl_ptr<index_list>
		make_index_list(void) const;
	excl_ptr<dynamic_param_expr_list>
		make_param_expr_list(void) const;
	// not const because may modify self
	excl_ptr<param_expression_assignment>
		make_param_assignment(void);
	excl_const_ptr<aliases_connection>
		make_alias_connection(void) const;
	excl_const_ptr<port_connection>
		make_port_connection(
			count_const_ptr<simple_instance_reference> ir) const;
};	// end class object_list

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
	typedef	qmap<string, never_const_ptr<name_space> >	alias_map_type;

	/**
		Container for open namespaces with optional aliases.  
		Doesn't have to be a map because never any need to search
		by key.  List implementation is sufficient, because
		whole list will always be searched, if it is searched at all.  
		These pointers are read-only, and thus not
		owned by this namespace.  
	 */
	typedef list<never_const_ptr<name_space> >	namespace_list;

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
		typedef qmap<string, never_const_ptr<name_space> >
							ns_bin_type;
		typedef qmap<string, never_const_ptr<definition_base> >
							def_bin_type;
		typedef qmap<string, never_const_ptr<typedef_base> >
							alias_bin_type;
		typedef qmap<string, never_const_ptr<instance_collection_base> >
							inst_bin_type;
		typedef qmap<string, never_const_ptr<param_instance_collection> >
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

public:
	scopespace();
virtual	~scopespace();

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const = 0;

virtual	const string& get_key(void) const = 0;
virtual	string get_qualified_name(void) const = 0;
virtual never_const_ptr<scopespace> get_parent(void) const = 0;

virtual	never_const_ptr<object>	lookup_object_here(const string& id) const;
virtual	never_ptr<object>	lookup_object_here_with_modify(const string& id) const;
virtual	never_const_ptr<object>	lookup_object(const string& id) const;
virtual	never_const_ptr<object>	lookup_object(const qualified_id_slice& id) const;

virtual	never_const_ptr<scopespace>
		lookup_namespace(const qualified_id_slice& id) const;

protected:
	never_const_ptr<instance_collection_base>
		add_instance(excl_ptr<instance_collection_base> i);
public:
	// need id because instantiation statement won't be named yet!
	never_const_ptr<instance_collection_base>
		add_instance(never_ptr<instantiation_statement> i, 
			const token_identifier& id);
	bool add_definition_alias(never_const_ptr<definition_base> d, 
		const string& a);

	size_t exclude_population(void) const;
virtual	bool exclude_object(const used_id_map_type::value_type& i) const;
	bool exclude_object_val(const used_id_map_type::value_type i) const;

// helper functions for object IO
protected:
	// for used_id_map
	void collect_used_id_map_pointers(persistent_object_manager& m) const;
	void write_object_used_id_map(const persistent_object_manager& m) const;
	void load_object_used_id_map(persistent_object_manager& m);

// no concrete method for loading -- that remains derived-class specific
// so each sub-class may impose its own restrictions
virtual	void load_used_id_map_object(excl_ptr<persistent>& o) = 0;
};	// end class scopespace

//=============================================================================
/**
	Namespace container class.  
 */
class name_space : public scopespace {
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
	const string				key;
	// ummm... should this have been removed? scopespace already has one
	const never_const_ptr<name_space>	parent;	// override parent

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
explicit name_space();

public:
explicit name_space(const string& n);
	name_space(const string& n, never_const_ptr<name_space>);
	~name_space();

	const string& get_key(void) const;
	never_const_ptr<scopespace> get_parent(void) const;

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	ostream& pair_dump(ostream& o) const;

string	get_qualified_name(void) const;
never_const_ptr<name_space>	get_global_namespace(void) const;

// update these return types later
never_ptr<name_space>	add_open_namespace(const string& n);
never_const_ptr<name_space>	leave_namespace(void);	// or close_namespace
never_const_ptr<name_space>	add_using_directive(const qualified_id& n);
never_const_ptr<name_space>	add_using_alias(const qualified_id& n, const string& a);

private:
never_ptr<name_space>	add_namespace(excl_ptr<name_space> ns);

public:

// do we really need to specialize adding definitions by class?
// to be used ONLY by the global namespace (???)
never_ptr<definition_base>	add_definition(excl_ptr<definition_base> db);

// convert me to pointer-class:

// returns type if unique match found, else NULL
never_const_ptr<scopespace>	lookup_namespace(const qualified_id_slice& id) const;
never_const_ptr<name_space>	lookup_open_alias(const string& id) const;

// type-specific counterparts, obsolete

// some private utility functions (may become public later)
// add versions for querying for types, instantiations, etc...
private:
never_const_ptr<name_space>
		query_namespace_match(const qualified_id_slice& id) const;
never_const_ptr<name_space>
		query_subnamespace_match(const qualified_id_slice& id) const;

void	query_import_namespace_match(namespace_list& m, const qualified_id& id) const;

// these will not be recursive, but iteratively invoked by
// add_blah_inst/def();

// the following are not used... yet
void	find_namespace_ending_with(namespace_list& m, 
		const qualified_id& id) const;
void	find_namespace_starting_with(namespace_list& m, 
		const qualified_id& id) const;

// void	inherit_built_in_types(void);		// OBSOLETE

// will we need generalized versions of queries that return object*
// if we don't know a priori what an identifier's class is?
// single symbol table or separate?

bool	exclude_object(const used_id_map_type::value_type& i) const;

public:
	void unroll_params(void);
	void unroll_instances(void);
	void unroll_connections(void);

// methods for object file I/O
public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS

/** helper method for adding a variety of objects */
void	load_used_id_map_object(excl_ptr<persistent>& o);
public:
	static const never_const_ptr<name_space>	null;
};	// end class name_space

//=============================================================================
/**
	Base class for definition objects.  
	Consider deriving from name_space to re-use the 
	name-resolving functionality.  
	All definitions are potentially templatable.  
 */
class definition_base : virtual public object, virtual public persistent {
public:
	/**
		Table of template formals.  
		Needs to be ordered for argument checking, 
		and have fast lookup, thus hashlist.  
		Remember: template formals are accessible to the rest 
		of the body and to the port formals as well.  
		For now, the contained type is datatype_instance_collection
			which is generalized to include the paramater types
			pbool and pint, not to be confused with the data 
			types bool and int.  
		In the far future, prepare to extend template formals to 
			include abstract types of processes, channels and 
			data types in template argument list.  
			*shudder*
			It'd be nice to be able to swap instance arguments
			that preserve specified interfaces...
		May need hashqlist, for const-queryable hash structure!!!
	**/
	typedef	never_const_ptr<param_instance_collection>
					template_formals_value_type;
	// double-maintenance...
	typedef	hash_qmap<string, template_formals_value_type>
					template_formals_map_type;
	typedef	list<template_formals_value_type>
					template_formals_list_type;
	/** map from param_instance_collection to actual value passed */
	typedef	hash_qmap<string, count_const_ptr<param_expr> >
					template_actuals_map_type;

protected:
//	const string			key;
//	const never_const_ptr<name_space>	parent;

protected:
	/** subset of used_id_map, must be coherent with list */
	template_formals_map_type	template_formals_map;
	/** subset of used_id_map, must be coherent with map */
	template_formals_list_type	template_formals_list;

	/**
		Whether or not this definition is complete or only declared.  
		As soon as a definition is opened, mark it as defined
		to allow self-recursive template definitions.  
	 */
	bool				defined;
public:
	definition_base();
virtual	~definition_base();

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const;	// temporary
	ostream& dump_template_formals(ostream& o) const;
	ostream& pair_dump(ostream& o) const;
//	bool dump_cerr(void) const;		// historical artifact

virtual	const string& get_key(void) const = 0;
virtual	never_const_ptr<scopespace> get_parent(void) const = 0;

	bool is_defined(void) const { return defined; }
	void mark_defined(void) { assert(!defined); defined = true; }

	void fill_template_actuals_map(template_actuals_map_type& am, 
		const param_expr_list& al) const;

	never_const_ptr<param_instance_collection>
		lookup_template_formal(const string& id) const;
/** should be pure virtual, but let's default to NULL */
virtual	never_const_ptr<instance_collection_base>
		lookup_port_formal(const string& id) const;
virtual	never_const_ptr<object>	lookup_object_here(const string& id) const;

virtual	bool check_null_template_argument(void) const;


protected:
	// Q: what if expressions are involved, can't statically resolve?
	// e.g. with arrays of parameters... and referenced indices.
	// well, they must be at least initialized (usable).  
	// need notion of formal equivalence
	// MAY be equivalent
	bool equivalent_template_formals(
		never_const_ptr<definition_base> d) const;

protected:
	bool certify_template_arguments(
		never_ptr<dynamic_param_expr_list> ta) const;
public:
	excl_ptr<dynamic_param_expr_list>
		make_default_template_arguments(void) const;
	/** by default returns false */
virtual	bool certify_port_actuals(const object_list& ol) const;

public:
// proposing to replace set_context_fundamental_type with the following:
virtual count_const_ptr<fundamental_type_reference>
		make_fundamental_type_reference(
			excl_ptr<dynamic_param_expr_list> ta) const = 0;
	// overloaded for no template argument, for convenience, 
	// but must check that everything has default arguments!
	count_const_ptr<fundamental_type_reference>
		make_fundamental_type_reference(void) const;
// why virtual? special cases for built-in types?

/**
	Create an empty, unresolved typedef, based on the 
	invoker's type.  
 */
virtual	excl_ptr<definition_base>
		make_typedef(never_const_ptr<scopespace> s, 
			const token_identifier& id) const = 0;

// need not be virtual?
virtual	string get_name(void) const;
// need not be virtual?
virtual	string get_qualified_name(void) const;

/** definition signature comparison, true if equal */
virtual	bool require_signature_match(
		never_const_ptr<definition_base> d) const
		{ return false; }	// temporary, should be pure

/**
	f should be const and owned -- pointer type conflict...  
	virtual so that types without templates can assert NULL.  
	TO DO: This function should be pure virtual and belong 
		to a different interface!
 */
virtual	never_const_ptr<instance_collection_base>
		add_template_formal(never_ptr<instantiation_statement> f, 
			const token_identifier& id);

/**
	Really, only some definitions should have ports...
 */
virtual	never_const_ptr<instance_collection_base>
		add_port_formal(never_ptr<instantiation_statement> f, 
			const token_identifier& id);

#if 0
virtual	bool exclude_object(const used_id_map_type::value_type& i) const;
#endif

protected:
void	collect_template_formal_pointers(persistent_object_manager& m) const;
void	write_object_template_formals(const persistent_object_manager& m) const;
void	load_object_template_formals(persistent_object_manager& m);
public:
	static const never_const_ptr<definition_base>	null;
};	// end class definition_base

//=============================================================================
class type_reference_base : public object, public persistent {
protected:
public:
	type_reference_base() : object(), persistent() { }
virtual	~type_reference_base() { }

};	// end class type_reference_base

//=============================================================================
/**
	This class is a reference to a type (datatype, process, channel), 
	and contains optional template argments.  
	Instantiations should contain wrapper-references of this type, 
	and not direct references to type definitions.  
	This level of indirection...
	Sub-classes thereof should contain const pointers to 
	definitions of the specific classes.  
 */
class fundamental_type_reference : public type_reference_base {
public:
	/** map from param_instance_collection to actual value passed */
	typedef	definition_base::template_actuals_map_type
						template_actuals_map_type;
protected:
	/**
		Optional set of template parameters with which a
		type is instantiated.  
		Types must match that of template signature.  
		Hmmm... maybe need a concrete_type class...
		distinguish type_reference from type_instance.  
		This is owned, and thus must be deleted.  
		Const?
	 */
	excl_const_ptr<param_expr_list>		template_params;

public:
	fundamental_type_reference();
explicit fundamental_type_reference(excl_const_ptr<param_expr_list> pl);
virtual	~fundamental_type_reference();

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const;
virtual never_const_ptr<definition_base> get_base_def(void) const = 0;
	string template_param_string(void) const;
	string get_qualified_name(void) const;
	string hash_string(void) const;

	excl_ptr<param_expr_list>
		get_copy_template_params(void) const;

	// limits the extend to which it can be statically type-checked
	// i.e. whether parameter is resolved to a scope's formal
	bool is_dynamically_parameter_dependent(void) const;

	// later add dimensions and indices?

excl_const_ptr<fundamental_type_reference>
	resolve_canonical_type(void) const;

static	excl_ptr<instantiation_statement>
		make_instantiation_statement(
			count_const_ptr<fundamental_type_reference> t, 
			index_collection_item_ptr_type d);

virtual	excl_ptr<instantiation_statement>
		make_instantiation_statement_private(
			count_const_ptr<fundamental_type_reference> t, 
			index_collection_item_ptr_type d) const = 0;

virtual	excl_ptr<instance_collection_base>
		make_instance_collection(
			never_const_ptr<scopespace> s, 
			const token_identifier& id, 
			const size_t d) const = 0;

public:
	bool may_be_equivalent(const fundamental_type_reference& t) const;
	bool must_be_equivalent(const fundamental_type_reference& t) const;

	// something for resolving typedefs
	// or return by value? statically would require copy constructor
	// wth, just allocate one...
	excl_const_ptr<fundamental_type_reference>
		make_canonical_type_reference(void) const;
};	// end class fundamental_type_reference

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
protected:
	/**
		Back-pointer to the namespace to which this instantiation
		belongs.  
		Can be a namespace of definition's scopespace.  
		Is NEVER null, should be reference?
		Should never be a loop or conditional namespace.  
	 */
	const never_const_ptr<scopespace>	owner;

	/**
		Name of instance.
	 */
	string				key;

	/**
		Optional array dimension sizes, which can be ranges.  

		REPLACE this with a hierarchical dimension tree!
		of expressions, mostly constants, some unresolved.  
		Multidimensional and sparse arrays.  

		Needs to be a grown stack of instances, because
		of changing collection.  
		Needs to be a deque so we can use iterators.  

		UPDATE:
		Is now a container of instantiation_statements.
	 */
	index_collection_type			index_collection;

	/**
		Dimensions, >= 0, limit is 4.  
		Once set, is fixed.  
	 */
	size_t	depth;

	// children will implement unrolled collection of instances?
	// but only instances that are not found in definitions?
protected:
explicit instance_collection_base();

public:
	// o should be reference, not pointer
	instance_collection_base(const scopespace& o, const string& n, 
		const size_t d);
virtual	~instance_collection_base();

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const;	// temporary
	ostream& pair_dump(ostream& o) const;
	string get_name(void) const { return key; }
virtual	string get_qualified_name(void) const;
virtual	string hash_string(void) const { return key; }

/**
	Why is this a count_ptr?  because type_references can be reused
	all over the place, so we reference count all type references.  
	Unfortunately this forces us to do the same with static 
	built-in types.  
 */
virtual	count_const_ptr<fundamental_type_reference>
		get_type_ref(void) const = 0;
	never_const_ptr<definition_base>
		get_base_def(void) const;

	never_const_ptr<scopespace> get_owner(void) const { return owner; }
	size_t dimensions(void) const { return depth; }
	instantiation_state collection_state_end(void) const;
	instantiation_state current_collection_state(void) const;

	const_range_list
	detect_static_overlap(index_collection_item_ptr_type r) const;

	const_range_list
	add_instantiation_statement(index_collection_type::value_type r);

private:
	bool formal_size_equivalent(
		never_const_ptr<instance_collection_base> b) const;
public:
	bool is_template_formal(void) const;
	bool is_port_formal(void) const;
	bool template_formal_equivalent(
		never_const_ptr<instance_collection_base> b) const;
	bool port_formal_equivalent(
		never_const_ptr<instance_collection_base> b) const;

protected:
	bool check_expression_dimensions(const param_expr& pr) const;

public:
/**
	always pushes onto context's object_stack, 
	currently always returns NULL, useless
 */
virtual	count_ptr<instance_reference_base>
		make_instance_reference(void) const = 0;
virtual	count_ptr<member_instance_reference_base>
		make_member_instance_reference(
			count_const_ptr<simple_instance_reference> b) const = 0;
protected:
	// utility functions for handling index collection
	void collect_index_collection_pointers(
			persistent_object_manager& m) const;
	void write_index_collection_pointers(
			const persistent_object_manager& m) const;
	void load_index_collection_pointers(
			persistent_object_manager& m);
public:
	/** just for convenience */
	static const never_const_ptr<instance_collection_base>	null;
};	// end class instance_collection_base

//=============================================================================
/**
	Abstract interface for scopes with sequential 
	instance management actions.
 */
class sequential_scope {
public:
	typedef list<excl_const_ptr<instance_management_base> >
					instance_management_list_type;
protected:
	/**
		The unified list of sequential instance management actions, 
		including parameters, instantiations, assignments, 
		and connections.  
		Used for maintaining actions in source order.  
	 */
	list<excl_const_ptr<instance_management_base> >
					instance_management_list;
public:
	sequential_scope();
virtual	~sequential_scope();

	ostream& dump(ostream& o) const;
	void append_instance_management(
		excl_const_ptr<instance_management_base> i);

	void collect_object_pointer_list(persistent_object_manager& m) const;
	void write_object_pointer_list(const persistent_object_manager& m) const;
	void load_object_pointer_list(const persistent_object_manager& m);

// need not be virtual?
// may need context later...
	void unroll(void) const;

};	// end class sequential_scope

//=============================================================================
/**
	Abstract base class for sequential instantiation management objects, 
	including instantiations, parameters, assignments, connections.  
	Don't bother deriving from object, unless it is necessary.  
 */
class instance_management_base : public persistent {
protected:
	// none
public:
	/**
		Helper functor for adding a dereference before dumping, 
		since the majority of objects are pointer-classed.
		Consider using this in object as well.  
	 */
	class dumper {
		private:
			ostream& os;
		public:
			explicit dumper(ostream& o);

			template <template <class> class P>
			ostream&
			operator () (const P<instance_management_base>& i) const;
	};	// end class dumper

public:
virtual	ostream& dump(ostream& o) const = 0;

#if 0
virtual	void collect_transient_info(persistent_object_manager& m) const = 0;
virtual	void write_object(const persistent_object_manager& m) const = 0;
virtual	void load_object(persistent_object_manager& m) = 0;
#endif

	// need pure virtual unrolling methods
	// argument should contain some stack of expression values
virtual	void unroll(void) const = 0;
};	// end class instance_management_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_BASE_H__

