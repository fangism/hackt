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
	class process_definition_base;
	class process_definition;
	class process_definition_alias;
	class channel_definition_base;
	class channel_definition_alias;
	class datatype_definition_base;
	class datatype_definition_alias;
	// user_defs?
	class enum_datatype_def;
	class built_in_datatype_def;
	class built_in_param_def;

//=============================================================================
// class definition_base declared in "art_object_base.h"

//=============================================================================
/**
	Abstract base class interface for typedef alias definitions.  
	All typedefs are templateable, and thus have their own little
	scope space for template parameters.  
	Awkward?
 */
class typedef_base : virtual public definition_base, public scopespace {
protected:
	// no new members
public:
	typedef_base();
virtual	~typedef_base();

virtual	const string& get_key(void) const = 0;
	string get_qualified_name(void) const;
virtual	ostream& what(ostream& o) const = 0;
	ostream& dump(ostream& o) const;
virtual never_const_ptr<fundamental_type_reference>
		get_base_type_ref(void) const = 0;
virtual	bool assign_typedef(excl_const_ptr<fundamental_type_reference> f) = 0;

	excl_const_ptr<fundamental_type_reference>
		resolve_complete_type(never_const_ptr<param_expr_list> p) const;

protected:
virtual	void load_used_id_map_object(excl_ptr<object> o) = 0;
};	// end class typedef_base

//=============================================================================
/**
	Process definition base class.  From this, there will arise: true
	process definitions, and typedef process definitions. 
 */
class process_definition_base : virtual public definition_base {
protected:
	// no new members?
public:
//	process_definition_base(const string& n);
	process_definition_base();
virtual	~process_definition_base();

	excl_ptr<definition_base>
		make_typedef(never_const_ptr<scopespace> s, 
			const token_identifier& id) const;

// inherited pure virtuals are still pure virtuals
};	// end class process_definition_base

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
class process_definition : public process_definition_base, public scopespace {
public:
	typedef	never_const_ptr<instantiation_base>
						port_formals_value_type;
	/**
		Table of port formals.
		The types can be data-types or channel-types, 
		either base-types or user-defined types.  
		Needs to be ordered for argument checking, 
		and have fast lookup, thus hashlist.  
		Implemented as a hash_qmap and list.  
	**/
	typedef list<port_formals_value_type>
						port_formals_list_type;
	typedef hash_qmap<string, port_formals_value_type>
						port_formals_map_type;

	// List of language bodies, separate or merged?

protected:
	const string		key;		// inherited
//	used_id_map_type	used_id_map;	// inherited
	const never_const_ptr<name_space>	parent;
	port_formals_list_type			port_formals_list;
	port_formals_map_type			port_formals_map;
	// list language bodies
	
public:
	process_definition(never_const_ptr<name_space> o, const string& s); 
	~process_definition();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;

	const string& get_key(void) const;
	string get_qualified_name(void) const;
	never_const_ptr<scopespace> get_parent(void) const;

	/** overrides definition_base's */
	never_const_ptr<instantiation_base>
		lookup_port_formal(const string& id) const;
	never_const_ptr<object> lookup_object_here(const string& id) const;

	count_const_ptr<fundamental_type_reference>
		make_fundamental_type_reference(
			excl_ptr<dynamic_param_expr_list> ta) const;

never_const_ptr<instantiation_base>
	add_port_formal(excl_ptr<instantiation_base> p);
	bool certify_port_actuals(const object_list& ol) const;

	bool equivalent_port_formals(
		never_const_ptr<process_definition> p) const;
	bool require_signature_match(never_const_ptr<definition_base> d) const;

// methods for object file I/O
public:
	ART_OBJECT_IO_METHODS
	void load_used_id_map_object(excl_ptr<object> o);
	void write_object_port_formals(persistent_object_manager& m) const;
	void load_object_port_formals(persistent_object_manager& m);
};	// end class process_definition

//=============================================================================
/**
	A process-class typedef.  
	Is usable as a process_definition_base for complete types, 
	but not definable.  
	Has no contents, just a level of indirection to the encapsulated
	process type. 
	May be templated for partial type specifications.  
 */
class process_definition_alias : public process_definition_base, 
		public typedef_base {
protected:
	const string					key;
	/** parent can be namespace or definition */
	never_const_ptr<scopespace>			parent;
	excl_const_ptr<process_type_reference>		base;
public:
	process_definition_alias(const string& n, 
		never_const_ptr<scopespace> p);
	~process_definition_alias();

	ostream& what(ostream& o) const;
	const string& get_key(void) const;
	never_const_ptr<scopespace> get_parent(void) const;
	never_const_ptr<fundamental_type_reference>
		get_base_type_ref(void) const;

	bool assign_typedef(excl_const_ptr<fundamental_type_reference> f);
	count_const_ptr<fundamental_type_reference>
		make_fundamental_type_reference(
			excl_ptr<dynamic_param_expr_list> ta) const;
public:
	ART_OBJECT_IO_METHODS
	void load_used_id_map_object(excl_ptr<object> o);
};	// end class process_definition_alias

//=============================================================================
/**
	Base class interface for data type definitions.  
 */
class datatype_definition_base : virtual public definition_base {
protected:
public:
	datatype_definition_base();
//	datatype_definition_base(const string& n);
virtual	~datatype_definition_base();

	excl_ptr<definition_base>
		make_typedef(never_const_ptr<scopespace> s, 
			const token_identifier& id) const;

virtual	ostream& what(ostream& o) const = 0;
virtual	count_const_ptr<fundamental_type_reference>
		make_fundamental_type_reference(
			excl_ptr<dynamic_param_expr_list> ta) const;
virtual	bool require_signature_match(
		never_const_ptr<definition_base> d) const = 0;
};	// end class datatype_definition_base

//=============================================================================
/**
	Reserved for special built-in fundamental data types.  
	All user-defined data types will boil down to these types.  
	Final class.  
	Need to derive from scopespace because, built-in data type
	int's template parameter, pint, needs a valid owner scopespace.  
 */
class built_in_datatype_def : public datatype_definition_base, 
		public scopespace {
protected:
	const string					key;
	const never_const_ptr<name_space>		parent;
public:
	built_in_datatype_def(never_const_ptr<name_space> o, const string& n);
	built_in_datatype_def(never_const_ptr<name_space> o, const string& n, 
		excl_ptr<param_instantiation> p);
	~built_in_datatype_def();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	const string& get_key(void) const;
	string get_qualified_name(void) const;
	never_const_ptr<scopespace> get_parent(void) const;

	count_const_ptr<fundamental_type_reference>
		make_fundamental_type_reference(
			excl_ptr<dynamic_param_expr_list> ta) const;
	// overrides definition_base's, exception to rule
	// because this is not a scopespace
	// ah, but it is now!
	never_const_ptr<instantiation_base>
		add_template_formal(excl_ptr<instantiation_base> f);

	bool require_signature_match(
		never_const_ptr<definition_base> d) const
		{ assert(d); return key == d->get_name(); }
		// really, this should never be called...
public:
	// actually going to de/serialize built-in type, only to be
	// intercepted and replaced by data_type_reference::load_object
//	ART_OBJECT_IO_METHODS
	// thus we need only collect and write...
	void collect_transient_info(persistent_object_manager& m) const;
	void write_object(persistent_object_manager& m) const;
	void load_used_id_map_object(excl_ptr<object> o);
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
class enum_datatype_def : public datatype_definition_base, public scopespace {
protected:
	const string					key;
	const never_const_ptr<name_space>		parent;
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
	// don't need yet
	count_const_ptr<fundamental_type_reference>
		make_fundamental_type_reference(
			excl_ptr<dynamic_param_expr_list> ta) const;
#endif

	bool require_signature_match(never_const_ptr<definition_base> d) const;

	bool add_member(const token_identifier& em);
public:
	ART_OBJECT_IO_METHODS
	void load_used_id_map_object(excl_ptr<object> o);
};	// end class enum_datatype_def

//-----------------------------------------------------------------------------
/**
	Reserved for special built-in parameter types, pint and pbool.  
	Nothing can really be derived from them... yet.  
	Note that there is no intermediate param_definition class, 
	because parameter types can only be built in; there are no
	user-defined parameter types, for now...
	Doesn't have a param_type_reference pointer member, because
	type is hard-wired to the built-ins.  
 */
class built_in_param_def : public definition_base {
protected:
	const string				key;
	const never_const_ptr<name_space>	parent;
public:
	built_in_param_def(never_const_ptr<name_space> p, const string& n);
	~built_in_param_def();

	ostream& what(ostream& o) const;
//	ostream& dump(ostream& o) const;

	const string& get_key(void) const;
	never_const_ptr<scopespace> get_parent(void) const;

	/** can't alias built-in param types, would be confusing */
	excl_ptr<definition_base>
		make_typedef(never_const_ptr<scopespace> s, 
			const token_identifier& id) const;

	count_const_ptr<fundamental_type_reference>
		make_fundamental_type_reference(
			excl_ptr<dynamic_param_expr_list> ta) const;
};	// end class built_in_param_def

//-----------------------------------------------------------------------------
/**
	Generalizable user-defined data type, which can (eventually) 
	build upon other user-defined data types.  
 */
class user_def_datatype : public datatype_definition_base, public scopespace {
protected:
	const string				key;
	const never_const_ptr<name_space>	parent;
public:
	user_def_datatype(never_const_ptr<name_space> o, const string& name);
	~user_def_datatype();

	const string& get_key(void) const;
	string get_qualified_name(void) const;
	never_const_ptr<scopespace> get_parent(void) const;

	never_const_ptr<object> lookup_object_here(const string& id) const;

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;

	bool require_signature_match(
		never_const_ptr<definition_base> d) const { return false; }

//	bool certify_port_actuals(const object_list& ol) const;
public:
	ART_OBJECT_IO_METHODS
	void load_used_id_map_object(excl_ptr<object> o);
};	// end class user_def_datatype

//-----------------------------------------------------------------------------
/**
	Data-type typedef.  
 */
class datatype_definition_alias : public datatype_definition_base, 
		public typedef_base {
protected:
	const string				key;
	const never_const_ptr<scopespace>	parent;
	// inherited template formals
	excl_const_ptr<data_type_reference>	base;
public:
	datatype_definition_alias(const string& n, 
		never_const_ptr<scopespace> p);
	~datatype_definition_alias();

	ostream& what(ostream& o) const;
	const string& get_key(void) const;
	never_const_ptr<scopespace> get_parent(void) const;
	never_const_ptr<fundamental_type_reference>
		get_base_type_ref(void) const;

	bool assign_typedef(excl_const_ptr<fundamental_type_reference> f);
	count_const_ptr<fundamental_type_reference>
		make_fundamental_type_reference(
			excl_ptr<dynamic_param_expr_list> ta) const;
	bool require_signature_match(
		never_const_ptr<definition_base> d) const;
public:
	ART_OBJECT_IO_METHODS
	void load_used_id_map_object(excl_ptr<object> o);
};	// end class datatype_definition_alias

//=============================================================================
/// abstract base class for channels and their representations
class channel_definition_base : virtual public definition_base {
protected:
public:
	channel_definition_base();
//	channel_definition_base(const string& n);
virtual	~channel_definition_base();

// virtual	ostream& what(ostream& o) const = 0;

	excl_ptr<definition_base>
		make_typedef(never_const_ptr<scopespace> s, 
			const token_identifier& id) const;

virtual	count_const_ptr<fundamental_type_reference>
		make_fundamental_type_reference(
			excl_ptr<dynamic_param_expr_list> ta) const;
};	// end class channel_definition_base

//-----------------------------------------------------------------------------
/**
	Generalizable user-defined channel type, which can (eventually) 
	build upon other user-defined channel types.  
 */
class user_def_chan : public channel_definition_base, public scopespace {
protected:
	// list of other type definitions
	const string				key;
	const never_const_ptr<name_space>	parent;
public:
	user_def_chan(never_const_ptr<name_space> o, const string& name);
	~user_def_chan();

	const string& get_key(void) const;
	string get_qualified_name(void) const;
	never_const_ptr<scopespace> get_parent(void) const;

	never_const_ptr<object> lookup_object_here(const string& id) const;

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;

#if 0
	count_const_ptr<fundamental_type_reference>
		make_fundamental_type_reference(
			excl_ptr<dynamic_param_expr_list> ta) const;
#endif
//	bool certify_port_actuals(const object_list& ol) const;
public:
	ART_OBJECT_IO_METHODS
	void load_used_id_map_object(excl_ptr<object> o);
};	// end class user_def_chan

//-----------------------------------------------------------------------------
/**
	Alias to a channel-type.  
 */
class channel_definition_alias : public channel_definition_base, 
		public typedef_base {
protected:
	const string				key;
	const never_const_ptr<scopespace>	parent;
	excl_const_ptr<channel_type_reference>	base;
public:
	channel_definition_alias(const string& n, 
		never_const_ptr<scopespace> p);
	~channel_definition_alias();

	ostream& what(ostream& o) const;
	const string& get_key(void) const;
	never_const_ptr<scopespace> get_parent(void) const;
	never_const_ptr<fundamental_type_reference>
		get_base_type_ref(void) const;

	bool assign_typedef(excl_const_ptr<fundamental_type_reference> f);
#if 0
	count_const_ptr<fundamental_type_reference>
		make_fundamental_type_reference(
			excl_ptr<dynamic_param_expr_list> ta) const;
#endif
public:
	ART_OBJECT_IO_METHODS
	void load_used_id_map_object(excl_ptr<object> o);
};	// end class channel_definition_alias

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

// class type_alias was here.

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_DEFINITION_H__

