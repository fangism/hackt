/**
	\file "Object/unroll/instance_management_base.h"
	Base class for any sequential instantiation or manupulation.  
	This file came from "Object/art_object_instance_management_base.h"
		in prehistoric revisions.  
	$Id: instance_management_base.h,v 1.6 2006/01/22 18:20:56 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_INSTANCE_MANAGEMENT_BASE_H__
#define	__HAC_OBJECT_UNROLL_INSTANCE_MANAGEMENT_BASE_H__

#include "util/persistent.h"
#include "util/boolean_types.h"

namespace HAC {
namespace entity {
using std::ostream;
using util::persistent;
using util::good_bool;
class unroll_context;
class footprint;

//=============================================================================
/**
	Abstract base class for sequential instantiation management objects, 
	including instantiations, parameters, assignments, connections.  
 */
class instance_management_base : virtual public persistent {
protected:
	// none
public:
	/**
		Helper functor for adding a dereference before dumping, 
		since the majority of objects are pointer-classed.
		Consider using this in object as well.  
	 */
	class dumper {
	private:
		ostream& os;
	public:
		explicit
		dumper(ostream& o);

		template <template <class> class P>
		ostream&
		operator () (const P<const instance_management_base>& i) const;
	};      // end class dumper

public:
virtual ostream&
	dump(ostream& o) const = 0;

#define	UNROLL_META_EVALUATE_PROTO					\
	good_bool							\
	unroll_meta_evaluate(const unroll_context& ) const

#define	UNROLL_META_INSTANTIATE_PROTO					\
	good_bool							\
	unroll_meta_instantiate(const unroll_context& ) const

#define	UNROLL_META_CONNECT_PROTO					\
	good_bool							\
	unroll_meta_connect(const unroll_context& ) const

	// need pure virtual unrolling methods
	// argument should contain some stack of expression values
	// possible single-pass unroll may be phased out...
virtual good_bool
	unroll(const unroll_context& ) const = 0;

virtual	UNROLL_META_EVALUATE_PROTO;
virtual	UNROLL_META_INSTANTIATE_PROTO;
virtual	UNROLL_META_CONNECT_PROTO;

/**
	Prototype for unique-creation pass.  
 */
#define	CREATE_UNIQUE_PROTO						\
	good_bool							\
	create_unique(const unroll_context&, footprint&) const

virtual	CREATE_UNIQUE_PROTO;

};	// end class instance_management_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_INSTANCE_MANAGEMENT_BASE_H__

