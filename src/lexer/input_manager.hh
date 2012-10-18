// aliased to different file name so "parser/lexyacc-prefix.awk" 
// won't rename it.  
#include "lexer/yyin_manager.hh"

namespace HAC {
namespace lexer {
typedef	yyin_manager		input_manager;
}	// end namespace lexer
}	// end namespace HAC

