/**
	\file "main/unroll.h"
	Interface header for unroll module.  
	$Id: unroll.h,v 1.2 2005/08/08 16:51:13 fang Exp $
 */

#ifndef	__MAIN_UNROLL_H__
#define	__MAIN_UNROLL_H__

#include "main/hackt_fwd.h"

namespace ART {

/**
	Instance-less class.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class unroll {
private:
	class options;

public:
	static const char		name[];
	static const char		brief_str[];

	unroll();

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
};	// end class unroll

}	// end namespace ART

#endif	// __MAIN_UNROLL_H__

