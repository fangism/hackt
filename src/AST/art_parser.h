/**
	\file "AST/art_parser.h"
	Includes all parser-related header files.  
	Using this without thinking may slow down compilations
	unnecessarily.  
	$Id: art_parser.h,v 1.7.50.1 2005/11/12 07:48:59 fang Exp $
 */

#ifndef	__AST_ART_PARSER_H__
#define	__AST_ART_PARSER_H__

#include "AST/art_parser_base.h"	// minimal parser interface base classes

#include "AST/art_parser_node_position.h"
#include "AST/import_root.h"
#include "AST/art_parser_expr.h"
#include "AST/art_parser_range_list.h"
#include "AST/art_parser_token.h"
#include "AST/art_parser_token_char.h"
#include "AST/art_parser_token_string.h"
#include "AST/art_parser_formal.h"
#include "AST/art_parser_definition.h"
#include "AST/art_parser_instance.h"

#include "AST/art_parser_prs.h"
#include "AST/art_parser_chp.h"
#include "AST/art_parser_hse.h"

#endif	// __AST_ART_PARSER_H__

