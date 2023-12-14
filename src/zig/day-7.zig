const std = @import("std");

// pub fn sliceContains(comptime T: type, haystack: []const T, needle: T) bool {
//     for (haystack) |hay| {
//         if (hay == needle) return true;
//     }
//
//     return false;
// }
//
// pub fn sliceContainsMultiple(comptime T: type, haystack: []const T, needles: []const T) bool {
//     for (needles) |needle| {
//         if (!sliceContains(T, haystack, needle)) return false;
//     }
//
//     return true;
// }

/// assert: first and second have operator >, able to be sorted
pub fn numsEqlUnsorted(comptime T: type, first: []const T, second: []const T) !bool {
    // if (first.len != second.len) return false;
    if (first.len != second.len) return std.debug.panic("reached numsEqlUnsorted", .{});

    // copy contents of arrays to mutable variables
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();
    const allocator = arena.allocator();

    var arr1: []T = try allocator.alloc(T, first.len);
    var arr2: []T = try allocator.alloc(T, second.len);
    @memcpy(arr1, first);
    @memcpy(arr2, second);

    //sort array
    std.sort.heap(usize, arr1, {}, std.sort.asc(T));
    std.sort.heap(usize, arr2, {}, std.sort.asc(T));

    //sorted array equality checking
    for (arr1, arr2) |e1, e2| {
        if (e1 != e2) return false;
    }

    return true;
}

const Hand = struct {
    const Self = @This();

    cards: []const u8,
    bid: u32,

    const HandType = enum {
        FiveOfAKind,
        FourOfAKind,
        FullHouse,
        ThreeOfAKind,
        TwoPair,
        OnePair,
        HighCard,
    };

    const UniqueCard = struct {
        value: u8,
        num_in_hand: usize,
    };

    pub fn init(cards: []const u8, bid: u32) Hand {
        return Hand{
            .cards = cards,
            .bid = bid,
        };
    }

    pub fn getHandType(self: *const Self) !HandType {
        if (self.cards.len != 5) {
            return error.InvalidHandSize;
        }

        var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
        defer arena.deinit();
        const allocator = arena.allocator();
        var unique_cards: []UniqueCard = try self.uniqueFromHand(allocator);

        var num_per_unique = try allocator.alloc(usize, unique_cards.len);

        for (0..unique_cards.len) |i| {
            num_per_unique[i] = unique_cards[i].num_in_hand;
        }

        switch (num_per_unique.len) {
            1 => return HandType.FiveOfAKind,
            2 => {
                if (try numsEqlUnsorted(usize, &[_]usize{ 1, 4 }, num_per_unique)) return HandType.FourOfAKind;
                if (try numsEqlUnsorted(usize, &[_]usize{ 2, 3 }, num_per_unique)) return HandType.FullHouse;
            },
            3 => {
                if (try numsEqlUnsorted(usize, &[_]usize{ 1, 1, 3 }, num_per_unique)) return HandType.ThreeOfAKind;
                if (try numsEqlUnsorted(usize, &[_]usize{ 1, 2, 2 }, num_per_unique)) return HandType.TwoPair;
            },
            4 => return HandType.OnePair,
            5 => return HandType.HighCard,
            else => std.debug.panic("card len > 5", .{}),
        }

        unreachable;
    }

    fn uniqueFromHand(self: *const Self, allocator: std.mem.Allocator) ![]UniqueCard {
        var unique_cards = std.ArrayList(UniqueCard).init(allocator);

        over_held_cards: for (self.cards) |card| {
            for (unique_cards.items) |*unique_card| {
                if (card == unique_card.value) {
                    unique_card.*.num_in_hand += 1;
                    continue :over_held_cards;
                }
            }

            //if no unique_card exists, add it to the unique_cards list
            try unique_cards.append(.{ .value = card, .num_in_hand = 1 });
        }

        return try unique_cards.toOwnedSlice();
    }
};

pub fn main() !void {
    //init
    var puzzle_input = @embedFile("day-7-data.txt");
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();
    const allocator = arena.allocator();

    //get user input
    var hands = std.ArrayList(Hand).init(allocator);
    var it = std.mem.tokenizeAny(u8, puzzle_input, "\n ");

    while (it.peek() != null) {
        var cards = it.next().?;
        var bid = try std.fmt.parseInt(u32, it.next().?, 10);

        try hands.append(Hand.init(cards, bid));
    }

    try rankHands(hands.items);

    var total_winnings: usize = 0;
    for (hands.items, 0..hands.items.len) |hand, i| {
        //hands is sorted worst to best
        //offset by 1 so first element has a rank multiplier = 1
        const rank_multiplier = i+1;
        total_winnings += hand.bid * rank_multiplier;

        std.debug.print("{s} : {any} - {d} * {d}\t {d}\n", .{hand.cards, hand.getHandType(), hand.bid, rank_multiplier, total_winnings});
    }

    std.debug.print("answer: {d}\n", .{total_winnings});
}

pub fn rankHands(hands: []Hand) !void {
    const handsLessThan = struct {
        pub fn inner(_: void, lhs: Hand, rhs: Hand) bool {
            const lhs_handtype: Hand.HandType = lhs.getHandType() catch std.debug.panic("Failed to get lhs handtype", .{});
            const rhs_handtype: Hand.HandType = rhs.getHandType() catch std.debug.panic("Failed to get rhs handtype", .{});

            // rank based on handtype first
            if (@intFromEnum(lhs_handtype) != @intFromEnum(rhs_handtype)) {
                return @intFromEnum(lhs_handtype) > @intFromEnum(rhs_handtype);
            }

            //handtypes are the same, rank based on order in the ascii table (higher is better)
            for (lhs.cards, rhs.cards) |lcard, rcard| {
                if (lcard == rcard) continue;
                return getCardStrength(lcard) < getCardStrength(rcard);
            }

            //cards are completely identical
            //lhs is not < rhs
            return false;
        }
    }.inner;

    std.sort.heap(Hand, hands, {}, handsLessThan);
}

test "hand types" {
    var puzzle_input =
        \\32T3K 765
        \\T55J5 684
        \\KK677 28
        \\KTJJT 220
        \\QQQJA 483
    ;

    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();
    const allocator = arena.allocator();

    //get user input
    var hands = std.ArrayList(Hand).init(allocator);
    var it = std.mem.tokenizeAny(u8, puzzle_input, "\n ");

    while (it.peek() != null) {
        var cards = it.next().?;
        var bid = try std.fmt.parseInt(u32, it.next().?, 10);

        try hands.append(Hand.init(cards, bid));
    }

    try rankHands(hands.items);

    var total_winnings: usize = 0;
    for (hands.items, 0..hands.items.len) |hand, i| {
        //hands is sorted worst to best
        //offset by 1 so first element has a rank multiplier = 1
        const rank_multiplier = i+1;
        total_winnings += hand.bid * rank_multiplier;
    }

    try std.testing.expect(total_winnings == 6440);
}

pub fn getCardStrength(card: u8) u8 {
    switch (card) {
        '2' => return 0,
        '3' => return 1,
        '4' => return 2,
        '5' => return 3,
        '6' => return 4,
        '7' => return 5,
        '8' => return 6,
        '9' => return 7,
        'T' => return 8,
        'J' => return 9,
        'Q' => return 10,
        'K' => return 11,
        'A' => return 12,
        else => std.debug.panic("invalid card", .{}),
    }
}
