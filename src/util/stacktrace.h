/**
	\file "stacktrace.h"
	Utility macros and header for convenient stack-trace debugging.
	$Id: stacktrace.h,v 1.1 2004/12/06 07:13:02 fang Exp $
 */

#ifndef	__STACK_TRACE_H__
#define	__STACK_TRACE_H__

#include <iosfwd>
#include <stack>
#include "macros.h"
#include "STL/list_fwd.h"
#include "string_fwd.h"

// macros for enabling/disabling stacktrace code

/**
	Strongly recommend using these macros to be able to 
	turn everything off at compile time.  
	Predefine this to 0 at compile time to turn-off.  
 */
#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE	1	// on or off by default
#endif


// You put the semicolon at the end!
#if ENABLE_STACKTRACE
	#define	STACKTRACE(str)	stacktrace __stacktrace__(str)
	#define ENABLE_TRACE	enable_stacktrace __enable_stacktrace__(1)
	#define DISABLE_TRACE	enable_stacktrace __enable_stacktrace__(0)
	#define REDIRECT_TRACE	redirect_stacktrace __redir_stacktrace__
	#define	ASSERT_STACKTRACE(expr)				\
		if (!(expr)) { stacktrace::full_dump(); assert(expr); }
#else
	#define	STACKTRACE(str)
	#define ENABLE_TRACE
	#define DISABLE_TRACE
	#define REDIRECT_TRACE
	#define	ASSERT_STACKTRACE(expr)		assert(expr)
#endif

namespace util {
USING_LIST
using std::ostream;
using std::string;
using std::stack;
//=============================================================================
/**
	Nothing is inlined because when you're debugging you shouldn't be
	varing about performance.  
	Can use for function scopes or any conditional statement scopes.  
	Two mechanisms for controlling behavior, 
	compile-time macros and public static variables.  
	TODO: make one entry per thread, mapped by thread ID.
 */
class stacktrace {
private:
	// static objects?
public:
	stacktrace(const char*);
	stacktrace(const string&);
	~stacktrace();
public:
	/**
		Explicit request by user to dump the stack trace.
		Useful in assertion failures.  
	 */
	static
	void
	full_dump(void);
};	// end class stacktrace

//-----------------------------------------------------------------------------
/**
	Whether or not to print upon entering and exiting.
	Pass in 0 to disable.  
	Enabling/disable lasts for the duration of the scope.  
 */
struct enable_stacktrace {
	enable_stacktrace(const int i = 1);
	~enable_stacktrace();
};

//-----------------------------------------------------------------------------
/**
	Redirect all stack dumps to this ostream until changed otherwise.
	Lasts for the duration of the scope where this is called.  
 */
struct redirect_stacktrace {
	redirect_stacktrace(ostream&);
	~redirect_stacktrace();
};

//=============================================================================

}	// end namespace util

#endif	// __STACK_TRACE_H__

