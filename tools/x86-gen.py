import sys, os.path, os, datetime

import cppy

# Have to do this hack in order to import the x86 library
path = os.path.abspath(os.path.dirname(__file__))
path = os.path.join(path, '../vendor/opcodes/opcodes')
sys.path.append(path)
import x86

isa = x86.read_instruction_set(os.path.join(path, 'x86.xml'))

def generate_and_write(template):
    print("Generating file from {0}...".format(template.templatefilename))

    start_time = datetime.datetime.now()
    result     = template.parse()
    delta      = datetime.datetime.now() - start_time

    print("Generated ({0}ms)...".format(int(round(delta.total_seconds() * 1000))))

    print("Writing file {0}...".format(template.outputfilename))

    with open(template.outputfilename, 'w') as f:
        f.write(result)

# List of all possible operand types, formatted in a C++ friendly (allthough not pretty)
# way. This data is used to generate operand type enums in the template file.
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
    'vm64x_k_', 'vm64y_k_', 'm32_k_'
]

# Setup, parse/generate and write the x86_data.h generated template file
headerfile = cppy.CppTemplateFile('tools/x86_data.template.h', 'tools/x86_data.h')
headerfile.register_var('operand_types', operand_types)
headerfile.register_var('isa', isa)

generate_and_write(headerfile)

# Setup, parse/generate and write the x86_data.cpp generated template file
sourcefile = cppy.CppTemplateFile('tools/x86_data.template.cpp', 'tools/x86_data.cpp')
sourcefile.register_var('isa', isa)
sourcefile.register_var('x86', x86)

generate_and_write(sourcefile)
