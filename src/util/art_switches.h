/**
	\file "art_switches.h"
	Global variables for ART front-end.  
 */

#ifndef	__ART_SWITCHES_H__
#define	__ART_SWITCHES_H__

#include "art_lex.h"

// for lexer
extern	ART::token_position current;		/* read-only please */

extern	int	allow_nested_comments;
extern	int	allow_nested_sources;

extern	int	token_feedback;
extern	int	string_feedback;
extern	int	comment_feedback;
extern	int	source_feedback;

// for parser

#endif	// __ART_SWITCHES_H__

