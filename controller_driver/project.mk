
PROJECT_MODULES = src/lld_control.c         \
                  src/lld_encoder.c         \
                  src/lld_sensor_module.c   \
                  src/tf_calculation.c      \
             
                  


PROJECT_TESTS   =  tests/test_lld_control.c    \
                   tests/test_lld_encoder.c    \
                   test/ test_sensor_module.c  \
                   tests/test_tf_calculation.c \

                  

PROJECT_CSRC    = src/main.c src/common.c \
    				$(PROJECT_MODULES) $(PROJECT_TESTS)
PROJECT_CPPSRC 	= 

PROJECT_INCDIR	= include tests

PROJECT_LIBS	= -lm

