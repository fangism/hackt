/**
	\file "lexer/hacflat-yystype.h"
	The symbol type passed to hacflat's yylex.  
	$Id: hacflat-yystype.h,v 1.1 2006/11/15 00:08:55 fang Exp $
 */

#ifndef	__HAC_LEXER_HACFLAT_YYSTYPE_H__
#define	__HAC_LEXER_HACFLAT_YYSTYPE_H__

#include "util/string_fwd.h"

namespace HAC {
namespace lexer {

enum hacflat_tokentype {
	/**
		To signal that an import directive was detected. 
	 */
	HF_IMPORT = 258,
	/**
		String literal tokenized.  
	 */
	HF_STRING = 259,
	/**
		All other non-special text without newline.
	 */
	HF_OTHER_TEXT = 260,
	/**
		Text ending with newline.  
	 */
	HF_TEXT_NEWLINE = 261
};

/**
	More overhead but safer for now.  
	If we're careful, we can use null-terminated const char*.  
 */
typedef	std::string	hacflat_yystype;

}	// end namespace lexer
}	// end namespace HAC

#endif	// __HAC_LEXER_HACFLAT_YYSTYPE_H__

