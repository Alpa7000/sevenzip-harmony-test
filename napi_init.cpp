#include "napi/native_api.h"
#include <dlfcn.h>
#include <thread>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <atomic>

typedef int (* main_t)(int argc, const char *argv[]);

std::thread g_thread;
std::atomic<bool> g_taskRunning{false};

// 辅助函数：复制文件（如果需要保留可执行文件方式，此处暂留，动态库方案其实用不到）
static bool copyFile(const std::string& src, const std::string& dst) {
    // ... 保持不变，如果你完全放弃可执行文件方案，此函数可删 ...
    int src_fd = open(src.c_str(), O_RDONLY);
    if (src_fd == -1) return false;
    int dst_fd = open(dst.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0755);
    if (dst_fd == -1) {
        close(src_fd);
        return false;
    }
    char buf[4096];
    ssize_t n;
    while ((n = read(src_fd, buf, sizeof(buf))) > 0) {
        if (write(dst_fd, buf, n) != n) {
            close(src_fd);
            close(dst_fd);
            return false;
        }
    }
    close(src_fd);
    close(dst_fd);
    return true;
}

static napi_value Add(napi_env env, napi_callback_info info) {
    size_t argc = 4;
    napi_value args[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    double value0, value1;
    napi_get_value_double(env, args[0], &value0);
    napi_get_value_double(env, args[1], &value1);

    size_t str_size;
    napi_get_value_string_utf8(env, args[2], nullptr, 0, &str_size);
    std::string sandbox_path(str_size, '\0');
    napi_get_value_string_utf8(env, args[2], &sandbox_path[0], str_size + 1, &str_size);

    // 注意：第四个参数 native_lib_path 在动态库方案中其实不需要了，因为 dlopen 会自动搜索。
    // 为了保持兼容，我们继续接收但不使用。
    // napi_get_value_string_utf8(env, args[3], ...);

    // 防止多个任务同时运行
    bool expected = false;
    if (!g_taskRunning.compare_exchange_strong(expected, true)) {
        napi_value sum;
        napi_create_double(env, value0 + value1, &sum);
        return sum;
    }

    // 启动后台线程
    g_thread = std::thread([sandbox_path]() {
        // 1. 先准备好调试日志文件（用于捕获 dlopen 错误）
        std::string debug_file = sandbox_path + "/debug.txt";
        int debug_fd = open(debug_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        
        // 2. 备份并重定向 stdout 和 stderr
        int stdout_backup = dup(STDOUT_FILENO);
        int stderr_backup = dup(STDERR_FILENO);
        if (stdout_backup == -1 || stderr_backup == -1) {
            if (debug_fd != -1) dprintf(debug_fd, "dup failed: stdout=%d, stderr=%d\n", stdout_backup, stderr_backup);
            if (stdout_backup != -1) close(stdout_backup);
            if (stderr_backup != -1) close(stderr_backup);
            if (debug_fd != -1) close(debug_fd);
            g_taskRunning = false;
            return;
        }

        std::string output_file = sandbox_path + "/7za_output.txt";
        int fd = open(output_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            if (debug_fd != -1) dprintf(debug_fd, "open output file failed: %s\n", output_file.c_str());
            close(stdout_backup);
            close(stderr_backup);
            if (debug_fd != -1) close(debug_fd);
            g_taskRunning = false;
            return;
        }

        // 执行重定向
        if (dup2(fd, STDOUT_FILENO) == -1 || dup2(fd, STDERR_FILENO) == -1) {
            if (debug_fd != -1) dprintf(debug_fd, "dup2 failed\n");
            close(fd);
            close(stdout_backup);
            close(stderr_backup);
            if (debug_fd != -1) close(debug_fd);
            g_taskRunning = false;
            return;
        }
        close(fd); // 原描述符已复制，可以关闭
        
        // 关键修复：强制禁用缓冲，确保输出立刻写入
        setvbuf(stdout, NULL, _IONBF, 0);
        setvbuf(stderr, NULL, _IONBF, 0);
        
        // 写入测试头，验证重定向是否工作
        printf("=== 7-Zip Benchmark Output ===\n");
        fflush(stdout);

        // 3. 加载动态库并执行
        void* lib = dlopen("lib7za.so", RTLD_LAZY | RTLD_GLOBAL);
        if (!lib) {
            if (debug_fd != -1) dprintf(debug_fd, "dlopen failed: %s\n", dlerror());
            // 即使失败也要恢复流，但此处直接返回
            dup2(stdout_backup, STDOUT_FILENO);
            dup2(stderr_backup, STDERR_FILENO);
            close(stdout_backup);
            close(stderr_backup);
            if (debug_fd != -1) close(debug_fd);
            g_taskRunning = false;
            return;
        }
        if (debug_fd != -1) dprintf(debug_fd, "dlopen success\n");

        auto mainfunc = (main_t)dlsym(lib, "main");
        if (!mainfunc) {
            if (debug_fd != -1) dprintf(debug_fd, "dlsym main failed: %s\n", dlerror());
        } else {
            if (debug_fd != -1) dprintf(debug_fd, "Calling main function...\n");
            const char* argv[] = {"lib7za.so", "b", "-mmt12"};
            int ret = mainfunc(3, argv);
            if (debug_fd != -1) dprintf(debug_fd, "main returned: %d\n", ret);
        }
        
        dlclose(lib);
        
        // 刷新缓冲区
        fflush(stdout);
        fflush(stderr);
        
        // 恢复原始流
        dup2(stdout_backup, STDOUT_FILENO);
        dup2(stderr_backup, STDERR_FILENO);
        close(stdout_backup);
        close(stderr_backup);
        
        if (debug_fd != -1) {
            dprintf(debug_fd, "Benchmark finished, output saved to %s\n", output_file.c_str());
            close(debug_fd);
        }
        
        g_taskRunning = false;
    });
    g_thread.detach(); // 分离线程，避免阻塞 UI

    napi_value sum;
    napi_create_double(env, value0 + value1, &sum);
    return sum;
}

// ... 下面的 Init 和模块注册代码保持不变 ...
EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports) {
    napi_property_descriptor desc[] = {
        { "add", nullptr, Add, nullptr, nullptr, nullptr, napi_default, nullptr }
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "entry",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void RegisterEntryModule(void) {
    napi_module_register(&demoModule);
}