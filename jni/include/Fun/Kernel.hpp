#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <vector>
#include <string>

class proKpm {
private:
    struct Ditpro_uct_kpm {
        int pid;
        uintptr_t addr;
        void *buffer;
        size_t size;
        uint32_t mode;
    };

    struct Ditpro_uct_list {
        int pid;
        uintptr_t addr[10];
        size_t addr_count;
        void *buffer;
        size_t buffer_size;
        uint32_t mode;
    };

    enum {
        __NR_syscall_ = 18,
        __FLAGS = 616,
        __READMEM = 0x400,
        __READMEMLIST = 0x401,
        __WRITEMEM = 0x200,
        __PROCPID = 0x50,
        __CALLFUNC_1 = 0x900,
    };

public:
    int pid = -1;
    float mode = 1.f;

    bool Read(uintptr_t addr, void *buffer, size_t size) {
        struct Ditpro_uct_kpm ptr;
        ptr.addr = addr;
        ptr.buffer = buffer;
        ptr.pid = this->pid;
        ptr.size = size;
        ptr.mode = (uint32_t)this->mode;
        return (syscall(__NR_syscall_, __FLAGS, &ptr, __READMEM) == 0);
    }

    bool Write(uintptr_t addr, void *buffer, size_t size) {
        struct Ditpro_uct_kpm ptr;
        ptr.addr = addr;
        ptr.buffer = buffer;
        ptr.pid = this->pid;
        ptr.size = size;
        ptr.mode = (uint32_t)this->mode;
        return (syscall(__NR_syscall_, __FLAGS, &ptr, __WRITEMEM) == 0);
    }

    int get_pid(const char *name) {
        return syscall(__NR_syscall_, __FLAGS, name, __PROCPID);
    }

    int tscape_input(const char *name) {
        return syscall(__NR_syscall_, __FLAGS, name, __CALLFUNC_1);
    }

    uintptr_t get_module_base(int pid, const char *module_name) {
        FILE *fp;
        long addr = 0;
        char *pch;
        char filename[64];
        char line[1024];
        snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);
        fp = fopen(filename, "r");
        if (fp != NULL) {
            while (fgets(line, sizeof(line), fp)) {
                if (strstr(line, module_name)) {
                    pch = strtok(line, "-");
                    addr = strtoul(pch, NULL, 16);
                    if (addr == 0x8000) addr = 0;
                    break;
                }
            }
            fclose(fp);
        }
        return addr;
    }
};

struct mem_operation {
    pid_t target_pid;
    uint64_t addr;
    void *buffer;
    uint64_t size;
};

#define PRCTL_MEM_READ  0x63687501
#define PRCTL_MEM_WRITE 0x63687502

class c_driver {
public:
    proKpm syscall_backend;
    bool use_syscall_fallback = false;
    bool use_tb_kpm = false;
int has_upper = 0;
int has_lower = 0;
int has_symbol = 0;
int has_digit = 0;
int fd;
pid_t pid;
char *derive;

typedef struct _COPY_MEMORY {
pid_t pid;
uintptr_t addr;
void *buffer;
size_t size;
} COPY_MEMORY, *PCOPY_MEMORY;

typedef struct _MODULE_BASE {
pid_t pid;
char *name;
uintptr_t base;
} MODULE_BASE, *PMODULE_BASE;


enum OPERATIONS {
OP_INIT_KEY = 0x800,
OP_READ_MEM = 0x801,
OP_WRITE_MEM = 0x802,
OP_MODULE_BASE = 0x803
};

int symbol_file(const char *filename) {
int length = strlen(filename);
for (int i = 0; i < length; i++) {
if (islower(filename[i])) {
has_lower = 1;
} else if (isupper(filename[i])) {
has_upper = 1;
} else if (ispunct(filename[i])) {
has_symbol = 1;
} else if (isdigit(filename[i])) {
has_digit = 1;
}
}
return has_lower && !has_upper && !has_symbol && !has_digit;
}

char *devproc() {
struct dirent *de;
DIR *dr = opendir("/proc");
char *device_path = NULL;

if (dr == NULL) {
printf("Could not open /proc directory\n");
return NULL;
}

while ((de = readdir(dr)) != NULL) {
if (strlen(de->d_name) != 6) {
continue;
}

if (strcmp(de->d_name, "NVTSPI") == 0 ||
strcmp(de->d_name, "ccci_log") == 0 ||
strcmp(de->d_name, "aputag") == 0 ||
strcmp(de->d_name, "asound") == 0 ||
strcmp(de->d_name, "clkdbg") == 0 ||
strcmp(de->d_name, "crypto") == 0 ||
strcmp(de->d_name, "modules") == 0 ||
strcmp(de->d_name, "mounts") == 0 ||
strcmp(de->d_name, "pidmap") == 0 ||
strcmp(de->d_name, "phoenix") == 0 ||
strcmp(de->d_name, "uptime") == 0 ||
strcmp(de->d_name, "vmstat") == 0) {
continue;
}

int is_valid = 1;
for (int i = 0; i < 6; i++) {
if (!isalnum(de->d_name[i])) {
is_valid = 0;
break;
}
}

if (is_valid) {
device_path = (char*)malloc(11 + strlen(de->d_name));
if (!device_path) continue;
sprintf(device_path, "/proc/%s", de->d_name);

struct stat sb;
if (stat(device_path, &sb) == 0 && S_ISREG(sb.st_mode)) {
closedir(dr);
return device_path;
} else {
free(device_path);
device_path = NULL;
}
}
}

closedir(dr);
return NULL;
}

char *dev()
{
const char *dev_path = "/dev";
DIR *dir = opendir(dev_path);
if (dir == NULL)
{
printf("\033[31m[!] failed to open /dev \033[0m\n");
return NULL;
}

const std::vector<std::string> excluded_names = {
"binder", "common", "ashmem", "stdin", "stdout", "stderr"};

struct dirent *entry;
char *file_path = NULL;
while ((entry = readdir(dir)) != NULL)
{
const char *current_name = entry->d_name;

if (strcmp(current_name, ".") == 0 || strcmp(current_name, "..") == 0)
{
continue;
}

if (strstr(current_name, "gpiochip") != NULL ||
strchr(current_name, '_') != NULL ||
strchr(current_name, '-') != NULL ||
strchr(current_name, ':') != NULL)
{
continue;
}

bool is_excluded = false;
for (const auto &name : excluded_names)
{
if (strcmp(current_name, name.c_str()) == 0)
{
is_excluded = true;
break;
}
}
if (is_excluded)
{
continue;
}

size_t path_length = strlen(dev_path) + strlen(current_name) + 2;
file_path = (char *)malloc(path_length);
if (!file_path)
continue;

snprintf(file_path, path_length, "%s/%s", dev_path, current_name);

struct stat file_info;
if (stat(file_path, &file_info) < 0)
{
free(file_path);
file_path = NULL;
continue;
}

if (S_ISCHR(file_info.st_mode) || S_ISBLK(file_info.st_mode))
{
if (localtime(&file_info.st_ctime)->tm_year + 1900 <= 1980)
{
free(file_path);
file_path = NULL;
continue;
}

if (file_info.st_atime == file_info.st_ctime &&
file_info.st_size == 0 &&
file_info.st_gid == 0 &&
file_info.st_uid == 0 &&
strlen(current_name) == 6)
{
closedir(dir);
return file_path;
}
}

free(file_path);
file_path = NULL;
}

closedir(dir);
return "!";
}

char* QX()
{
const char* command = "dir=$(ls -l /proc/*/exe 2>/dev/null | grep -E '/data/[^/]* \\(deleted\\)' | sed 's/ /\\n/g' | grep '/proc' | sed 's/\\/[^/]*$//g' 2>/dev/null);if [[ \"$dir\" ]]; then sbwj=$(head -n 1 \"$dir/comm\" 2>/dev/null);open_file=\"\";for file in \"$dir\"/fd/*; do link=$(readlink \"$file\" 2>/dev/null);if [[ \"$link\" == \"/dev/$sbwj (deleted)\" ]]; then open_file=\"$file\";break;fi;done;if [[ -n \"$open_file\" ]]; then nhjd=$(echo \"$open_file\");sbid=$(ls -L -l \"$nhjd\" | sed 's/\\([^,]*\\).*/\\1/' | sed 's/.*root //' 2>/dev/null);echo \"/dev/$sbwj\" >/dev/null 2>&1;rm -rf \"/dev/$sbwj\" 2>/dev/null;mknod \"/dev/$sbwj\" c \"$sbid\" 0 2>/dev/null;fi;fi;";
FILE* file = popen(command, "r");
if (file == NULL) {
return NULL;
}
static char result[512];
if (fgets(result, sizeof(result), file) == NULL) {
return "!";
}
pclose(file);
result[strlen(result)-1] = '\0';
derive = result;
return result;
}
int open_driver() {
char *dev_path3 = QX();
if (dev_path3 != NULL && dev_path3[0] != '!') {
fd = open(dev_path3, O_RDWR);
if (fd > 0) {
return 1;
}
}

char *dev_path1 = dev();
if (dev_path1 != NULL && dev_path1[0] != '!') {
fd = open(dev_path1, O_RDWR);
if (fd > 0) {
return 1;
}
}

char *dev_path2 = devproc();
if (dev_path2 != NULL && dev_path2[0] != '!') {
fd = open(dev_path2, O_RDWR);
free(dev_path2);
if (fd > 0) {
return 1;
}
}
return 0;
}

public:
char *drive_path = NULL;
c_driver() {
open_driver();
drive_path = derive;
}

void Off_Drive(){
if (fd > 0)
close(fd);
}

void initialize(pid_t pid) {
this->pid = pid;
}

bool Read(uintptr_t addr, void *buffer, size_t size) {
COPY_MEMORY cm;
cm.pid = this->pid;
cm.addr = addr;
cm.buffer = buffer;
cm.size = size;
if (ioctl(fd, OP_READ_MEM, &cm) != 0) {
return false;
}
return true;
}

bool Write(uintptr_t addr, void *buffer, size_t size) {
COPY_MEMORY cm;

cm.pid = this->pid;
cm.addr = addr;
cm.buffer = buffer;
cm.size = size;

if (ioctl(fd, OP_WRITE_MEM, &cm) != 0) {
return false;
}
return true;
}

template <typename T>
T Read(uintptr_t addr) {
T res;
if (this->Read(addr, &res, sizeof(T)))
return res;
return {};
}
template <typename T>
bool Write(uintptr_t addr, const T& value) {
return this->Write(addr, (void*)&value, sizeof(T));
}

int getPID(const char *packageName) {
int id = -1;
DIR *dir;
FILE *fp;
char filename[64];
char cmdline[64];
struct dirent *entry;
dir = opendir("/proc");
while ((entry = readdir(dir)) != NULL) {
id = atoi(entry->d_name);
if (id != 0) {
sprintf(filename, "/proc/%d/cmdline", id);
fp = fopen(filename, "r");
if (fp) {
fgets(cmdline, sizeof(cmdline), fp);
fclose(fp);
if (strcmp(packageName, cmdline) == 0) {
return id;
}
}
}
} closedir(dir);
return -1;
}
uintptr_t 获取模块头2(int pid,char* module_name)
{
long addr = 0;
char filename[32];
char line[1024];
if (pid < 0)
{
snprintf(filename, sizeof(filename), "/proc/self/maps", pid);
}
else
{
snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);
}
FILE *fp = fopen(filename, "r");
if (fp != NULL)
{
while (fgets(line, sizeof(line), fp))
{
if (strstr(line, module_name))
{
sscanf(line,"%lx-%*lx",&addr);
break;
}
}
fclose(fp);
}
return addr;
}
uintptr_t 获取模块头(int pid,char* name) {
MODULE_BASE mb;
char buf[0x100];
strcpy(buf,name);
mb.pid = this->pid;
mb.name = buf;

if (ioctl(fd, OP_MODULE_BASE, &mb) != 0) {
return 0;
}
return mb.base;
}
};

static c_driver Driver;