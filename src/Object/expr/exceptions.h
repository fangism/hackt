/**
	\file "Object/expr/exceptions.h"
	Various exceptions classes thrown by expression methods and functions.  
	For more robust error handling.  
	$Id: exceptions.h,v 1.2 2007/02/26 22:00:46 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_EXCEPTIONS_H__
#define	__HAC_OBJECT_EXPR_EXCEPTIONS_H__

#include "Object/expr/exceptions_fwd.h"
// #include <exception>		// no real need
// #include <stdexcept>		// no real need

namespace HAC {
namespace entity {
//=============================================================================
/**
	Base class for all compile-time exceptions.  
	Possibly derive from a common compiler exception class.  
 */
class meta_expr_exception {
protected:
	meta_expr_exception() throw() { }
virtual	~meta_expr_exception() throw();

};	// end class meta_expr_exception

//=============================================================================
/**
	Base class for all run-time exceptions.  
 */
class nonmeta_expr_exception {
	// string for error message?
public:
	nonmeta_expr_exception() throw() { }
virtual ~nonmeta_expr_exception() throw();

// virtual	const char* what() const throw();
};	// end class nonmeta_exception

//-----------------------------------------------------------------------------
/**
	This is thrown when an run-time value index is found to be 
	out-of-range of a collection (nonexistent member).  
	TODO: use these.
 */
class nonmeta_expr_out_of_range : public nonmeta_expr_exception {
public:
};	// end class nonmeta_expr_out_of_range

//-----------------------------------------------------------------------------
/**
	This is thrown when evaluating a value probe expression, 
	when the probe is false.  This means the use of value probes
	should be explicitly guarded by channel probes.  
	Q: how to we value-probe channels with multiple fields?
 */
class nonmeta_expr_false_probe : public nonmeta_expr_exception {
public:
};	// end class nonmet_expr_false_probe

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_EXCEPTIONS_H__

