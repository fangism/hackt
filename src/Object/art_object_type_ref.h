// "art_object_type_ref.h"

#ifndef	__ART_OBJECT_TYPE_REF_H__
#define	__ART_OBJECT_TYPE_REF_H__

// #include "art_macros.h"
#include "art_object_base.h"

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
	never_const_ptr<type_reference_base>	base;
	never_const_ptr<array_index_list>		dim;
public:
	collective_type_reference(const type_reference_base& b, 
		never_const_ptr<array_index_list> d);
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
protected:
//	excl_const_ptr<param_expr_list>	template_params;	// inherited
	never_const_ptr<datatype_definition_base>	base_type_def;
private:
	data_type_reference();
public:
	data_type_reference(
		never_const_ptr<datatype_definition_base> td);
	data_type_reference(
		never_const_ptr<datatype_definition_base> td, 
		excl_const_ptr<param_expr_list> pl);
		// not gcc-2.95.3 friendly default argument = NULL
virtual	~data_type_reference();

	ostream& what(ostream& o) const;
	never_const_ptr<definition_base> get_base_def(void) const;

private:
	excl_ptr<instantiation_statement>
		make_instantiation_statement_private(
			count_const_ptr<fundamental_type_reference> t, 
			index_collection_item_ptr_type d) const;
			
	excl_ptr<instance_collection_base>
		make_instance_collection(
#if 0
			count_const_ptr<fundamental_type_reference> t, 
#endif
			never_const_ptr<scopespace> s, 
			const token_identifier& id, 
			const size_t d) const;
public:
	ART_OBJECT_IO_METHODS
};	// end class data_type_reference

//-----------------------------------------------------------------------------
/**
	Reference to a channel-type definition.  
	Includes optional template parameters.  
 */
class channel_type_reference : public fundamental_type_reference {
protected:
//	excl_const_ptr<param_expr_list>	template_params;	// inherited
	never_const_ptr<channel_definition_base>	base_chan_def;
private:
	channel_type_reference();
public:
	channel_type_reference(
		never_const_ptr<channel_definition_base> td);
	channel_type_reference(
		never_const_ptr<channel_definition_base> td, 
		excl_const_ptr<param_expr_list> pl);
virtual	~channel_type_reference();

	ostream& what(ostream& o) const;
	never_const_ptr<definition_base> get_base_def(void) const;
private:
	excl_ptr<instantiation_statement>
		make_instantiation_statement_private(
			count_const_ptr<fundamental_type_reference> t, 
			index_collection_item_ptr_type d) const;
			
	excl_ptr<instance_collection_base>
		make_instance_collection(
#if 0
			count_const_ptr<fundamental_type_reference> t, 
#endif
			never_const_ptr<scopespace> s, 
			const token_identifier& id, 
			const size_t d) const;
public:
	ART_OBJECT_IO_METHODS
};	// end class channel_type_reference

//-----------------------------------------------------------------------------
/**
	Reference to a process-type definition.  
	Includes optional template parameters.  
 */
class process_type_reference : public fundamental_type_reference {
protected:
//	excl_const_ptr<param_expr_list>	template_params;	// inherited
// should be const?  reference to base definition shouldn't change...
	never_const_ptr<process_definition_base>	base_proc_def;
private:
explicit process_type_reference();
public:
	process_type_reference(
		never_const_ptr<process_definition_base> td);
	process_type_reference(
		never_const_ptr<process_definition_base> td, 
		excl_const_ptr<param_expr_list> pl);
		// not gcc-2.95.3 friendly default argument = NULL
virtual	~process_type_reference();

	ostream& what(ostream& o) const;
	never_const_ptr<definition_base> get_base_def(void) const;
private:
	excl_ptr<instantiation_statement>
		make_instantiation_statement_private(
			count_const_ptr<fundamental_type_reference> t, 
			index_collection_item_ptr_type d) const;
			
	excl_ptr<instance_collection_base>
		make_instance_collection(
#if 0
			count_const_ptr<fundamental_type_reference> t, 
#endif
			never_const_ptr<scopespace> s, 
			const token_identifier& id, 
			const size_t d) const;
public:
	// macro expand to method prototypes
	ART_OBJECT_IO_METHODS
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
protected:
//	excl_const_ptr<param_expr_list>	template_params;	// inherited, unused
	never_const_ptr<built_in_param_def>	base_param_def;
public:
	param_type_reference(never_const_ptr<built_in_param_def> td);
virtual	~param_type_reference();

	ostream& what(ostream& o) const;
	never_const_ptr<definition_base> get_base_def(void) const;
private:
	excl_ptr<instantiation_statement>
		make_instantiation_statement_private(
			count_const_ptr<fundamental_type_reference> t, 
			index_collection_item_ptr_type d) const;
			
	excl_ptr<instance_collection_base>
		make_instance_collection(
#if 0
			count_const_ptr<fundamental_type_reference> t, 
#endif
			never_const_ptr<scopespace> s, 
			const token_identifier& id, 
			const size_t d) const;

};	// end class param_type_reference

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_TYPE_REF_H__

