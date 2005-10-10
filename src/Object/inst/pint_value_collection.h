/**
	\file "Object/inst/pint_value_collection.h"
	Parameter instance collection classes for ART.  
	This file came from "Object/art_object_instance_param.h"
		in a previous life.  
	$Id: pint_value_collection.h,v 1.2.20.2 2005/10/10 22:13:50 fang Exp $
 */

#ifndef	__OBJECT_INST_PINT_VALUE_COLLECTION_H__
#define	__OBJECT_INST_PINT_VALUE_COLLECTION_H__

#include "Object/inst/param_value_collection.h"
#include "Object/inst/value_collection.h"
#include "Object/expr/pint_const.h"
#include "Object/traits/pint_traits.h"

namespace ART {
namespace entity {
struct pint_tag;
using util::memory::count_ptr;

template <class, size_t>
class value_array;

//=============================================================================
/**
	Run-time instance of integer parameter.  
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// convenient typedefs

typedef	value_array<pint_tag,0>	pint_scalar;
typedef	value_array<pint_tag,1>	pint_array_1D;
typedef	value_array<pint_tag,2>	pint_array_2D;
typedef	value_array<pint_tag,3>	pint_array_3D;
typedef	value_array<pint_tag,4>	pint_array_4D;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_PINT_VALUE_COLLECTION_H__

