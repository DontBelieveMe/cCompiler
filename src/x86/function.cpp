#include <cc/x86/function.h>

using namespace cc::x86;

Function Function::make_function(const cc::String& name, cc::u32 func_attribs) {
	Function func;
	func.m_name = name;
	func.m_attribs = func_attribs;

	return func;
}

void Function::add_instruction(const Instruction& ins) {
	m_instructions.add(ins);
}

void Function::add_instructions(const cc::Array<Instruction>& instructions) {
	m_instructions.add(instructions);
}
