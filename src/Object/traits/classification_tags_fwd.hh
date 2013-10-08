/**
	\file "Object/traits/classification_tags_fwd.hh"
	Forward declaration of meta-class type tags.  
	$Id: classification_tags_fwd.hh,v 1.6 2010/09/21 00:18:27 fang Exp $
 */

#ifndef	__HAC_OBJECT_TRAITS_CLASSIFICATION_TAGS_FWD_H__
#define	__HAC_OBJECT_TRAITS_CLASSIFICATION_TAGS_FWD_H__

namespace HAC {
namespace entity {

struct datatype_tag;
struct bool_tag;
struct int_tag;
struct enum_tag;
struct real_tag;
struct datastruct_tag;
struct string_tag;
struct node_tag;

struct physical_instance_tag;
struct channel_tag;
struct process_tag;

struct parameter_value_tag;
struct pint_tag;
struct pbool_tag;
struct preal_tag;
struct pstring_tag;

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TRAITS_CLASSIFICATION_TAGS_FWD_H__

