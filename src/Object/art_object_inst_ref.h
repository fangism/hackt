/**
	\file "art_object_inst_ref.h"
	Class family for instance references in ART.  
	$Id: art_object_inst_ref.h,v 1.15.16.1.10.1.2.1 2005/02/19 06:56:49 fang Exp $
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

protected:
	datatype_instance_reference();

	explicit
	datatype_instance_reference(const instantiation_state& s);

#if 0
	// may be obsolete...
	datatype_instance_reference(excl_ptr<index_list>& i, 
		const instantiation_state& s);
#endif

public:
virtual	~datatype_instance_reference();

virtual	ostream&
	what(ostream& o) const = 0;

//	ostream& dump(ostream& o) const;

virtual	never_ptr<const instance_collection_base>
	get_inst_base(void) const = 0;

private:
virtual	excl_ptr<aliases_connection_base>
	make_aliases_connection_private(void) const = 0;

protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
public:
};	// end class datatype_instance_reference

//-----------------------------------------------------------------------------
/**
	A reference to a simple instance of channel.  
 */
class channel_instance_reference : public simple_instance_reference {
private:
	typedef	simple_instance_reference		parent_type;
public:
	typedef	channel_instance_collection	instance_collection_type;
	typedef	channel_alias_connection	alias_connection_type;
protected:
//	excl_ptr<index_list>			array_indices;	// inherited
	const never_ptr<const channel_instance_collection>
							channel_inst_ref;

protected:
	channel_instance_reference();
public:
	explicit
	channel_instance_reference(
		const never_ptr<const channel_instance_collection> ci);

#if 0
	channel_instance_reference(
		const never_ptr<const channel_instance_collection> ci, 
		excl_ptr<index_list>& i);
#endif

virtual	~channel_instance_reference();

virtual	ostream&
	what(ostream& o) const;

//	ostream& dump(ostream& o) const;

	never_ptr<const instance_collection_base>
	get_inst_base(void) const;

protected:
	excl_ptr<aliases_connection_base>
	make_aliases_connection_private(void) const;

protected:
	void
	write_object_base(const persistent_object_manager& m, ostream&) const;

	void
	load_object_base(const persistent_object_manager& m, istream&);

public:
	// need to be virtual? for member_instance_reference?
	PERSISTENT_METHODS_DECLARATIONS
};	// end class channel_instance_reference

//-----------------------------------------------------------------------------
/**
	A reference to a simple instance of process.  
 */
class process_instance_reference : public simple_instance_reference {
private:
	typedef	simple_instance_reference		parent_type;
public:
	typedef	process_instance_collection	instance_collection_type;
	typedef	process_alias_connection	alias_connection_type;
protected:
//	excl_ptr<index_list>			array_indices;	// inherited
	const never_ptr<const process_instance_collection>
							process_inst_ref;

protected:
	process_instance_reference();
public:
	explicit
	process_instance_reference(
		const never_ptr<const process_instance_collection> pi);

#if 0
	process_instance_reference(
		const never_ptr<const process_instance_collection> pi, 
		excl_ptr<index_list>& i);
#endif

virtual	~process_instance_reference();

virtual	ostream&
	what(ostream& o) const;

	never_ptr<const instance_collection_base>
	get_inst_base(void) const;

protected:
	excl_ptr<aliases_connection_base>
	make_aliases_connection_private(void) const;

protected:
	void
	write_object_base(const persistent_object_manager& m, ostream&) const;

	void
	load_object_base(const persistent_object_manager& m, istream&);

public:
	// need to be virtual? for member_instance_reference?
	PERSISTENT_METHODS_DECLARATIONS
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
public:
	typedef	parent_type::alias_connection_type	alias_connection_type;
protected:
// inherited:
//	excl_ptr<index_list>			array_indices;
//	const never_ptr<const process_instance_collection>	process_inst_ref;
//	const count_ptr<const simple_instance_reference>	base;
private:
	process_member_instance_reference();
public:
	process_member_instance_reference(
		const count_ptr<const simple_instance_reference>& b, 
		const never_ptr<const process_instance_collection> m);

	~process_member_instance_reference();

	ostream&
	what(ostream& o) const;
// can also attach indices!

private:
#if 0
	excl_ptr<aliases_connection_base>
	make_aliases_connection_private(void) const;
#else
	using parent_type::make_aliases_connection_private;
#endif

public:
	PERSISTENT_METHODS_DECLARATIONS
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
		const count_ptr<const simple_instance_reference>& b, 
		const never_ptr<const datatype_instance_collection> m);

	~datatype_member_instance_reference();

	ostream&
	what(ostream& o) const;
// can also attach indices!

	never_ptr<const instance_collection_base>
	get_inst_base(void) const;

private:
	excl_ptr<aliases_connection_base>
	make_aliases_connection_private(void) const;

public:
	PERSISTENT_METHODS_DECLARATIONS

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
public:
	typedef	parent_type::alias_connection_type	alias_connection_type;
protected:
// inherited:
//	excl_ptr<index_list>			array_indices;
//	const never_ptr<const channel_instance_collection>	channel_inst_ref;
//	const count_ptr<const simple_instance_reference>	base;
private:
	channel_member_instance_reference();
public:
	channel_member_instance_reference(
		const count_ptr<const simple_instance_reference>& b, 
		const never_ptr<const channel_instance_collection> m);

	~channel_member_instance_reference();

	ostream&
	what(ostream& o) const;
// can also attach indices!
private:
#if 0
	excl_ptr<aliases_connection_base>
	make_aliases_connection_private(void) const;
#else
	using parent_type::make_aliases_connection_private;
#endif

public:
	PERSISTENT_METHODS_DECLARATIONS

};	// end class channel_member_instance_reference

//=============================================================================
// classes pint_instance_reference and pbool_instance_reference
//	are in "art_object_expr_param_ref.*"

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INST_REF_H__

