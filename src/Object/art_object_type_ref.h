/**
	\file "art_object_type_ref.h"
	Type-reference classes of the ART language.  
 	$Id: art_object_type_ref.h,v 1.15 2004/12/11 06:22:43 fang Exp $
 */

#ifndef	__ART_OBJECT_TYPE_REF_H__
#define	__ART_OBJECT_TYPE_REF_H__

#include "art_object_type_ref_base.h"

namespace ART {
namespace parser {
	class token_identifier;
}

namespace entity {

USING_LIST
using std::ostream;
using parser::token_identifier;
using namespace util::memory;	// for experimental pointer classes

//=============================================================================
// class type_reference_base declared in "art_object_base.h"
// class fundamental_type_reference declared in "art_object_base.h"

//=============================================================================
#if 0
MAY BE OBSOLETE
/**
	Class for reference to a collection or array of fundamental types.
	Or should we allow collective_types to contain collective_types?
	Depends on whether or not gramma allows both styles of arrays:
	x[i][j] vs. x[i,j], and whether or not they are equivalent.  
 */
class collective_type_reference : public type_reference_base {
protected:
	// don't own these members
	never_ptr<const type_reference_base>	base;
	never_ptr<const array_index_list>		dim;
public:
	collective_type_reference(const type_reference_base& b, 
		never_ptr<const array_index_list> d);
	~collective_type_reference();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
};	// end class collective_type_reference
#endif

//-----------------------------------------------------------------------------
/**
	Reference to a data-type definition.  
	Includes optional template parameters.  
 */
class data_type_reference : public fundamental_type_reference {
private:
	typedef	fundamental_type_reference		parent_type;
protected:
//	excl_ptr<const param_expr_list>	template_params;	// inherited
	never_ptr<const datatype_definition_base>	base_type_def;
private:
	data_type_reference();
public:
	data_type_reference(
		never_ptr<const datatype_definition_base> td);
	data_type_reference(
		never_ptr<const datatype_definition_base> td, 
		excl_ptr<const param_expr_list> pl);
		// not gcc-2.95.3 friendly default argument = NULL
virtual	~data_type_reference();

	ostream&
	what(ostream& o) const;

	never_ptr<const definition_base>
	get_base_def(void) const;

	never_ptr<const datatype_definition_base>
	get_base_datatype_def(void) const;

private:
	excl_ptr<instantiation_statement>
	make_instantiation_statement_private(
		count_ptr<const fundamental_type_reference> t, 
		index_collection_item_ptr_type d) const;
			
	excl_ptr<instance_collection_base>
	make_instance_collection(never_ptr<const scopespace> s, 
		const token_identifier& id, const size_t d) const;
public:
	PERSISTENT_METHODS
};	// end class data_type_reference

//-----------------------------------------------------------------------------
/**
	Reference to a channel-type definition.  
	Includes optional template parameters.  
 */
class channel_type_reference : public fundamental_type_reference {
private:
	typedef	fundamental_type_reference		parent_type;
protected:
//	excl_ptr<const param_expr_list>	template_params;	// inherited
	never_ptr<const channel_definition_base>	base_chan_def;
private:
	channel_type_reference();
public:
	channel_type_reference(
		never_ptr<const channel_definition_base> td);
	channel_type_reference(
		never_ptr<const channel_definition_base> td, 
		excl_ptr<const param_expr_list> pl);
virtual	~channel_type_reference();

	ostream&
	what(ostream& o) const;

	never_ptr<const definition_base>
	get_base_def(void) const;
private:
	excl_ptr<instantiation_statement>
	make_instantiation_statement_private(
		count_ptr<const fundamental_type_reference> t, 
		index_collection_item_ptr_type d) const;
			
	excl_ptr<instance_collection_base>
	make_instance_collection(never_ptr<const scopespace> s, 
		const token_identifier& id, const size_t d) const;
public:
	PERSISTENT_METHODS
};	// end class channel_type_reference

//-----------------------------------------------------------------------------
/**
	Reference to a process-type definition.  
	Includes optional template parameters.  
 */
class process_type_reference : public fundamental_type_reference {
private:
	typedef	fundamental_type_reference		parent_type;
protected:
//	excl_ptr<const param_expr_list>	template_params;	// inherited
// should be const?  reference to base definition shouldn't change...
	never_ptr<const process_definition_base>	base_proc_def;
private:
explicit process_type_reference();
public:
	process_type_reference(
		never_ptr<const process_definition_base> td);
	process_type_reference(
		never_ptr<const process_definition_base> td, 
		excl_ptr<const param_expr_list> pl);
		// not gcc-2.95.3 friendly default argument = NULL
virtual	~process_type_reference();

	ostream&
	what(ostream& o) const;

	never_ptr<const definition_base>
	get_base_def(void) const;

private:
	excl_ptr<instantiation_statement>
	make_instantiation_statement_private(
		count_ptr<const fundamental_type_reference> t, 
		index_collection_item_ptr_type d) const;
			
	excl_ptr<instance_collection_base>
	make_instance_collection(never_ptr<const scopespace> s, 
		const token_identifier& id, const size_t d) const;
public:
	// macro expand to method prototypes
	PERSISTENT_METHODS
};	// end class process_type_reference

//-----------------------------------------------------------------------------
/**
	Reference to a param definition.  
	This class really does nothing, but exists merely for consistency's
	sake.  Parameters are non-templatable, thus it suffices for 
	parameter instances to refer directly to the built-in definitions, 
	but we rather than make a special-case exception, we use this class.  
	The template_params member is inherited but not used.  
	Built-in implementation: only will ever be two instance of this class. 
 */
class param_type_reference : public fundamental_type_reference {
private:
	typedef	fundamental_type_reference	parent_type;	// not used
protected:
//	excl_ptr<const param_expr_list>	template_params;	// inherited, unused
	never_ptr<const built_in_param_def>	base_param_def;
public:
	param_type_reference(never_ptr<const built_in_param_def> td);
virtual	~param_type_reference();

	ostream&
	what(ostream& o) const;

	never_ptr<const definition_base>
	get_base_def(void) const;

private:
	excl_ptr<instantiation_statement>
	make_instantiation_statement_private(
		count_ptr<const fundamental_type_reference> t, 
		index_collection_item_ptr_type d) const;
			
	excl_ptr<instance_collection_base>
	make_instance_collection(never_ptr<const scopespace> s,
		const token_identifier& id, const size_t d) const;

private:
	// dummy implementation, never called
	PERSISTENT_METHODS

};	// end class param_type_reference

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_TYPE_REF_H__

