# "test/parser/Make.inc"

PARSER_TEST_DIR = test/parser

include $(PARSER_TEST_DIR)/basic/Make.inc
include $(PARSER_TEST_DIR)/namespace/Make.inc
include $(PARSER_TEST_DIR)/param/Make.inc
include $(PARSER_TEST_DIR)/array/Make.inc
include $(PARSER_TEST_DIR)/process/Make.inc
include $(PARSER_TEST_DIR)/datatype/Make.inc
# include $(PARSER_TEST_DIR)/channel/Make.inc

# gmake would allow addprefix macro, but bsdmake is lame and can't

