//
// Auto-generated from: 08_Zig_C_Connection\07_Real_World_Migration_Cases\README.md
// Line: 114
// Language: zig
// Block ID: 6f4b14ce
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

pub const BtreePageHeader = packed struct {
    first_freeblock: u16,
    cell_count: u16,
    cell_content_offset: u16,
    fragmented_free_bytes: u8,
    right_child_page: u32,
};

pub const BtreePage = struct {
    allocator: Allocator,
    data: []u8,
    page_size: u16,
    page_number: u32,
    is_leaf: bool,

    pub fn init(allocator: Allocator, page_size: u16,
                page_number: u32, data: []u8) !Self {
        if (data.len != page_size) return error.InvalidPageSize;

        const page_type = data[0];
        return Self{
            .allocator = allocator,
            .data = try allocator.dupe(u8, data),
            .page_size = page_size,
            .page_number = page_number,
            .is_leaf = (page_type == 0x0D or page_type == 0x0A),
        };
    }

    pub fn getCell(self: Self, cell_idx: u16) ![]const u8 {
        if (cell_idx >= self.getHeader().cell_count)
            return error.InvalidCellIndex;

        const offset = self.getCellOffset(cell_idx);
        if (offset >= self.page_size)
            return error.InvalidCellOffset;

        var payload_size: u64 = 0;
        const payload_bytes = try readVarint(self.data[offset..], &payload_size);

        return self.data[offset..offset + payload_bytes + @as(usize, @intCast(payload_size))];
    }
};
