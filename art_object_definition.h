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
	class channel_definition;
	class datatype_definition;
	class process_definition;
	class enum_datatype_def;
	class built_in_datatype_def;
	class built_in_param_def;

//=============================================================================
// class definition_base declared in "art_object_base.h"

//=============================================================================
/**
	Process definition.  Contains optional set of template formals, 
	set of port formals, and body of instantiations and language bodies.  
	Is there a way to re-use name resolution code
	from class name_space without copying?  
	No other class derives from this?
 */
class process_definition : public definition_base {
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

	never_const_ptr<fundamental_type_reference>
		set_context_fundamental_type(context& c) const;

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

virtual	never_const_ptr<fundamental_type_reference>
		set_context_fundamental_type(context& c) const;
virtual	bool type_equivalent(const datatype_definition& t) const = 0;
virtual	bool require_signature_match(
		never_const_ptr<definition_base> d) const = 0;
};	// end class datatype_definition

//-----------------------------------------------------------------------------
/**
	Reserved for special built-in fundamental data types.  
	All user-defined data types will boil down to these types.  
	Final class.  
 */
class built_in_datatype_def : public datatype_definition {
public:
	built_in_datatype_def(never_const_ptr<name_space> o, const string& n);
	built_in_datatype_def(never_const_ptr<name_space> o, const string& n, 
		excl_ptr<param_instantiation> p);
	~built_in_datatype_def();

	ostream& what(ostream& o) const;
	never_const_ptr<definition_base>
		set_context_definition(context& c) const;
	never_const_ptr<fundamental_type_reference>
		set_context_fundamental_type(context& c) const;
	bool type_equivalent(const datatype_definition& t) const;
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
class enum_datatype_def : public datatype_definition {
protected:
	// no new members
	// don't we need to track ordering of identifiers added?  later...
public:
	enum_datatype_def(never_const_ptr<name_space> o, const string& n);
	~enum_datatype_def();

	ostream& what(ostream& o) const;

	never_const_ptr<fundamental_type_reference>
		set_context_fundamental_type(context& c) const;
	bool type_equivalent(const datatype_definition& t) const;
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
	/**
		Forward pointer to unique type-reference, 
		safe because built in param types are never templated.
	 */
	never_const_ptr<param_type_reference>	type_ref;
public:
	built_in_param_def(never_const_ptr<name_space> p, const string& n, 
		const param_type_reference& t);
	~built_in_param_def();

	ostream& what(ostream& o) const;
	never_const_ptr<definition_base>
		set_context_definition(context& c) const;
	never_const_ptr<fundamental_type_reference>
		set_context_fundamental_type(context& c) const;
};	// end class built_in_param_def

//-----------------------------------------------------------------------------
/**
	Generalizable user-defined data type, which can (eventually) 
	build upon other user-defined data types.  
 */
class user_def_datatype : public datatype_definition {
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
	~user_def_datatype() { }

	ostream& what(ostream& o) const;
	bool type_equivalent(const datatype_definition& t) const;
	bool require_signature_match(
		never_const_ptr<definition_base> d) const { return false; }
		// temporary
};	// end class user_def_datatype

//-----------------------------------------------------------------------------
/// abstract base class for channels and their representations
class channel_definition : public definition_base {
protected:
//	string			key;		// inherited
public:
	channel_definition(never_const_ptr<name_space> o, const string& n);
virtual	~channel_definition();

virtual	ostream& what(ostream& o) const = 0;

virtual	never_const_ptr<fundamental_type_reference>
		set_context_fundamental_type(context& c) const;
};	// end class channel_definition

//-----------------------------------------------------------------------------
/**
	Generalizable user-defined channel type, which can (eventually) 
	build upon other user-defined channel types.  
 */
class user_def_chan : public channel_definition {
private:
#if 0
TEMPORARY
	/**
		Members will be kept as a list for ordered checking.  
	 */
	typedef	hashlist<string, never_const_ptr<datatype_definition> >
						type_members;
	/**
		Template parameter will be kept in a list because their
		order matters in type-checking.
		Redundant? inherited?
	 */
	typedef	hashlist<string, never_const_ptr<datatype_definition> >
						temp_param_list;
		// template params already inherited...
#endif
protected:
	// list of other type definitions
#if 0
	// temporary
	temp_param_list		template_params;
	type_members		members;
#endif
public:
	user_def_chan(never_const_ptr<name_space> o, const string& name);
	~user_def_chan();

	ostream& what(ostream& o) const;
};	// end class user_def_chan

//-----------------------------------------------------------------------------
/// Type aliases are analogous to typedefs in C (not yet implemented)
// for renaming convenience
// going to use this mechanism to inherit built-in types into each namespace
//	to accelerate their resolution, not having to search sequentially
//	up to the global namespace.  
// add an alias into each scope's used_id_map...
// also templatable, for partial template spcifications?
class type_alias : public definition_base {
protected:
	/// pointer to the type represented by this type-id
	never_const_ptr<definition_base>	canonical;
public:
	type_alias(
		never_const_ptr<name_space> o,
		const string& n, 
		never_const_ptr<definition_base> t);
//		template_formals_set* tf = NULL;
virtual	~type_alias();
	// never delete canonical (can't, it's const!)

// need not be virtual
never_const_ptr<definition_base>	resolve_canonical(void) const;

virtual	ostream& what(ostream& o) const;
};	// end class type_alias

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_DEFINITION_H__

