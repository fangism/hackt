/**
	\file "Object/object_fwd.h"
	Forward declarations for all HAC::entity classes and typedefs.
	$Id: object_fwd.h,v 1.6.6.1 2006/04/10 23:21:24 fang Exp $
	This file used to be:
	Id: art_object_fwd.h,v 1.18.20.1 2005/12/11 00:45:13 fang Exp
 */

#ifndef	__HAC_OBJECT_OBJECT_FWD_H__
#define	__HAC_OBJECT_OBJECT_FWD_H__

#include "Object/traits/class_traits_fwd.h"
#include "Object/expr/types.h"
#include "Object/type/canonical_type_fwd.h"
#include "Object/ref/references_fwd.h"
#include "Object/inst/collection_fwd.h"
#include "Object/inst/instance_fwd.h"
#include "Object/expr/expr_fwd.h"
#include "Object/unroll/unroll_fwd.h"
#include "Object/def/definition_fwd.h"
#include "Object/type/type_fwd.h"

namespace HAC {
namespace entity {

// TODO: organize into groups by where full declarations are found

	class module;
	class object;
	class scopespace;
	class sequential_scope;
	class name_space;
	class instance_collection_stack_item;

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_OBJECT_FWD_H__

