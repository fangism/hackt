/**
	\file "main/version.h"
	Interface header for version module.  
	$Id: version.h,v 1.2 2005/11/03 07:52:07 fang Exp $
 */

#ifndef	__MAIN_VERSION_H__
#define	__MAIN_VERSION_H__

#include "main/hackt_fwd.h"

namespace ART {

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
};	// end class version

}	// end namespace ART

#endif	// __MAIN_VERSION_H__

