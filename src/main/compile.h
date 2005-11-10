/**
	\file "main/compile.h"
	Interface header for compile module.  
	$Id: compile.h,v 1.2.24.2 2005/11/10 00:47:45 fang Exp $
 */

#ifndef	__MAIN_COMPILE_H__
#define	__MAIN_COMPILE_H__

#include "main/hackt_fwd.h"
#include "util/boolean_types.h"
#include "util/qmap_fwd.h"
#include "util/string_fwd.h"

namespace ART {
class compile_options;
//=============================================================================
/**
	Instance-less class for parser/self-test module.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class compile {
public:
	typedef	compile_options			options;
	typedef	util::good_bool (*options_modifier)(options&);
private:
	struct options_modifier_info;
	typedef	util::qmap<std::string, options_modifier_info>
						options_modifier_map_type;
	static const options_modifier_map_type	options_modifier_map;
public:
	class register_options_modifier;
public:
	static const char		name[];
	static const char		brief_str[];

	compile();

	static
	int
	main(const int, char*[], const global_options&);

public:
	static
	void
	usage(void);

	static
	int
	parse_command_options(const int, char*[], options&);

private:
	static
	const size_t
	program_id;
};	// end class compile

//=============================================================================
}	// end namespace ART

#endif	// __MAIN_COMPILE_H__

