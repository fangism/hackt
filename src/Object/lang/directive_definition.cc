/**
	\file "Object/lang/directive_defintion.cc"
	$Id: directive_definition.cc,v 1.3 2006/04/23 07:37:24 fang Exp $
 */

#include <vector>
#include "Object/lang/directive_definition.hh"
#include "Object/expr/const_param.hh"
#include "util/memory/count_ptr.hh"

namespace HAC {
namespace entity {
// using PRS::cflat_prs_printer;
//=============================================================================
// class directive_definition method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
directive_definition::check_num_params(const size_t i) const {
if (_check_num_params) {
	return (*_check_num_params)(i);
} else  return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
directive_definition::check_num_nodes(const size_t i) const {
if (_check_num_nodes) {
	return (*_check_num_nodes)(i);
} else  return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
directive_definition::check_param_args(const param_args_type& i) const {
if (_check_param_args) {
	return (*_check_param_args)(i);
} else  return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
directive_definition::check_node_args(const node_args_type& i) const {
if (_check_node_args) {
	return (*_check_node_args)(i);
} else  return good_bool(true);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

