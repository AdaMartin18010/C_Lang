/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\02_SQLite_Engineering.md
 * Line: 1326
 * Language: c
 * Block ID: b8d0e926
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* os.h - 操作系统抽象 */

// 文件锁抽象
struct sqlite3_file {
    const struct sqlite3_io_methods *pMethods;
};

struct sqlite3_io_methods {
    int iVersion;
    int (*xClose)(sqlite3_file*);
    int (*xRead)(sqlite3_file*, void*, int iAmt, sqlite3_int64 iOfst);
    int (*xWrite)(sqlite3_file*, const void*, int iAmt, sqlite3_int64 iOfst);
    int (*xTruncate)(sqlite3_file*, sqlite3_int64 size);
    int (*xSync)(sqlite3_file*, int flags);
    int (*xFileSize)(sqlite3_file*, sqlite3_int64 *pSize);
    int (*xLock)(sqlite3_file*, int);
    int (*xUnlock)(sqlite3_file*, int);
    int (*xCheckReservedLock)(sqlite3_file*, int *pResOut);
    int (*xFileControl)(sqlite3_file*, int op, void *pArg);
    int (*xSectorSize)(sqlite3_file*);
    int (*xDeviceCharacteristics)(sqlite3_file*);
};

// 跨平台原子操作
#ifdef SQLITE_ATOMIC_INTRINSICS
  #define SQLITE_ATOMIC_LOAD(ptr) __atomic_load_n(ptr, __ATOMIC_SEQ_CST)
  #define SQLITE_ATOMIC_STORE(ptr, val) __atomic_store_n(ptr, val, __ATOMIC_SEQ_CST)
#else
  // 使用互斥锁回退
  #define SQLITE_ATOMIC_LOAD(ptr) (*(ptr))
  #define SQLITE_ATOMIC_STORE(ptr, val) (*(ptr) = (val))
#endif
