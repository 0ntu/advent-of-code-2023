const std = @import("std");

pub fn main() !void {
    const asdf: *const [4:0]u8 = "asdf";
    const array = asdf[0..asdf.len].*;
    array[1] = 'q';

    std.debug.print("{} /// {s} {s}\n", .{@TypeOf(asdf) ,array, asdf});
}
