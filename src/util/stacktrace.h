/**
	\file "stacktrace.h"
	Utility macros and header for convenient stack-trace debugging.
	$Id: stacktrace.h,v 1.4 2005/01/15 06:03:03 fang Exp $
 */

#ifndef	__STACK_TRACE_H__
#define	__STACK_TRACE_H__

// macros for enabling/disabling stacktrace code
/**
	Strongly recommend using these macros to be able to 
	turn everything off at compile time.  
	Predefine this to 0 at compile time to turn-off.  
 */
#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE	0	// on (1) or off (0) by default
#endif


// This is the macro interface intended for the programmer.  
#if ENABLE_STACKTRACE
	#define	USING_STACKTRACE	using util::stacktrace;
	#define	STACKTRACE(str)	stacktrace __stacktrace__(str)
	/**
		This enables echoing each time trace stack is updated, i.e., 
		upon entering and leaving function call stack 
		or lexical scopes.  
	 */
	#define STACKTRACE_ECHO_ON					\
			stacktrace::echo __echo_stacktrace__(1)
	#define STACKTRACE_ECHO_OFF					\
			stacktrace::echo __echo_stacktrace__(0)
	#define	STACKTRACE_STREAM					\
			stacktrace::stream()
	#define REDIRECT_STACKTRACE(os)					\
			stacktrace::redirect __redir_stacktrace__(os)
	#define	ASSERT_STACKTRACE(expr)					\
			if (!(expr)) { stacktrace::full_dump(); assert(expr); }
#else
	#define	USING_STACKTRACE
	#define	STACKTRACE(str)
	#define STACKTRACE_ECHO_ON
	#define STACKTRACE_ECHO_OFF
	#define	STACKTRACE_STREAM		std::cerr
	#define REDIRECT_STACKTRACE(os)
	#define	ASSERT_STACKTRACE(expr)		assert(expr)
#endif


//=============================================================================

#if ENABLE_STACKTRACE

#include <iosfwd>
// #include <stack>
#include "macros.h"
#include "STL/list_fwd.h"
#include "string_fwd.h"
// #include "memory/pointer_classes_fwd.h"



namespace util {
USING_LIST
using std::ostream;
using std::string;
// using std::stack;
// using memory::excl_ptr;

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
public:
	class manager;
	struct echo;
	struct redirect;
	// static objects?
private:
#if 0
// not really used
	/// bare-naked pointer, guaranteed to be 0 before static initialization
	static manager*			the_manager;

	/// pointer class to manage static deallocation upon termination
	static excl_ptr<manager>	the_manager_manager;

	/// interface accessor to the stack manager
	static
	manager&
	get_the_manager(void);
#endif
public:
//	stacktrace(const char*);
	stacktrace(const string&);
	~stacktrace();
public:
	static
	ostream&
	stream(void);

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
struct stacktrace::echo {
	echo(const int i = 1);
	~echo();
};

//-----------------------------------------------------------------------------
/**
	Redirect all stack dumps to this ostream until changed otherwise.
	Lasts for the duration of the scope where this is called.  
 */
struct stacktrace::redirect {
	redirect(ostream&);
	~redirect();
};

//=============================================================================

}	// end namespace util

#else
	// don't even bother processing class declaration!
#endif	// ENABLE_STACKTRACE

#endif	// __STACK_TRACE_H__

