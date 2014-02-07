/**
	\file "AST/AST.hh"
	Includes all parser-related header files.  
	Using this without thinking may slow down compilations unnecessarily.  
	$Id: AST.hh,v 1.4 2006/07/30 05:49:09 fang Exp $
	This file lived under the following name prior to renaming.  
	Id: art_parser.h,v 1.8.2.1 2005/12/11 00:45:02 fang Exp
 */

#ifndef	__HAC_AST_AST_H__
#define	__HAC_AST_AST_H__

#include "AST/common.hh"	// minimal parser interface base classes

#include "AST/node_position.hh"
#include "AST/import_root.hh"
#include "AST/expr.hh"
#include "AST/reference.hh"
#include "AST/range_list.hh"
#include "AST/token.hh"
#include "AST/token_char.hh"
#include "AST/token_string.hh"
#include "AST/formal.hh"
#include "AST/definition.hh"
#include "AST/instance.hh"

#include "AST/PRS.hh"
#include "AST/RTE.hh"
#include "AST/CHP.hh"
#include "AST/HSE.hh"
#include "AST/SPEC.hh"

#endif	// __HAC_AST_AST_H__

