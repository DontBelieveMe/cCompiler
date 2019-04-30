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

namespace cc {
    enum class EX86Instruction {
'''

last_instruction_name = None
unique_instruction_number = 1

for instruction in isa:
    for form in instruction.forms:
        cpp_file += instruction.name + ' '
        
        if instruction.name != last_instruction_name:
            inst_name_pretty = instruction.name.capitalize()
            h_file += '\t\t' + inst_name_pretty + ' = ' + str(unique_instruction_number) + ',\n'
            unique_instruction_number += 1

        last_instruction_name = instruction.name

        for operand in form.operands:
            cpp_file += operand.type + ','

        cpp_file += ' '

        for encoding in form.encodings:
            for component in encoding.components:
                if type(component) is x86.Opcode:
                    cpp_file += hex(component.byte) + '(' + str(component.addend) + ') '
                    
        cpp_file += '\n'

h_file += '\t};\n}\n'

write_to_output_file(cpp_file, 'x86.cpp')
write_to_output_file(h_file, 'x86.h')