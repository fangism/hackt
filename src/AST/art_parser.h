/**
	\file "art_parser.h"
	Includes all parser-related header files.  
	Using this without thinking may slow down compilations
	unnecessarily.  
	$Id: art_parser.h,v 1.5 2005/03/06 22:45:48 fang Exp $
 */

#ifndef	__ART_PARSER_H__
#define	__ART_PARSER_H__

#include "art_parser_base.h"	// minimal parser interface base classes

#include "art_parser_expr.h"
#include "art_parser_range_list.h"
#include "art_parser_token.h"
#include "art_parser_token_char.h"
#include "art_parser_token_string.h"
#include "art_parser_formal.h"
#include "art_parser_definition.h"
#include "art_parser_instance.h"

#include "art_parser_prs.h"
#include "art_parser_chp.h"
#include "art_parser_hse.h"

#endif	// __ART_PARSER_H__

