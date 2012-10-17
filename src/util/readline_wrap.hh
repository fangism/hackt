/**
	\file "util/readline_wrap.hh"
	Convenience wrapper for readline and editline.  
	NOTE: the readline headers really aren't needed here, 
	only needed in the implementation of this module.  
	$Id: readline_wrap.hh,v 1.6 2009/02/25 03:31:07 fang Exp $
 */

#ifndef	__UTIL_READLINE_WRAP_H__
#define	__UTIL_READLINE_WRAP_H__

#include "config.h"
#include <iosfwd>
#include <string>
#include "util/memory/excl_malloc_ptr.hh"

#if	defined(HAVE_GNUREADLINE) || defined(HAVE_BSDEDITLINE)
/**
	If defined, then neither readline nor editline are enabled.  
	Fallback onto simpler functions with the same interface.  
	The purpose is to do all the configure-dependent work in this
	class to provide a consistent interface to the developer.  
 */
#define	USE_READLINE
#elif	defined(USE_READLINE)
// else leave USE_READLINE undefined
#error	"USE_READLINE is already defined, but I want it undefined!"
#endif

#ifndef	USE_READLINE
#include <list>
#endif

#if	defined(HAVE_BSDEDITLINE) && defined(EDITLINE_HAS_HISTEDIT_INTERFACE) && defined(HAVE_HISTEDIT_H)
#define	USE_HISTEDIT
#elif	defined(USE_HISTEDIT)
#error	"USE_HISTEDIT is already defined, but I want it undefined!"
#endif

#ifdef 	USE_HISTEDIT
#include <histedit.h>
#include "util/attributes.h"
#endif

namespace util {
using std::string;
using std::ostream;
using memory::excl_malloc_ptr;
#ifdef	USE_HISTEDIT
using memory::excl_ptr;
#endif
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
#ifdef	USE_HISTEDIT
	/**
		histedit's (editline) el_gets return a pointer to
		its own managed internal buffer, and thus, should not
		be claimed as owned.  
	 */
	typedef	memory::never_ptr<const_char_type>	hold_line_type;
#else
	/**
		Readline returns a malloc-allocated string, and thus
		should be free()d automatically upon destruction.
	 */
	typedef	excl_malloc_ptr<const_char_type>::type	hold_line_type;
#endif
#ifdef	USE_HISTEDIT
	// was __VISIBILITY_HIDDEN__, but cannot because _editline field
	// would be less visible than wrapping class.  
	struct editline_tag {
		void
		operator () (EditLine* e) const {
			el_end(e);
		}
	};

	// was __VISIBILITY_HIDDEN__
	struct editline_history_tag {
		void
		operator () (History* h) const {
			history_end(h);
		}
	};

	typedef	excl_ptr<EditLine, editline_tag>::type	editline_ptr_type;
	typedef	excl_ptr<History, editline_history_tag>::type
							history_ptr_type;
	// typedef	const char_type* el_prompt_fn_type(EditLine*);
	static	const char_type*	el_prompt(EditLine*);
	static	string			current_prompt;
#endif
private:
	/**
		Temporary storage for the current line.  
	 */
	hold_line_type			hold_line;
#ifdef	USE_HISTEDIT
	/**
		Managed EditLine pointer.
	 */
	editline_ptr_type		_editline;
	/**
		Manager (editline) History pointer.  
	 */
	history_ptr_type		_el_history;
	/**
		This holds the pointer to the former prompt, 
		in case of nested editline interpreters.  
		This is restored upon destruction.  
	 */
	string				former_prompt;
#else
	/**
		Fixed prompt.
		TODO: more sophisticated prompt later.  
	 */
	string				prompt;
#endif
#ifndef	USE_READLINE
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
	explicit
	readline_wrapper(const string& = string());

	~readline_wrapper();

	void
	set_prompt(const string&);

	const string&
	get_prompt(void) const {
#ifdef	USE_HISTEDIT
		return current_prompt;
#else
		return prompt;
#endif
	}

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

	static
	void
	refresh(void);

	static
	void
	display_match_list(char**, int, int);

private:
	const_char_type*
	__add_history(const_char_type* const);

};	// end class readline_wrapper

//=============================================================================
}	// end namespace util


#endif	/* __UTIL_READLINE_WRAP_H__ */

