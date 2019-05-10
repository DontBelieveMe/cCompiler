import sys, os.path, os
import datetime

path = os.path.abspath(os.path.dirname(__file__))
path = os.path.join(path, '../vendor/opcodes/opcodes')

sys.path.append(path)

import x86

def write_to_output_file(content, filename):
    path = os.path.abspath(os.path.dirname(__file__))
    path = os.path.join(path, filename)

    with open(path, 'w') as f:
        f.write(content)

isa = x86.read_instruction_set(os.path.join(path, 'x86.xml'))

cpp_file = ''
h_file = '''// Generated Source File. Do Not Edit!
// Creation Time: ''' + str(datetime.datetime.now()) + '''
// Generated by tools/x86-gen.py
// Do not include this file in source control!
// It will be created by the build system

#pragma once

#include <cstddef>
#include <vector>
#include <array>
#include <cc/int_types.h>

namespace cc {
    enum class EX86Instruction {
'''

last_instruction_name = None
unique_instruction_number = 0

for instruction in isa:
    for form in instruction.forms:
        if instruction.name != last_instruction_name:
            inst_name_pretty = instruction.name.capitalize()
            h_file += '\t\t' + inst_name_pretty + ' = ' + str(unique_instruction_number) + ',\n'
            unique_instruction_number += 1

        last_instruction_name = instruction.name

h_file += '\t};\n' # Close brace for EX86Instruction enum

registers = [
    'eax',
    'ebx',
    'ecx',
    'edx',
    'esi',
    'edi',
    'esp'
]

h_file += '\n\tenum class EX86Registers {\n'

for register in registers:
    h_file += '\t\t' + register.capitalize() + ',\n'

h_file += '\t};\n' # Close brace for EX86Registers enum
h_file += '''
    class X86InstructionForm
    {
    private:
        std::array<u8, 4> m_opcodes;
        u8 m_num_opcodes;

    public:
        X86InstructionForm(std::array<u8, 4> opcodes, u8 num_opcodes)
            : m_opcodes(opcodes), m_num_opcodes(num_opcodes)
        {
        }

        const std::array<u8, 4>& OpcodeBytes() const { return m_opcodes; }
        u8 NumOpcodeBytes() const { return m_num_opcodes; }
    };

    class X86Instruction
    {
    private:
        const char* m_name;
        EX86Instruction m_ins;
        std::vector<X86InstructionForm> m_forms;

    public:
        X86Instruction(const char* name, EX86Instruction ins, const std::vector<X86InstructionForm>& forms)
            : m_name(name), m_forms(forms), m_ins(ins)
        { }

        const char* Name() const { return m_name; }
        const std::vector<X86InstructionForm> Forms() const { return m_forms; }
    };

    class X86InstructionSet
    {
    public:
        static constexpr int NumberOfInstructions = ''' + str(unique_instruction_number) + ''';
    private:
        static X86Instruction s_instructions[NumberOfInstructions];

    public:

        static X86Instruction* GetInstructionFromName(const char* name);
    };
'''
h_file += '}\n' # Close brace for cc namespace

cpp_file = '''#include <cc/x86_data.h>
using namespace cc;

X86Instruction X86InstructionSet::s_instructions[] = {
'''
for instruction in isa:
    inst_name = instruction.name.lower()
    enum_ident = 'EX86Instruction::' + instruction.name.capitalize()

    cpp_file += '\t{\n'
    cpp_file += '\t\t"' + inst_name + '", ' + enum_ident + ',\n\t\t{\n'

    for form in instruction.forms:
        for encoding in form.encodings:
            opcodes = []

            for component in encoding.components:
                if type(component) is x86.Opcode:
                    opcodes.append(component.byte)

                if type(component) is x86.Prefix:
                    if component.is_mandatory:
                        opcodes.append(component.byte)

            if len(opcodes) > 0:
                assert(len(opcodes) <= 4) # Limit imposed by code generator

                cpp_file += '\t\t\tX86InstructionForm({'

                for op in opcodes:
                    cpp_file += hex(op) + ','

                cpp_file += '}, ' + str(len(opcodes)) + '),\n'

    cpp_file += '\t\t}\n\t},\n'

cpp_file += '};'

write_to_output_file(cpp_file, 'x86_data.cpp')
write_to_output_file(h_file, 'x86_data.h')
