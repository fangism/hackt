# "test/parser/param/Make.inc"

PARAM_PARSER_TEST_DIR = test/parser/param

TEST_SUBJECTS += \
	$(PARAM_PARSER_TEST_DIR)/001 \
	$(PARAM_PARSER_TEST_DIR)/002 \
	$(PARAM_PARSER_TEST_DIR)/003 \
	$(PARAM_PARSER_TEST_DIR)/004 \
	$(PARAM_PARSER_TEST_DIR)/005 \
	$(PARAM_PARSER_TEST_DIR)/006 \
	$(PARAM_PARSER_TEST_DIR)/007

# gmake would allow addprefix macro, but bsdmake is lame and can't

