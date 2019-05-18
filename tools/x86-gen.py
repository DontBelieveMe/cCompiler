import sys, os.path, os
import datetime

import cppy

path = os.path.abspath(os.path.dirname(__file__))
path = os.path.join(path, '../vendor/opcodes/opcodes')

sys.path.append(path)

import x86

isa = x86.read_instruction_set(os.path.join(path, 'x86.xml'))

operand_types = [
    'xmm0', 'rel8', 'rel32', 'imm4', 'imm8', 'imm16', 'imm32',
    'r8', 'r16', 'r32', 'mm', 'xmm', 'ymm', 'zmm', 'm', 'm8', 'm16',
    'm32', 'm64', 'm80', 'm128', 'm256', 'eax', 'al', 'ax', 'cl', 'r64',
    'constant1', 'constant3', 'k', 'moffs32', 'xmm_k_', 'xmm_k__z_', 'ymm_k_',
    'ymm_k__z_', 'zmm_k_', 'zmm_k__z_', 'k_k_', '_er_', 'm512', 'm128_m64bcst',
    'm256_m64bcst', 'm512_m64bcst', 'm128_m32bcst', 'm256_m32bcst', 'm512_m32bcst',
    '_sae_', 'm128_k__z_', 'm256_k__z_', 'm512_k__z_', 'm64_m32bcst', 'm64_k__z_',
    'vm32y_k_', 'vm32z_k_', 'vm64z_k_', 'vm64y', 'vm32y', 'vm32x',
    'vm64x', 'vm32z', 'm64_k_', 'vm64z', 'm32_k__z_', 'm16_k__z_', 'vm32x_k_',
    'vm64x_k_', 'vm64y_k_', 'm32_k_',
]

headerfile = cppy.CppTemplateFile('tools/x86_data.template.h', 'tools/x86_data.h')
headerfile.register_var('operand_types', operand_types)
headerfile.register_var('isa', isa)

result = headerfile.parse()

with open(headerfile.outputfilename, 'w') as f:
	f.write(result)


sourcefile = cppy.CppTemplateFile('tools/x86_data.template.cpp', 'tools/x86_data.cpp')
sourcefile.register_var('isa', isa)

result = sourcefile.parse()

with open(sourcefile.outputfilename, 'w') as f:
	f.write(result)

"""
cpp_file = '''#include <cc/x86_data.h>
using namespace cc;

const X86Register X86Register::Eax("eax", 0x00);
const X86Register X86Register::Ecx("ecx", 0x01);
const X86Register X86Register::Edx("edx", 0x02);
const X86Register X86Register::Ebx("ebx", 0x03);
const X86Register X86Register::Esp("esp", 0x04);
const X86Register X86Register::Ebp("ebp", 0x05);
const X86Register X86Register::Esi("esi", 0x06);
const X86Register X86Register::Edi("edi", 0x07);

X86Instruction X86InstructionSet::s_instructions[] = {
'''
for instruction in isa:
    inst_name = instruction.name.lower()
    enum_ident = 'EX86Instruction::' + instruction.name.capitalize()

    cpp_file += '\t{\n'
    cpp_file += '\t\t"' + inst_name + '", ' + enum_ident + ',\n\t\t{\n'

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
                cpp_file += '\t\t\tX86InstructionForm({'

                for op in opcodes:
                    cpp_file += hex(op) + ','

                cpp_file += '}, ' + str(len(opcodes))
                cpp_file += ',\n\t\t\t\t'

                cpp_file += '{'

                for op in operands:
                    t = op.type.replace('{', '_').replace('}', '_').replace('/', '_')

                    if op.type == '1' or op.type == '3':
                        cpp_file += 'EX86Operand::Constant' + op.type + ','
                    else:
                        cpp_file += 'EX86Operand::' + t.capitalize() + ','

                cpp_file += '},' + str(len(operands))

                cpp_file += '\n\t\t\t),\n'

    cpp_file += '\t\t}\n\t},\n'

cpp_file += '};'

write_to_output_file(cpp_file, 'x86_data.cpp')
write_to_output_file(h_file, 'x86_data.h')
"""