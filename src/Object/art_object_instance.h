/**
	\file "art_object_instance.h"
	Instance collection classes for ART.  
	$Id: art_object_instance.h,v 1.23 2004/12/07 02:22:09 fang Exp $
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
protected:
	// reserve these for connections between instance_references
	// list of template actuals
	// list of port actuals

private:
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
/// Instantiation of a data type, either inside or outside definition.  
class datatype_instance_collection : public instance_collection_base {
protected:
private:
	datatype_instance_collection();
public:
	datatype_instance_collection(const scopespace& o, 
		const string& n, const size_t d);
	~datatype_instance_collection();

	ostream& what(ostream& o) const;
	count_ptr<const fundamental_type_reference> get_type_ref(void) const;

// need to do this for real... using object not parse tree
//	bool equals_template_formal(const template_formal_decl& tf) const;
	// why is this never?
	count_ptr<instance_reference_base>
		make_instance_reference(void) const;
	count_ptr<member_instance_reference_base>
		make_member_instance_reference(
			count_ptr<const simple_instance_reference> b) const;
public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class datatype_instance_collection

//=============================================================================
/**
	Instantiation of a channel type.  
	Final class?
 */
class channel_instance_collection : public instance_collection_base {
protected:
private:
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

