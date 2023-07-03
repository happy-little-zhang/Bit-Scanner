# 指定编译目录
COMMON     := common
COMMON_DIR := $(shell find $(COMMON) -type d)

# 指定模块文件路径
CFLAGS += $(foreach DIR, $(COMMON_DIR), -I$(DIR))
VPATH  += $(foreach DIR, $(COMMON_DIR),   $(DIR))

# 获取目录下所有源文件(含路径)
COMMON_DIR_SRC += $(foreach DIR, $(COMMON_DIR), $(wildcard $(DIR)/*.c))
COMMON_DIR_SRC += $(foreach DIR, $(COMMON_DIR), $(wildcard $(DIR)/*.cpp))

# 去除路径前缀
COMMON_SRC := $(notdir $(COMMON_DIR_SRC))

# 编译
COMMON_OBJ := $(patsubst %.c, %.o, $(patsubst %.cpp, %.o, $(COMMON_SRC)))
