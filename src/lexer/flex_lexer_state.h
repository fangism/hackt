/**
	\file "lexer/flex_lexer_state.h"
	Structure holding all of the flex scanner's stateful information.  
	$Id: flex_lexer_state.h,v 1.7.10.2 2008/03/11 22:02:11 fang Exp $
 */

#ifndef	__LEXER_FLEX_LEXER_STATE_H__
#define	__LEXER_FLEX_LEXER_STATE_H__

// #include "config.h"
#include "util/FILE_fwd.h"
#include "util/NULL.h"
#include "util/size_t.h"


#if	!defined(FLEX_SCANNER)
// because parser file needs at least a forward declaration
typedef	struct yy_buffer_state*	YY_BUFFER_STATE;
#endif

/**
	Namespace for flex-related hacks.  
 */
namespace flex {

//=============================================================================
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
	/**
		Should this char buffer be freed?  by whom?
	 */
	char*			yy_c_buf_p;		// = NULL;
	int			yy_init;		// = 1;
	int			yy_start;		// = 0; state number
	int			yy_did_buffer_switch_on_eof;
	/**
		Trusting the lexer to free this upon EOF.
		TODO: But what if lexer error is encountered?
	 */
	char*			yytext;
	yy_state_type		yy_last_accepting_state;
	char*			yy_last_accepting_cpos;
	int			yy_more_flag;		// = 0;
	int			yy_more_len;		// = 0;

/**
	We're not using the stack, don't worry about it for now.  
 */
#if 0
#if YY_STACK_USED
	int			yy_start_stack_ptr;	// = 0;
	int			yy_start_stack_depth;	// = 0;
	int*			yy_start_stack;		// = NULL;
#endif
#endif
// the following appear in flex 2.5.31 or later
	/**
		Run-time debug switch.
		In flex 2.5.33.
	 */
	int			yy_flex_debug;
#ifdef LEXER_HAS_YYLINENO
	int			yylineno;		// = 0;
#define	FLEX_LEXER_CTOR_INIT_YYLINENO	, yylineno(0)
#else
#define	FLEX_LEXER_CTOR_INIT_YYLINENO
#endif

#ifdef LEXER_HAS_BUFFER_STACK
	size_t			yy_buffer_stack_top;	// = 0;
	size_t			yy_buffer_stack_max;	// = 0;
	YY_BUFFER_STATE*	yy_buffer_stack;	// = NULL;
#define	FLEX_LEXER_CTOR_INIT_BUFFER_STACK				\
			, yy_buffer_stack_top(0) 			\
			, yy_buffer_stack_max(0) 			\
			, yy_buffer_stack(NULL)
#else
#define	FLEX_LEXER_CTOR_INIT_BUFFER_STACK
#endif

#define	FLEX_LEXER_CTOR_EXTRA_INIT					\
	FLEX_LEXER_CTOR_INIT_YYLINENO					\
	FLEX_LEXER_CTOR_INIT_BUFFER_STACK

	/**
		Default constructor initializes the values faithfully
		from the flex boilerplate code.  
	 */
	lexer_state() : yyin(NULL), yyout(NULL),
			yy_current_buffer(NULL), 
			yy_c_buf_p(NULL), yy_init(1), yy_start(0), 
			yy_more_flag(0), yy_more_len(0), 
			yy_flex_debug(0)
			FLEX_LEXER_CTOR_EXTRA_INIT
			{ }

	explicit
	lexer_state(FILE* _yyin) : yyin(_yyin), yyout(NULL),
			yy_current_buffer(NULL), 
			yy_c_buf_p(NULL), yy_init(1), yy_start(0), 
			yy_more_flag(0), yy_more_len(0), 
			yy_flex_debug(0)
			FLEX_LEXER_CTOR_EXTRA_INIT
			{ }

	explicit
	lexer_state(const char* str);

	/**
		For now, default destructor, trusting the lexer
		to clean itself up on EOF.  
	 */
	~lexer_state();

	int
	at_eof(void) const;

};	// end struct lexer_state
}	// end namespace flex

//=============================================================================
// wanna-be member functions, common functions extracted from flex skeletons

//=============================================================================

#endif	// __LEXER_FLEX_LEXER_STATE_H__

