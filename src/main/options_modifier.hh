/**
	\file "main/options_modifier.hh"
	Utility class for modifying options, using a map and registry.  
	$Id: options_modifier.hh,v 1.4 2006/04/28 03:20:14 fang Exp $
 */

#ifndef	__HAC_MAIN_OPTIONS_MODIFIER_H__
#define	__HAC_MAIN_OPTIONS_MODIFIER_H__

#include <iosfwd>
#include <map>
#include "util/string_fwd.hh"

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
class options_modifier_policy {
public:
	typedef	OptionsType			options_type;
	typedef Modifier			modifier_type;
	class options_modifier_info;
	typedef	typename std::map<std::string, options_modifier_info>
					options_modifier_map_type;
	typedef	typename options_modifier_map_type::const_iterator
					options_modifier_map_iterator;
	class register_options_modifier_base;
protected:
	static const options_modifier_map_type	options_modifier_map;
public:
	static std::ostream&
	dump_options_briefs(std::ostream&);
};	// end class options_modifier_policy

}	// end namespace HAC

#endif	// __HAC_MAIN_OPTIONS_MODIFIER_H__

