/**
	\file "main/objdump.h"
	Interface header for the objdump module.  
	$Id: objdump.h,v 1.2 2005/08/08 16:51:12 fang Exp $
 */

#ifndef	__MAIN_OBJDUMP_H__
#define	__MAIN_OBJDUMP_H__

#include "main/hackt_fwd.h"

namespace ART {

/**
	Instance-less class.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class objdump {
private:
	class options;

public:
	static const char		name[];
	static const char		brief_str[];

	objdump();

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
};	// end class objdump

}	// end namespace ART

#endif	// __MAIN_OBJDUMP_H__

