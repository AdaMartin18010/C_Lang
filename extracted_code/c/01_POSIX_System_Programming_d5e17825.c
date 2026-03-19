/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\POSIX_1_2024\01_POSIX_System_Programming.md
 * Line: 785
 * Language: c
 * Block ID: d5e17825
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file posix_filesystem.c
 * @brief POSIX文件系统操作
 * @description 包含目录遍历、文件属性、符号链接等操作
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <errno.h>
#include <limits.h>

/**
 * @brief 打印文件权限字符串
 * @param mode 文件模式
 */
void print_permissions(mode_t mode) {
    /* 文件类型 */
    if (S_ISREG(mode)) printf("-");
    else if (S_ISDIR(mode)) printf("d");
    else if (S_ISLNK(mode)) printf("l");
    else if (S_ISCHR(mode)) printf("c");
    else if (S_ISBLK(mode)) printf("b");
    else if (S_ISFIFO(mode)) printf("p");
    else if (S_ISSOCK(mode)) printf("s");
    else printf("?");

    /* 所有者权限 */
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ?
           ((mode & S_ISUID) ? "s" : "x") :
           ((mode & S_ISUID) ? "S" : "-"));

    /* 组权限 */
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ?
           ((mode & S_ISGID) ? "s" : "x") :
           ((mode & S_ISGID) ? "S" : "-"));

    /* 其他用户权限 */
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ?
           ((mode & S_ISVTX) ? "t" : "x") :
           ((mode & S_ISVTX) ? "T" : "-"));
}

/**
 * @brief 获取用户名
 * @param uid 用户ID
 * @return 用户名（静态缓冲区）
 */
const char *get_username(uid_t uid) {
    struct passwd *pw = getpwuid(uid);
    return pw ? pw->pw_name : "unknown";
}

/**
 * @brief 获取组名
 * @param gid 组ID
 * @return 组名（静态缓冲区）
 */
const char *get_groupname(gid_t gid) {
    struct group *gr = getgrgid(gid);
    return gr ? gr->gr_name : "unknown";
}

/**
 * @brief 列出目录内容（类似ls -l）
 * @param path 目录路径
 */
void list_directory(const char *path) {
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    char fullpath[PATH_MAX];

    printf("\n=== 目录列表: %s ===\n", path);
    printf("%-10s %3s %8s %8s %10s %20s %s\n",
           "权限", "链接", "所有者", "组", "大小", "修改时间", "名称");
    printf("--------------------------------------------------------------------------------\n");

    dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        /* 构造完整路径 */
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        /* 获取文件信息 */
        if (lstat(fullpath, &st) == -1) {
            perror("lstat");
            continue;
        }

        /* 打印权限 */
        print_permissions(st.st_mode);
        printf(" %3ld", (long)st.st_nlink);
        printf(" %8s", get_username(st.st_uid));
        printf(" %8s", get_groupname(st.st_gid));
        printf(" %10ld", (long)st.st_size);

        /* 格式化时间 */
        char timebuf[20];
        strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S",
                 localtime(&st.st_mtime));
        printf(" %s", timebuf);

        /* 打印文件名 */
        printf(" %s", entry->d_name);

        /* 如果是符号链接，显示指向 */
        if (S_ISLNK(st.st_mode)) {
            char linktarget[PATH_MAX];
            ssize_t len = readlink(fullpath, linktarget, sizeof(linktarget) - 1);
            if (len != -1) {
                linktarget[len] = '\0';
                printf(" -> %s", linktarget);
            }
        }

        printf("\n");
    }

    closedir(dir);
}

/**
 * @brief 递归遍历目录树
 * @param path 起始路径
 * @param level 当前层级（用于缩进）
 */
void traverse_directory(const char *path, int level) {
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    char fullpath[PATH_MAX];

    dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        /* 跳过.和.. */
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        /* 打印缩进 */
        for (int i = 0; i < level; i++) {
            printf("  ");
        }

        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        if (lstat(fullpath, &st) == -1) {
            printf("%s (无法访问)\n", entry->d_name);
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            printf("[%s/]\n", entry->d_name);
            /* 递归遍历子目录 */
            traverse_directory(fullpath, level + 1);
        } else if (S_ISLNK(st.st_mode)) {
            char linktarget[PATH_MAX];
            ssize_t len = readlink(fullpath, linktarget, sizeof(linktarget) - 1);
            if (len != -1) {
                linktarget[len] = '\0';
                printf("%s -> %s\n", entry->d_name, linktarget);
            }
        } else {
            printf("%s\n", entry->d_name);
        }
    }

    closedir(dir);
}

/**
 * @brief 创建目录结构
 * @param path 目录路径
 * @param mode 权限模式
 * @return 成功返回0，失败返回-1
 */
int create_directory_recursive(const char *path, mode_t mode) {
    char tmp[PATH_MAX];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);

    /* 移除末尾的斜杠 */
    if (tmp[len - 1] == '/')
        tmp[len - 1] = '\0';

    /* 逐级创建目录 */
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(tmp, mode) != 0 && errno != EEXIST) {
                return -1;
            }
            *p = '/';
        }
    }

    /* 创建最后一级目录 */
    if (mkdir(tmp, mode) != 0 && errno != EEXIST) {
        return -1;
    }

    return 0;
}

/**
 * @brief 改变文件权限
 * @param path 文件路径
 * @param mode 新权限
 */
void change_permissions(const char *path, mode_t mode) {
    printf("\n修改 %s 的权限为 %04o\n", path, mode);

    if (chmod(path, mode) == -1) {
        perror("chmod");
        return;
    }

    /* 验证 */
    struct stat st;
    if (stat(path, &st) == 0) {
        printf("新权限: ");
        print_permissions(st.st_mode);
        printf(" (%04o)\n", st.st_mode & 07777);
    }
}

/**
 * @brief 符号链接操作演示
 */
void symlink_demo(void) {
    const char *target = "symlink_target.txt";
    const char *linkname = "symlink_link";
    char buf[PATH_MAX];
    ssize_t len;

    printf("\n=== 符号链接操作 ===\n");

    /* 创建目标文件 */
    FILE *fp = fopen(target, "w");
    if (fp) {
        fprintf(fp, "This is the target file.\n");
        fclose(fp);
    }

    /* 创建符号链接 */
    if (symlink(target, linkname) == -1) {
        if (errno == EEXIST) {
            printf("符号链接已存在\n");
        } else {
            perror("symlink");
        }
    } else {
        printf("创建符号链接: %s -> %s\n", linkname, target);
    }

    /* 读取符号链接内容（不是目标文件） */
    len = readlink(linkname, buf, sizeof(buf) - 1);
    if (len != -1) {
        buf[len] = '\0';
        printf("符号链接指向: %s\n", buf);
    }

    /* 清理 */
    unlink(linkname);
    unlink(target);
}

/**
 * @brief 获取和修改文件时间
 */
void file_times_demo(void) {
    const char *filename = "time_test.txt";
    struct stat st;
    struct timespec times[2];

    printf("\n=== 文件时间操作 ===\n");

    /* 创建文件 */
    FILE *fp = fopen(filename, "w");
    if (fp) {
        fprintf(fp, "Test\n");
        fclose(fp);
    }

    /* 获取当前时间 */
    if (stat(filename, &st) == 0) {
        printf("访问时间: %s", ctime(&st.st_atime));
        printf("修改时间: %s", ctime(&st.st_mtime));
    }

    /* 修改访问和修改时间 */
    times[0].tv_sec = 1609459200;  /* 2021-01-01 00:00:00 */
    times[0].tv_nsec = 0;
    times[1].tv_sec = 1609459200;
    times[1].tv_nsec = 0;

    if (utimensat(AT_FDCWD, filename, times, 0) == -1) {
        perror("utimensat");
    } else {
        printf("已修改文件时间\n");
        if (stat(filename, &st) == 0) {
            printf("新访问时间: %s", ctime(&st.st_atime));
            printf("新修改时间: %s", ctime(&st.st_mtime));
        }
    }

    unlink(filename);
}

int main(void) {
    printf("=== POSIX文件系统操作演示 ===\n");

    /* 列出当前目录 */
    list_directory(".");

    /* 目录树遍历 */
    printf("\n=== 目录树遍历 ===\n");
    traverse_directory(".", 0);

    /* 创建目录 */
    const char *test_dir = "posix_test_dir/subdir1/subdir2";
    if (create_directory_recursive(test_dir, 0755) == 0) {
        printf("\n创建目录: %s\n", test_dir);
    }

    /* 权限修改演示 */
    const char *perm_file = "posix_test_dir/perm_test.txt";
    FILE *fp = fopen(perm_file, "w");
    if (fp) {
        fprintf(fp, "test\n");
        fclose(fp);
    }
    change_permissions(perm_file, 0640);

    /* 符号链接演示 */
    symlink_demo();

    /* 文件时间演示 */
    file_times_demo();

    /* 清理 */
    remove(perm_file);
    rmdir("posix_test_dir/subdir1/subdir2");
    rmdir("posix_test_dir/subdir1");
    rmdir("posix_test_dir");

    printf("\n=== 文件系统演示完成 ===\n");
    return 0;
}
