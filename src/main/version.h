/**
	\file "main/version.h"
	Interface header for version module.  
	$Id: version.h,v 1.6.60.1 2007/03/28 06:11:53 fang Exp $
 */

#ifndef	__HAC_MAIN_VERSION_H__
#define	__HAC_MAIN_VERSION_H__

#include <iosfwd>
#include "main/hackt_fwd.h"

namespace HAC {
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

};	// end class version

//=============================================================================
}	// end namespace HAC

#endif	// __HAC_MAIN_VERSION_H__

