/**
	\file "main/compile.h"
	Interface header for compile module.  
	$Id: compile.h,v 1.9 2010/05/13 00:32:02 fang Exp $
 */

#ifndef	__HAC_MAIN_COMPILE_H__
#define	__HAC_MAIN_COMPILE_H__

#include "main/hackt_fwd.h"
#include "util/boolean_types.h"
#include "util/STL/map_fwd.h"
#include "util/string_fwd.h"
#include "util/memory/pointer_classes_fwd.h"

#define	COMPILE_USE_OPTPARSE				1

namespace HAC {
class compile_options;
namespace entity {
class module;
}
using util::memory::count_ptr;

//=============================================================================
/**
	Instance-less class for parser/self-test module.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class compile {
public:
	typedef	compile_options			options;
#if !COMPILE_USE_OPTPARSE
	typedef	util::good_bool (*options_modifier)(options&);
private:
	struct options_modifier_info;
	typedef	std::default_map<std::string, options_modifier_info>::type
						options_modifier_map_type;
	static const options_modifier_map_type	options_modifier_map;
#endif
public:
	class register_options_modifier;
public:
	static const char		name[];
	static const char		brief_str[];

	compile();

	static
	int
	make_module(int, char*[], options&, count_ptr<entity::module>&);

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
}	// end namespace HAC

#endif	// __HAC_MAIN_COMPILE_H__

