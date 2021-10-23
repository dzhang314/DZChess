using StaticArrays

const BitBoard = UInt64
const BOARD_WIDTH = 8
const BOARD_HEIGHT = 8

@assert BOARD_WIDTH >= 1
@assert BOARD_HEIGHT >= 1

const BOARD_SIZE = BOARD_WIDTH * BOARD_HEIGHT
@assert BOARD_SIZE <= 8 * sizeof(BitBoard)

const IntPair = SVector{2,Int}
const ALL_COORDS = [SA[i, j] for i = 1 : BOARD_HEIGHT for j = 1 : BOARD_WIDTH]
const ROOK_DISPLACEMENTS = SA[SA[+1, 0], SA[-1, 0], SA[0, +1], SA[0, -1]]
const BISHOP_DISPLACEMENTS = SA[SA[+1, +1], SA[+1, -1], SA[-1, +1], SA[-1, -1]]

to_square(coords::IntPair) =
    @inbounds ((coords[1] - 1) * BOARD_WIDTH + (coords[2] - 1))

to_board(square::Int) = BitBoard(1) << square
to_board(coords::IntPair) = to_board(to_square(coords))

in_bounds(coords::IntPair) =
    @inbounds ((1 <= coords[1] <= BOARD_HEIGHT) &&
               (1 <= coords[2] <= BOARD_WIDTH))

function slider_mask(origin::IntPair, displs)
    result = BitBoard(0)
    for displ in displs
        temp = origin + displ
        while in_bounds(temp + displ)
            result |= to_board(temp)
            temp += displ
        end
    end
    return result
end

function slider_moves(origin::IntPair, displs, blockers::BitBoard)
    result = BitBoard(0)
    for displ in displs
        temp = origin + displ
        while in_bounds(temp)
            result |= to_board(temp)
            if (to_board(temp) & blockers) != 0
                break
            end
            temp += displ
        end
    end
    return result
end

function bitwise_subsets(x::BitBoard)
    if x == 0
        return BitBoard[0]
    else
        index = trailing_zeros(x)
        bit = BitBoard(1) << index
        y = x & (x - 1)
        rec = bitwise_subsets(y)
        result = BitBoard[]
        for z in rec
            push!(result, z)
            push!(result, z | bit)
        end
        return result
    end
end

move_table(origin::IntPair, displs) = [
    (blockers, slider_moves(origin, displs, blockers))
    for blockers in bitwise_subsets(slider_mask(origin, displs))
]

const BitBoardTable = Vector{Tuple{BitBoard,BitBoard}}
const ROOK_TABLES = [move_table(origin, ROOK_DISPLACEMENTS)
                     for origin in ALL_COORDS]
const BISHOP_TABLES = [move_table(origin, BISHOP_DISPLACEMENTS)
                       for origin in ALL_COORDS]

function is_valid_bit_count(table::BitBoardTable, bit_count)
    dict = Dict{BitBoard,BitBoard}()
    for (long_key, value) in table
        key = long_key >> bit_count
        if haskey(dict, key)
            if dict[key] != value
                return false
            end
        else
            dict[key] = value
        end
    end
    return true
end

function find_bit_count(table::BitBoardTable, magic_number::BitBoard)
    magic_table = [(magic_number * key, value) for (key, value) in table]
    lo = 0
    hi = 64
    for i = 1 : 6
        mid = lo + div(hi - lo, 2)
        if is_valid_bit_count(magic_table, mid)
            lo = mid
        else
            hi = mid
        end
    end
    return lo
end

view_board(board::BitBoard) = transpose(reshape(
    [((board >> i) & 1) != 0 for i = 0 : BOARD_WIDTH * BOARD_HEIGHT - 1],
    (BOARD_WIDTH, BOARD_HEIGHT)
))

function find_magic_numbers()
    magic_numbers = [(0, UInt64(0)) for _ in ALL_COORDS]
    while true
        magic_number = rand(BitBoard) & rand(BitBoard)
        for i = 1 : BOARD_SIZE
            table = ROOK_TABLES[i]
            bit_count = find_bit_count(table, magic_number)
            if magic_numbers[i][1] < bit_count
                magic_numbers[i] = (bit_count, magic_number)
                for j = 1 : BOARD_SIZE
                    if i == j
                        println('*', lpad(j, 2),
                                "*: ", lpad(magic_numbers[j][1], 2),
                                " : 0x", string(magic_numbers[j][2], base=16))
                    else
                        println(' ', lpad(j, 2),
                                " : ", lpad(magic_numbers[j][1], 2),
                                " : 0x", string(magic_numbers[j][2], base=16))
                    end
                end
                println()
            end
        end
    end
end

const ROOK_MAGIC_NUMBERS = UInt64[
    0x6580001040008120, 0x0cc0004410082001, 0x03800ad000832000, 0x31000804500100a0,
    0x02001830604e0044, 0x4600101402004b48, 0x02002804a2002409, 0x0200040240802506,
    0x060b8004804008a8, 0x2356400850016000, 0x00090040e0049101, 0x0202002150406a00,
    0x822a002a0a00b022, 0x8827001900040012, 0x6086000883042200, 0x4202002400c48601,
    0x134560800880c005, 0xb80c830040010428, 0x4143050040a00012, 0x0140990020b00100,
    0x0800808018001402, 0xc312008004008086, 0x34280c0010080d12, 0x648422000984004b,
    0x03c0028080002642, 0x4910084040012002, 0x160010c100200107, 0x8504633900100100,
    0x8071002700080090, 0x0a260106000c5028, 0x3a485d1c00a81006, 0x000188820005034c,
    0x880a401088800024, 0x0051104005002181, 0x4281801000802000, 0xd520801000800800,
    0x834200208e000870, 0x235200487200100c, 0x484630080c001691, 0x0012006492000403,
    0x4000882040018008, 0x7c20004530024000, 0x9189002000c30012, 0x24ab690090010020,
    0x10282cb801010010, 0x8002e40006008080, 0x1a225001085c0086, 0x0b12418420420001,
    0xcc44832200c90600, 0xcc44832200c90600, 0x004201b482604200, 0x0202002150406a00,
    0x0340280104008080, 0x8002e40006008080, 0x6086000883042200, 0x098d4ac08c110a00,
    0x00890512c1220082, 0x00890512c1220082, 0x00004d4011002001, 0xc0816900a00c1001,
    0x012100141800b003, 0x284200107c050812, 0x818761930210080c, 0x11800c010020da82
]

const ROOK_BIT_COUNT = 52

const BISHOP_MAGIC_NUMBERS = UInt64[
    0x5341280611c64100, 0xa660142d36042025, 0x45d00d5041018703, 0x0418061144002080,
    0xaa06021000113410, 0x2182c4206004a8c4, 0x1175413008220208, 0x1004124210042000,
    0x39820858b0740a41, 0xa08a2204030c0104, 0x4040410301190a09, 0x03000c0402888c30,
    0x8200360211016041, 0x100a02084c8418d8, 0x03000c0402888c30, 0x0043452103482028,
    0x4340300488080b30, 0x0022811812081600, 0x1110081807401260, 0x42920a4401220002,
    0xd124001205210640, 0xd032006501860140, 0x0043452103482028, 0x2401c0048a181102,
    0x617620434108ca10, 0x0031a03810040301, 0x00866203f0040442, 0x00400c0010410260,
    0x80160020020080c5, 0x12c802000d405200, 0x94010e0994025904, 0x0000c25121820801,
    0x8c0410159440ec10, 0x200c3a20818a0407, 0x28cc004110081208, 0x00b7601800110105,
    0x012c420400120090, 0x24020800404a004a, 0x818808088a804248, 0x2cc1104486120600,
    0xc42611106806c0a1, 0x500200a208082004, 0x0e01028041081001, 0x0026212051018800,
    0x0114a0061c008082, 0x0908090806010460, 0x82700442a0851c08, 0x8022008a22026081,
    0x1175413008220208, 0x04428a080b244459, 0x2004020462081680, 0x8494825184141064,
    0x4f0822d08a020020, 0x08344124980280a4, 0xa660142d36042025, 0x90bf900cc2c65aac,
    0x1004124210042000, 0x076c16494808036e, 0x0012160900824112, 0x02c11810ece08800,
    0x2c2a008820a0c100, 0x70042418b0311a04, 0x39820858b0740a41, 0x5341280611c64100
]

const BISHOP_BIT_COUNT = 55

function print_magic_tables()
    for i = 1 : BOARD_SIZE
        magic_table = zeros(UInt64, 1 << (64 - ROOK_BIT_COUNT))
        for (blockers, value) in ROOK_TABLES[i]
            key = ((blockers * ROOK_MAGIC_NUMBERS[i]) >> ROOK_BIT_COUNT) + 1
            @assert (magic_table[key] == 0) || (magic_table[key] == value)
            magic_table[key] = value
        end
        for i = 1 : 8 : length(magic_table)
            println("    ", string(magic_table[i:i+7])[8:end-1], ',')
        end
    end
end

print_magic_tables()
