// "art_parser_chp.cc"

#include "art_parser_template_methods.h"
#include "art_parser_chp.h"

namespace ART {
namespace parser {
namespace CHP {
//=============================================================================

//=============================================================================
// EXPLICIT TEMPLATE INSTANTIATIONS -- entire classes

template class node_list<statement,semicolon>;		// CHP::stmt_list
template class node_list<guarded_command,thickbar>;	// CHP::det_sel_base
							// CHP::prob_sel_base
template class node_list<guarded_command,colon>;	// CHP::nondet_sel_base
template class node_list<communication,comma>;		// CHP::comm_list

//=============================================================================
};	// end namespace CHP
};	// end namespace parser
};	// end namespace ART

