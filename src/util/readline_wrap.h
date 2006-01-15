/**
	\file "util/readline_wrap.h"
	Convenience wrapper for readline and editline.  
	NOTE: the readline headers really aren't needed here, 
	only needed in the implementation of this module.  
	$Id: readline_wrap.h,v 1.1.4.3.2.1 2006/01/15 22:02:51 fang Exp $
 */

#ifndef	__UTIL_READLINE_WRAP_H__
#define	__UTIL_READLINE_WRAP_H__

#include "config.h"
#include <iosfwd>
#include <string>
#include "util/memory/excl_malloc_ptr.h"

#if	!defined(HAVE_GNUREADLINE) && !defined(HAVE_BSDEITLINE)
/**
	If defined, then neither readline nor editline are enabled.  
	Fallback onto simpler functions with the same interface.  
	The purpose is to do all the configure-dependent work in this
	class to provide a consistent interface to the developer.  
 */
#define	USE_READLINE		0
#else
#define	USE_READLINE		1
#endif

#if !USE_READLINE
#include <list>
#endif

namespace util {
using std::string;
using std::ostream;
using memory::excl_malloc_ptr;
//=============================================================================
/**
	Rather than use global variables, we try to maintain
	readline state information in this class. 
	Also for memory protection.  
	TODO: implement own history buffer if readline is missing.  
 */
class readline_wrapper {
public:
	// prepare to accommodate other character types!
	typedef	char				char_type;
	typedef	const char			const_char_type;
private:
	typedef	excl_malloc_ptr<char_type>		get_line_type;
	typedef	excl_malloc_ptr<const_char_type>	hold_line_type;
	/**
		Temporary storage for the current line.  
	 */
	hold_line_type			hold_line;
	/**
		Fixed prompt.
		TODO: more sophisticated prompt later.  
	 */
	string				prompt;
#if !USE_READLINE
	typedef	std::list<string>	history_type;
	history_type			history;
#endif
public:
	readline_wrapper();

	explicit
	readline_wrapper(const string&);

	~readline_wrapper();

	void
	set_prompt(const string&);

	const string&
	get_prompt(void) const { return prompt; }

	/**
		Just a wrapper around gets to give the same interface as 
		readline().  Of course, this lacks any readline features.  
		\return line of text read, which need NOT be deleted.  
	 */
	const_char_type*
	gets(void);

	/**
		Same as gets, but uses a custom prompt instead
		of the set one.  
	 */
	const_char_type*
	gets(const string&);

	static
	ostream&
	version_string(ostream&);

private:
	const_char_type*
	__add_history(const_char_type* const) const;

};	// end class readline_wrapper

//=============================================================================
}	// end namespace util


#endif	/* __UTIL_READLINE_WRAP_H__ */

