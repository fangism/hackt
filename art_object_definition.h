// "art_object_definition.h"

#ifndef	__ART_OBJECT_DEFINITION_H__
#define	__ART_OBJECT_DEFINITION_H__

#include "art_macros.h"
#include "art_object_base.h"

/*********** note on use of data structures ***************
Lists are needed for sets that need to maintain sequence, such as
formal declarations in definitions.  Type-checking is done in order
of elements, comparing actuals against formals one-by-one.  
For some lists, however, we'd like constant time access to 
elements in the sequence by hashing indices.  Hashlist provides
this added functionality by associating a key to each element in the 
list.  

Maps...

********************** end note **************************/

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
	using namespace fang;		// for experimental pointer classes

//=============================================================================
// forward declarations

	class definition_base;
#if NEW_DEF_HIER
	class process_definition_base;
	class process_definition;
	class process_definition_alias;
	class channel_definition_base;
	class channel_definition_alias;
	class datatype_definition_base;
	class datatype_definition_alias;
	// user_defs?
#else
	class channel_definition;
	class datatype_definition;
	class process_definition;
#endif
	class enum_datatype_def;
	class built_in_datatype_def;
	class built_in_param_def;

//=============================================================================
// class definition_base declared in "art_object_base.h"

//=============================================================================
#if NEW_DEF_HIER
/**
	Process definition base class.  From this, there will arise: true
	process definitions, and typedef process definitions. 
 */
class process_definition_base : public definition_base {
protected:
	// no new members?
public:
	process_definition_base(const string& n,
		never_const_ptr<name_space> p);
virtual	~process_definition_base();

// inherited pure virtuals are still pure virtuals
};	// end class process_definition_base
#endif

//=============================================================================
/**
	Process definition.  
	All processes are user-defined.  
	Contains optional set of template formals, 
	set of port formals, and body of instantiations and language bodies.  
	Is there a way to re-use name resolution code
	from class name_space without copying?  
	No other class derives from this?
 */
#if NEW_DEF_HIER
class process_definition : public process_definition_base, public scopespace
#else
class process_definition : public definition_base
#endif
{
public:

	/**
		Table of port formals.
		The types can be data-types or channel-types, 
		either base-types or user-defined types.  
		Needs to be ordered for argument checking, 
		and have fast lookup, thus hashlist.  
	typedef hashlist<string, never_const_ptr<instantiation_base> >
							port_formals_set;
	**/

	typedef list<never_const_ptr<instantiation_base> >
						port_formals_list_type;
	typedef hash_qmap<string, never_const_ptr<instantiation_base> >
						port_formals_map_type;

	// List of language bodies, separate or merged?

protected:
//	string			key;		// inherited
//	used_id_map_type	used_id_map;	// inherited
	port_formals_list_type			port_formals_list;
	port_formals_map_type			port_formals_map;
	// list language bodies
	
public:
	process_definition(never_const_ptr<name_space> o, 
		const string& s); 
	~process_definition();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;

	const string& get_key(void) const;
	string get_qualified_name(void) const;
	never_const_ptr<scopespace> get_parent(void) const;

	never_const_ptr<object> lookup_object_here(const string& id) const;

#if 0
	never_const_ptr<fundamental_type_reference>
		set_context_fundamental_type(context& c) const;
#endif
	count_const_ptr<fundamental_type_reference>
		make_fundamental_type_reference(
			excl_ptr<param_expr_list> ta) const;

never_const_ptr<instantiation_base>
	add_port_formal(excl_ptr<instantiation_base> p);

//	bool is_defined(void) const { return def; }

// just a thought... need to do this soon
// void	add_port_formal(const instantiation_base* d, const port_formal_id& p);

	bool equivalent_port_formals(
		never_const_ptr<process_definition> p) const;
	bool require_signature_match(never_const_ptr<definition_base> d) const;

};	// end class process_definition

//=============================================================================
#if NEW_DEF_HIER
/**
	A process-class typedef.  
	Is usable as a process_definition_base for complete types, 
	but not definable.  
	Has no contents, just a level of indirection to the encapsulated
	process type. 
	May be templated for partial type specifications.  
 */
class process_definition_alias : public process_definition_base {
protected:
	excl_const_ptr<process_type_reference>		base;
public:
	process_definition_alias(const string& n, 
		never_const_ptr<name_space> p);
	~process_definition_alias();
};	// end class process_definition_alias
#endif

//=============================================================================
#if NEW_DEF_HIER
/**
	Base class interface for data type definitions.  
 */
class datatype_definition_base : public definition_base {
protected:
public:
	datatype_definition_base(const string& n, 
		never_const_ptr<name_space> p);
	datatype_definition_base(never_const_ptr<name_space> p, 
		const string& n);
virtual	~datatype_definition_base();

virtual	ostream& what(ostream& o) const = 0;
virtual	count_const_ptr<fundamental_type_reference>
		make_fundamental_type_reference(
			excl_ptr<param_expr_list> ta) const;
virtual	bool require_signature_match(
		never_const_ptr<definition_base> d) const = 0;
};	// end class datatype_definition_base
#endif

//=============================================================================
#if !NEW_DEF_HIER
/**
	Abstract base class for types and their representations.
 */
class datatype_definition : public definition_base {
protected:
//	string			key;		// name of type, inherited
public:
	datatype_definition(
		never_const_ptr<name_space> o, 
		const string& n);
virtual	~datatype_definition();

virtual	ostream& what(ostream& o) const = 0;

#if 0
virtual	never_const_ptr<fundamental_type_reference>
		set_context_fundamental_type(context& c) const;
#endif
virtual	count_const_ptr<fundamental_type_reference>
		make_fundamental_type_reference(
			excl_ptr<param_expr_list> ta) const;

#if 0
virtual	bool type_equivalent(const datatype_definition& t) const = 0;
#endif
virtual	bool require_signature_match(
		never_const_ptr<definition_base> d) const = 0;
};	// end class datatype_definition
#endif

//-----------------------------------------------------------------------------
/**
	Reserved for special built-in fundamental data types.  
	All user-defined data types will boil down to these types.  
	Final class.  
 */
#if NEW_DEF_HIER
class built_in_datatype_def : public datatype_definition_base
#else
class built_in_datatype_def : public datatype_definition
#endif
{
public:
	built_in_datatype_def(never_const_ptr<name_space> o, const string& n);
	built_in_datatype_def(never_const_ptr<name_space> o, const string& n, 
		excl_ptr<param_instantiation> p);
	~built_in_datatype_def();

	ostream& what(ostream& o) const;
#if 0
	never_const_ptr<definition_base>
		set_context_definition(context& c) const;
#endif
#if 0
	never_const_ptr<fundamental_type_reference>
		set_context_fundamental_type(context& c) const;
#endif
	count_const_ptr<fundamental_type_reference>
		make_fundamental_type_reference(
			excl_ptr<param_expr_list> ta) const;
	// overrides definition_base's, exception to rule
	// because this is not a scopespace
	never_const_ptr<instantiation_base>
		add_template_formal(excl_ptr<instantiation_base> f);
#if 0
	bool type_equivalent(const datatype_definition& t) const;
#endif
	bool require_signature_match(
		never_const_ptr<definition_base> d) const
		{ assert(d); return key == d->get_name(); }
		// really, this should never be called...
};	// end class_built_in_datatype_def

//-----------------------------------------------------------------------------
/**
	Member of an enumeration, just an identifier.  
 */
class enum_member : public object {
protected:
	const string			id;
public:
	enum_member(const string& n);
	~enum_member();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
};	// end class enum_member

//-----------------------------------------------------------------------------
/**
	Enumerations are special fundamental types of data, like int and bool.  
	There are no built in enumerations, all are user-defined.  
 */
#if NEW_DEF_HIER
class enum_datatype_def : public datatype_definition_base, 
		public scopespace
#else
class enum_datatype_def : public datatype_definition
#endif
{
protected:
	// no new members
	// don't we need to track ordering of identifiers added?  later...
public:
	enum_datatype_def(never_const_ptr<name_space> o, const string& n);
	~enum_datatype_def();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;

	const string& get_key(void) const;
	string get_qualified_name(void) const;
	never_const_ptr<scopespace> get_parent(void) const;

#if 0
	never_const_ptr<fundamental_type_reference>
		set_context_fundamental_type(context& c) const;
#endif
#if 0
	// don't need yet
	count_const_ptr<fundamental_type_reference>
		make_fundamental_type_reference(
			excl_ptr<param_expr_list> ta) const;
#endif
#if 0
	bool type_equivalent(const datatype_definition& t) const;
#endif
	bool require_signature_match(never_const_ptr<definition_base> d) const;

	bool add_member(const token_identifier& em);
};	// end class enum_datatype_def

//-----------------------------------------------------------------------------
/**
	Reserved for special built-in parameter types, pint and pbool.  
	Nothing can really be derived from them... yet.  
	Note that there is no intermediate param_definition class, 
	because parameter types can only be built in; there are no
	user-defined parameter types, for now...
 */
class built_in_param_def : public definition_base {
protected:
//	string			key;		// inherited
#if 0
	/**
		Forward pointer to unique type-reference, 
		safe because built in param types are never templated.
		Do not use count_ptr, else will form cycle.  
	 */
	never_const_ptr<param_type_reference>	type_ref;
#endif
public:
#if 0
	built_in_param_def(never_const_ptr<name_space> p, const string& n, 
		const param_type_reference& t);
#else
	built_in_param_def(never_const_ptr<name_space> p, const string& n);
#endif
	~built_in_param_def();

	ostream& what(ostream& o) const;
//	ostream& dump(ostream& o) const;
#if 0
	never_const_ptr<definition_base>
		set_context_definition(context& c) const;
#endif
#if 0
	never_const_ptr<fundamental_type_reference>
		set_context_fundamental_type(context& c) const;
#endif
	count_const_ptr<fundamental_type_reference>
		make_fundamental_type_reference(
			excl_ptr<param_expr_list> ta) const;
};	// end class built_in_param_def

//-----------------------------------------------------------------------------
/**
	Generalizable user-defined data type, which can (eventually) 
	build upon other user-defined data types.  
 */
#if NEW_DEF_HIER
class user_def_datatype : public datatype_definition_base, 
		public scopespace
#else
class user_def_datatype : public datatype_definition
#endif
{
private:
#if 0
TEMPORARY... later replace
	/**
		Members will be kept as a hashlist
		because their order matters, or don't they?
	 */
	typedef	hashlist<string, never_const_ptr<datatype_definition> >
						type_members;
	/**
		Template parameter will be kept in a list because their
		order matters in type-checking.
	 */
	typedef	hashlist<string, never_const_ptr<datatype_definition> >
						temp_param_list;
#endif
protected:
	// list of other type definitions
#if 0
	TEMPORARY
	temp_param_list		template_params;
	type_members		members;
#endif
public:
	user_def_datatype(never_const_ptr<name_space> o, const string& name);
	~user_def_datatype();

	const string& get_key(void) const;
	string get_qualified_name(void) const;
	never_const_ptr<scopespace> get_parent(void) const;

	never_const_ptr<object> lookup_object_here(const string& id) const;

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
#if 0
	bool type_equivalent(const datatype_definition& t) const;
#endif
	bool require_signature_match(
		never_const_ptr<definition_base> d) const { return false; }
		// temporary
};	// end class user_def_datatype

//-----------------------------------------------------------------------------
/**
	Data-type typedef.  
 */
class datatype_definition_alias : public datatype_definition_base {
protected:
	// inherited template formals
	never_const_ptr<data_type_reference>	base;
public:
	datatype_definition_alias(const string& n, 
		never_const_ptr<name_space> p);
	~datatype_definition_alias();

};	// end class datatype_definition_alias

//=============================================================================
#if NEW_DEF_HIER
/// abstract base class for channels and their representations
class channel_definition_base : public definition_base {
protected:
//	string			key;		// inherited
public:
	channel_definition_base(never_const_ptr<name_space> o, const string& n);
virtual	~channel_definition_base();

// virtual	ostream& what(ostream& o) const = 0;

virtual	count_const_ptr<fundamental_type_reference>
		make_fundamental_type_reference(
			excl_ptr<param_expr_list> ta) const;
};	// end class channel_definition_base

#else

/// abstract base class for channels and their representations
class channel_definition : public definition_base {
protected:
//	string			key;		// inherited
public:
	channel_definition(never_const_ptr<name_space> o, const string& n);
virtual	~channel_definition();

virtual	ostream& what(ostream& o) const = 0;

#if 0
virtual	never_const_ptr<fundamental_type_reference>
		set_context_fundamental_type(context& c) const;
#endif
virtual	count_const_ptr<fundamental_type_reference>
		make_fundamental_type_reference(
			excl_ptr<param_expr_list> ta) const;
};	// end class channel_definition
#endif

//-----------------------------------------------------------------------------
/**
	Generalizable user-defined channel type, which can (eventually) 
	build upon other user-defined channel types.  
 */
#if NEW_DEF_HIER
class user_def_chan : public channel_definition_base, 
		public scopespace
#else
class user_def_chan : public channel_definition
#endif
{
protected:
	// list of other type definitions
public:
	user_def_chan(never_const_ptr<name_space> o, const string& name);
	~user_def_chan();

	const string& get_key(void) const;
	string get_qualified_name(void) const;
	never_const_ptr<scopespace> get_parent(void) const;

	never_const_ptr<object> lookup_object_here(const string& id) const;

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
};	// end class user_def_chan

//-----------------------------------------------------------------------------
/// Type aliases are analogous to typedefs in C (not yet implemented)
// for renaming convenience
// going to use this mechanism to inherit built-in types into each namespace
//	to accelerate their resolution, not having to search sequentially
//	up to the global namespace.  
// add an alias into each scope's used_id_map...
// also templatable, for partial template spcifications?  YES
// should we sub-type? hold off...
// new definition class hierarchy sub-types!

#if !NEW_DEF_HIER
class type_alias : public definition_base {
protected:
	// inherits template formals
	/// pointer to the type represented by this type-id
#if 0
	never_const_ptr<definition_base>	canonical;
#else
	excl_const_ptr<fundamental_type_reference>	canonical;
#endif
public:
	type_alias(
		never_const_ptr<name_space> o,
		const string& n);
#if 0
	type_alias(
		never_const_ptr<name_space> o,
		const string& n, 
		never_const_ptr<definition_base> t);
#endif
virtual	~type_alias();

// need not be virtual
#if 0
never_const_ptr<definition_base>	resolve_canonical(void) const;
never_const_ptr<fundamental_type_reference>	resolve_canonical(void) const;
#endif

	count_const_ptr<fundamental_type_reference>
		make_fundamental_type_reference(
			excl_ptr<param_expr_list> ta) const;

	void assign_typedef(excl_const_ptr<fundamental_type_reference> t);

virtual	ostream& what(ostream& o) const;
};	// end class type_alias
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_DEFINITION_H__

