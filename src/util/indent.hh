/**
	\file "util/indent.hh"
	Utility for semi-automatically indenting output.  

	$Id: indent.hh,v 1.7 2005/09/04 21:15:07 fang Exp $
 */

#ifndef	__UTIL_INDENT_H__
#define	__UTIL_INDENT_H__

#include <iosfwd>
#include <string>
#include "util/cppcat.h"		// for UNIQUIFY macro
#include "util/attributes.h"

/**
	Handy macro for declaring an indented section.  
	NOTE: Using __LINE__ to automatically generate unique identifiers
		doesn't seem to be working.  WTF?
	\param f file stream, such as cout, cerr.  
 */
#define	INDENT_SECTION(f)						\
const util::indent UNIQUIFY(__indent_) (f)

namespace util {
using std::string;
using std::ostream;

//=============================================================================
/**
	Per stream indentation manager.  
	To use it: declare an indent object for the ostream you want to 
	indent.  
	The indentation is effective in the scope of the indent object's
	declaration, so you never have to manually balance the indentation
	yourself -- it is done in the destructor.  
	Indentation is done by o << auto_indent, 
	where o is an ostream object (like cerr, cout)
	and auto_indent is the global object magic indentation manager
	that behaves like a stream manipulator.  
 */
class indent {
private:
	/**
		Promise not to modify the ostream from here.  
	 */
	const ostream& os;
public:
	indent(const ostream& o);
	indent(const ostream& o, const string& s);
	~indent();

} __ATTRIBUTE_UNUSED__ ;	// end class indent

//=============================================================================
// utilities to control indentation

class enable_indent {
private:
	const ostream& os;
public:
	enable_indent(const ostream& o);
	~enable_indent();
} __ATTRIBUTE_UNUSED__ ;	// end struct enable_indent

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class disable_indent {
private:
	const ostream& os;
public:
	disable_indent(const ostream& o);
	~disable_indent();
} __ATTRIBUTE_UNUSED__ ;	// end struct disable_indent

//=============================================================================
class auto_indenter { };

// use this like an IO-manipulator like endl
extern	const auto_indenter	auto_indent;

//=============================================================================
// public interface functions

ostream&
operator << (ostream& o, const auto_indenter&);

/**
	Though explicit registration is unnecessary, 
	this can be used to customize the indentation string.  
	Note: this clobbers the existing entry, which can cause
	problems if misused (will issue a warning)!
 */
void
register_indent(const ostream& o, const string& s);

//=============================================================================
}	// end namespace util

#endif	// __UTIL_INDENT_H__

