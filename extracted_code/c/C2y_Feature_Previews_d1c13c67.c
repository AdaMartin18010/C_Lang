/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_Feature_Previews.md
 * Line: 571
 * Language: c
 * Block ID: d1c13c67
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============ C2y defer ============
void c2y_example(void) {
    FILE* f = fopen("file.txt", "r");
    defer fclose(f);
    // 简单直接，语句级
}

// ============ Go defer ============
// func example() {
//     f, _ := os.Open("file.txt")
//     defer f.Close()
//     // defer是函数级的，不是块级的
// }
// 差异: Go的defer在函数返回时执行，C2y在作用域退出时执行

// ============ Swift defer ============
// func example() {
//     let f = try! FileHandle(forReadingFrom: url)
//     defer { f.closeFile() }
//     // 类似C2y，块级作用域
// }
// 差异: Swift支持多个defer，也是LIFO

// ============ Rust (Drop trait) ============
// fn example() {
//     let _guard = scopeguard::guard((), |_| {
//         println!("cleanup");
//     });
//     // RAII模式，需要类型支持
// }
// 差异: Rust使用RAII，C2y使用显式defer
