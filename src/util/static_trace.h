/**
	\file "util/static_trace.h"
	Utility class for identifiying global static initialization
	and destruction of modules, and debugging ordering...
	$Id: static_trace.h,v 1.7 2009/02/20 20:39:44 fang Exp $
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
#define DEFAULT_STATIC_TRACE		STATIC_TRACE_HERE(__FILE__ ":" STRINGIFY(__LINE__))
#define DEFAULT_STATIC_TRACE_END	STATIC_TRACE_END(__FILE__)

//=============================================================================
#if ENABLE_STATIC_TRACE

#include <string>
// #include "util/attributes.h"		// for __unused__
#include <ios>			// for std::ios_base::Init

namespace util {

//=============================================================================
/**
	Base class for static-debug objects.  
	Non-copy-able, non-assignable.  
 */
class static_common {
private:
	const std::ios_base::Init	ios_init;
protected:
	/// string that uniquely identifies this module
	const std::string		msg;

	explicit
	static_common(const std::string&);
	~static_common();

};	// end class static_common

//=============================================================================
/**
	Class intended for diagnosing when static initialization of a module
	(translation unit) begins, and when its destruction ends.  
 */
class static_begin : private static_common {
public:
	explicit
	static_begin(const std::string&);
	~static_begin();

};	// end class static_begin

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Class intended for diagnosing when static initialization of a module
	(translation unit) ends, and when its destruction begins.  
 */
class static_end : private static_common {
public:
	explicit
	static_end(const std::string&);
	~static_end();
};	// end class static_end

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Debugging placeholder for anywhere in translation unit.  
	Useful for marking initialization/destruction progress
	in the middle of a translation unit.  
 */
class static_here : private static_common {
public:
	explicit
	static_here(const std::string&);
	~static_here();
};	// end class static_here

//=============================================================================

}	// end namespace util
#endif	// ENABLE_STATIC_TRACE
//=============================================================================
// else don't even bother defining class

#endif	// __UTIL_STATIC_TRACE_H__

