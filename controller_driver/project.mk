

PROJECT_MODULES =	src/lld_control.c         \
                    src/mt_control.c          \
                    src/lld_can.c             \
                    scr/lld_ext_dac.c         \
                    src/controllers.c         \
                    src/pedals.c              \

PROJECT_TESTS   =	tests/test_mt_control.c   \


PROJECT_CSRC    =	src/main.c src/common.c \
					$(PROJECT_MODULES) $(PROJECT_TESTS)
PROJECT_CPPSRC 	= 

PROJECT_INCDIR	=	include tests $(ROSINC)

PROJECT_LIBS	= -lm

