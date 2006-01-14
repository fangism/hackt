/**
	\file "lexer/flex_lexer_state.h"
	Structure holding all of the flex scanner's stateful information.  
	$Id: flex_lexer_state.h,v 1.2.4.1 2006/01/14 04:18:43 fang Exp $
 */

#ifndef	__LEXER_FLEX_LEXER_STATE_H__
#define	__LEXER_FLEX_LEXER_STATE_H__

/**
	Only enable this header inside a flex-generated scanner.  
	No one else has any business using this.  
	Well, except the parser, because it needs to create one
	and pass in the state...
 */
#if	defined(FLEX_SCANNER) || defined(YYBISON) || defined(YYBYACC)

#include "util/FILE_fwd.h"
#include "util/NULL.h"


#if	!defined(FLEX_SCANNER)
// because parser file needs at least a forward declaration
typedef	struct yy_buffer_state*	YY_BUFFER_STATE;
#endif

/**
	Namespace for flex-related hacks.  
 */
namespace flex {

/**
	What was global state information has been ported into this
	struct.  
 */
struct lexer_state {
	typedef	int		yy_state_type;

	int			yyleng;
	FILE*			yyin;			// = NULL;
	FILE*			yyout;			// = NULL;
	/**
		This needs to be defined beforehand.  
	 */
	YY_BUFFER_STATE		yy_current_buffer;	// = NULL;
	char			yy_hold_char;
	int			yy_n_chars;
	char*			yy_c_buf_p;		// = NULL;
	int			yy_init;		// = 1;
	int			yy_start;		// = 0; state number
	int			yy_did_buffer_switch_on_eof;
	char*			yytext;
	yy_state_type		yy_last_accepting_state;
	char*			yy_last_accepting_cpos;
	int			yy_more_flag;		// = 0;
	int			yy_more_len;		// = 0;

/**
	We're not using the stack, don't worry about it for now.  
 */
#if 0 && YY_STACK_USED
	int			yy_start_stack_ptr;	// = 0;
	int			yy_start_stack_depth;	// = 0;
	int*			yy_start_stack;		// = NULL;
#endif
// the following appear in flex 2.5.31
	int			yylineno;		// = 0;
	size_t			yy_buffer_stack_top;	// = 0;
	size_t			yy_buffer_stack_max;	// = 0;
	YY_BUFFER_STATE*	yy_buffer_stack;	// = NULL;

	/**
		Default constructor initializes the values faithfully
		from the flex boilerplate code.  
	 */
	lexer_state() : yyin(NULL), yyout(NULL),
			yy_current_buffer(NULL), 
			yy_c_buf_p(NULL), yy_init(1), yy_start(0), 
			yy_more_flag(0), yy_more_len(0), 
			yylineno(0), 
			yy_buffer_stack_top(0), 
			yy_buffer_stack_max(0), 
			yy_buffer_stack(NULL) { }

	explicit
	lexer_state(FILE* _yyin) : yyin(_yyin), yyout(NULL),
			yy_current_buffer(NULL), 
			yy_c_buf_p(NULL), yy_init(1), yy_start(0), 
			yy_more_flag(0), yy_more_len(0), 
			yylineno(0), 
			yy_buffer_stack_top(0), 
			yy_buffer_stack_max(0), 
			yy_buffer_stack(NULL) { }

	/**
		For now, default destructor, trusting the lexer
		to clean itself up on EOF.  
	 */
	~lexer_state() { }

};	// end struct lexer_state

}	// end namespace flex

#endif	// FLEX_SCANNER

#endif	// __LEXER_FLEX_LEXER_STATE_H__

