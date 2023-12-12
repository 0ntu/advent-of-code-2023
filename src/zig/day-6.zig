const std = @import("std");
const stdout = std.io.getStdOut().writer();
const ArrayList = std.ArrayList;
var gpa = std.heap.GeneralPurposeAllocator(.{}){};
const allocator = gpa.allocator();

const num_races = 4;

const Range = struct {
    start: u32,
    end: u32,
};

const Race = struct {
    race_time: u32,
    record_distance: u32,
};

fn distanceTraveled(charge_time: u32, total_time: u32) u32 {
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
        const charge: u32 = @intCast(i);
        if (distanceTraveled(charge, race.race_time) > race.record_distance) {
            res.start = charge;
            break;
        }
    }

    //backward, find end
    for (1..race.race_time) |i| {
        const charge: u32 = race.race_time - @as(u32, @intCast(i));
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

    var it = std.mem.tokenizeAny(u8, data, "\n ");

    // --- Get Input ----
    // ignore Time:
    _ = it.next();

    var Races: [4]Race = undefined;
    for (0..Races.len) |i| {
        Races[i].race_time = try std.fmt.parseInt(u32, it.next().?, 10);
    }
    // ignore Distance
    _ = it.next();

    for (0..Races.len) |i| {
        Races[i].record_distance = try std.fmt.parseInt(u32, it.next().?, 10);
    }
    // -----------------

    var answer: u32 = 1;
    for (Races) |race| {
        const winnning_charges = winningChargeTimesRange(race);
        std.log.info("winning_charges {any}", .{winnning_charges});
        answer *= winnning_charges.end - winnning_charges.start;
    }

    try stdout.print("Answer: {d}\n", .{answer});
}
