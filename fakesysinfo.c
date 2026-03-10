#define _GNU_SOURCE
#include <dlfcn.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/utsname.h>

#define HWID_FILE "fake_machine_id"
#define FAKE_HOSTNAME "host-name"
#define FAKE_OS_FILE "/tmp/.fake_hostname"

// Создаем файл с фейковым hostname
static void create_fake_hostname(void) {
    static int created = 0;
    if (created) return;
    created = 1;
    
    int fd = open(FAKE_OS_FILE, O_WRONLY|O_CREAT|O_TRUNC, 0644);
    if (fd >= 0) {
        write(fd, FAKE_HOSTNAME, strlen(FAKE_HOSTNAME));
        close(fd);
    }
}

// Подменяем пути к файлам
static const char* fake_path(const char *p) {
    if (!p) return NULL;
    
    // HWID файлы
    if (strstr(p, "machine-id") || strstr(p, "product_uuid")) {
        return HWID_FILE;
    }
    
    // Hostname
    if (strstr(p, "hostname")) {
        create_fake_hostname();
        return FAKE_OS_FILE;
    }
    
    return NULL;
}

// Подменяем uname (поле nodename)
int uname(struct utsname *buf) {
    static int (*orig)(struct utsname *) = NULL;
    if (!orig) orig = dlsym(RTLD_NEXT, "uname");
    
    int res = orig(buf);
    if (res == 0) {
        strcpy(buf->nodename, FAKE_HOSTNAME);
    }
    return res;
}

// Перехват open
int open(const char *p, int flags, ...) {
    static int (*orig)(const char*, int, ...) = NULL;
    if (!orig) orig = dlsym(RTLD_NEXT, "open");
    
    const char *fp = fake_path(p);
    if (fp) p = fp;
    
    mode_t m = 0;
    if (flags & O_CREAT) {
        va_list a; va_start(a, flags);
        m = va_arg(a, mode_t);
        va_end(a);
    }
    return orig(p, flags, m);
}

// Перехват open64
int open64(const char *p, int flags, ...) {
    static int (*orig)(const char*, int, ...) = NULL;
    if (!orig) orig = dlsym(RTLD_NEXT, "open64");
    
    const char *fp = fake_path(p);
    if (fp) p = fp;
    
    mode_t m = 0;
    if (flags & O_CREAT) {
        va_list a; va_start(a, flags);
        m = va_arg(a, mode_t);
        va_end(a);
    }
    return orig(p, flags, m);
}

// Перехват fopen
FILE *fopen(const char *p, const char *m) {
    static FILE *(*orig)(const char*, const char*) = NULL;
    if (!orig) orig = dlsym(RTLD_NEXT, "fopen");
    
    const char *fp = fake_path(p);
    return orig(fp ? fp : p, m);
}
