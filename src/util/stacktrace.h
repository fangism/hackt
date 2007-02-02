/**
	\file "util/stacktrace.h"
	Utility macros and header for convenient stack-trace debugging.
	$Id: stacktrace.h,v 1.16 2007/02/02 19:20:33 fang Exp $
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

#include "config.h"

#if defined(__cplusplus) && defined(HAVE_CASSERT) && HAVE_CASSERT
#include <cassert>
#else
#include <assert.h>
#endif

#include <iosfwd>		// needed for std::ostream forward declaration

#define	NULL_STACKTRACE_STREAM		util::null_stacktrace_stream

namespace util {
/**
	This definition is always available regardless of ENALBLE_STACKTRACE.  
	Fake stream object for sake of disabling STACKTRACE_STREAM.  
 */
struct null_stacktrace_stream_type {
	typedef	null_stacktrace_stream_type		this_type;

	template <class T>
	const null_stacktrace_stream_type&
	operator << (const T&) const { return *this; }

#if 0
	/**
		Failed attempt to generalize for all
		manipulators.  
	 */
	template <class stream_type>
	const this_type&
	operator << (stream_type& (*__pf)(stream_type&)) const {
		return *this;
	}
#endif

	/**
		Ugly kludge needed to handle case where RHS is
		an iomanip function, such as flush, endl, ends.  
	 */
	const this_type&
	operator << (std::ostream& (*__pf)(std::ostream&)) const {
		return *this;
	}

};	// end struct null_stacktrace_stream_type


/**
	Private null-stream object.  
	No linking required.  
 */
static
const null_stacktrace_stream_type
null_stacktrace_stream = null_stacktrace_stream_type();

}	// end namespace util

//=============================================================================
// This is the macro interface intended for the programmer.  
#if ENABLE_STACKTRACE
#include "util/cppcat.h"		// for the UNIQUIFY macros
#include "util/attributes.h"

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
/**
	ostream << style printing.  
	This interface is preferable when compiler isn't smart enough
	to optimize away no-ops with the null_stacktrace_stream.  
	\param x may be a set of <<-cascaded arguments.  
 */
#define	STACKTRACE_INDENT_PRINT(x)	STACKTRACE_INDENT << x
#define REDIRECT_STACKTRACE(os)						\
	const util::stacktrace::redirect UNIQUIFY(__redir_stacktrace__) (os)
#define	ASSERT_STACKTRACE(expr)						\
	if (!(expr)) { util::stacktrace::full_dump(); assert(expr); }
#define	REQUIRES_STACKTRACE_STATIC_INIT					\
	static const util::stacktrace::init_token			\
	UNIQUIFY(__stacktrace_init_) (util::stacktrace::require_static_init());

#else	// ENABLE_STACKTRACE --------------------------------------------------

#define	STACKTRACE(str)
#define	STACKTRACE_BRIEF
#define	STACKTRACE_VERBOSE
#define STACKTRACE_ECHO_ON
#define STACKTRACE_ECHO_OFF
#define	STACKTRACE_STREAM		NULL_STACKTRACE_STREAM
#define STACKTRACE_INDENT		NULL_STACKTRACE_STREAM
#define	STACKTRACE_INDENT_PRINT(x)
#define REDIRECT_STACKTRACE(os)
#define	ASSERT_STACKTRACE(expr)		assert(expr)
#define	REQUIRES_STACKTRACE_STATIC_INIT	
#endif	// ENABLE_STACKTRACE --------------------------------------------------

//=============================================================================
// additional macros for general purpose use
// we provide these macro layers for ease controlling
// certain subsets of debugging

#ifndef	STACKTRACE_DESTRUCTORS
#define	STACKTRACE_DESTRUCTORS		(0 && ENABLE_STACKTRACE)
#endif
#ifndef	STACKTRACE_CONSTRUCTORS
#define	STACKTRACE_CONSTRUCTORS		(0 && ENABLE_STACKTRACE)
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
	#define	STACKTRACE_DTOR_PRINT(x)	STACKTRACE_INDENT_PRINT(x)
#else
	#define	STACKTRACE_DTOR(x)
	#define	STACKTRACE_DTOR_BRIEF
	#define	STACKTRACE_DTOR_VERBOSE
	#define	STACKTRACE_DTOR_PRINT(x)
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
	#define	STACKTRACE_CTOR_PRINT(x)	STACKTRACE_INDENT_PRINT(x)
#else
	#define	STACKTRACE_CTOR(x)
	#define	STACKTRACE_CTOR_BRIEF
	#define	STACKTRACE_CTOR_VERBOSE
	#define	STACKTRACE_CTOR_PRINT(x)
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
using std::list;
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
	typedef std::default_list<string>::type	stack_text_type;
	/// the type of stack used to track on/off mode
	typedef std::default_stack<int>::type	stack_echo_type;
	/// the type of stack used to track stream redirections
	typedef std::default_stack<std::ostream*>::type	stack_streams_type;

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

