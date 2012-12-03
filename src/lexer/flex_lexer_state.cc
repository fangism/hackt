/**
	\file "lexer/flex_lexer_state.cc"
	$Id: flex_lexer_state.cc,v 1.3 2008/03/21 00:20:13 fang Exp $
 */

#include "lexer/flex_lexer_state.hh"
#include "lexer/flex_lexer_common.hh"
#include "lexer/flex_buffer_state.hh"	// auto-generated
#include "util/macros.h"

// TODO: re-generate "flex_lexer_common.cc" as member functions?

extern "C" {
int
yywrap(void) {
	return 1;
}
}

namespace flex {
//=============================================================================
// struct lexer_state method definitions

/**
	\param str a NULL-terminated string to use for lexing.
 */
lexer_state::lexer_state(const char* str) : yyin(NULL), yyout(NULL),
		yy_current_buffer(NULL),
		yy_c_buf_p(NULL), yy_init(1), yy_start(0), 
		yy_more_flag(0), yy_more_len(0), 
		yy_flex_debug(0)
		FLEX_LEXER_CTOR_EXTRA_INIT
		{
	yy_scan_string(str, *this);
	// sets yy_current_buffer in yy_switch_to_buffer
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	See notes on one-time memory leak:
http://flex.sourceforge.net/manual/Memory-leak-_002d-16386-bytes-allocated-by-malloc_002e.html
	This is actually not critical, but will satisfy memory checkers
	like valgrind.
 */
lexer_state::~lexer_state() {
#ifdef	LEXER_HAS_YYLEX_DESTROY
	// flex 2.5.31
	// newer flex supports stack of buffers
	yylex_destroy(*this);
#else
	// flex 2.5.4
	// old flex only supported one current_buffer
	yy_delete_buffer(yy_current_buffer, *this);
	// yy_init = 1;		// unnecessary at end-of-life
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Public function that indicates whether or not the lexer is
	in the EOF (end-of-file) state.  
	This must be defined in this file because it makes reference
	to a statically linked variable, (which makes it invisible 
	to the outside world).  
 */
int
lexer_state::at_eof(void) const {
	const lexer_state& foo(*this);
	assert(YY_CURRENT_BUFFER);
	// return YY_CURRENT_BUFFER->yy_n_chars == 0;
	return YY_CURRENT_BUFFER->YY_N_CHARS == 0;
	// was yy_n_chars until "lexer/purify_flex.awk"
}

//=============================================================================
}	// end namespace flex

