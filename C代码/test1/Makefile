# ========= 编译器和参数 =========
CC      = gcc
CFLAGS  = -Wall -Wextra -g

# ========= 目标程序名 =========
TARGET  = test

# ========= 源文件 =========
SRCS    = ipc.c main.c
OBJS    = $(SRCS:.c=.o)

# ========= 默认目标 =========
all: $(TARGET)

# ========= 链接 =========
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# ========= 编译规则 =========
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ========= 清理 =========
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean