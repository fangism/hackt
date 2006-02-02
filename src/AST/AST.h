/**
	\file "AST/AST.h"
	Includes all parser-related header files.  
	Using this without thinking may slow down compilations unnecessarily.  
	$Id: AST.h,v 1.2.16.1 2006/02/02 22:44:53 fang Exp $
	This file lived under the following name prior to renaming.  
	Id: art_parser.h,v 1.8.2.1 2005/12/11 00:45:02 fang Exp
 */

#ifndef	__HAC_AST_AST_H__
#define	__HAC_AST_AST_H__

#include "AST/common.h"	// minimal parser interface base classes

#include "AST/node_position.h"
#include "AST/import_root.h"
#include "AST/expr.h"
#include "AST/range_list.h"
#include "AST/token.h"
#include "AST/token_char.h"
#include "AST/token_string.h"
#include "AST/formal.h"
#include "AST/definition.h"
#include "AST/instance.h"

#include "AST/PRS.h"
#include "AST/CHP.h"
#include "AST/HSE.h"
#include "AST/SPEC.h"

#endif	// __HAC_AST_AST_H__

