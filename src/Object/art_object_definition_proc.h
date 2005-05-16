/**
	\file "Object/art_object_definition_proc.h"
	Process-definition-related ART object classes.  
	$Id: art_object_definition_proc.h,v 1.1.2.1 2005/05/16 03:52:21 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_DEFINITION_PROC_H__
#define	__OBJECT_ART_OBJECT_DEFINITION_PROC_H__

#include "Object/art_object_definition.h"
#include "Object/art_object_PRS_base.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	Process definition base class.  From this, there will arise: true
	process definitions, and typedef process definitions. 
 */
class process_definition_base : virtual public definition_base {
private:
	typedef	definition_base		parent_type;
protected:
	// no new members?
public:
//	process_definition_base(const string& n);
	process_definition_base() { }
virtual	~process_definition_base() { }

	excl_ptr<definition_base>
	make_typedef(never_ptr<const scopespace> s, 
		const token_identifier& id) const;

// inherited pure virtuals are still pure virtuals
protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
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
class process_definition : public process_definition_base, public scopespace, 
	public sequential_scope {
private:
	typedef	process_definition		this_type;
public:
	typedef	never_ptr<const instance_collection_base>
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
	const never_ptr<const name_space>	parent;
	port_formals_list_type			port_formals_list;
	port_formals_map_type			port_formals_map;
	// list language bodies
	PRS::rule_set				prs;
private:
	process_definition();
public:
	process_definition(never_ptr<const name_space> o, const string& s); 
	~process_definition();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	const string&
	get_key(void) const;

	string
	get_qualified_name(void) const;

	never_ptr<const scopespace>
	get_parent(void) const;

	/** overrides definition_base's */
	never_ptr<const instance_collection_base>
	lookup_port_formal(const string& id) const;

	never_ptr<const object>
	lookup_object_here(const string& id) const;

	count_ptr<const fundamental_type_reference>
	make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list>& ta) const;

	never_ptr<const instance_collection_base>
	add_port_formal(const never_ptr<instantiation_statement_base> f, 
		const token_identifier& id);

	good_bool
	certify_port_actuals(const checked_refs_type& ol) const;

	bool
	equivalent_port_formals(
		const never_ptr<const process_definition> p) const;

	good_bool
	require_signature_match(const never_ptr<const definition_base> d) const;

// methods for object file I/O
public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

private:
	void
	load_used_id_map_object(excl_ptr<persistent>& o);
protected:
	void
	write_object_port_formals(const persistent_object_manager& m, 
		ostream& o) const;

	void
	load_object_port_formals(const persistent_object_manager& m,
		istream& i);

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
private:
	typedef	process_definition_alias		this_type;
protected:
	const string					key;
	/** parent can be namespace or definition */
	never_ptr<const scopespace>			parent;
	excl_ptr<const process_type_reference>		base;
private:
	process_definition_alias();
public:
	process_definition_alias(const string& n, 
		const never_ptr<const scopespace> p);

	~process_definition_alias();

	ostream&
	what(ostream& o) const;

	const string&
	get_key(void) const;

	never_ptr<const scopespace>
	get_parent(void) const;

	never_ptr<const fundamental_type_reference>
	get_base_type_ref(void) const;

	bool
	assign_typedef(excl_ptr<const fundamental_type_reference>& f);

	count_ptr<const fundamental_type_reference>
	make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list>& ta) const;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

private:
	void
	load_used_id_map_object(excl_ptr<persistent>& o);
};	// end class process_definition_alias

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_DEFINITION_PROC_H__

