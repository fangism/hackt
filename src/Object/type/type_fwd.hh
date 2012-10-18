/**
	\file "Object/type/type_fwd.hh"
	Forward declarations for all type_reference-related classes. 
	$Id: type_fwd.hh,v 1.2 2006/04/12 08:53:19 fang Exp $
 */

#ifndef	__HAC_OBJECT_TYPE_TYPE_FWD_H__
#define	__HAC_OBJECT_TYPE_TYPE_FWD_H__

namespace HAC {
namespace entity {

class type_reference_base;
template <class> class canonical_type; // "Object/type/canonical_type.hh"
class fundamental_type_reference;
class channel_type_reference_base;
class builtin_channel_type_reference;
class channel_type_reference;
class data_type_reference;
class process_type_reference;
class param_type_reference;
class pbool_type_reference;
class pint_type_reference;
class preal_type_reference;

}	// end namespace entity
}	// end namespace HAC


#endif	// __HAC_OBJECT_TYPE_TYPE_FWD_H__

