/**
	\file "indent.h"
	Utility for semi-automatically indenting output.  

	$Id: indent.h,v 1.1 2004/12/01 05:16:17 fang Exp $
 */

#ifndef	__INDENT_H__
#define	__INDENT_H__

#include <iosfwd>
#include <string>

namespace util {
using std::string;
using std::ostream;

//=============================================================================
/**
	Per stream indentation manager.  
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

};	// end class indent

//=============================================================================
// utilities to control indentation

class enable_indent {
private:
	const ostream& os;
public:
	enable_indent(const ostream& o);
	~enable_indent();
};	// end struct enable_indent

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class disable_indent {
private:
	const ostream& os;
public:
	disable_indent(const ostream& o);
	~disable_indent();
};	// end struct disable_indent

//=============================================================================
// public interface functions

ostream&
auto_indent(ostream& o);

/**
	Though explicit registration is unnecessary, 
	this can be used to customize the 
	Note: this clobbers the existing entry, which can cause
	problems if misused (will issue a warning)!
 */
void
register_indent(const ostream& o, const string& s);

//=============================================================================
}	// end namespace util

#endif	// __INDENT_H__

