//
// Auto-generated from: 05_Deep_Structure_MetaPhysics\02_Algebraic_Topology\03_Homotopy_Type_Theory.md
// Line: 231
// Language: zig
// Block ID: df7d9941
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 代数数据类型的HoTT视角

// 两个表示可能等价
const TreeA = union(enum) {
    leaf: i32,
    node: struct { left: *TreeA, right: *TreeA },
};

const TreeB = ?struct {
    value: i32,
    children: [2]*TreeB,
};

// 在HoTT中，可以证明 TreeA ≅ TreeB
// 即存在互逆的转换函数
// to : TreeA → TreeB
// from : TreeB → TreeA
// ∀t, from(to(t)) = t
// ∀u, to(from(u)) = u

// 这种结构等价性比语法等价更深刻
