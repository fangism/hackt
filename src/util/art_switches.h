/**
	\file "art_switches.h"
	Global variables for ART front-end.  
	$Id: art_switches.h,v 1.4.42.1 2005/02/24 06:17:16 fang Exp $
 */

#ifndef	__ART_SWITCHES_H__
#define	__ART_SWITCHES_H__

#include "art_lex.h"

// for lexer
extern	ART::lexer::token_position current;	/* read-only please */

extern	int	allow_nested_comments;
extern	int	allow_nested_sources;

extern	int	token_feedback;
extern	int	string_feedback;
extern	int	comment_feedback;
extern	int	source_feedback;

// for parser

#endif	// __ART_SWITCHES_H__

