/**
	\file "art_object_instance.h"
	Instance collection classes for ART.  
	$Id: art_object_instance.h,v 1.25 2004/12/10 23:18:08 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_H__
#define	__ART_OBJECT_INSTANCE_H__

#include "art_object_instance_base.h"
#include "memory/pointer_classes.h"

#if 0
// will need these later...
#include "multikey_fwd.h"
#include "multikey_qmap_fwd.h"
#endif

namespace ART {
namespace entity {
//=============================================================================
USING_LIST
using namespace util::memory;	// for experimental pointer classes
#if 0
using namespace MULTIKEY_NAMESPACE;
using namespace MULTIKEY_MAP_NAMESPACE;
#endif

//=============================================================================
// class instance_collection_base declared in "art_object_instance_base.h"

//=============================================================================
/**
	Process instantiation.  
	Type information is now in the instance_collection_list.
 */
class process_instance_collection : public instance_collection_base {
private:
	typedef	instance_collection_base	parent_type;
protected:
	// reserve these for connections between instance_references
	// list of template actuals
	// list of port actuals

protected:
	process_instance_collection();
public:
	process_instance_collection(const scopespace& o, 
		const string& n, const size_t d);
	~process_instance_collection();

	ostream& what(ostream& o) const;
//	ostream& dump(ostream& o) const;
	count_ptr<const fundamental_type_reference> get_type_ref(void) const;
	// why is this a never_ptr?
	count_ptr<instance_reference_base>
		make_instance_reference(void) const;
	count_ptr<member_instance_reference_base>
		make_member_instance_reference(
			count_ptr<const simple_instance_reference> b) const;

public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS

};	// end class process_instance_collection

//=============================================================================
/**
	Base class for instantiation of a data type, 
	either inside or outside definition.  
 */
class datatype_instance_collection : public instance_collection_base {
private:
	typedef	instance_collection_base	parent_type;
protected:
	datatype_instance_collection();
public:
	datatype_instance_collection(const scopespace& o, 
		const string& n, const size_t d);

virtual	~datatype_instance_collection();

virtual	ostream&
	what(ostream& o) const = 0;

	/** returns the type of the first instantiation statement */
	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;

virtual bool
	is_partially_unrolled(void) const = 0;

virtual ostream&
	dump_unrolled_instances(ostream& o) const = 0;

// methods for connection and aliasing?

// need to do this for real... using object not parse tree
//	bool equals_template_formal(const template_formal_decl& tf) const;
virtual	count_ptr<instance_reference_base>
	make_instance_reference(void) const = 0;

	count_ptr<member_instance_reference_base>
	make_member_instance_reference(
		count_ptr<const simple_instance_reference> b) const;

protected:	// propagate to children
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class datatype_instance_collection

//=============================================================================
/**
	Instantiation of a channel type.  
	Final class?
 */
class channel_instance_collection : public instance_collection_base {
private:
	typedef	instance_collection_base	parent_type;
protected:
	channel_instance_collection();
public:
	channel_instance_collection(const scopespace& o, 
		const string& n, const size_t d);
	~channel_instance_collection();

	ostream& what(ostream& o) const;
	count_ptr<const fundamental_type_reference> get_type_ref(void) const;
	count_ptr<instance_reference_base>
		make_instance_reference(void) const;
	count_ptr<member_instance_reference_base>
		make_member_instance_reference(
			count_ptr<const simple_instance_reference> b) const;
public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class channel_instance_collection

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_H__

