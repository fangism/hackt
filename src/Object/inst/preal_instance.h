/**
	\file "Object/inst/preal_instance.h"
	$Id: preal_instance.h,v 1.2 2006/02/21 04:48:31 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PREAL_INSTANCE_H__
#define	__HAC_OBJECT_INST_PREAL_INSTANCE_H__

#include <cassert>
#include <iosfwd>
#include "Object/expr/types.h"
#include "util/boolean_types.h"

namespace HAC {
namespace entity {
struct preal_tag;
using std::ostream;
using util::good_bool;

//=============================================================================
/**
	A run-time instance of a real-valued parameter. 
 */
struct preal_instance {
public:
	typedef	preal_value_type		value_type;
public:
	/**
		The unroll-time value of this preal parameter.
	 */
	value_type	value;
	/**
		Whether or not this instance was truly instantiated,
		Safeguards against extraneous instances in arrays.  
	 */
	bool		instantiated : 1;
	/**	Whether or not value has been initialized exactly 
		once to a value
	 */
	bool		valid : 1;
public:
	preal_instance() : value(false), instantiated(false), valid(false) { }

	explicit
	preal_instance(const value_type b) :
		value(b), instantiated(true), valid(false) { }
	// default copy constructor
	// default destructor

	/**
		\return false on error, true on success.  
	 */
	good_bool
	operator = (const value_type b) {
		assert(instantiated);
		if (valid)
			// error: already initialized
			// or allow multiple assignments with the same value?
			return good_bool(false);
		else {
			value = b;
			valid = true;
			return good_bool(true);
		}
	}

};	// end struct preal_instance

bool
operator == (const preal_instance& p, const preal_instance& q);

ostream&
operator << (ostream& o, const preal_instance& p);

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PREAL_INSTANCE_H__

