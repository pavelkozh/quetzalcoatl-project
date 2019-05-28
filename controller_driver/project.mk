
PROJECT_MODULES =	src/lld_can.c \
					src/lld_ext_dac.c \
					src/controllers.c \
					src/lld_control.c \
					src/lld_px4flow.c \
					src/fuzzy_logic.c \

PROJECT_TESTS   =	tests/fuzzy_logic_test.c   
          

PROJECT_CSRC    =	src/main.c src/common.c \
					$(PROJECT_MODULES) $(PROJECT_TESTS)
PROJECT_CPPSRC 	= 

PROJECT_INCDIR	=	include tests $(ROSINC)

PROJECT_LIBS	=

