######Makefile模板########
# 系统平台
ARCH = $(shell arch)
BITS =
ifeq "$(findstring x86_64,$(ARCH))" "x86_64"
BITS = 64
endif
$(shell mkdir -p objs) 
#版本
LIVE_VERSION = 0.5
_LIVE_VERSION = \"$(LIVE_VERSION)\"
#日期
DATE=$(shell date +%Y%m%d)
# 目标文件名字
TARGET = lives
APP = lives.v$(LIVE_VERSION).$(ARCH)
#录播写h264文件
REC_F = 0
#指定录播ip
_ONLY_REC = 0
_REC_IP=\"192.168.7.119\"
#是否开启HTTP上报或接受信息日志
_HTTP_RECV_P = 0
_HTTP_SEND_P = 0
#是否开启接受录播数据select处的日志信息
_RECV_SELECT_S = 0
#是否开启接受录播UDP数据日志信息
_RECV_UDPDATA_S = 0
#是否开启发送给直播用户源数据日志信息
_SEND_LIVE_DATA_S = 0
# c源文件
SRC = $(wildcard src/live/*.c) $(wildcard src/msg/*.c) $(wildcard src/reach_share/*.c)\
 $(wildcard src/silverlight/*.c) $(wildcard src/intermedia/xml/*.c) $(wildcard src/test/*.c)\
 $(wildcard src/intermedia/*.c) $(wildcard src/intermedia/connect_rec_server/*.c)\
 $(wildcard src/intermedia/http/*.c) $(wildcard src/intermedia/sysinfo/*.c)\
 $(wildcard src/log/*.c)
# object文件
OBJS = $(SRC:.c=.o)
OBJS_DIR = objs
OBJS_DEST = $(wildcard $(OBJS_DIR)/*.o)
# 编译器
CC = gcc
# 头文件路径，可以任意指定头文件
INCLUDE = -I./src/live -I./src/msg -I./src/reach_share\
 -I./src/silverlight -I./src/intermedia -I./src/key\
 -I./src/intermedia/http/curl$(BITS) -I./src/intermedia/xml/libxml\
 -I./src/intermedia/connect_rec_server -I./src/intermedia/http\
 -I./src/intermedia/sysinfo -I./src/intermedia/xml\
 -I./src/log
# 链接库
LIBS := -lpthread
LIBS += -lrt -lm
LOCAL_LIBS := $(wildcard lib$(BITS)/*.a) 
LIBS += '-Wl,--start-group' $(LOCAL_LIBS) '-Wl,--end-group'
# 编译选项
#CFLAGS = -g -Wall
CFLAGS := -O2 -Wall -Wno-strict-aliasing 
all:$(TARGET)
# 定义目标文件生成规则
$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS_DEST) $(LIBS)
.SUFFIXES:.c
#宏定义
# 定义obejct文件生成规则
.c.o:
	$(CC) $(INCLUDE) $(CFLAGS)\
 -DBDATE=$(DATE)\
 -DRECV_REC_FILE=$(REC_F)\
 -DONLY_REC=$(_ONLY_REC)\
 -DREC_IP=$(_REC_IP)\
 -DHTTP_RECV_P=$(_HTTP_RECV_P)\
 -DHTTP_SEND_P=$(_HTTP_SEND_P)\
 -DRECV_SELECT_S=$(_RECV_SELECT_S)\
 -DRECV_UDPDATA_S=$(_RECV_UDPDATA_S)\
 -DSEND_LIVE_DATA_S=$(_SEND_LIVE_DATA_S)\
 -DVERSION=$(_LIVE_VERSION)\
 -c $<
	mv *.o $(OBJS_DIR)
# 打安装包
pkgs:
	mkdir -p pkg/$(APP)
	cp -rf $(TARGET) pkg/$(APP)
	cp -rf pkg/lived pkg/$(APP)
	cp -rf pkg/$(TARGET)d pkg/$(APP)
	cp -rf pkg/install pkg/$(APP)
	cp -rf pkg/uninstall pkg/$(APP)
	cp -rf pkg/readme pkg/$(APP)
	cd pkg;tar cfvz $(APP).build$(DATE).tgz $(APP);rm -rf $(APP)
# 定义清理函数
.PHONY:clean
clean:
	rm -rf $(OBJS_DEST) $(TARGET)
	rm -rf pkg/$(APP)*
