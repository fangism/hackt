/**
	\file "util/stacktrace.h"
	Utility macros and header for convenient stack-trace debugging.
	$Id: stacktrace.h,v 1.7 2005/05/10 04:51:30 fang Exp $
 */

#ifndef	__UTIL_STACKTRACE_H__
#define	__UTIL_STACKTRACE_H__

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
	#define	STACKTRACE(str)	util::stacktrace __stacktrace__(str)
	/**
		No user-supplied string required, uses __PRETTY_FUNCTION__
		built-in internal string.  Is this gcc-only?
		There's always __func__ for brevity.
	 */
	#define	STACKTRACE_BRIEF					\
			util::stacktrace __stacktrace__(__func__)
	#define	STACKTRACE_VERBOSE					\
			util::stacktrace __stacktrace__(__PRETTY_FUNCTION__)
	/**
		This enables echoing each time trace stack is updated, i.e., 
		upon entering and leaving function call stack 
		or lexical scopes.  
	 */
	#define STACKTRACE_ECHO_ON					\
			util::stacktrace::echo __echo_stacktrace__(1)
	#define STACKTRACE_ECHO_OFF					\
			util::stacktrace::echo __echo_stacktrace__(0)
	#define	STACKTRACE_STREAM					\
			util::stacktrace::stream()
	#define REDIRECT_STACKTRACE(os)					\
			util::stacktrace::redirect __redir_stacktrace__(os)
	#define	ASSERT_STACKTRACE(expr)					\
			if (!(expr)) { util::stacktrace::full_dump(); assert(expr); }
	#define	REQUIRES_STACKTRACE_STATIC_INIT				\
			static const util::stacktrace::init_token		\
			__stacktrace_init__(util::stacktrace::require_static_init());
#else
	#define	USING_STACKTRACE
	#define	STACKTRACE(str)
	#define	STACKTRACE_BRIEF
	#define	STACKTRACE_VERBOSE
	#define STACKTRACE_ECHO_ON
	#define STACKTRACE_ECHO_OFF
	#define	STACKTRACE_STREAM		std::cerr
	#define REDIRECT_STACKTRACE(os)
	#define	ASSERT_STACKTRACE(expr)		assert(expr)
	#define	REQUIRES_STACKTRACE_STATIC_INIT	
#endif


//=============================================================================

#if ENABLE_STACKTRACE

#include <iosfwd>
#include "util/macros.h"
#include "util/string_fwd.h"

#include "util/STL/list_fwd.h"
#include "util/STL/stack_fwd.h"
#include <stack>

// need count pointer to be able to guarantee proper initialization
// across modules during static construction debugging
#include "util/memory/count_ptr.h"


namespace util {
USING_LIST
using std::ostream;
using std::string;
using memory::count_ptr;

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
	/// the type of stack used to hold feedback text
	typedef std::list<string>	stack_text_type;
	/// the type of stack used to track on/off mode
	typedef DEFAULT_STACK(int)	stack_echo_type;
	/// the type of stack used to track stream redirections
	typedef DEFAULT_STACK(ostream*)	stack_streams_type;

public:
	class manager;
	struct echo;
	struct redirect;

	class init_token;

private:
//	const string	local_str;	///< deep copy of string argument

public:
//	stacktrace(const char*);
	stacktrace(const string&);
	~stacktrace();
public:
	static
	ostream&
	stream(void);

	static
	init_token&
	require_static_init(void);
	
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
	Initializer token is requires for all modules that want to 
	use stacktrace for debugging during static construction and 
	destruction of the program.  
	Reference counting guarantees that the last module to need
	stacktrace's servives will take responsibility for freeing
	its resources.  
 */
class stacktrace::init_token {
	friend class stacktrace;
private:
	const count_ptr<const stack_text_type>		stack_text_ref;
	const count_ptr<const stack_text_type>		stack_indent_ref;
	const count_ptr<const stack_echo_type>		stack_echo_ref;
	const count_ptr<const stack_streams_type>	stack_streams_ref;
public:
	init_token();

	// default copy constructor suffices

	~init_token();

	void
	check(void) const;
};	// end class init_token

//-----------------------------------------------------------------------------
/**
	Whether or not to print upon entering and exiting.
	Pass in 0 to disable.  
	Enabling/disable lasts for the duration of the scope.  
 */
struct stacktrace::echo {
	echo(const int i = 1);
	~echo();
};	// end struct echo

//-----------------------------------------------------------------------------
/**
	Redirect all stack dumps to this ostream until changed otherwise.
	Lasts for the duration of the scope where this is called.  
 */
struct stacktrace::redirect {
	redirect(ostream&);
	~redirect();
};	// end struct redirect

//=============================================================================

}	// end namespace util

#else
	// don't even bother processing class declaration!
#endif	// ENABLE_STACKTRACE

#endif	// __UTIL_STACKTRACE_H__

