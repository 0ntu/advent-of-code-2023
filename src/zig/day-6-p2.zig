const std = @import("std");
const stdout = std.io.getStdOut().writer();
const ArrayList = std.ArrayList;
var gpa = std.heap.GeneralPurposeAllocator(.{}){};
const allocator = gpa.allocator();

const num_races = 4;

const Range = struct {
    start: u64,
    end: u64,
};

const Race = struct {
    race_time: u64,
    record_distance: u64,
};

fn distanceTraveled(charge_time: u64, total_time: u64) u64 {
    const remaining_time = total_time - charge_time;
    const speed = charge_time;

    return remaining_time * speed;
}

/// returns Range of times that beat record_distance
/// inclusive, non-inclusive
fn winningChargeTimesRange(race: Race) Range {
    // test charge times
    // charge times of 0 and last always result in loss (distance of 0), do not test
    var res = Range{
        .start = undefined,
        .end = undefined,
    };

    //forward, find start
    for (1..race.race_time) |i| {
        const charge: u64 = @intCast(i);
        if (distanceTraveled(charge, race.race_time) > race.record_distance) {
            res.start = charge;
            break;
        }
    }

    //backward, find end
    for (1..race.race_time) |i| {
        const charge: u64 = race.race_time - @as(u64, @intCast(i));
        if (distanceTraveled(charge, race.race_time) > race.record_distance) {
            res.end = charge + 1;
            break;
        }
    }

    return res;
}

pub fn main() !void {
    //compute at runtime
    var data = @embedFile("day-6-data.txt");

    // --- Get Input ----
    // ignore Time:

    var times_str = ArrayList(u8).init(allocator);
    defer times_str.deinit();
    var i: usize = 0;
    while (data[i] != '\n') : (i += 1) {
        if (std.ascii.isDigit(data[i])) {
            try times_str.append(data[i]);
        }
    }

    // ignore Distance
    var distances_str = ArrayList(u8).init(allocator);
    defer distances_str.deinit();
    // skip newline
    i += 1;
    while ((data[i] != '\n') and (data[i] != 0)) : (i += 1) {
        if (std.ascii.isDigit(data[i])) {
            try distances_str.append(data[i]);
        }
    }

    // -----------------

    const race: Race = .{ .race_time = try std.fmt.parseInt(u64, times_str.items, 10), .record_distance = try std.fmt.parseInt(u64, distances_str.items, 10) };
    const winnning_charges = winningChargeTimesRange(race);
    const answer = winnning_charges.end - winnning_charges.start;
    try stdout.print("Answer: {d}\n", .{answer});
}
