#include <cc/x86/function.h>

using namespace cc::x86;

function function::make_function(const cc::string& name, cc::u32 func_attribs) {
	function func;
	func.m_name = name;
	func.m_attribs = func_attribs;

	return func;
}

void function::add_instruction(const instruction& ins) {
	m_instructions.add(ins);
}

void function::add_instructions(const cc::array<instruction>& instructions) {
	m_instructions.add(instructions);
}
