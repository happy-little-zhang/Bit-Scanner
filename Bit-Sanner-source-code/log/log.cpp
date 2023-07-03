#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/file.h>

#include <time.h>
#include <sys/time.h>

#include <stdarg.h>
#include <ctype.h>

#include <pthread.h>

#include "log.h"

// 日志模块保存路径
#define LOG_PATH_PREFIX     "./log"

// 日志打印缓存大小 64K
#define LOG_BUFF_SIZE       (64*1024)

// 日志文件大小限制 5M
#define LOG_FILE_SIZE       (5*1024*1024)

// 日志模块名称大小限制
#define LOG_MODULE_SIZE     256

// 日志通用模块 - sys
static char log_default_module[LOG_MODULE_SIZE] = "sys";

// static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

// 日志模块名称检测, 包括: 字符串长度 [不超过最大限制] + 字符类型 [字母, 数字, 下划线, 连字符, 点]
static int log_check_module(const char *module)
{
    if (module == NULL)
        return -1;

    size_t len = strlen(module);

    // module 长度不允许超过最大限制
    if (len == 0 || len >= LOG_MODULE_SIZE) {
        fprintf(stderr, "invalid module: '%s'\n", module);
        return -1;
    }

    if (strcmp(module, log_default_module) == 0)
        return 0;

    // module 命名规范: 字母, 数字, "." (点), "_" (下划线), "-"(连字符).
    int i;
    for (i = 0; (size_t)i < len; i++) {
        if (!isalnum(module[i]) && module[i] != '.'
            && module[i] != '_' && module[i] != '-') {
            fprintf(stderr, "invalid module: '%s'\n", module);
            return -1;
        }
    }

    return 0;
}

// 设置日志通用模块 - module
void log_set_default(const char *module)
{
    if (log_check_module(module) != 0)
        return;

    snprintf(log_default_module, sizeof(log_default_module), "%s", module);
}

static const char *log_get_level(enum LOG_LEVEL level)
{
    const char *p_level = "UNKNOWN";

    switch (level) {
    case LOG_LEVEL_DEBUG:
        p_level = "DEBUG";
        break;
    case LOG_LEVEL_INFO:
        p_level = "INFO";
        break;
    case LOG_LEVEL_WARNING:
        p_level = "WARNING";
        break;
    case LOG_LEVEL_ERROR:
        p_level = "ERROR";
        break;
    }

    return p_level;
}

// 根据文件路径获取文件大小
static off_t log_get_file_size(const char *file)
{
    struct stat st;
    memset(&st, 0, sizeof(st));

    stat(file, &st);

    // stat 出错时, st.st_size = 0;
    return st.st_size;
}

// 根据参数 module [模块], 打开模块日志文件: ${LOG_PATH_PREFIX}/module/module.log
static FILE *log_open_file(const char *module)
{
    if (log_check_module(module) != 0)
        module = log_default_module;

    // 生成日志文件路径, 并检测是否存在, 不存在则创建
    char path[4 * LOG_MODULE_SIZE] = {0};
    snprintf(path, sizeof(path), "%s/%s", LOG_PATH_PREFIX, module);

    if (access(path, F_OK) != 0) {
        char cmd[4 * LOG_MODULE_SIZE + 50];
        snprintf(cmd, sizeof(cmd), "mkdir -p %s", path);

        system(cmd);
    }

    // 借用 path 空间生成日志文件路径
    char *file = path;
    size_t len = sizeof(path);

    snprintf(file, len, "%s/%s/%s.log", LOG_PATH_PREFIX, module, module);

    // 日志文件超过定义大小时, 备份文件
    if (log_get_file_size(file) > LOG_FILE_SIZE) {
        time_t tloc;
        struct tm tm;

        time(&tloc);
        localtime_r(&tloc, &tm);

        char log_time[32];
        snprintf(log_time, sizeof(log_time), "%04d-%02d-%02d-%02d-%02d-%02d",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

        char new_file[4 * LOG_MODULE_SIZE] = {0};
        snprintf(new_file, sizeof(new_file), "%s/%s/%s-%s.log", LOG_PATH_PREFIX, module, module, log_time);

//        // 1.0: 重命名日志文件
//        rename(file, new_file);

//        // 1.1: 多线程日志打印异常: 出现1KB左右小文件. 多线程同步 - 互斥锁
//        pthread_mutex_lock(&log_mutex);
//
//        if (log_get_file_size(file) > LOG_FILE_SIZE)
//            rename(file, new_file);
//
//        pthread_mutex_unlock(&log_mutex);

//        // 1.2: 多进程日志打印异常: 出现1KB左右小文件. 多进程同步 - 文件互斥锁
        int fd = open(file, O_RDWR);
        if (fd != -1 && flock(fd, LOCK_EX) == 0) {
            if (log_get_file_size(file) > LOG_FILE_SIZE) {
                int new_fd = open(new_file, O_RDWR|O_CREAT);
                if (new_fd != -1) {
                    char buff[4096];
                    ssize_t size;

                    while ((size = read(fd, buff, sizeof(buff))))
                        write(new_fd, buff, size);

                    close(new_fd);

                    // 截断文件
                    ftruncate(fd, 0);
                }
            }
            flock(fd, LOCK_UN);
        }

        if (fd != -1)
            close(fd);
    }

    return fopen(file, "a");
}

// 日志 msg 输出, 终端(DEBUG) or 日志文件(其它)
static int log_msg(const char *file, unsigned long line, const char *func,
    enum LOG_LEVEL level, const char *module, const char *msg)
{
#ifndef DEBUG
    if (level == LOG_LEVEL_DEBUG)
        return 0;
#endif

    const char *log_level = log_get_level(level);

    time_t tloc;
    struct tm tm;
    struct timeval tv;

    time(&tloc);
    localtime_r(&tloc, &tm);
    gettimeofday(&tv, NULL);

    char log_time[32];
    snprintf(log_time, sizeof(log_time), "%04d-%02d-%02d %02d:%02d:%02d.%03d",
        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
        tm.tm_hour, tm.tm_min, tm.tm_sec, (int)tv.tv_usec/1000);

    FILE *fp = NULL;

    if (level != LOG_LEVEL_DEBUG)
        fp = log_open_file(module);
    else
        fp = stdout;

    if (fp == NULL)
        fp = stderr;

    int re = fprintf(fp, "[%s][%s][%s(%lu):%s] %s\n",
        log_time, log_level, file, line, func, msg);

    fflush(fp);

    if (fp != stdout && fp != stderr)
        fclose(fp);

    return re;
}

void log_write(const char *file, unsigned long line, const char *func,
    enum LOG_LEVEL level, const char *module, const char *fmt, ...)
{
    char msg[LOG_BUFF_SIZE] = {0};

    va_list args;

    va_start(args, fmt);
    vsnprintf(msg, sizeof(msg), fmt, args);
    va_end(args);

    log_msg(file, line, func, level, module, msg);
}





