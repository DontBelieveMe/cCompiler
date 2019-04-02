json = require "../../vendor/json.lua/json"

local x86file = io.open("x86-input.json", "r")

local x86 = json.decode(x86file:read("*all"))
x86file:close()

local instructions = x86.instructions
print(#instructions)
local cdef = io.open("x86.cdef", "w");
local cdef_str = ""
for i = 1, #instructions do
	local instruction = instructions[i]
	cdef_str = cdef_str .. instruction[1] .. '\n'
end
cdef:write(cdef_str)
cdef:close()