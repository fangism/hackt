/**
	\file "bogus_stacktrace.h"
	Striped down fake bogus_stacktrace for link test.
	$Id: bogus_stacktrace.h,v 1.2 2005/03/01 21:26:48 fang Exp $
 */

#ifndef	__BOGUS_STACKTRACE_H__
#define	__BOGUS_STACKTRACE_H__

// macros for enabling/disabling bogus_stacktrace code
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
	#define	USING_STACKTRACE	using util::bogus_stacktrace;
	#define	STACKTRACE(str)						\
		util::bogus_stacktrace __bogus_stacktrace__(str)
	#define	REQUIRES_BOGUS_STACKTRACE_STATIC_INIT			\
		static const util::bogus_stacktrace::init_token		\
		__bogus_stacktrace_init__(				\
			util::bogus_stacktrace::require_static_init());
#else
	#define	USING_STACKTRACE
	#define	STACKTRACE(str)
	#define	REQUIRES_BOGUS_STACKTRACE_STATIC_INIT	
#endif


//=============================================================================

#if ENABLE_STACKTRACE

#include <string>
#include <list>

namespace util {
using std::string;

//=============================================================================
/**
	Nothing is inlined because when you're debugging you shouldn't be
	varing about performance.  
	Can use for function scopes or any conditional statement scopes.  
	Two mechanisms for controlling behavior, 
	compile-time macros and public static variables.  
	TODO: make one entry per thread, mapped by thread ID.
 */
class bogus_stacktrace {
#if 1
public:
	typedef	std::list<string>	stack_text_type;
#endif
public:
	class init_token;

public:
	bogus_stacktrace(const string&);
	~bogus_stacktrace();
public:

	static
	init_token&
	require_static_init(void);
	
};	// end class bogus_stacktrace

//-----------------------------------------------------------------------------
/**
	Initializer token is requires for all modules that want to 
	use bogus_stacktrace for debugging during static construction and 
	destruction of the program.  
	Reference counting guarantees that the last module to need
	bogus_stacktrace's servives will take responsibility for freeing
	its resources.  
 */
class bogus_stacktrace::init_token {
	friend class bogus_stacktrace;
private:
	// this causes multiply defined symbol warnings: _List_base...
	const stack_text_type		blah;
public:
	init_token();

	// default copy constructor suffices

	~init_token();

	void
	check(void) const;
};	// end class init_token

//=============================================================================
}	// end namespace util

#else
	// don't even bother processing class declaration!
#endif	// ENABLE_STACKTRACE

#endif	// __BOGUS_STACKTRACE_H__

