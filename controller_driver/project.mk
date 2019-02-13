
PROJECT_MODULES = src/lld_sensor_module.c   \


PROJECT_TESTS   = tests/test.c     \
                  tests/test_sensor_module.c

PROJECT_CSRC    = src/main.c src/common.c \
    				$(PROJECT_MODULES) $(PROJECT_TESTS)
PROJECT_CPPSRC 	= 

PROJECT_INCDIR	= include tests $(ROSINC)

PROJECT_LIBS	=

