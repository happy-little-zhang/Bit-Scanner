# 指定编译目录
LOG     := log
LOG_DIR := $(shell find $(LOG) -type d)

# 指定模块文件路径
CFLAGS += $(foreach DIR, $(LOG_DIR), -I$(DIR))
VPATH  += $(foreach DIR, $(LOG_DIR),   $(DIR))

# 获取目录下所有源文件(含路径)
LOG_DIR_SRC += $(foreach DIR, $(LOG_DIR), $(wildcard $(DIR)/*.c))
LOG_DIR_SRC += $(foreach DIR, $(LOG_DIR), $(wildcard $(DIR)/*.cpp))

# 去除路径前缀
LOG_SRC := $(notdir $(LOG_DIR_SRC))

# 编译
LOG_OBJ := $(patsubst %.c, %.o, $(patsubst %.cpp, %.o, $(LOG_SRC)))
