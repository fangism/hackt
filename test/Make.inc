# "test/Make.inc"

# clear list first, subdirectories will accumulate tests in order
TEST_SUBJECTS = 

# include test/lexer/Make.inc
include test/parser/Make.inc
# include test/utils/Make.inc

# need to add directory prefix, gmake feature... BAH!

