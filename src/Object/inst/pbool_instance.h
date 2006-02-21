/**
	\file "Object/inst/pbool_instance.h"
	$Id: pbool_instance.h,v 1.2 2006/02/21 04:48:30 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PBOOL_INSTANCE_H__
#define	__HAC_OBJECT_INST_PBOOL_INSTANCE_H__

#include <cassert>
#include <iosfwd>
#include "Object/expr/types.h"
#include "util/boolean_types.h"

namespace HAC {
namespace entity {
struct pbool_tag;
using std::ostream;
using util::good_bool;

//=============================================================================
/**
	A run-time instance of a boolean parameter. 
 */
struct pbool_instance {
public:
	typedef	pbool_value_type		value_type;
public:
	/**
		The unroll-time value of this pbool parameter.
	 */
	value_type	value : 1;
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
	pbool_instance() : value(false), instantiated(false), valid(false) { }

	explicit
	pbool_instance(const value_type b) :
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

};	// end struct pbool_instance

bool
operator == (const pbool_instance& p, const pbool_instance& q);

ostream&
operator << (ostream& o, const pbool_instance& p);

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PBOOL_INSTANCE_H__

