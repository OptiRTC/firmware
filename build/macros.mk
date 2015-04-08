include $(COMMON_BUILD)/os.mk
include $(COMMON_BUILD)/verbose.mk

# recursively finds files matching the given pattern
# $1 is the directory to search for files (should end with a slash)
# $2 is the wildcard specifying the files to find
rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

ifneq (OSX,$(MAKE_OS))
filesize=`stat --print %s $1`
else
filesize=`stat -f%z $1`
endif

test=$(strip $(shell if test $1 $2 $3; then echo 1; fi))
_assert_equal = $(if $(call test,$2,-ne,$3),$(error "expected $1 to be $2 but was $3))

# asserts the given file is the given size
# $1 the file to test
# $2 the expected size of the file in decimal
assert_filesize = $(call _assert_equal,$1,$2,$(shell echo $(call filesize,$1)),"file $1")