//
// Auto-generated from: Zig\README.md
// Line: 1245
// Language: zig
// Block ID: eb070321
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 合并多个错误集
const CombinedError = FileError || NetworkError || ParseError;

// 函数可以返回合并后的错误
fn complexOperation() CombinedError!Result {
    try fileOperation();  // 可能返回 FileError
    try networkOperation(); // 可能返回 NetworkError
    try parseData();      // 可能返回 ParseError
}
