/**
	\file "util/readline_wrap.h"
	Convenience wrapper for readline and editline.  
	NOTE: the readline headers really aren't needed here, 
	only needed in the implementation of this module.  
	$Id: readline_wrap.h,v 1.1 2005/12/10 03:56:57 fang Exp $
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
#define	NO_READLINE
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
 */
class readline_wrapper {
private:
	typedef	excl_malloc_ptr<char>	hold_line_type;
	/**
		Temporary storage for the current line.  
	 */
	hold_line_type			hold_line;
	/**
		Fixed prompt.
		TODO: more sophisticated prompt later.  
	 */
	string				prompt;
public:
	readline_wrapper();

	explicit
	readline_wrapper(const string&);

	~readline_wrapper();

	void
	set_prompt(const string&);

	/**
		Just a wrapper around gets to give the same interface as 
		readline().  Of course, this lacks any readline features.  
		\return line of text read, which need NOT be deleted.  
	 */
	char*
	gets(void);

	static
	ostream&
	version_string(ostream&);

};	// end class readline_wrapper

//=============================================================================
}	// end namespace util


#endif	/* __UTIL_READLINE_WRAP_H__ */

