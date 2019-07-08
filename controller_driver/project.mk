

PROJECT_MODULES =	src/lld_can.c             \
					src/lld_ext_dac.c         \
					src/lld_control.c         \
					src/lld_px4flow.c         \
                    src/feedback.c            \
                    src/pedals.c              \
					src/fuzzy_logic.c         \
					src/low_speed_control.c   \
                    src/pid.c                 \
                    src/speed.c               \
                    src/MT.c                  \
                    src/MT_control.c          \
                    src/high_speed_control.c  \


PROJECT_TESTS   =	tests/test_pedals.c            \
                    tests/test_speed.c             \
                    tests/test_mt_control.c        \
                    tests/fuzzy_logic_test.c       \
                    test/test_high_speed_control.c \



PROJECT_CSRC    =	src/main.c src/common.c \
					$(PROJECT_MODULES) $(PROJECT_TESTS)
PROJECT_CPPSRC 	= 

PROJECT_INCDIR	=	include tests $(ROSINC)

PROJECT_LIBS	= -lm

