/**
	\file "main/flatten.hh"
	Interface header for flatten module.  
	$Id: flatten.hh,v 1.1 2006/11/15 00:08:59 fang Exp $
 */

#ifndef	__HAC_MAIN_FLATTEN_H__
#define	__HAC_MAIN_FLATTEN_H__

#include <map>
#include "main/hackt_fwd.hh"
#include "util/boolean_types.hh"
#include "util/string_fwd.hh"

namespace HAC {
class compile_options;
//=============================================================================
/**
	Instance-less class for parser/self-test module.  
	Yes, most everything is private, not supposed to use this directly, 
	but rather, through program registration.  
 */
class flatten {
public:
	typedef	compile_options			options;
	typedef	util::good_bool (*options_modifier)(options&);
private:
	struct options_modifier_info;
	typedef	std::map<std::string, options_modifier_info>
						options_modifier_map_type;
	static const options_modifier_map_type	options_modifier_map;
public:
	class register_options_modifier;
public:
	static const char		name[];
	static const char		brief_str[];

	flatten();

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
};	// end class flatten

//=============================================================================
}	// end namespace HAC

#endif	// __HAC_MAIN_FLATTEN_H__

