/**
	\file "art_object_inst_ref.h"
	Class family for instance references in ART.  
	$Id: art_object_inst_ref.h,v 1.10 2004/12/06 07:11:20 fang Exp $
 */

#ifndef	__ART_OBJECT_INST_REF_H__
#define	__ART_OBJECT_INST_REF_H__

#include "art_object_inst_ref_base.h"
#include "art_object_instance_base.h"
#include "memory/pointer_classes.h"

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
	using std::ostream;
	using std::istream;
	USING_LIST
	using namespace util::memory;

//=============================================================================
/**
	A reference to a simple instance of datatype.  
 */
class datatype_instance_reference : public simple_instance_reference {
protected:
//	excl_ptr<index_list>			array_indices;	// inherited
	const never_ptr<const datatype_instance_collection>	data_inst_ref;

protected:
	datatype_instance_reference();
public:
	datatype_instance_reference(never_ptr<const datatype_instance_collection> di, 
		excl_ptr<index_list> i);
virtual	~datatype_instance_reference();

virtual	ostream& what(ostream& o) const;
//	ostream& dump(ostream& o) const;
	never_ptr<const instance_collection_base> get_inst_base(void) const;

public:
	// need to be virtual? for member_instance_reference?
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class datatype_instance_reference

//-----------------------------------------------------------------------------
/**
	A reference to a simple instance of channel.  
 */
class channel_instance_reference : public simple_instance_reference {
protected:
//	excl_ptr<index_list>			array_indices;	// inherited
	const never_ptr<const channel_instance_collection>	channel_inst_ref;

protected:
	channel_instance_reference();
public:
	channel_instance_reference(never_ptr<const channel_instance_collection> ci, 
		excl_ptr<index_list> i);
virtual	~channel_instance_reference();

virtual	ostream& what(ostream& o) const;
//	ostream& dump(ostream& o) const;
	never_ptr<const instance_collection_base> get_inst_base(void) const;

public:
	// need to be virtual? for member_instance_reference?
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class channel_instance_reference

//-----------------------------------------------------------------------------
/**
	A reference to a simple instance of process.  
 */
class process_instance_reference : public simple_instance_reference {
protected:
//	excl_ptr<index_list>			array_indices;	// inherited
	const never_ptr<const process_instance_collection>	process_inst_ref;

protected:
	process_instance_reference();
public:
	process_instance_reference(never_ptr<const process_instance_collection> pi, 
		excl_ptr<index_list> i);
virtual	~process_instance_reference();

virtual	ostream& what(ostream& o) const;
	never_ptr<const instance_collection_base> get_inst_base(void) const;

public:
	// need to be virtual? for member_instance_reference?
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class process_instance_reference

//=============================================================================
/**
	Reference to a process instance member of something else.
	Derive from some generic member_instance_reference?
 */
class process_member_instance_reference :
		public member_instance_reference_base, 
		public process_instance_reference {
protected:
// inherited:
//	excl_ptr<index_list>			array_indices;
//	const never_ptr<const process_instance_collection>	process_inst_ref;
//	const count_ptr<const simple_instance_reference>	base;
private:
	process_member_instance_reference();
public:
	process_member_instance_reference(
		count_ptr<const simple_instance_reference> b, 
		never_ptr<const process_instance_collection> m);
	~process_member_instance_reference();

	ostream& what(ostream& o) const;
// can also attach indices!

public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class process_member_instance_reference

//=============================================================================
/**
	Reference to a datatype instance member of another struct.  
 */
class datatype_member_instance_reference : 
		public member_instance_reference_base, 
		public datatype_instance_reference {
protected:
// inherited:
//	excl_ptr<index_list>			array_indices;
//	const never_ptr<const datatype_instance_collection>	data_inst_ref;
//	const count_ptr<const simple_instance_reference>	base;
private:
	datatype_member_instance_reference();
public:
	datatype_member_instance_reference(
		count_ptr<const simple_instance_reference> b, 
		never_ptr<const datatype_instance_collection> m);
	~datatype_member_instance_reference();

	ostream& what(ostream& o) const;
// can also attach indices!

public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS

};	// end class datatype_member_instance_reference

//=============================================================================
/**
	Reference to a channel instance member of another struct.  
 */
class channel_member_instance_reference : 
		public member_instance_reference_base, 
		public channel_instance_reference {
protected:
// inherited:
//	excl_ptr<index_list>			array_indices;
//	const never_ptr<const channel_instance_collection>	channel_inst_ref;
//	const count_ptr<const simple_instance_reference>	base;
private:
	channel_member_instance_reference();
public:
	channel_member_instance_reference(
		count_ptr<const simple_instance_reference> b, 
		never_ptr<const channel_instance_collection> m);
	~channel_member_instance_reference();

	ostream& what(ostream& o) const;
// can also attach indices!

public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS

};	// end class channel_member_instance_reference

//=============================================================================
#if 0
/**
	A reference to a simple instance of parameter.  
 */
class param_instance_reference : public simple_instance_reference {
protected:
//	excl_ptr<index_list>			array_indices;	// inherited

// virtualized
//	never_ptr<param_instance_collection>		param_inst_ref;

protected:
	param_instance_reference();
public:
	param_instance_reference(excl_ptr<index_list> i, 
		const instantiation_state& st);
virtual	~param_instance_reference() { }

virtual	ostream& what(ostream& o) const = 0;
virtual	never_ptr<const instance_collection_base>
		get_inst_base(void) const = 0;
virtual	never_ptr<const param_instance_collection>
		get_param_inst_base(void) const = 0;

	// consider moving these functions into instance_reference_base
	//	where array_indices are inherited from.  
	bool may_be_initialized(void) const;
	bool must_be_initialized(void) const;
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;

#if 0
// PHASED OUT: is type-specific
virtual	bool initialize(count_ptr<const param_expr> i) = 0;
#endif

};	// end class param_instance_reference
#endif

//=============================================================================
// classes pint_instance_reference and pbool_instance_reference
//	are in "art_object_expr.*"

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INST_REF_H__

