
PROJECT_MODULES = 

PROJECT_TESTS   =  tests/lld_can_test.c   
          

PROJECT_CSRC    = src/main.c src/common.c \
                  src/lld_can.c
    				$(PROJECT_MODULES) $(PROJECT_TESTS)
PROJECT_CPPSRC 	= 

PROJECT_INCDIR	= include tests $(ROSINC)

PROJECT_LIBS	=

