# 定义编译器
CC  := aarch64-linux-gnu-gcc
CXX := aarch64-linux-gnu-g++

# 定义 BIN/INC/LIB 路径
BIN := bin
LIB := lib

# 添加模块
include common/common.mk
include log/log.mk
include param_config/param_config.mk

# 增加 和 opencv
#CFLAGS += $(shell pkg-config --cflags opencv4)

# 增加 lib / bin 路径
LDFLAGS += -L$(LIB) -L$(BIN)


# 设置链接选项
LIBS += -l$(COMMON) \
		-l$(LOG) \
        -l$(PARAM_CONFIG) \
#		$(shell pkg-config --libs --static opencv4)

# 自定义宏: DEBUG
USER_MACROS += -DDEBUG


# 可执行文件主函数编译
MAIN_OBJ  := main.o
#DETECT_OBJ  := detect.o
#OPENCV_RENDER_OBJ := opencv_render.o

.PHONY: clean

# 编译所有
all: $(COMMON) $(LOG) $(PARAM_CONFIG) main 
#all: $(COMMON) $(LOG) $(PARAM_CONFIG) train detect opencv_render

# 编译可执行文件
main : $(MAIN_OBJ)
	g++ -Wl,-rpath=. $(LDFLAGS) -o $(BIN)/$@ $^ $(LIBS)

#detect : $(DETECT_OBJ)
#	g++ -Wl,-rpath=. $(LDFLAGS) -o $(BIN)/$@ $^ $(LIBS)

#opencv_render: $(OPENCV_RENDER_OBJ)
#	g++ -Wl,-rpath=. $(LDFLAGS) -o $(BIN)/$@ $^ $(LIBS)

# 编译模块动态库
$(LOG): $(LOG_OBJ)
	g++ -shared -o $(BIN)/lib$@.so $^
$(COMMON): $(COMMON_OBJ)
	g++ -shared -o $(BIN)/lib$@.so $^
$(PARAM_CONFIG): $(PARAM_CONFIG_OBJ)
	g++ -shared -o $(BIN)/lib$@.so $^


# 编译 .o 文件
%.o: %.c
	g++ -c -std=c++11 -g -Wall $(CFLAGS) $(CPPFLAGS) $(USER_MACROS) -fPIC -o $@ $<

%.o: %.cpp
	g++ -c -std=c++11 -g -Wall $(CFLAGS) $(CPPFLAGS) $(USER_MACROS) -fPIC -o $@ $<


# 删除 .o 文件
# 删除动态库和可执行文件
clean:
	@rm -rf *.o
	@rm -rf bin/*
	@rm -rf dataset/simulation/label/*  dataset/simulation/result/*
