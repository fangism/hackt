/**
	\file "Object/lang/SPEC_registry.h"
	$Id: SPEC_registry.h,v 1.2 2006/02/04 06:43:20 fang Exp $
 */

#ifndef	__OBJECT_LANG_SPEC_REGISTRY_H__
#define	__OBJECT_LANG_SPEC_REGISTRY_H__

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
}
namespace SPEC {
using PRS::cflat_prs_printer;
using std::string;
using util::good_bool;
//=============================================================================
/**
	This is an actual registered entry of a spec definition.  
 */
class spec_definition_entry {
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
		Function type for verifying number of spec arguments.  
	 */
	typedef	good_bool (check_num_args_type)(const size_t);
	typedef	check_num_args_type*		check_num_args_ptr_type;

private:
	/// the name of the spec
	string					_key;
	main_ptr_type				_main;
	check_num_args_ptr_type			_check_num_args;
public:
	spec_definition_entry() : _key(), _main(NULL),
		_check_num_args(NULL) { }

	spec_definition_entry(const string& k, const main_ptr_type m, 
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

};	// end class spec_definition_entry

//=============================================================================
typedef	util::qmap<string, spec_definition_entry>	spec_registry_type;

extern const spec_registry_type				spec_registry;

//=============================================================================
}	// end namespace SPEC
}	// end namespace entity
}	// and namespace HAC

#endif	// __OBJECT_LANG_SPEC_REGISTRY_H__

