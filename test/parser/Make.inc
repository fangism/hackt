# "test/parser/Make.inc"

PARSER_TEST_DIR = test/parser

include $(PARSER_TEST_DIR)/basic/Make.inc
# include $(PARSER_TEST_DIR)/namespace/Make.inc
# include $(PARSER_TEST_DIR)/process/Make.inc
# include $(PARSER_TEST_DIR)/datatype/Make.inc
# include $(PARSER_TEST_DIR)/channel/Make.inc

PARSER_TEST_SUBJECTS = \
	$(BASIC_PARSER_TEST_SUBJECTS) \
	$(PARSER_NAMESPACE_TEST_SUBJECTS) \
	$(PARSER_PROCESS_TEST_SUBJECTS) \
	$(PARSER_DATATYPE_TEST_SUBJECTS) \
	$(PARSER_CHANNEL_TEST_SUBJECTS)

# gmake would allow addprefix macro, but bsdmake is lame and can't

