/**
	\file "Object/lang/PRS_macro_registry.h"
	$Id: PRS_macro_registry.h,v 1.4.18.1 2006/04/20 03:34:50 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_MACRO_REGISTRY_H__
#define	__HAC_OBJECT_LANG_PRS_MACRO_REGISTRY_H__

#include "util/qmap.h"
#include "Object/lang/directive_definition.h"

namespace HAC {
namespace entity {
namespace PRS {
using std::string;
//=============================================================================
/**
	This is an actual registered entry of a macro definition.  
 */
template <class VisitorType>
class macro_visitor_entry : public directive_definition {
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
	macro_visitor_entry() : directive_definition(), _main(NULL) { }

	macro_visitor_entry(const string& k, const main_ptr_type m, 
		const check_num_args_ptr_type np, 
		const check_num_args_ptr_type nn, 
		const check_param_args_ptr_type p,
		const check_node_args_ptr_type c) :
		directive_definition(k, np, nn, p, c), 
		_main(m) { }

	operator bool () const { return this->_main; }

	void
	main(visitor_type&, const param_args_type&, 
		const node_args_type&) const;

	void
	main(visitor_type&, const node_args_type&) const;

	// everything else inherited from directive_definition

};	// end class macro_definition_entry

//=============================================================================
// TODO: factor these typedefs and symbols out to tool-dependent header

typedef	macro_visitor_entry<PRS::cflat_prs_printer>
						cflat_macro_definition_entry;

typedef	util::qmap<string, cflat_macro_definition_entry>
						cflat_macro_registry_type;

extern const cflat_macro_registry_type		cflat_macro_registry;

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// and namespace HAC

#endif	// __HAC_OBJECT_LANG_PRS_MACRO_REGISTRY_H__

