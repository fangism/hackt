/**
	\file "util/stacktrace.h"
	Utility macros and header for convenient stack-trace debugging.
	$Id: stacktrace.h,v 1.12.22.1 2006/01/12 21:31:50 fang Exp $
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


#if defined(__cplusplus) && defined(HAVE_CASSERT) && HAVE_CASSERT
#include <cassert>
#else
#include <assert.h>
#endif

//=============================================================================
// This is the macro interface intended for the programmer.  
#if ENABLE_STACKTRACE
#include "util/cppcat.h"		// for the UNIQUIFY macros
#include "util/attributes.h"

#define	USING_STACKTRACE	using util::stacktrace;
/**
	The macro that keeps track of scope and call stacks.  
	\param str the string to be printed.  
 */
#define	STACKTRACE(str)							\
	const util::stacktrace UNIQUIFY(__stacktrace_) (str)
/**
	No user-supplied string required, uses __PRETTY_FUNCTION__
	built-in internal string.  Is this gcc-only?
	There's always __func__ for brevity.
 */
#define	STACKTRACE_BRIEF						\
	const util::stacktrace UNIQUIFY(__stacktrace_) (__func__)
#define	STACKTRACE_VERBOSE						\
	const util::stacktrace UNIQUIFY(__stacktrace__) (__PRETTY_FUNCTION__)
/**
	This enables echoing each time trace stack is updated, i.e., 
	upon entering and leaving function call stack 
	or lexical scopes.  
 */
#define STACKTRACE_ECHO_ON						\
	const util::stacktrace::echo UNIQUIFY(__echo_stacktrace__) (1)
#define STACKTRACE_ECHO_OFF						\
	const util::stacktrace::echo UNIQUIFY(__echo_stacktrace__) (0)
#define	STACKTRACE_STREAM						\
		util::stacktrace::stream()
/**
	Indents and returns the current stream used by stacktrace.  
 */
#define STACKTRACE_INDENT						\
		STACKTRACE_STREAM << util::stacktrace_auto_indent
#define REDIRECT_STACKTRACE(os)						\
	const util::stacktrace::redirect UNIQUIFY(__redir_stacktrace__) (os)
#define	ASSERT_STACKTRACE(expr)						\
	if (!(expr)) { util::stacktrace::full_dump(); assert(expr); }
#define	REQUIRES_STACKTRACE_STATIC_INIT					\
	static const util::stacktrace::init_token			\
	UNIQUIFY(__stacktrace_init_) (util::stacktrace::require_static_init());

#else	// ENABLE_STACKTRACE --------------------------------------------------

#define	USING_STACKTRACE
#define	STACKTRACE(str)
#define	STACKTRACE_BRIEF
#define	STACKTRACE_VERBOSE
#define STACKTRACE_ECHO_ON
#define STACKTRACE_ECHO_OFF
#define	STACKTRACE_STREAM		std::cerr
#define STACKTRACE_INDENT		STACKTRACE_STREAM << ""
#define REDIRECT_STACKTRACE(os)
#define	ASSERT_STACKTRACE(expr)		assert(expr)
#define	REQUIRES_STACKTRACE_STATIC_INIT	
#endif	// ENABLE_STACKTRACE --------------------------------------------------

//=============================================================================
// additional macros for general purpose use
// we provide these macro layers for ease controlling
// certain subsets of debugging

#ifndef	STACKTRACE_DESTRUCTORS
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#endif
#ifndef	STACKTRACE_CONSTRUCTORS
#define	STACKTRACE_CONSTRUCTORS		0 && ENABLE_STACKTRACE
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	STACKTRACE_DTOR is intended for use with destructors
 */
#ifndef	STACKTRACE_DTOR
#if STACKTRACE_DESTRUCTORS
	#define	STACKTRACE_DTOR(x)	STACKTRACE(x)
	#define	STACKTRACE_DTOR_BRIEF	STACKTRACE_BRIEF
	#define	STACKTRACE_DTOR_VERBOSE	STACKTRACE_VERBOSE
#else
	#define	STACKTRACE_DTOR(x)
	#define	STACKTRACE_DTOR_BRIEF
	#define	STACKTRACE_DTOR_VERBOSE
#endif	// STACKTRACE_DESTRUCTORS
#endif	// STACKTRACE_DTOR

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	STACKTRACE_CTOR is intended for use with constructors
 */
#ifndef	STACKTRACE_CTOR
#if STACKTRACE_CONSTRUCTORS
	#define	STACKTRACE_CTOR(x)	STACKTRACE(x)
	#define	STACKTRACE_CTOR_BRIEF	STACKTRACE_BRIEF
	#define	STACKTRACE_CTOR_VERBOSE	STACKTRACE_VERBOSE
#else
	#define	STACKTRACE_CTOR(x)
	#define	STACKTRACE_CTOR_BRIEF
	#define	STACKTRACE_CTOR_VERBOSE
#endif	// STACKTRACE_CONSTRUCTORS
#endif	// STACKTRACE_CTOR

//=============================================================================

#if ENABLE_STACKTRACE

#include <iosfwd>
// #include "util/macros.h"
#include "util/string_fwd.h"

#include "util/STL/list_fwd.h"
#include "util/STL/stack_fwd.h"
#include <stack>	// needed

// need count pointer to be able to guarantee proper initialization
// across modules during static construction debugging
#include "util/memory/count_ptr.h"


namespace util {
USING_LIST
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
	typedef DEFAULT_STACK(std::ostream*)	stack_streams_type;

public:
	class manager;
	struct echo;
	struct redirect;
	struct indent { };
	class init_token;

private:
//	const string	local_str;	///< deep copy of string argument

public:
//	stacktrace(const char*);
	stacktrace(const string&);
	~stacktrace();
public:
	static
	std::ostream&
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
} __ATTRIBUTE_UNUSED__ ;	// end class stacktrace

//-----------------------------------------------------------------------------
extern const stacktrace::indent	stacktrace_auto_indent;

std::ostream&
operator << (std::ostream&, const stacktrace::indent&);

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
} __ATTRIBUTE_UNUSED__ ;	// end struct echo

//-----------------------------------------------------------------------------
/**
	Redirect all stack dumps to this ostream until changed otherwise.
	Lasts for the duration of the scope where this is called.  
 */
struct stacktrace::redirect {
	redirect(std::ostream&);
	~redirect();
} __ATTRIBUTE_UNUSED__ ;	// end struct redirect

//=============================================================================

}	// end namespace util

#else	// ENABLE_STACKTRACE
	// don't even bother processing class declaration!
#endif	// ENABLE_STACKTRACE

#endif	// __UTIL_STACKTRACE_H__

