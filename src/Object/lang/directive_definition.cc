/**
	\file "Object/lang/directive_defintion.cc"
	$Id: directive_definition.cc,v 1.2 2006/02/10 21:50:41 fang Exp $
 */

#include <vector>
#include "Object/lang/directive_definition.h"
#include "Object/expr/const_param.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
using PRS::cflat_prs_printer;
//=============================================================================

// class directive_definition method definitions

void
directive_definition::main(cflat_prs_printer& c, const param_args_type& p, 
		const node_args_type& n) const {
	NEVER_NULL(_main);
	(*_main)(c, p, n);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directive_definition::main(cflat_prs_printer& c,
		const node_args_type& n) const {
	NEVER_NULL(_main);
	const param_args_type empty;
	(*_main)(c, empty, n);
}

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

