#include <cc/x86_data.h>

using namespace cc;

const X86Register X86Register::Eax("eax", 0x00);
const X86Register X86Register::Ecx("ecx", 0x01);
const X86Register X86Register::Edx("edx", 0x02);
const X86Register X86Register::Ebx("ebx", 0x03);
const X86Register X86Register::Esp("esp", 0x04);
const X86Register X86Register::Ebp("ebp", 0x05);
const X86Register X86Register::Esi("esi", 0x06);
const X86Register X86Register::Edi("edi", 0x07);

X86Instruction X86InstructionSet::s_instructions[] =
{
$(
for instruction in isa:
	inst_name = instruction.name.lower()
	enum_identifier = "EX86Instruction::{0}".format(instruction.name.capitalize())

	emit_line("\t{")
	emit("\t\t")
	emit_line("\"{0}\", {1},\n\t\t{{".format(inst_name, enum_identifier))

	for form in instruction.forms:
		operands = form.operands

		for encoding in form.encodings:
			opcodes = []

			for component in encoding.components:
				if type(component) is x86.Opcode:
					opcodes.append(component.byte)
				if type(component) is x86.Prefix:
					opcodes.append(component.byte)

			if len(opcodes) > 0:
				indent(3)
				emit("X86InstructionForm({")

				for opcode in opcodes:
					emit(hex(opcode) + ",")

				emit("}}, {0}".format(str(len(opcodes))))
				emit_line(",")
				indent(4)

				emit("{")

				for operand in operands:
					# Sanitize the operand type that comes in in raw form (aka r/m32 or k{0}) and
					# make it C++ "safe"

					t = operand.type.replace('{', '_').replace('}', '_').replace('/', '_')

					if operand.type == "1" or operand.type == "3":
						emit("EX86Operand::Constant{0},".format(operand.type))
					else:
						emit("EX86Operand::{0},".format(t.capitalize()))

				emit("}}, {0}".format(str(len(operands))))
				emit_line("\n\t\t\t),")
	indent(2)
	emit_line("}\n\t},")
$)
};
