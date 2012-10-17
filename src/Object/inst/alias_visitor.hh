/**
	\file "Object/inst/alias_visitor.hh"
	Visitor pattern that walks all aliases.  
	$Id: alias_visitor.hh,v 1.4 2009/02/28 01:20:41 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_ALIAS_VISITOR_H__
#define	__HAC_OBJECT_INST_ALIAS_VISITOR_H__

#include "Object/traits/classification_tags.hh"
#include "Object/devel_switches.hh"

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
#if ENABLE_DATASTRUCTS
virtual	VISIT_INSTANCE_ALIAS_INFO_PROTO(datastruct_tag) = 0;
#endif
virtual	VISIT_INSTANCE_ALIAS_INFO_PROTO(channel_tag) = 0;
virtual	VISIT_INSTANCE_ALIAS_INFO_PROTO(process_tag) = 0;


/**
	Convenient macro for declaring repetitive methods.  
 */
#if ENABLE_DATASTRUCTS
#define	VISIT_INSTANCE_ALIAS_INFO_PROTOS(virt)				\
virt	VISIT_INSTANCE_ALIAS_INFO_PROTO(bool_tag);			\
virt	VISIT_INSTANCE_ALIAS_INFO_PROTO(int_tag);			\
virt	VISIT_INSTANCE_ALIAS_INFO_PROTO(enum_tag);			\
virt	VISIT_INSTANCE_ALIAS_INFO_PROTO(datastruct_tag);		\
virt	VISIT_INSTANCE_ALIAS_INFO_PROTO(channel_tag);			\
virt	VISIT_INSTANCE_ALIAS_INFO_PROTO(process_tag);
#else
#define	VISIT_INSTANCE_ALIAS_INFO_PROTOS(virt)				\
virt	VISIT_INSTANCE_ALIAS_INFO_PROTO(bool_tag);			\
virt	VISIT_INSTANCE_ALIAS_INFO_PROTO(int_tag);			\
virt	VISIT_INSTANCE_ALIAS_INFO_PROTO(enum_tag);			\
virt	VISIT_INSTANCE_ALIAS_INFO_PROTO(channel_tag);			\
virt	VISIT_INSTANCE_ALIAS_INFO_PROTO(process_tag);
#endif
};	// end class alias_visitor

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_ALIAS_VISITOR_H__

