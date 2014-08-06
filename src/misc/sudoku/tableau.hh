/**
	\file "misc/sudoku/tableau.hh"
	$Id: tableau.h,v 1.2 2006/01/22 06:53:23 fang Exp $
 */

#ifndef	__SUDOKU_TABLEAU_H__
#define	__SUDOKU_TABLEAU_H__

#include <iosfwd>

namespace sudoku {
using std::istream;
//=============================================================================
#define	TABLEAU_TEMPLATE_SIGNATURE					\
	template <class BoardType, class RulesType>
#define	TABLEAU_CLASS		tableau<BoardType,RulesType>

/**
	\param RulesType is a fixed rule-set.  
	\param BoardType is a fixed board.
	Their cell_types must match.  
 */
TABLEAU_TEMPLATE_SIGNATURE
class tableau {
	typedef	TABLEAU_CLASS			this_type;
public:
	typedef	RulesType			rules_type;
	typedef	BoardType			board_type;
	typedef	typename board_type::cell_type	cell_type;
private:
	/// defined in "misc/sudoku/solve.hh"
	template <class SolutionType>
	class subproblem_solver;
private:
	board_type				board;
	rules_type				rules;
public:
	tableau();
	~tableau();
private:
	/// intentionally privately undefined copy-constructor.
	explicit
	tableau(const tableau&);
public:
#if 0
	bool
	load_board(istream& i) { return this->board.load(i); }
#endif

	template <class SolutionsType>
	void
	solve(SolutionsType&) const;

private:
	template <class SolutionsType>
	void
	__solve(SolutionsType&);

};	// end class tableau

//=============================================================================
/**
	This is a generalized tableau with a programmable board type
	and a programmable rule set. 
	The board and rules can be described using a meta-language.  
 */
class meta_tableau {
};	// end class programmable_tableau

//=============================================================================
}	// end namespace sudoku

#endif	// __SUDOKU_TABLEAU_H__

