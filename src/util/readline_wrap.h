/**
	\file "util/readline_wrap.h"
	Convenience wrapper for readline and editline.  
	NOTE: the readline headers really aren't needed here, 
	only needed in the implementation of this module.  
	$Id: readline_wrap.h,v 1.2 2006/01/22 06:53:36 fang Exp $
 */

#ifndef	__UTIL_READLINE_WRAP_H__
#define	__UTIL_READLINE_WRAP_H__

#include "config.h"
#include <iosfwd>
#include <string>
#include "util/memory/excl_malloc_ptr.h"

#if	defined(HAVE_GNUREADLINE) || defined(HAVE_BSDEDITLINE)
/**
	If defined, then neither readline nor editline are enabled.  
	Fallback onto simpler functions with the same interface.  
	The purpose is to do all the configure-dependent work in this
	class to provide a consistent interface to the developer.  
 */
#define	USE_READLINE		1
#else
#define	USE_READLINE		0
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
	This readline wrapper class provide a consistent programming interface
	where the user need not worry about readline/editline configuration
	because it is all isolated in the implementation of this class.
	Configure-once-and-use-forever.  

	We restrict the use of this to stdin/cin only because readline
	capabilities are not used for non-interactive input.  

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
	typedef	excl_malloc_ptr<char_type>::type	get_line_type;
	typedef	excl_malloc_ptr<const_char_type>::type	hold_line_type;
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
	/**
		control whether or not blank (whitespace-only) lines 
		are returned.
		Default: true -- skip blank lines.  
	 */
	bool				_skip_blank_lines;
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
	__add_history(const_char_type* const);

};	// end class readline_wrapper

//=============================================================================
}	// end namespace util


#endif	/* __UTIL_READLINE_WRAP_H__ */

