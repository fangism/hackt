/**
	\file "main/objdump.hh"
	Interface header for the objdump module.  
	$Id: objdump.hh,v 1.5 2009/02/16 01:38:31 fang Exp $
 */

#ifndef	__HAC_MAIN_OBJDUMP_H__
#define	__HAC_MAIN_OBJDUMP_H__

#include "main/hackt_fwd.hh"

namespace HAC {

/**
	Instance-less class.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class objdump {
private:
	struct options;

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

	static
	int
	parse_command_options(const int, char*[], options&);

	static
	const size_t
	program_id;
};	// end class objdump

}	// end namespace HAC

#endif	// __HAC_MAIN_OBJDUMP_H__

