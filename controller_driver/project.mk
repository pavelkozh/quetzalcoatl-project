
PROJECT_MODULES =	src/lld_can.c \
					src/lld_ext_dac.c \
					src/controllers.c  \
					src/lld_control.c 

PROJECT_TESTS   =	tests/lld_engine_speed_test.c   
          

PROJECT_CSRC    =	src/main.c src/common.c \
					$(PROJECT_MODULES) $(PROJECT_TESTS)
PROJECT_CPPSRC 	= 

PROJECT_INCDIR	=	include tests $(ROSINC)

PROJECT_LIBS	=

