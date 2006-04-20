/**
	\file "Object/lang/SPEC_registry.h"
	$Id: SPEC_registry.h,v 1.3.18.1 2006/04/20 03:34:52 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_SPEC_REGISTRY_H__
#define	__HAC_OBJECT_LANG_SPEC_REGISTRY_H__

#include "util/qmap.h"
#include "Object/lang/directive_definition.h"

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
template <class VisitorType>
class spec_visitor_entry : public directive_definition {
public:
	typedef	VisitorType			visitor_type;
	/**
		The primary execution function should take a sequence
		of node_id's and a global state_manager as arguments.  
		This is what is executed during cflat.  
	 */
	typedef void (main_type)(visitor_type&,
		const param_args_type&, const node_args_type&);
	typedef main_type*			main_ptr_type;
protected:
	main_ptr_type				_main;

public:
	spec_visitor_entry() : directive_definition(), _main(NULL) { }

	spec_visitor_entry(const string& k,
		const main_ptr_type m, 
		const check_num_args_ptr_type np = NULL,
		const check_num_args_ptr_type nn = NULL,
		const check_param_args_ptr_type p = NULL,
		const check_node_args_ptr_type n = NULL) :
		directive_definition(k, np, nn, p, n), 
		_main(m) { }

	operator bool () const { return this->_main; }

	void
	main(visitor_type&, const param_args_type&,
		const node_args_type&) const;

	void
	main(visitor_type&, const node_args_type&) const;


	// everything else inherited from directive_definition

};	// end class spec_definition_entry

//=============================================================================
// TODO: rename for cflat pass

typedef	spec_visitor_entry<cflat_prs_printer>	cflat_spec_definition_entry;

typedef	util::qmap<string, cflat_spec_definition_entry>
						cflat_spec_registry_type;

extern const cflat_spec_registry_type		cflat_spec_registry;

//=============================================================================
}	// end namespace SPEC
}	// end namespace entity
}	// and namespace HAC

#endif	// __HAC_OBJECT_LANG_SPEC_REGISTRY_H__

