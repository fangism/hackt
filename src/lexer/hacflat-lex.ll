/**
 *	\file "lexer/hacflat-lex.ll"
 *	vi: ft=lex
 *	Will flattens a HAC source file into a single file by dumping
 *	imported files (visit-once only).  
 *	$Id: hacflat-lex.ll,v 1.9 2010/04/30 00:21:43 fang Exp $
 */

/****** DEFINITIONS **********************************************************/

%{
/* scanner-specific header */

#define	ENABLE_STACKTRACE		(0 && !defined(LIBBOGUS))

#include <iostream>
#include <iomanip>
#include <cstdlib>

#ifdef	LIBBOGUS
// HACK: prevent the inclusion of "parser/hackt-prefix.h"
#define	__LEXYACC_HACKT__PREFIX_H__
#endif

#include "util/macros.h"
#include "util/using_ostream.h"
#include "util/boolean_types.h"
#include "lexer/input_manager.h"
#include "lexer/file_manager.h"
#include "lexer/hacflat-yystype.h"

#include "lexer/hac_lex.h"
#include "lexer/hacflat-lex-options.h"
#include "lexer/flex_lexer_state.h"
#include "util/stacktrace.h"
#include "util/sstream.h"
using flex::lexer_state;

/**
	This is the file stack and include path manager for 
	the hackt parser.  
	This is globally visible and accessible (unfortunately).  
 */
extern HAC::lexer::file_manager
hackt_parse_file_manager;

#if 0
/// generated in "parser/hackt-union.cc" for deleting tokens
extern
void
yy_union_lookup_delete(const YYSTYPE&, const int);

extern
std::ostream&
yy_union_lookup_dump(const YYSTYPE&, const int, std::ostream&);
#endif

namespace HAC {
namespace lexer {

//=============================================================================
// global variable (flag)

/**
	Set to true to print with "#FILE %{ ... %} // #FILE" 
	hierarchy wrappers.
	Set to false to suppress.  
	NOTE: The caller is responsible for properly initializing this!
	ubuntu6-x86-linux-gcc-4.0.3:
		if initialized with value here, gets placed into 
		initialized data section, but doesn't get properly linked
		with undefined reference error, so we do NOT initialize here.
 */
static bool
flatten_with_file_wrappers = true;

/**
	Mutator interface to avoid data symbol linkage fiasco...
 */
void
flatten_with_wrappers(const bool b) {
	flatten_with_file_wrappers = b;
}

//=============================================================================

// defined in here, at the end of file
extern
util::good_bool
__flatten_source(FILE*);

// MOST of this ripped from "lexer/hackt-lex.ll"
/**
	Maximum string length.  Can be extended arbitrarily.  
 */
#define	STRING_MAX_LEN		32768

/** line and position tracking data for tokens */
#define	CURRENT		hackt_parse_file_manager.current_position()
static	token_position comment_pos(1, 0, 1);
static	token_position string_pos(1, 0, 1);
	/* even though strings may not be multi-line */
/***
	Observation: comments will never contain strings to lex, 
		nor will strings ever lex comments.  
	Also note that a file cannot be referenced while inside a 
		comment or string.  
***/
/* for string matching and escape sequence expansion */
static	char string_buf[STRING_MAX_LEN];
static	char* string_buf_ptr = string_buf;

/**
	Thie macro is intended for use with ostream& operator << .
	\param c is a token_position.  
 */
#define	LINE_COL(c)	"on line " << c.line << ":" << c.col

static int allow_nested_comments = 0;
static int comment_level = 0;		/* useful for nested comments */

/* debugging switches -- consider making these macro-defined */
static const int token_feedback = 0;
static const int string_feedback = 0;
static const int comment_feedback = 0;		/* reporting of comment state */
	/*	0 = off, 
		1 = nested levels only, 
		2 = null and endline comments, 
		3 = ignored text feedback details
	*/

/**
	Debugging tool.  
	This will generate excessive feedback for every detailed
	action of the lexer, even in the middle of tokenizing.  
 */
static inline void
DEFAULT_TOKEN_FEEDBACK(const lexer_state& foo) {
	if (token_feedback) {
		cerr << "token = " << yytext <<
			" " LINE_COL(CURRENT) << endl;
	}
}

/* macros for tracking single line tokens (no new line) */

static inline void
TOKEN_UPDATE(const lexer_state& foo) {
	DEFAULT_TOKEN_FEEDBACK(foo);
	CURRENT.col += yyleng;
}


/**
	Now this flushes out the line buffer. 
	Do this before and after import directive.  
 */
static inline void
NEWLINE_UPDATE(void) {
	CURRENT.line++;
	CURRENT.col = 1;
#if 0
	cerr << "Line number advanced to " << CURRENT.line << endl;
#endif
}

static inline void
STRING_UPDATE(const lexer_state& foo) {
	TOKEN_UPDATE(foo);
	assert(string_buf_ptr -string_buf < STRING_MAX_LEN);
}

static inline void
STRING_FINISH(hacflat_yystype& _lval, lexer_state& foo) {
	STRING_UPDATE(foo);
	_lval = string_buf;	// copy: string = const char*
	string_buf_ptr = string_buf;	// reset
}

static inline void
TEXT_FINISH(hacflat_yystype& _lval, lexer_state& foo) {
	STRING_UPDATE(foo);
	_lval = yytext;		// copy: string = const char*
}

/**
	This is obsolete, because no tokens span multiple lines, 
	not even comments.  (Comments are returned as normal text.)
	macros for tracking long, multiline tokens
	pass into p either source_pos or comment_pos.
 */
static inline void
MULTILINE_START(token_position& p, const lexer_state& foo) {
	p.col = CURRENT.col;
	p.line = CURRENT.line;
	p.leng = yyleng;
//	TOKEN_UPDATE(foo);
}

#if 0
static inline void
MULTILINE_MORE(const token_position& p, const lexer_state& foo) {
	if (p.line == CURRENT.line) {
		CURRENT.col = yyleng +p.col
#if USE_TOKEN_POSITION_OFFSET
			+p.off
#endif
		;
	} else {
		CURRENT.col = yyleng -p.leng;
	}
}
#endif

#if 0
static inline void
MULTILINE_NEWLINE(token_position& p, const lexer_state& foo) {
	p.leng = yyleng -1;
	NEWLINE_UPDATE();
}
#endif

}	/* end namespace lexer */
}	/* end namespace HAC */

using namespace HAC::lexer;

%}

WHITESPACE	[ \t]+
NEWLINE		"\n"
WS		{WHITESPACE}

SEMICOLON	[;]

ENDLINECOMMENT	"//"(.*)$
NULLCOMMENT	"/*"[^\n]*"*/"
OPENCOMMENT	"/*"
CLOSECOMMENT	[*]+"/"
CLOSEINCOMMENT	"*/"

OPENSTRING	"\""
MORESTRING	[^\\\"\n]+
CLOSESTRING	"\""
FILESTRING	"\"[^\"]+\""

/* need to catch ID to prevent matching of keywords in substrings */
/* also allow pass bad identifiers through */
IDHEAD		[a-zA-Z_]
IDBODY		[a-zA-Z0-9_]
ID		{IDHEAD}{IDBODY}*
FAKEID		{IDBODY}*

OCTAL_ESCAPE	"\\"[0-7]{1,3}
BAD_ESCAPE	"\\"[0-9]+

/****** keywords ****/
IMPORT		"import"

/****** states ******/
%s incomment
%s instring
%s inescape

/*
	Explicitly stating options to guarantee proper definition of 
	macros in the generated source file, because I've turned on
	-Wundef for all translation units.  
 */
%option never-interactive
%option nomain
%option nostack
/** I wish! **/
/** 	%option reentrant	**/

/****** rules ****************************************************************/
%%

<INITIAL>{

{WS}?{SEMICOLON}	{
	STACKTRACE("got \';\'");
	TEXT_FINISH(hacflat_lval, foo);
	return ';';
}

{IMPORT}{WS}? {
	STACKTRACE("lexing import");
#if ENABLE_STACKTRACE
#define	DUMP_FILE_NAME_STACK(ostr)					\
	hackt_parse_file_manager.dump_file_names(ostr) << endl
#else
#define	DUMP_FILE_NAME_STACK(ostr)
#endif

	DUMP_FILE_NAME_STACK(cerr);
/***
	That's right, manually opening the file in the lexer.  
	Can't necessarily count on the [LA]LR parser to do this properly.  
***/
	// const int bol = CURRENT.col;
	/* need some string-hacking to extract file name */
	TOKEN_UPDATE(foo);
	// meh, just re-use the passed in lval, rather than local temporary
	// hacflat_yystype& temp(hacflat_lval);
	/* OK to reuse this lexer state in recursive lex */
	{
	const int expect_string = __hacflat_lex(hacflat_lval, foo);
	if (expect_string != HF_STRING) {
		cerr << "\nExpecting \"file\" after import." << endl;
		const string& pstr(hackt_parse_file_manager.top_FILE_name());
		cerr << "From: \"" << pstr << "\":" <<
			CURRENT.line << ':' << endl;
		THROW_EXIT;
	}
	}
	// STRING_FINISH(hacflat_lval, foo);
	const string fstr(hacflat_lval);
	/* claim the semicolon first before opening file */
	{
	const int expect_semi = __hacflat_lex(hacflat_lval, foo);
	if (expect_semi != ';') {
		cerr << "\nExpecting \';\' after import \"...\"." << endl;
		// cerr << "got: " << expect_semi << endl;
		const string& pstr(hackt_parse_file_manager.top_FILE_name());
		cerr << "From: \"" << pstr << "\":" <<
			CURRENT.line << ':' << endl;
		THROW_EXIT;
	}
	}
	/* NODE_POSITION_UPDATE(*hackt_lval, foo); */
	/* excl_ptr will delete token if unused */
	// const excl_ptr<const node_position> ssp(temp._node_position);
	// cerr << fstr << endl;
	// cerr << "current FILE* (before) = " << yyin << endl;
	// TODO: better error reporting and handling, when I have time...
	// try to open the file, using search paths (true)
	const input_manager ym(hackt_parse_file_manager, fstr.c_str(), true);
	// get the full path to the file name
	const file_status::status stat = ym.get_status();
	switch(stat) {
	case file_status::NEW_FILE: {
		STACKTRACE("import new file");
		DUMP_FILE_NAME_STACK(cerr);
		const string& pstr(hackt_parse_file_manager.top_FILE_name());
		if (CURRENT.col != 1) { cout << "\n"; }
		// wrap file's contents in a #FILE embedding directive:
		// EMBEDFILE STRING BEGINFILE embedded_module ENDFILE
		// TODO: maintain consistency with
		//	"$(top_srcdir)/test/vpath_file_filter.awk"
	if (!flatten_with_file_wrappers) {
		cout << "// ";		// just comment out
	}
		cout << "#FILE \"" << pstr << "\" %{" << endl;
		// cout << "// enter: \"" << pstr << "\"" << endl;
		if (!__flatten_source(ym.get_file()).good) {
			// presumably already have error message from callee
#if 0
			cerr << "From: \"" << pstr << "\":" <<
				CURRENT.line << ':' << endl;
#endif
			THROW_EXIT;
		}
	if (!flatten_with_file_wrappers) {
		cout << "// ";		// just comment out
	}
		cout << "%}\t// #FILE \"" << pstr << "\"";	// << endl;
		// the newline at the end of the import is already preserved
		// cout << "// leave: \"" << pstr << "\"" << endl;
		return HF_IMPORT;
	}
	case file_status::SEEN_FILE: {
		STACKTRACE("old file");
		DUMP_FILE_NAME_STACK(cerr);
		// true: already seen file, this will be a placeholder
		return HF_IMPORT;
	}
	case file_status::CYCLE: {
		STACKTRACE("cyclic file!");
		hackt_parse_file_manager.dump_file_stack(cerr);
		cerr << "Detected cyclic file dependency: " << fstr << endl;
		THROW_EXIT;
	}
	case file_status::NOT_FOUND: {
		STACKTRACE("file not found");
		hackt_parse_file_manager.dump_file_stack(cerr);
		cerr << "Unable to open file: " << fstr << endl;
		THROW_EXIT;
	}
	default:
		abort();
	}
#undef	DUMP_FILE_NAME_STACK
}

{FAKEID}	{
	STACKTRACE("got FAKEID");
	TEXT_FINISH(hacflat_lval, foo);
	return HF_OTHER_TEXT;
}

{NEWLINE}	{ NEWLINE_UPDATE(); hacflat_lval = ""; return HF_TEXT_NEWLINE; }

{NULLCOMMENT} { 
	if (comment_feedback > 1) {
		cerr << "null comment ignored " << LINE_COL(CURRENT) << endl;
	}
	TEXT_FINISH(hacflat_lval, foo);
	return HF_OTHER_TEXT;
}

{CLOSECOMMENT} {
	cout << yytext << endl;
	hackt_parse_file_manager.dump_file_stack(cerr);
	cerr << "*/ (close-comment) found outside of <comment> " <<
		LINE_COL(CURRENT) << endl;
	TOKEN_UPDATE(foo);
	THROW_EXIT;
}

{OPENCOMMENT} {
	/* crazy... allowing nested comments */
	comment_level++;
	if (comment_feedback) {
		cerr << "start of comment-level " << comment_level << " " <<
			LINE_COL(CURRENT) << endl;
	}
	// yymore();
	MULTILINE_START(comment_pos, foo);
	TEXT_FINISH(hacflat_lval, foo);
	BEGIN(incomment); 
	return HF_OTHER_TEXT;
}

{OPENSTRING}	{
	STACKTRACE("start string");
	if (string_feedback) {
		cerr << "start of quoted-string " << LINE_COL(CURRENT) << endl;
	}
	/* no yymore(), skip open quote */
	string_buf_ptr = string_buf;
	MULTILINE_START(string_pos, foo);	/* just for column positioning */
	TOKEN_UPDATE(foo);
	BEGIN(instring); 
}

{ENDLINECOMMENT} {
	if (comment_feedback > 1) {
		cerr << "end-of-line comment ignored " <<
			LINE_COL(CURRENT) << endl;
	}
	// NEWLINE_UPDATE();
	TEXT_FINISH(hacflat_lval, foo);	// includes '\n'
	return HF_OTHER_TEXT;
}

[^\"\n/i*;]+	{
	/*
		Everything else including comments and whitespace 
		should be echoed to stdout.
		We stop at the letter 'i' to detect imports.  
		We stop at '*' to detect premature end-of-comments.  
		We stop at ';' to detect semicolons, duh!
		This scheme minimizes the number of returns, since we have
		no other keywords or identifiers.  
	 */
	TEXT_FINISH(hacflat_lval, foo);
	return HF_OTHER_TEXT;
}

.	{
	/* everything including comments should be echoed to stdout */
	TEXT_FINISH(hacflat_lval, foo);
	return HF_OTHER_TEXT;
}
}

<incomment>{

[^*/\n]*	|
[/][^*\n]*	{
	if (comment_feedback > 2) {
		cerr << "eaten up more comment " << LINE_COL(CURRENT) << endl;
	}
	// MULTILINE_MORE(comment_pos, foo);
	// yymore();
	TEXT_FINISH(hacflat_lval, foo);
	return HF_OTHER_TEXT;
}

[^/\n]*{CLOSEINCOMMENT} {
	// MULTILINE_MORE(comment_pos, foo);
	if (comment_feedback) {
		cerr << "end of comment-level " << comment_level << " " <<
			LINE_COL(CURRENT) << endl;
	}
	comment_level--;
	if (!comment_level) {
		BEGIN(INITIAL); 
		TEXT_FINISH(hacflat_lval, foo);
		return HF_OTHER_TEXT;
	} else {
		yymore();		// doesn't seem to make a difference
	}
}

[*]+[^/\n]*	{
	if (comment_feedback > 2) {
		cerr << "eaten up more comment " << LINE_COL(CURRENT) << endl;
	}
	// MULTILINE_MORE(comment_pos, foo);
	// yymore();
	TEXT_FINISH(hacflat_lval, foo);
	return HF_OTHER_TEXT;
}

{OPENCOMMENT} {
	if (allow_nested_comments) {
		comment_level++;
		if (comment_feedback) {
			cerr << "start of comment-level " << comment_level <<
				" " << LINE_COL(CURRENT) << endl;
		}
		// MULTILINE_MORE(comment_pos, foo);
		yymore();
	} else {
		hackt_parse_file_manager.dump_file_stack(cerr);
		cerr << "nested comments forbidden, found /* " <<
			LINE_COL(CURRENT) << endl;
		THROW_EXIT;
	}
}

{NEWLINE}	{
	if (comment_feedback > 1) {
		cerr << "new-line in comment at end of line "
			<< CURRENT.line << endl;
	}
	// MULTILINE_NEWLINE(comment_pos, foo);	// prints and flushes
	// yymore();
	NEWLINE_UPDATE();
	TEXT_FINISH(hacflat_lval, foo);	// includes newline
	return HF_OTHER_TEXT;
}

<<EOF>>	{
	// cerr << "in-comment-EOF!" << endl;
	// in this case, yywrap() is already called too early, and we don't get
	// the full file-stack, thus we must push some error handling into
	// yywrap() and detect the state there.  
	// MULTILINE_MORE(comment_pos, foo);
	hackt_parse_file_manager.dump_file_stack_top(cerr);
	cerr << "unterminated comment, starting on line "
		<< comment_pos.line << ", got <<EOF>>" << endl;
	THROW_EXIT;
}

}

<instring>{
{NEWLINE}	{
	*string_buf_ptr = '\0';		/* null-terminate */
	cout << '\"' << string_buf << endl;
	hackt_parse_file_manager.dump_file_stack(cerr);
	cerr << "unterminated quoted-string on line " << CURRENT.line <<
		", got \\n" << endl;
	STRING_UPDATE(foo);
	THROW_EXIT;
}

{MORESTRING} {
	STACKTRACE("more string...");
	char *copy = yytext;
	while (*copy) {			/* until null-termination */
		*string_buf_ptr++ = *copy++;
	}
	/* don't really need yymore, just for tracking column position here */
	STRING_UPDATE(foo);
}

{CLOSESTRING}	{
	STACKTRACE("end string");
	*string_buf_ptr = '\0';		/* null-terminate */
	if (string_feedback) {
		cerr << "end of quoted-string: \"" << string_buf << "\" " <<
			LINE_COL(CURRENT) << endl;
	}
	*yytext = '\0';
	STRING_FINISH(hacflat_lval, foo);
	BEGIN(INITIAL);
	return HF_STRING;
}

"\\0"	|
"\\b"	|
"\\f"	|
"\\n"	|
"\\r"	|
"\\t"	|
"\\\\"	|
"\\\'"	|
"\\\""	|
{OCTAL_ESCAPE}	{
#if 0
	unsigned int result;
	sscanf(yytext +1, "%o", &result);
	if ( result > 0xff ) {
		hackt_parse_file_manager.dump_file_stack(cerr);
		cerr << "bad octal escape sequence " << yytext << " " <<
			LINE_COL(CURRENT) << endl;
		THROW_EXIT;
	}
	*string_buf_ptr++ = result;
#else
	char *copy = yytext;
	while (*copy) {			/* until null-termination */
		*string_buf_ptr++ = *copy++;
	}
#endif
	STRING_UPDATE(foo);
}

{BAD_ESCAPE}	{
	hackt_parse_file_manager.dump_file_stack(cerr);
	cerr << "bad octal escape sequence " << yytext << " " <<
		LINE_COL(CURRENT) << endl;
	THROW_EXIT;
}


<<EOF>>	{
	// for same reason as stated above, in EOF in COMMENT
	// we push some error handling into yywrap().  
	*string_buf_ptr = '\0';		/* null-terminate */
	cout << '\"' << string_buf << endl;
	hackt_parse_file_manager.dump_file_stack(cerr);
	cerr << "unterminated string, starting on line " << CURRENT.line << 
		", got <<EOF>>" << endl;
	THROW_EXIT;
}

}

%%
/****** user-code ************************************************************/

namespace HAC {
namespace lexer {
using util::good_bool;

/**
	Internal routine for flattening source.  
	\param infile the input FILE stream, assumed valid.  
	NOTE: this uses the global hackt_parse_file_manager.  
 */
good_bool
__flatten_source(FILE* infile) {
	NEVER_NULL(infile);
	try { 
		// LOOP
		flex::lexer_state _lexer_state(infile);
		string dummy;
		int ret;
		while ((ret = __hacflat_lex(dummy, _lexer_state))) {
		switch (ret) {
		case HAC::lexer::HF_IMPORT:
			// could print a file/line directive
			break;  
		case HAC::lexer::HF_STRING:
			cout << '\"' << dummy << '\"';
			break;
		case HAC::lexer::HF_OTHER_TEXT: // includes comments
			cout << dummy;
			break;
		case HAC::lexer::HF_TEXT_NEWLINE:
			cout << dummy << endl;
			break;
		case ';':
			cout << dummy;  // may be preceded by whitespace
			break;
		default:
			cerr << "FATAL: Got unexpected token: " << ret << endl;
			throw std::exception();
		}
		} 
	} catch (...) { 
#if 0
		cerr << "From: \"" << pstr << "\":" <<
			CURRENT.line << ':' << endl;
#endif
		/* error message? */
		return good_bool(false);
		/* or rethrow? */
	}       
	return good_bool(true);
}

}	/* end namespace lexer */
}	/* end namespace HAC */

