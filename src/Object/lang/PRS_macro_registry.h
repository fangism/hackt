/**
	\file "Object/lang/PRS_macro_registry.h"
	$Id: PRS_macro_registry.h,v 1.3 2006/02/04 06:43:18 fang Exp $
 */

#ifndef	__OBJECT_LANG_PRS_MACRO_REGISTRY_H__
#define	__OBJECT_LANG_PRS_MACRO_REGISTRY_H__

#include <string>
#include "util/NULL.h"
#include "util/size_t.h"
#include "util/qmap.h"
#include "util/boolean_types.h"
#include "util/STL/vector_fwd.h"

namespace HAC {
namespace entity {
class state_manager;
namespace PRS {
class cflat_prs_printer;
using std::string;
using util::good_bool;
//=============================================================================
/**
	This is an actual registered entry of a macro definition.  
 */
class macro_definition_entry {
public:
	typedef	std::default_vector<size_t>::type	node_args_type;
	/**
		The primary execution function should take a sequence
		of node_id's and a global state_manager as arguments.  
		This is what is executed during cflat.  
	 */
	typedef	void (main_type)(cflat_prs_printer&, const node_args_type&);
	typedef	main_type*			main_ptr_type;
	/**
		Function type for verifying number of macro arguments.  
	 */
	typedef	good_bool (check_num_args_type)(const size_t);
	typedef	check_num_args_type*		check_num_args_ptr_type;

private:
	/// the name of the macro
	string					_key;
	main_ptr_type				_main;
	check_num_args_ptr_type			_check_num_args;
public:
	macro_definition_entry() : _key(), _main(NULL),
		_check_num_args(NULL) { }

	macro_definition_entry(const string& k, const main_ptr_type m, 
		const check_num_args_ptr_type c = NULL) :
		_key(k), _main(m),
		_check_num_args(c)
		{ }

	// default destructor

	operator bool () const { return _main; }

	void
	main(cflat_prs_printer&, const node_args_type&) const;

	good_bool
	check_num_args(const size_t) const;

};	// end class macro_definition_entry

//=============================================================================
/**
	This is the interface for a macro_definition_entry.
	Classes whose members will be registered should look like this.  
 */
struct macro_interface {
};	// end struct macro_interface

//=============================================================================
typedef	util::qmap<string, macro_definition_entry>	macro_registry_type;

extern const macro_registry_type			macro_registry;

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// and namespace HAC

#endif	// __OBJECT_LANG_PRS_MACRO_REGISTRY_H__

