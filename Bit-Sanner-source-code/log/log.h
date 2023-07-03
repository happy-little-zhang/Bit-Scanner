#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

/* NOTE: 日志模块, 可以将日志信息输出到终端( DEBUG 模式), 或者磁盘文件(其它).
 *       使用 log 模块提供的日志输出宏, 可以将日志输出到通用模块日志文件;
 *       为方便单个模块独立输出日志文件的需要, 可以在自己的模块中自定义日志输出宏.
 */

////////////////////////////////////////////////////////////////////////////////////////////////////
// 模块宏定义

/* 日志输出宏: 将日志输出到通用模块日志文件( DEBUG 模式除外. 当定义 DEBUG 宏时输出到终端, 否则忽略).
 * 通用模块定义默认为 sys , 亦可以在 main 函数调用 log_set_default 来重新设置.
 *
 * 日志输出情况: 1. 在当前进程所在路径的同级目录下创建 log 文件夹.
 *               2. 在 log 文件夹下 创建 sys (或其它, 以通用模块名) 文件夹.
 *               3. 在 sys (或其它) 文件夹下, 创建 sys.log (或其它.log).
 *               4. 单个日志文件超过 5M 时, 或根据当前时间以重命名的方式来备份日志文件.
 */
#define LOG_DEBUG(...)   log_write(__FILE__, __LINE__, __FUNCTION__, LOG_LEVEL_DEBUG,   NULL, __VA_ARGS__)
#define LOG_INFO(...)    log_write(__FILE__, __LINE__, __FUNCTION__, LOG_LEVEL_INFO,    NULL, __VA_ARGS__)
#define LOG_WARNING(...) log_write(__FILE__, __LINE__, __FUNCTION__, LOG_LEVEL_WARNING, NULL, __VA_ARGS__)
#define LOG_ERROR(...)   log_write(__FILE__, __LINE__, __FUNCTION__, LOG_LEVEL_ERROR,   NULL, __VA_ARGS__)


/* 如果模块的日志需要独立输出到文件, 即不希望输出到通用模块日志中, 以方便调试或其它.
 * 此时, 可以在模块中通过自定义宏来实现:
 *
 * e.g:
 *
 *  #define MODULE  "keep_alive"
 *
 *  #define KA_LOG_DEBUG(...)   log_write(__FILE__, __LINE__, __FUNCTION__, LOG_LEVEL_DEBUG,   MODULE, __VA_ARGS__)
 *  #define KA_LOG_INFO(...)    log_write(__FILE__, __LINE__, __FUNCTION__, LOG_LEVEL_INFO,    MODULE, __VA_ARGS__)
 *  #define KA_LOG_WARNING(...) log_write(__FILE__, __LINE__, __FUNCTION__, LOG_LEVEL_WARNING, MODULE, __VA_ARGS__)
 *  #define KA_LOG_ERROR(...)   log_write(__FILE__, __LINE__, __FUNCTION__, LOG_LEVEL_ERROR,   MODULE, __VA_ARGS__)
 *
 * 日志输出情况: 1. 在当前进程所在路径的同级目录下创建 log 文件夹.
 *               2. 在 log 文件夹下 创建 keep_alive 文件夹.
 *               3. 在 keep_alive 文件夹下, 创建 keep_alive.log.
 *               4. 单个日志文件超过 5M 时, 或根据当前时间以重命名的方式来备份日志文件.
 */

////////////////////////////////////////////////////////////////////////////////////////////////////
// 模块类型定义

// 日志模式
enum LOG_LEVEL {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// 模块接口定义

#ifdef __cplusplus
extern "C" {
#endif

/* 设置日志通用模块, 未设置则默认为 sys .
 * NOTE: 仅在 main 函数中使用, 用于初始化进程全局日志模块.
 */
void log_set_default(const char *module);

/* 日志输出函数, 仅供宏调用.
 */
void log_write(const char *file, unsigned long line, const char *func,
    enum LOG_LEVEL level, const char *module, const char *fmt, ...);

#ifdef __cplusplus
}       // extern "C"
#endif

#endif  // LOG_H_INCLUDED
