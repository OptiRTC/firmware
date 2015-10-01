
CSRC += $(call target_files,$(USRSRC_SLASH),*.c)
CPPSRC += $(call target_files,$(USRSRC_SLASH),*.cpp)

INCLUDE_DIRS += $(USRSRC_SLASH)include \
		$(USRSRC_SLASH)lib/port/qv/gnu

