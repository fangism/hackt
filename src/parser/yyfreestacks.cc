// "parser/yyfreestacks.cc"
// template file to be included in error-handling code of parsers
// pasted here for ease of maintainence and reuse

/**
	Upon error or exception, must clean up stacks!
	Now clean-up the symbol stack by calling destructors.
	Technically, this is not needed, as bulk memory is 
	reclaimed upon exit().  (This is a good exercise anyhow.)
	We are currently assuming that no other handler will
	take care of deleting the pointers on the stack.  

	OBSOLETE COMMENT, after removing mother-node type.  
	Because the union-pointer resolution can only return
	one type, the base type, the mother destructor, 
	HAC::parser::node::~node(), must be virtual.  
 */
static
void
yyfreestacks(const short* _yyss_, const short* _yyssp_,
		const YYSTYPE* _yyvs_, const YYSTYPE* _yyvsp_,
		const YYSTYPE _yylval_, const int _yychar_,
		const flex::lexer_state& _lexer_state) {
	STACKTRACE_VERBOSE;
	const short* s;
	const YYSTYPE* v;
	s=_yyss_+1;
	v=_yyvs_+1;
	for ( ; s <= _yyssp_ && v <= _yyvsp_; s++, v++) {
		if (v) {
			// cerr << "Deleting stack token..." << endl;
			yy_union_resolve_delete(*v, *(s-1), *s);
		}
	}
	if (!_lexer_state.at_eof() && _yychar_) {
		// cerr << "Deleting last token..." << endl;
		// free the last token (if not EOF)
		yy_union_lookup_delete(_yylval_, _yychar_);
	}
}


