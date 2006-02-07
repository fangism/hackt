/**
	\file "Object/inst/preal_value_collection.h"
	Parameter instance collection classes for HAC.  
	$Id: preal_value_collection.h,v 1.1.2.1 2006/02/07 02:57:57 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PREAL_VALUE_COLLECTION_H__
#define	__HAC_OBJECT_INST_PREAL_VALUE_COLLECTION_H__

#include "Object/inst/param_value_collection.h"
#include "Object/inst/value_collection.h"
#include "Object/expr/preal_const.h"
#include "Object/traits/preal_traits.h"

namespace HAC {
namespace entity {
struct preal_tag;
using util::memory::count_ptr;

template <class, size_t>
class value_array;

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// convenient typedefs

typedef	value_array<preal_tag,0>	preal_scalar;
typedef	value_array<preal_tag,1>	preal_array_1D;
typedef	value_array<preal_tag,2>	preal_array_2D;
typedef	value_array<preal_tag,3>	preal_array_3D;
typedef	value_array<preal_tag,4>	preal_array_4D;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PREAL_VALUE_COLLECTION_H__

