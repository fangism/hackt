/**
	\file "main/parse_test.h"
	Interface header for parse_test module.  
	$Id: parse_test.h,v 1.2.24.1 2005/11/10 00:47:46 fang Exp $
 */

#ifndef	__MAIN_PARSE_TEST_H__
#define	__MAIN_PARSE_TEST_H__

#include "main/hackt_fwd.h"

namespace ART {
//=============================================================================
class compile_options;
/**
	Instance-less class.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class parse_test {
private:
	typedef	compile_options		options;

public:
	static const char		name[];
	static const char		brief_str[];

	parse_test();

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
};	// end class parse_test

//=============================================================================
}	// end namespace ART

#endif	// __MAIN_PARSE_TEST_H__

