# "test/Make.inc"

# include test/lexer/Make.inc
include test/parser/Make.inc
# include test/utils/Make.inc

# need to add directory prefix, gmake feature... BAH!
TEST_SUBJECTS = $(PARSER_TEST_SUBJECTS)

