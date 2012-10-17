/**
	\file "Object/inst/pint_instance.hh"
	$Id: pint_instance.hh,v 1.3 2006/10/18 01:19:39 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PINT_INSTANCE_H__
#define	__HAC_OBJECT_INST_PINT_INSTANCE_H__

#include <cassert>
#include <iosfwd>
#include "Object/expr/types.hh"
#include "util/boolean_types.hh"

namespace HAC {
namespace entity {
struct pint_tag;
using std::ostream;
using util::good_bool;

//=============================================================================
/**
	Run-time instance of integer parameter.  
	TODO: add a 'relaxed' poison bit to trace origin of value, 
		to avoid polluting strict template parameters with relaxed ones.
 */
struct pint_instance {
public:
	typedef pint_value_type		value_type;
public:
	/**
		The unroll-time value of this pint parameter.
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
	pint_instance() : value(-1), instantiated(false), valid(false) { }

	explicit
	pint_instance(const bool b) :
		value(-1), instantiated(b), valid(false) { }

	explicit
	pint_instance(const value_type v) :
		value(v), instantiated(true), valid(true) { }
	// default copy constructor
	// default destructor

	/**
		\return false on error, true on success.  
	 */
	good_bool
	operator = (const value_type i) {
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

};	// end struct pint_instance

bool
operator == (const pint_instance& p, const pint_instance& q);

ostream&
operator << (ostream& o, const pint_instance& p);

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PINT_INSTANCE_H__

