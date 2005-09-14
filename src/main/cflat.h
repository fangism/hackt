/**
	\file "main/cflat.h"
	Interface header for cflat module.  
	$Id: cflat.h,v 1.1.2.1 2005/09/14 19:20:04 fang Exp $
 */

#ifndef	__MAIN_CFLAT_H__
#define	__MAIN_CFLAT_H__

#include "main/hackt_fwd.h"

namespace ART {

/**
	Instance-less class.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class cflat {
private:
	class options;

public:
	static const char		name[];
	static const char		brief_str[];

	cflat();

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
};	// end class cflat

}	// end namespace ART

#endif	// __MAIN_CFLAT_H__

