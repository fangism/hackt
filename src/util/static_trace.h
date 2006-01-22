/**
	\file "util/static_trace.h"
	Utility class for identifiying global static initialization
	and destruction of modules, and debugging ordering...
	$Id: static_trace.h,v 1.5 2006/01/22 06:53:37 fang Exp $
 */

#ifndef	__UTIL_STATIC_TRACE_H__
#define	__UTIL_STATIC_TRACE_H__

#ifndef	ENABLE_STATIC_TRACE
#define	ENABLE_STATIC_TRACE		0
#endif

//=============================================================================
// Public Macros
#if ENABLE_STATIC_TRACE
#include "util/cppcat.h"
	/**
		Define this as the first static object in a module
		to trace initialization and destruction ordering.
		\param m any string.
	 */
	#define	STATIC_TRACE_BEGIN(m)					\
		static const util::static_begin __static_trace_begin__(m);
	/**
		Define this as the last static object in a module
		to trace initialization and destruction ordering.
		\param m any string, preferably the same one as the 
			beginning counterpart.
	 */
	#define	STATIC_TRACE_END(m)					\
		static const util::static_end __static_trace_end__(m);
	/**
		Macro for tracing intermediate points.  
	 */
	#define	STATIC_TRACE_HERE(m)					\
		static const util::static_here UNIQUIFY(__st_here)(m);
#else
	#define	STATIC_TRACE_BEGIN(m)
	#define	STATIC_TRACE_HERE(m)
	#define	STATIC_TRACE_END(m)
#endif

#define DEFAULT_STATIC_TRACE_BEGIN	STATIC_TRACE_BEGIN(__FILE__)
#define DEFAULT_STATIC_TRACE		STATIC_TRACE_HERE(__FILE__ # ":" # __LINE__)
#define DEFAULT_STATIC_TRACE_END	STATIC_TRACE_END(__FILE__)

//=============================================================================
#if ENABLE_STATIC_TRACE

#include <string>
// #include "util/attributes.h"		// for __unused__

namespace util {
using std::string;

//=============================================================================
/**
	Class intended for diagnosing when static initialization of a module
	(translation unit) begins, and when its destruction ends.  
 */
class static_begin {
private:
	/// string that uniquely identifies this module
	const string	msg;
public:
	explicit
	static_begin(const string&);
	~static_begin();

};	// end class static begin

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Class intended for diagnosing when static initialization of a module
	(translation unit) ends, and when its destruction begins.  
 */
class static_end {
private:
	/// string that uniquely identifies this module
	const string	msg;
public:
	explicit
	static_end(const string&);
	~static_end();
};	// end class static end

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class static_here {
private:
	/// string that uniquely identifies this module
	const string	msg;
public:
	explicit
	static_here(const string&);
	~static_here();
};	// end class static end

//=============================================================================

}	// end namespace util
#endif	// ENABLE_STATIC_TRACE
//=============================================================================
// else don't even bother defining class

#endif	// __UTIL_STATIC_TRACE_H__

