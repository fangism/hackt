/**
	\file "Object/unroll/instance_management_base.hh"
	Base class for any sequential instantiation or manupulation.  
	This file came from "Object/art_object_instance_management_base.h"
		in prehistoric revisions.  
	$Id: instance_management_base.hh,v 1.10 2006/04/18 18:42:41 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_INSTANCE_MANAGEMENT_BASE_H__
#define	__HAC_OBJECT_UNROLL_INSTANCE_MANAGEMENT_BASE_H__

#include "util/persistent.hh"
#include "util/boolean_types.hh"

namespace HAC {
namespace entity {
using std::ostream;
using util::persistent;
using util::good_bool;
class unroll_context;
class footprint;
struct expr_dump_context;

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
		ostream&			os;
		const expr_dump_context&	edc;
	public:
		dumper(ostream&, const expr_dump_context&);

		template <class P>
		ostream&
		operator () (const P& i) const;
	};      // end class dumper

public:
virtual ostream&
	dump(ostream&, const expr_dump_context&) const = 0;

	// need pure virtual unrolling methods
	// argument should contain some stack of expression values
	// possible single-pass unroll may be phased out...
virtual good_bool
	unroll(const unroll_context& ) const = 0;

};	// end class instance_management_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_INSTANCE_MANAGEMENT_BASE_H__

