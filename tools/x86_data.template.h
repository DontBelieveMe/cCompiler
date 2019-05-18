#pragma once

#include <cstddef>
#include <vector>
#include <array>
#include <cc/int_types.h>

namespace cc
{
	enum EX86Instruction
	{
$(
last_instruction_name = None
global num_unique_instructions
num_unique_instructions = 0
for instruction in isa:
	for form in instruction.forms:
		if instruction.name != last_instruction_name:
			inst_name = instruction.name.capitalize()

			indent(2)
			emit_line("{0} = {1},".format(inst_name, num_unique_instructions))

			num_unique_instructions += 1
			last_instruction_name = instruction.name
$)
	};

	enum EX86Operand
	{
$(
for operand_type in operand_types:
	indent(2)
	emit_line(operand_type.capitalize() + ",")
$)
	};

	class X86Register
	{
	private:
		const char* m_name;
		u8			m_value;

	public:
		X86Register(const char* name, u8 v): m_name(name), m_value(v) {}

		static const X86Register Eax;
		static const X86Register Ecx;
		static const X86Register Edx;
		static const X86Register Ebx;
		static const X86Register Esp;
		static const X86Register Ebp;
		static const X86Register Esi;
		static const X86Register Edi;

	public:
		u8 Value() const
		{
			return m_value;
		}

		const char* Name() const
		{
			return m_name;
		}
	};

	class X86InstructionForm
	{
	private:
		static const std::size_t				MaxNumOperands = 5;
		static const std::size_t				MaxNumOpcodeBytes = 5;

		std::array<u8, MaxNumOpcodeBytes>		m_opcodes;
		u8										m_num_opcodes;

		std::array<EX86Operand, MaxNumOperands> m_operands;
		u8										m_num_operands;

	public:
		X86InstructionForm(std::array<u8, MaxNumOpcodeBytes> opcodes,
						   u8 num_opcodes,
						   std::array<EX86Operand, MaxNumOperands> operands,
						   u8 num_operands)
			: m_opcodes(opcodes),
			  m_num_opcodes(num_opcodes),
			  m_operands(operands),
			  m_num_operands(num_operands)
		{
		}

		const std::array<u8, MaxNumOpcodeBytes>& OpcodeBytes() const
		{
			return m_opcodes;
		}

		u8 NumOpcodeBytes() const
		{
			return m_num_opcodes;
		}

		std::size_t SizeBytes() const
		{
			return 0;
		}

		const std::array<EX86Operand, MaxNumOperands>& Operands() const
		{
			return m_operands;
		}

		u8 NumOperands() const
		{
			return m_num_operands;
		}
	};

	class X86Instruction
	{
	private:
		const char*						m_name;
		EX86Instruction					m_ins;
		std::vector<X86InstructionForm> m_forms;

	public:
		X86Instruction(const char* name, EX86Instruction ins,
					   const std::vector<X86InstructionForm>& forms)
			: m_name(name), m_forms(forms), m_ins(ins)
		{ }

		const char* Name() const
		{
			return m_name;
		}
		
		const std::vector<X86InstructionForm> Forms() const
		{
			return m_forms;
		}

		EX86Instruction InstructionEnum() const
		{
			return m_ins;
		}

		X86InstructionForm* ResolveForm(const std::vector<EX86Operand>& operand_types);
	};

	class X86InstructionSet
	{
	public:
		static constexpr int NumberOfInstructions = $(num_unique_instructions$);

	private:
		static X86Instruction s_instructions[NumberOfInstructions];

	public:
		static X86Instruction* GetInstructionFromName(const char* name);
	};	
}
