/**
	\file "Object/inst/pbool_value_collection.h"
	Parameter instance collection classes for ART.  
	This file came from "Object/art_object_instance_param.h"
		in a previous life.  
	$Id: pbool_value_collection.h,v 1.3 2005/10/25 20:51:55 fang Exp $
 */

#ifndef	__OBJECT_INST_PBOOL_VALUE_COLLECTION_H__
#define	__OBJECT_INST_PBOOL_VALUE_COLLECTION_H__

#include "Object/inst/param_value_collection.h"
#include "Object/inst/value_collection.h"
#include "Object/expr/pbool_const.h"
#include "Object/traits/pbool_traits.h"

namespace ART {
namespace entity {
struct pbool_tag;
using util::memory::count_ptr;

template <class, size_t>
class value_array;

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// convenient typedefs

typedef	value_array<pbool_tag,0>	pbool_scalar;
typedef	value_array<pbool_tag,1>	pbool_array_1D;
typedef	value_array<pbool_tag,2>	pbool_array_2D;
typedef	value_array<pbool_tag,3>	pbool_array_3D;
typedef	value_array<pbool_tag,4>	pbool_array_4D;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_PBOOL_VALUE_COLLECTION_H__

