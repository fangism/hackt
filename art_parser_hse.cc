// "art_parser_hse.cc"

#include "art_parser_template_methods.h"
#include "art_parser_hse.h"

namespace ART {
namespace parser {
namespace HSE {
//=============================================================================

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS -- entire classes

template class node_list<statement,semicolon>;		// HSE::stmt_list
template class node_list<guarded_command,thickbar>;	// HSE::det_sel_base
							// HSE::prob_sel_base
template class node_list<guarded_command,colon>;	// HSE::nondet_sel_base

//=============================================================================
};	// end namespace HSE
};	// end namespace parser
};	// end namespace ART

