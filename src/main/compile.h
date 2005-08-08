/**
	\file "main/compile.h"
	Interface header for compile module.  
	$Id: compile.h,v 1.2 2005/08/08 16:51:12 fang Exp $
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

public:
	static const char		name[];
	static const char		brief_str[];

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

