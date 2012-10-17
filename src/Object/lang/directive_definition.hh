/**
	\file "Object/lang/directive_definition.hh"
	$Id: directive_definition.hh,v 1.3 2006/04/23 07:37:24 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_DIRECTIVE_DEFINITION_H__
#define	__HAC_OBJECT_LANG_DIRECTIVE_DEFINITION_H__

#include <string>
#include "util/NULL.h"
#include "util/boolean_types.hh"
#include "Object/lang/SPEC_fwd.hh"

namespace HAC {
namespace entity {
class state_manager;
namespace PRS {
class cflat_prs_printer;
}
using std::string;
using util::good_bool;
//=============================================================================
/**
	This is base class for a macro or spec directive definition.  
	(They're nearly identical.)
 */
class directive_definition {
public:
	typedef	directive_base_nodes_type	node_args_type;
	typedef	directive_base_params_type	param_args_type;
	/**
		Function type for verifying macro arguments.  
	 */
	typedef	good_bool (check_node_args_type)(const node_args_type&);
	typedef	check_node_args_type*		check_node_args_ptr_type;
	/**
		Function type for verifying macro parameters.  
	 */
	typedef	good_bool (check_param_args_type)(const param_args_type&);
	typedef	check_param_args_type*		check_param_args_ptr_type;

	/**
		Function type for verifying the number of arguments.  
	 */
	typedef	good_bool (check_num_args_type)(const size_t);
	typedef	check_num_args_type*		check_num_args_ptr_type;

protected:
	/// the name of the macro
	string					_key;
	check_num_args_ptr_type			_check_num_params;
	check_num_args_ptr_type			_check_num_nodes;
	check_param_args_ptr_type		_check_param_args;
	check_node_args_ptr_type		_check_node_args;
public:
	directive_definition() : _key(),
		_check_num_params(NULL), _check_num_nodes(NULL), 
		_check_param_args(NULL), _check_node_args(NULL) { }

	directive_definition(const string& k,
		const check_num_args_ptr_type np = NULL,
		const check_num_args_ptr_type nn = NULL,
		const check_param_args_ptr_type p = NULL, 
		const check_node_args_ptr_type c = NULL) :
		_key(k),
		_check_num_params(np), _check_num_nodes(nn), 
		_check_param_args(p), _check_node_args(c)
		{ }

	// default destructor

	good_bool
	check_num_params(const size_t) const;

	good_bool
	check_param_args(const param_args_type&) const;

	good_bool
	check_num_nodes(const size_t) const;

	good_bool
	check_node_args(const node_args_type&) const;

};	// end class macro_definition_entry

//=============================================================================
}	// end namespace entity
}	// and namespace HAC

#endif	// __HAC_OBJECT_LANG_DIRECTIVE_DEFINITION_H__

