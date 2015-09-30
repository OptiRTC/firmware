
# location of the QP/C framework (if not provided in an environemnt var.)
ifeq ($(QPC),)
QPC := ../../../../..
endif

# QP port used in this project
QP_PORT_DIR := $(QPC)/ports/arm-cm/qv/gnu

# list of all source directories used by this project
VPATH = \
	.. \
	../.. \
	$(QPC)/source \
	$(QP_PORT_DIR) \
	$(QPC)/3rd_party/stm32f2 \
	$(QPC)/3rd_party/stm32f2/gcc

# list of all include directories needed by this project
INCLUDES += \
	-I../.. \
	-I$(QPC)/include \
	-I$(QPC)/source \
	-I$(QP_PORT_DIR) \
	-I$(QPC)/3rd_party/CMSIS/Include \
	-I$(QPC)/3rd_party/stm32f2

# C source files
C_SRCS := \
	bsp.c \
	main.c \
	philo.c \
	table.c \
	system_stm32f2xx.c \
	startup_stm32f2xx.c

QP_SRCS := \
	qep_hsm.c \
	qep_msm.c \
	qf_act.c \
	qf_actq.c \
	qf_defer.c \
	qf_dyn.c \
	qf_mem.c \
	qf_ps.c \
	qf_qact.c \
	qf_qeq.c \
	qf_qmact.c \
	qf_time.c \
	qv.c

QS_SRCS := \
	qs.c \
	qs_fp.c

CSRCS += $(C_SRCS) $(QP_SRCS)
