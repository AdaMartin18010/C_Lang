//
// Auto-generated from: Zig\Zig_Embedded_Systems.md
// Line: 820
// Language: zig
// Block ID: f45c5ec4
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// freertos.zig

const freertos = @cImport({
    @cInclude("FreeRTOS.h");
    @cInclude("task.h");
    @cInclude("queue.h");
    @cInclude("semphr.h");
});

pub const Task = struct {
    handle: freertos.TaskHandle_t,

    pub fn create(name: []const u8, stack_size: u16, priority: u32, func: fn () void) !Task {
        var handle: freertos.TaskHandle_t = undefined;

        const result = freertos.xTaskCreate(
            @ptrCast(func),
            name.ptr,
            stack_size,
            null,
            priority,
            &handle
        );

        if (result != freertos.pdPASS) {
            return error.TaskCreateFailed;
        }

        return .{ .handle = handle };
    }

    pub fn delay(ms: u32) void {
        freertos.vTaskDelay(ms / portTICK_PERIOD_MS);
    }

    pub fn start_scheduler() noreturn {
        freertos.vTaskStartScheduler();
        unreachable;
    }
};

pub const Queue = struct {
    handle: freertos.QueueHandle_t,

    pub fn create(length: u32, item_size: u32) !Queue {
        const handle = freertos.xQueueCreate(length, item_size);
        if (handle == null) return error.QueueCreateFailed;
        return .{ .handle = handle };
    }

    pub fn send(self: Queue, item: *const anyopaque, timeout_ms: u32) bool {
        return freertos.xQueueSend(self.handle, item, timeout_ms) == freertos.pdTRUE;
    }

    pub fn receive(self: Queue, buffer: *anyopaque, timeout_ms: u32) bool {
        return freertos.xQueueReceive(self.handle, buffer, timeout_ms) == freertos.pdTRUE;
    }
};

// 使用示例
fn led_task() void {
    while (true) {
        led.toggle();
        Task.delay(500);
    }
}

fn sensor_task() void {
    while (true) {
        const value = adc.read();
        _ = queue.send(&value, 100);
        Task.delay(100);
    }
}
