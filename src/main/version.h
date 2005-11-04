/**
	\file "main/version.h"
	Interface header for version module.  
	$Id: version.h,v 1.2.2.1 2005/11/04 23:30:24 fang Exp $
 */

#ifndef	__MAIN_VERSION_H__
#define	__MAIN_VERSION_H__

#include <iosfwd>
#include "main/hackt_fwd.h"

namespace ART {
using std::ostream;
//=============================================================================
/**
	Instance-less class for parser/self-test module.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class version {
private:
	class options;

public:
	static const char		name[];
	static const char		brief_str[];

	version();

	static
	int
	main(const int, char*[], const global_options&);

private:
	static
	void
	usage(void);

#if 0
	static
	int
	parse_command_options(const int, char*[], options&);
#endif

	static
	const size_t
	program_id;
public:
	static
	ostream&
	package(ostream&);

	static
	ostream&
	cvs(ostream&);

	static
	ostream&
	cxx(ostream&);

	static
	ostream&
	lex(ostream&);

	static
	ostream&
	yacc(ostream&);

	static
	ostream&
	build(ostream&);

};	// end class version

//=============================================================================
}	// end namespace ART

#endif	// __MAIN_VERSION_H__

