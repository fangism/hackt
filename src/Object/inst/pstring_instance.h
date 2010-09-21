/**
	\file "Object/inst/pstring_instance.h"
	$Id: pstring_instance.h,v 1.2 2010/09/21 00:18:22 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PSTRING_INSTANCE_H__
#define	__HAC_OBJECT_INST_PSTRING_INSTANCE_H__

#include <cassert>
#include <iosfwd>
#include <string>
#include "Object/expr/types.h"
#include "util/boolean_types.h"

namespace HAC {
namespace entity {
struct pstring_tag;
using std::ostream;
using util::good_bool;

//=============================================================================
/**
	Run-time instance of stringeger parameter.  
	TODO: add a 'relaxed' poison bit to trace origin of value, 
		to avoid polluting strict template parameters with relaxed ones.
 */
struct pstring_instance {
public:
	typedef pstring_value_type		value_type;
public:
	/**
		The unroll-time value of this pstring parameter.
	 */
	value_type	value;
	/**
		Whether or not this instance was truly instantiated,
		Safeguards against extraneous instances in arrays.  
	 */
	bool		instantiated : 1;
	/**
		Whether or not value has been initialized exactly 
		once to a value.
	 */
	bool		valid : 1;
public:
	pstring_instance() : value(), instantiated(false), valid(false) { }

	explicit
	pstring_instance(const bool b) :
		value(), instantiated(b), valid(false) { }

	explicit
	pstring_instance(const value_type& v) :
		value(v), instantiated(true), valid(true) { }
	// default copy constructor
	// default destructor

	/**
		\return false on error, true on success.  
	 */
	good_bool
	operator = (const value_type& i) {
		assert(instantiated);
		if (valid)
			// error: already initialized
			// or allow multiple assignments with the same value?
			return good_bool(false);
		else {
			value = i;
			valid = true;
			return good_bool(true);
		}
	}

};	// end struct pstring_instance

bool
operator == (const pstring_instance& p, const pstring_instance& q);

ostream&
operator << (ostream& o, const pstring_instance& p);

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PSTRING_INSTANCE_H__

