# 指定编译目录
PARAM_CONFIG     := param_config
PARAM_CONFIG_DIR := $(shell find $(PARAM_CONFIG) -type d)

# 指定模块文件路径
CFLAGS += $(foreach DIR, $(PARAM_CONFIG_DIR), -I$(DIR))
VPATH  += $(foreach DIR, $(PARAM_CONFIG_DIR),   $(DIR))

# 获取目录下所有源文件(含路径)
PARAM_CONFIG_DIR_SRC += $(foreach DIR, $(PARAM_CONFIG_DIR), $(wildcard $(DIR)/*.c))
PARAM_CONFIG_DIR_SRC += $(foreach DIR, $(PARAM_CONFIG_DIR), $(wildcard $(DIR)/*.cpp))

# 去除路径前缀
PARAM_CONFIG_SRC := $(notdir $(PARAM_CONFIG_DIR_SRC))

# 编译
PARAM_CONFIG_OBJ := $(patsubst %.c, %.o, $(patsubst %.cpp, %.o, $(PARAM_CONFIG_SRC)))
