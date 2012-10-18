/**
	\file "Object/object_fwd.hh"
	Forward declarations for all HAC::entity classes and typedefs.
	$Id: object_fwd.hh,v 1.8 2006/04/12 08:53:12 fang Exp $
	This file used to be:
	Id: art_object_fwd.h,v 1.18.20.1 2005/12/11 00:45:13 fang Exp
 */

#ifndef	__HAC_OBJECT_OBJECT_FWD_H__
#define	__HAC_OBJECT_OBJECT_FWD_H__

#include "Object/traits/class_traits_fwd.hh"
#include "Object/expr/types.hh"
#include "Object/type/canonical_type_fwd.hh"
#include "Object/ref/references_fwd.hh"
#include "Object/inst/collection_fwd.hh"
#include "Object/inst/instance_fwd.hh"
#include "Object/expr/expr_fwd.hh"
#include "Object/unroll/unroll_fwd.hh"
#include "Object/def/definition_fwd.hh"
#include "Object/type/type_fwd.hh"

namespace HAC {
namespace entity {

	class module;
	class object;
	class scopespace;
	class sequential_scope;
	class name_space;
	class instance_collection_stack_item;

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_OBJECT_FWD_H__

