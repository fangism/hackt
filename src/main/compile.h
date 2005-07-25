/**
	\file "main/compile.h"
	Interface header for compile module.  
	$Id: compile.h,v 1.1 2005/07/25 02:10:07 fang Exp $
 */

#ifndef	__MAIN_COMPILE_H__
#define	__MAIN_COMPILE_H__

#include "main/hackt_fwd.h"

namespace ART {

/**
	Instance-less class for parser/self-test module.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class compile {
private:
	class options;

private:
	static const char		name[];
	static const char		brief_str[];

public:
	compile();

	static
	int
	main(const int, char*[], const global_options&);

private:
	static
	void
	usage(void);

	static
	int
	parse_command_options(const int, char*[], options&);

	static
	const size_t
	program_id;
};	// end class compile

}	// end namespace ART

#endif	// __MAIN_COMPILE_H__

