/**
	\file "sim/command_category.h"
	$Id: command_category.h,v 1.2 2007/01/21 06:00:23 fang Exp $
 */

#ifndef	__HAC_SIM_COMMAND_CATEGORY_H__
#define	__HAC_SIM_COMMAND_CATEGORY_H__

#include <iosfwd>
#include <string>
#include <map>
#include "util/macros.h"
#include "util/tokenize_fwd.h"
#include "util/member_saver_fwd.h"

namespace HAC {
namespace SIM {
using std::string;
using std::map;
using util::string_list;
using std::string;
using std::ostream;
template <class> class command_category;

//=============================================================================
/**
	We organize all prsim commands into categories.  
	Bother factoring out base class? nah...
 */
template <class Command>
class command_category {
	typedef	command_category<Command>	this_type;
public:
	typedef	Command				command_type;
private:
	typedef	map<string, command_type>	command_map_type;
	typedef	typename command_type::main_ptr_type	main_ptr_type;
	typedef	typename command_type::usage_ptr_type	usage_ptr_type;
	typedef	typename command_map_type::const_iterator
						const_iterator;
private:
	string					_name;
	string					_brief;
	command_map_type			command_map;
public:
	command_category();
	command_category(const string& _n, const string& _b);
private:
	// private, undefined copy-constructor
	command_category(const this_type&);
public:
	~command_category();

	// this should not be called directly, but rather through
	// command_registry<>::register_command().
	size_t
	register_command(const command_type&);

	const string&
	name(void) const { return _name; }

	const string&
	brief(void) const { return _brief; }

	void
	list(ostream&) const;

	const_iterator
	lookup_command(const string&) const;

	bool
	is_valid(const const_iterator i) const {
		return i != command_map.end();
	}

};	// end class command_category

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_COMMAND_CATEGORY_H__

