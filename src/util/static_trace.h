/**
	\file "static_trace.h"
	Utility class for identifiying global static initialization
	and destruction of modules, and debugging ordering...
	$Id: static_trace.h,v 1.1.2.1 2005/01/25 05:24:20 fang Exp $
 */

#ifndef	__UTIL_STATIC_TRACE_H__
#define	__UTIL_STATIC_TRACE_H__

#include <string>
using std::string;

#ifndef	ENABLE_STATIC_TRACE
#define	ENABLE_STATIC_TRACE		1
#endif

// Public Macros
#if ENABLE_STATIC_TRACE
	#define	STATIC_TRACE_BEGIN(m)					\
		static const util::static_begin __static_trace_begin__(m);
	#define	STATIC_TRACE_END(m)					\
		static const util::static_end __static_trace_end__(m);
#else
	#define	STATIC_TRACE_BEGIN(m)
	#define	STATIC_TRACE_END(m)
#endif

namespace util {

class static_begin {
private:
	const string	msg;
public:
	static_begin(const string&);
	~static_begin();

};	// end class static begin

class static_end {
private:
	const string	msg;
public:
	static_end(const string&);
	~static_end();
};	// end class static end

}	// end namespace util

#endif	// __UTIL_STATIC_TRACE_H__

