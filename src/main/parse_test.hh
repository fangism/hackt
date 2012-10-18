/**
	\file "main/parse_test.hh"
	Interface header for parse_test module.  
	$Id: parse_test.hh,v 1.5 2006/01/27 08:07:21 fang Exp $
 */

#ifndef	__HAC_MAIN_PARSE_TEST_H__
#define	__HAC_MAIN_PARSE_TEST_H__

#include "main/hackt_fwd.hh"

namespace HAC {
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
}	// end namespace HAC

#endif	// __HAC_MAIN_PARSE_TEST_H__

