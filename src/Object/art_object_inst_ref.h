/**
	\file "art_object_inst_ref.h"
	Class family for instance references in ART.  
	$Id: art_object_inst_ref.h,v 1.12 2004/12/10 22:02:16 fang Exp $
 */

#ifndef	__ART_OBJECT_INST_REF_H__
#define	__ART_OBJECT_INST_REF_H__

#include "art_object_inst_ref_base.h"
#include "art_object_instance_base.h"
#include "memory/pointer_classes.h"

namespace ART {
namespace entity {
using std::ostream;
using std::istream;
using namespace util::memory;

//=============================================================================
// consider relocating to "art_object_inst_ref_data.h"
/**
	A reference to a simple instance of datatype.  
	Consider sub-typing into user-defined and built-in, 
	making this an abstract base.
 */
class datatype_instance_reference : public simple_instance_reference {
private:
	typedef	simple_instance_reference		parent_type;
protected:
//	excl_ptr<index_list>			array_indices;	// inherited

#if 0
	// PHASING OUT:
	const never_ptr<const datatype_instance_collection>	data_inst_ref;
#endif

protected:
	datatype_instance_reference();
#if 0
public:
	datatype_instance_reference(
		never_ptr<const datatype_instance_collection> di, 
		excl_ptr<index_list> i);
#else
	datatype_instance_reference(excl_ptr<index_list> i, 
		const instantiation_state& s);
#endif

public:
virtual	~datatype_instance_reference();

#if 0
virtual	ostream& what(ostream& o) const;
#else
virtual	ostream& what(ostream& o) const = 0;
#endif
//	ostream& dump(ostream& o) const;

// becoming pure virtual
virtual	never_ptr<const instance_collection_base>
	get_inst_base(void) const = 0;

protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
public:
#if 0
	// need to be virtual? for member_instance_reference?
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
#endif
};	// end class datatype_instance_reference

//-----------------------------------------------------------------------------
/**
	A reference to a simple instance of channel.  
 */
class channel_instance_reference : public simple_instance_reference {
private:
	typedef	simple_instance_reference		parent_type;
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

protected:
	void
	write_object_base(const persistent_object_manager& m, ostream&) const;

	void
	load_object_base(persistent_object_manager& m, istream&);

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
private:
	typedef	simple_instance_reference		parent_type;
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

protected:
	void
	write_object_base(const persistent_object_manager& m, ostream&) const;

	void
	load_object_base(persistent_object_manager& m, istream&);

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
private:
	typedef	process_instance_reference		parent_type;
	typedef	member_instance_reference_base		interface_type;
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
	Potential problem to address: nested structs (easy, just fix)
		Containership vs. inheritance.  
 */
class datatype_member_instance_reference : 
		public member_instance_reference_base, 
		public datatype_instance_reference {
private:
	typedef	datatype_instance_reference		parent_type;
	typedef	member_instance_reference_base		interface_type;
protected:
// inherited:
//	excl_ptr<index_list>			array_indices;
//	const count_ptr<const simple_instance_reference>	base;
	const never_ptr<const datatype_instance_collection>	data_inst_ref;
private:
	datatype_member_instance_reference();
public:
	datatype_member_instance_reference(
		count_ptr<const simple_instance_reference> b, 
		never_ptr<const datatype_instance_collection> m);
	~datatype_member_instance_reference();

	ostream& what(ostream& o) const;
// can also attach indices!

	never_ptr<const instance_collection_base>
	get_inst_base(void) const;

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
private:
	typedef	channel_instance_reference		parent_type;
	typedef	member_instance_reference_base		interface_type;
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
// classes pint_instance_reference and pbool_instance_reference
//	are in "art_object_expr_param_ref.*"

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INST_REF_H__

