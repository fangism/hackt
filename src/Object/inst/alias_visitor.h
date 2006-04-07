/**
	\file "Object/inst/alias_visitor.h"
	Visitor pattern that walks all aliases.  
	$Id: alias_visitor.h,v 1.1.2.2 2006/04/07 22:54:29 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_ALIAS_VISITOR_H__
#define	__HAC_OBJECT_INST_ALIAS_VISITOR_H__

#include "Object/traits/classification_tags.h"

namespace HAC {
namespace entity {
class footprint;
class physical_instance_collection;
template <class Tag>
class instance_alias_info;

//=============================================================================
/**
	Visitor class for walking all hierarchical aliases.  
 */
class alias_visitor {
public:
virtual	~alias_visitor() { }

#define	VISIT_INSTANCE_ALIAS_INFO_PROTO(Tag)				\
	void								\
	visit(const instance_alias_info<Tag>&)

virtual	VISIT_INSTANCE_ALIAS_INFO_PROTO(bool_tag) = 0;
virtual	VISIT_INSTANCE_ALIAS_INFO_PROTO(int_tag) = 0;
virtual	VISIT_INSTANCE_ALIAS_INFO_PROTO(enum_tag) = 0;
virtual	VISIT_INSTANCE_ALIAS_INFO_PROTO(datastruct_tag) = 0;
virtual	VISIT_INSTANCE_ALIAS_INFO_PROTO(channel_tag) = 0;
virtual	VISIT_INSTANCE_ALIAS_INFO_PROTO(process_tag) = 0;

/**
	Convenient macro for declaring repetitive methods.  
 */
#define	VISIT_INSTANCE_ALIAS_INFO_PROTOS				\
	VISIT_INSTANCE_ALIAS_INFO_PROTO(bool_tag);			\
	VISIT_INSTANCE_ALIAS_INFO_PROTO(int_tag);			\
	VISIT_INSTANCE_ALIAS_INFO_PROTO(enum_tag);			\
	VISIT_INSTANCE_ALIAS_INFO_PROTO(datastruct_tag);		\
	VISIT_INSTANCE_ALIAS_INFO_PROTO(channel_tag);			\
	VISIT_INSTANCE_ALIAS_INFO_PROTO(process_tag);

};	// end class alias_visitor

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_ALIAS_VISITOR_H__

