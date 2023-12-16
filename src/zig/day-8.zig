const std = @import("std");

const Move = enum {
    Left,
    Right,
};

//fork in the road
const Node = struct {
    id: []const u8,
    left: ?*Node = null,
    right: ?*Node = null,
};


pub fn main() !void {
    //init
    var puzzle_input = @embedFile("day-8-data.txt");
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();
    const allocator = arena.allocator(); 

    //input handling
    var it = std.mem.tokenizeAny(u8, puzzle_input, "\n=(,) ");
    
    //parse instructions from input to arraylist
    var instructions = std.ArrayList(Move).init(allocator);
    const moves_txt = it.next().?;
    for (moves_txt) |move| {
        try instructions.append(try parseMove(move));
    }

    //get all fork ids & instantiate
    std.debug.print("Parsing & Generating map...\n", .{});
    var network = std.ArrayList(Node).init(allocator);
    try network.ensureTotalCapacity(3000);
    

    //parse node, and its choices
    //if left or right choice does not exist, create node for choice
    while (it.next()) |node_id| {
        const lnode_id = it.next().?;
        const rnode_id = it.next().?;

        //dont invalidate pointers when addingone
        //get lnode and rnode, create them if they dont exist in the network
        const lnode: *Node = findNode(network.items, lnode_id) orelse blk: {
            const ptr = network.addOneAssumeCapacity();
            ptr.*.id = lnode_id;
            ptr.*.left = null;
            ptr.*.right = null;

            break :blk ptr;
        };
        const rnode: *Node = findNode(network.items, rnode_id) orelse blk: {
            const ptr = network.addOneAssumeCapacity();
            ptr.*.id = rnode_id;
            ptr.*.left = null;
            ptr.*.right = null;

            break :blk ptr;
        };

        //set lnode and rnode to current_node
        //create current_node if it does not exist in the network
        if (findNode(network.items, node_id)) |current_node| {
            current_node.left = lnode;
            current_node.right = rnode;
        } else {
            network.appendAssumeCapacity(.{
                .id = node_id,
                .left = lnode,
                .right = rnode,
            });
        }
    }

    //print
    for (network.items) |node| {
        var nodel: ?[]const u8 = null;
        if (node.left) |nodel_unwrapped|{
            nodel = nodel_unwrapped.*.id;
        }

        var noder: ?[]const u8 = null;
        if (node.right) |noder_unwrapped|{
            noder = noder_unwrapped.*.id;
        }

        std.debug.print("{s} - {?s} {?s}\n", .{node.id, nodel, noder});
    }
    
    // -------
    
    const steps: u32 = countSteps(network.items, instructions.items, "AAA", "ZZZ");
    std.debug.print("Answer: {d}\n", .{steps});
}

pub fn parseMove(ch: u8) !Move {
    switch (ch) {
        'L' => return Move.Left,
        'R' => return Move.Right,
        else => return error.InvalidMove,
    }
}

pub fn findNode(node_list: []Node, id: []const u8) ?*Node {
    for (node_list) |*node| {
        if (std.mem.eql(u8, node.*.id, id)) return node;
    }

    return null;
}

pub fn countSteps(
    network: []Node,
    instructions: []Move,
    start_id:[]const u8,
    end_id: []const u8,
) u32 {
    var current_node: *Node = findNode(network, start_id).?;
    var steps: u32 = 0;
    
    var i: usize = 0;
    while (!std.mem.eql(u8, current_node.id, end_id)) : ({i += 1; steps += 1;}) {
        if (i == instructions.len) i = 0;
        switch (instructions[i]) {
            Move.Left => current_node = current_node.left.?,
            Move.Right => current_node = current_node.right.?,
        }
    }
    return steps;
}

test "findNode" {
    var network = std.ArrayList(Node).init(std.testing.allocator);
    defer network.deinit();

    try network.append(.{
        .id = "asd"
    });
    try network.append(.{
        .id = "fgh"
    });
    try network.append(.{
        .id = "jkl"
    });

    try std.testing.expect(findNode(network.items, "asd") != null);
    try std.testing.expect(findNode(network.items, "fgh") != null);
    try std.testing.expect(findNode(network.items, "jkl") != null);
    try std.testing.expect(findNode(network.items, "add") == null);
}
