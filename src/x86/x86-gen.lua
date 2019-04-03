json = require "../../vendor/json.lua/json"

local x86file = io.open("x86-input.json", "r")

local x86 = json.decode(x86file:read("*all"))
x86file:close()

local instructions = x86.instructions

local cdef = io.open("x86-cdefTMP.cdef", "w");
local cdef_str = ""

function parse_instruction(ins)
	local out = {	}
	
	out.name = ins[1]

	local opcode_string = ins[4]

	if not opcode_string:find("^%x%x") then
		return nil
	end

	local mods = {}

	local accepting_mods = {
		"/%d", "/r", "c[bwdpcot]", "i[bwdo]",
		"%+r[bwdo]", "%+i"
	}

	for i = 1, #accepting_mods do
		local m = opcode_string:match(accepting_mods[i])
		if(m ~= nil) then
			table.insert(mods, m)
		end
	end

	local tmp_opcodes = opcode_string:gmatch("[0-9A-F][0-9A-F]")
	out.opcodes = {}

	for op in tmp_opcodes do
		table.insert(out.opcodes, op)
	end

	out.mods = mods

	local tmp_operands = ins[2]
	if #tmp_operands > 0 then
		tmp_operands = string.sub(tmp_operands, 2)
	end

	tmp_operands = tmp_operands:gsub("[~:<>]", "")

	out.operands = tmp_operands

	return out
end

function build_instruction_string(ins)
	local str = '"' .. ins.name .. "\": ["

	str = str .. ins.operands .. "], ["
	str = str .. "0x" .. table.concat(ins.opcodes) .. '], '

	for i = 1, #ins.mods do
		str = str .. ins.mods[i] .. ", "
	end

	str = str .. '\n'
	return str
end

function capitalize(str)
    return (str:gsub("^%l", string.upper))
end

for i = 1, #instructions do
	local instruction_raw = instructions[i]
	local instruction = parse_instruction(instruction_raw)
	
	if instruction ~= nil then
		cdef_str = cdef_str .. build_instruction_string(instruction)
	end
end

cdef:write(cdef_str)
cdef:close()