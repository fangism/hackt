/**
	\file "Object/art_object_type_ref.h"
	Type-reference classes of the ART language.  
 	$Id: art_object_type_ref.h,v 1.27 2005/06/23 03:00:30 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_TYPE_REF_H__
#define	__OBJECT_ART_OBJECT_TYPE_REF_H__

#include "Object/art_object_type_ref_base.h"
#include "Object/art_object_expr_types.h"

namespace ART {
namespace parser {
	class token_identifier;
}

namespace entity {
class datatype_definition_base;
class unroll_context;
class builtin_channel_type_reference;
class channel_definition_base;
class process_definition_base;
class built_in_param_def;
USING_LIST
using std::ostream;
using parser::token_identifier;

//=============================================================================
// class type_reference_base declared in "art_object_base.h"
// class fundamental_type_reference declared in "art_object_base.h"

//=============================================================================
/**
	Reference to a data-type definition.  
	Includes optional template parameters.  
 */
class data_type_reference : public fundamental_type_reference {
private:
	typedef	fundamental_type_reference		parent_type;
	typedef	data_type_reference			this_type;
	typedef	datatype_definition_base		definition_type;
	typedef	never_ptr<const definition_type>	definition_ptr_type;
protected:
	typedef	parent_type::template_args_ptr_type	template_args_ptr_type;
//	excl_ptr<const param_expr_list>	template_params;	// inherited
	/**
		Reference to data type definition, which may be a 
		built-in type, enumeration, struct, or another typedef.  
	 */
	definition_ptr_type				base_type_def;

	// NEW: resolved type
	// resolve const_param_expr_list...
	// but needs to be able to take arrays of constants.  
	
private:
	data_type_reference();
public:
	explicit
	data_type_reference(const definition_ptr_type td);

	data_type_reference(const definition_ptr_type td, 
		template_args_ptr_type& pl);
		// not gcc-2.95.3 friendly default argument = NULL

	// virtualize if something derives from this
	~data_type_reference();

	ostream&
	what(ostream& o) const;

	never_ptr<const definition_base>
	get_base_def(void) const;

	never_ptr<const datatype_definition_base>
	get_base_datatype_def(void) const;

	/// unroll-time type-resolution... arguments? return? context?
	// need to be able to lookup parameters... update later...
	count_ptr<const this_type>
	unroll_resolve(unroll_context&) const;

	static
	data_type_reference*
	make_quick_int_type_ref(const pint_value_type);

private:
	excl_ptr<instantiation_statement_base>
	make_instantiation_statement_private(
		const count_ptr<const fundamental_type_reference>& t, 
		const index_collection_item_ptr_type& d) const;
			
	excl_ptr<instance_collection_base>
	make_instance_collection(const never_ptr<const scopespace> s, 
		const token_identifier& id, const size_t d) const;
public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class data_type_reference

//=============================================================================
/**
	Abstract parent class for all channel types.  
 */
class channel_type_reference_base : public fundamental_type_reference {
protected:
	typedef	fundamental_type_reference		parent_type;
public:
#if 0
	typedef	enum {
		BIDIRECTIONAL, 
		SEND, 
		RECEIVE
	}	direction_type;
#endif
protected:
	/**
		Three possible values: '\0' means bidirections (unspecified), 
		'!' means send-only, '?' means receive-only.
	 */
	char						direction;
protected:
	channel_type_reference_base() : parent_type(), direction('\0') { }

	explicit
	channel_type_reference_base(template_args_ptr_type&);

public:
virtual	~channel_type_reference_base() { }

virtual	ostream&
	dump(ostream&) const = 0;

	void
	set_direction(const char c) { direction = c; }

	char
	get_direction(void) const { return direction; }

	ostream&
	dump_direction(ostream&) const;

virtual	never_ptr<const builtin_channel_type_reference>
	resolve_builtin_channel_type(void) const = 0;

protected:
	using parent_type::collect_transient_info_base;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class channel_type_reference_base

//-----------------------------------------------------------------------------
/**
	Reference to an intrinsic channel type, chan(...).
	There is no built-in channel definition, which is why we need
	this channel type split off.  
 */
class builtin_channel_type_reference : public channel_type_reference_base {
	typedef	builtin_channel_type_reference		this_type;
	typedef	channel_type_reference_base		parent_type;
public:
	typedef	count_ptr<const data_type_reference>	datatype_ptr_type;
	typedef	vector<datatype_ptr_type>		datatype_list_type;
private:
	datatype_list_type				datatype_list;
public:
	builtin_channel_type_reference();
	~builtin_channel_type_reference();

	ostream&
	what(ostream& o) const;

	// overrides grandparent's
	ostream&
	dump(ostream&) const;

	ostream&
	dump_long(ostream&) const;

	never_ptr<const definition_base>
	get_base_def(void) const;

	void
	reserve_datatypes(const size_t);

	void
	add_datatype(const datatype_list_type::value_type&);

	size_t
	num_datatypes(void) const { return datatype_list.size(); }

	// for convenience...
	const datatype_list_type&
	get_datatype_list(void) const { return datatype_list; }

	datatype_ptr_type
	index_datatype(const size_t) const;

	never_ptr<const builtin_channel_type_reference>
	resolve_builtin_channel_type(void) const;

private:
	excl_ptr<instantiation_statement_base>
	make_instantiation_statement_private(
		const count_ptr<const fundamental_type_reference>& t, 
		const index_collection_item_ptr_type& d) const;
			
	excl_ptr<instance_collection_base>
	make_instance_collection(const never_ptr<const scopespace> s, 
		const token_identifier& id, const size_t d) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
};	// end class builtin_channel_type_reference

//-----------------------------------------------------------------------------
/**
	Reference to a channel-type definition.  
	Includes optional template parameters.  
 */
class channel_type_reference : public channel_type_reference_base {
private:
	typedef	channel_type_reference			this_type;
	typedef	channel_type_reference_base		parent_type;
protected:
	typedef	parent_type::template_args_ptr_type	template_args_ptr_type;
//	excl_ptr<const param_expr_list>	template_params;	// inherited
	never_ptr<const channel_definition_base>	base_chan_def;
private:
	channel_type_reference();
public:
	explicit
	channel_type_reference(
		const never_ptr<const channel_definition_base> td);

	channel_type_reference(
		const never_ptr<const channel_definition_base> td, 
		template_args_ptr_type& pl);

	~channel_type_reference();

	ostream&
	what(ostream& o) const;

	// override grandparent's
	ostream&
	dump(ostream& o) const;

	never_ptr<const definition_base>
	get_base_def(void) const;

	never_ptr<const builtin_channel_type_reference>
	resolve_builtin_channel_type(void) const;

private:
	excl_ptr<instantiation_statement_base>
	make_instantiation_statement_private(
		const count_ptr<const fundamental_type_reference>& t, 
		const index_collection_item_ptr_type& d) const;
			
	excl_ptr<instance_collection_base>
	make_instance_collection(const never_ptr<const scopespace> s, 
		const token_identifier& id, const size_t d) const;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class channel_type_reference

//-----------------------------------------------------------------------------
/**
	Reference to a process-type definition.  
	Includes optional template parameters.  
 */
class process_type_reference : public fundamental_type_reference {
private:
	typedef	fundamental_type_reference		parent_type;
	typedef	process_type_reference			this_type;
	typedef	process_definition_base			definition_type;
	typedef	never_ptr<const definition_type>	definition_ptr_type;
protected:
	typedef	parent_type::template_args_ptr_type		template_args_ptr_type;
//	excl_ptr<const param_expr_list>	template_params;	// inherited
// should be const?  reference to base definition shouldn't change...
	never_ptr<const process_definition_base>	base_proc_def;
private:
	process_type_reference();
public:
	explicit
	process_type_reference(
		const never_ptr<const process_definition_base> td);

	process_type_reference(
		const never_ptr<const process_definition_base> td, 
		template_args_ptr_type& pl);
		// not gcc-2.95.3 friendly default argument = NULL

	~process_type_reference();

	ostream&
	what(ostream& o) const;

	never_ptr<const definition_base>
	get_base_def(void) const;

	count_ptr<const this_type>
	unroll_resolve(unroll_context& ) const;

private:
	excl_ptr<instantiation_statement_base>
	make_instantiation_statement_private(
		const count_ptr<const fundamental_type_reference>& t, 
		const index_collection_item_ptr_type& d) const;
			
	excl_ptr<instance_collection_base>
	make_instance_collection(const never_ptr<const scopespace> s, 
		const token_identifier& id, const size_t d) const;
public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class process_type_reference

//-----------------------------------------------------------------------------
/**
	Reference to a param definition.  
	This class really does nothing, but exists merely for consistency's
	sake.  Parameters are non-templatable, thus it suffices for 
	parameter instances to refer directly to the built-in definitions, 
	but we rather than make a special-case exception, we use this class.  
	The template_params member is inherited but not used.  
	Built-in implementation: only will ever be two instances of this class. 
 */
class param_type_reference : public fundamental_type_reference {
private:
	typedef	param_type_reference		this_type;
	typedef	fundamental_type_reference	parent_type;	// not used
protected:
//	excl_ptr<const param_expr_list>	template_params;	// inherited, unused
	never_ptr<const built_in_param_def>	base_param_def;
public:
	explicit
	param_type_reference(const never_ptr<const built_in_param_def> td);

	// no need to virtual, unless something derives from this
	~param_type_reference();

	ostream&
	what(ostream& o) const;

	never_ptr<const definition_base>
	get_base_def(void) const;

private:
	excl_ptr<instantiation_statement_base>
	make_instantiation_statement_private(
		const count_ptr<const fundamental_type_reference>& t, 
		const index_collection_item_ptr_type& d) const;
			
	excl_ptr<instance_collection_base>
	make_instance_collection(const never_ptr<const scopespace> s,
		const token_identifier& id, const size_t d) const;

private:
	// dummy implementation, never called
	PERSISTENT_METHODS_DECLARATIONS

};	// end class param_type_reference

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_TYPE_REF_H__

