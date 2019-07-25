

PROJECT_MODULES =	src/lld_can.c                \
			        src/lld_ext_dac.c            \
				    src/lld_control.c            \
				    src/lld_px4flow.c            \
                    src/feedback.c               \
                    src/pedals.c                 \
				    src/fuzzy_logic.c            \
				    src/low_speed_control.c      \
                    src/pid.c                    \
                    src/speed.c                  \
                    src/lld_steer.c              \
                    src/emergency_stop.c         \
                    src/MT.c                     \
                    src/MT_control.c             \
                    src/engine_ignition.c        \
                    src/high_speed_control.c     \
                    src/communication.c          \
                    src/usbcfg.c                 \
                    src/steer_control.c          \
                    src/lld_steer_step_motor.c   \
                    src/sound_signal.c           \
                 # src/lld_steer_step_threaded.c   \


PROJECT_TESTS   =	tests/test_pedals.c                    \
                    tests/test_speed.c                     \
                    tests/test_mt_control.c                \
                    tests/fuzzy_logic_test.c               \
                    tests/test_engine_ignition.c           \
                    tests/test_high_speed_control.c        \
                    tests/test_communication.c             \
                    tests/test_lld_steer.c                 \
                    tests/test_steer_control.c             \
                    tests/test_emergency_stop.c            \
                    tests/test_steer_with_communication.c  \
                    tests/test_main.c                      \
                    tests/test_lld_steer_step_motor.c      \
                    test/test_sound_signal.c               \




PROJECT_CSRC    =	src/main.c src/common.c \
					$(PROJECT_MODULES) $(PROJECT_TESTS)
PROJECT_CPPSRC 	= 

PROJECT_INCDIR	=	include tests $(ROSINC)

PROJECT_LIBS	= -lm

