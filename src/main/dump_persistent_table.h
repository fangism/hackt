/**
	\file "main/dump_persistent_table.h"
	Interface header for dump_persistent_table module.  
	$Id: dump_persistent_table.h,v 1.1.4.1 2005/08/05 14:05:03 fang Exp $
 */

#ifndef	__MAIN_DUMP_PERSISTENT_TABLE_H__
#define	__MAIN_DUMP_PERSISTENT_TABLE_H__

#include "main/hackt_fwd.h"

namespace ART {

/**
	Instance-less class.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class dump_persistent_table {
#if 0
private:
	class options;
#endif

public:
	static const char		name[];
	static const char		brief_str[];

	dump_persistent_table();

	static
	int
	main(const int, char*[], const global_options&);

private:
#if 0
	static
	void
	usage(void);

	static
	int
	parse_command_options(const int, char*[], options&);
#endif

	static
	const size_t
	program_id;
};	// end class dump_persistent_table

}	// end namespace ART

#endif	// __MAIN_DUMP_PERSISTENT_TABLE_H__

