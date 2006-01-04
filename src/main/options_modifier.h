/**
	\file "main/options_modifier.h"
	Utility class for modifying options, using a map and registry.  
	$Id: options_modifier.h,v 1.1.2.1 2006/01/04 08:42:10 fang Exp $
 */

#ifndef	__HAC_MAIN_OPTIONS_MODIFIER_H__
#define	__HAC_MAIN_OPTIONS_MODIFIER_H__

#include <iosfwd>
#include "util/string_fwd.h"
#include "util/qmap_fwd.h"

namespace HAC {

#define	OPTIONS_MODIFIER_POLICY_TEMPLATE_SIGNATURE			\
	template <class OptionsType, typename Modifier>

#define	OPTIONS_MODIFIER_POLICY_CLASS					\
	options_modifier_policy<OptionsType,Modifier>

/**
	General pattern class for a registry of functions. 
	Here, intended for modification of a structure.  
	\param OptionsType the type of the options structure to be modified.  
	\param Modifier is overrideable with any other function pointer
	of functor class type.  
 */
template <class OptionsType, typename Modifier = void (*)(OptionsType&)>
struct options_modifier_policy {
	typedef	OptionsType			options_type;
	typedef Modifier			modifier_type;
	struct options_modifier_info;
	typedef	util::qmap<std::string, options_modifier_info>
					options_modifier_map_type;
	class register_options_modifier_base;
protected:
	static const options_modifier_map_type	options_modifier_map;
public:
	static std::ostream&
	dump_options_briefs(std::ostream&);
};	// end class options_modifier_policy

}	// end namespace HAC

#endif	// __HAC_MAIN_OPTIONS_MODIFIER_H__

