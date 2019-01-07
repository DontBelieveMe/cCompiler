#pragma once

#include <cc/stdlib.h>
#include <cc/x86/gp_register.h>

// Instructions:
//
//   1-2		 0-1       0-1       0-4           0-4
// [OpCode] [Mod-Reg-R/M] [SIB] [Displacement] [Immediate]

#define NUM_OPCODES 7

namespace cc {
	namespace x86 {
		enum mnemonic {
			kMov_r32imm32 = 0,
			kAdd_rm32imm32 = 1,
			kMov_r32rm32 = 2,
			kRet = 3,
			kAdd_r32rm32 = 4,
			kSub_rm32imm32 = 5,
			kSub_r32rm32 = 6
		};

		struct opcode {
		public:
			opcode(cc::u8 op, bool encodes_reg_in_opcode, bool uses_rm_and_reg, bool reg_is_extension, cc::u8 extension);

			cc::u8 primary_opcode() const { return m_opcode; }

			inline bool encodes_reg_in_opcode() const { return m_encodes_reg_in_opcode; }
			inline bool uses_rm_and_reg()       const { return m_uses_rm_and_reg; }
			inline bool reg_is_extension()      const { return m_reg_is_extension; }

			cc::u8 extension() const { 
				if (!m_reg_is_extension)
					return -1;

				return m_extension; 
			}

		private:
			cc::u8 m_opcode;
			bool m_encodes_reg_in_opcode;
			bool m_uses_rm_and_reg;

			bool m_reg_is_extension;
			cc::u8 m_extension;
		};

		class instruction {
		public:
			typedef cc::fixed_array<cc::u8, 15> instruction_buffer;

			static instruction make_reg_imm_op(mnemonic op, const cc::x86::gp_register& reg, cc::u32 data);
			static instruction make_2reg_op(mnemonic op, const cc::x86::gp_register& reg0, const cc::x86::gp_register& reg1);
			static instruction make_op(mnemonic op);

			cc::size_t size() const { return m_size; }
			instruction_buffer& data() { return m_data; }

		private:
			static opcode m_opcodes[NUM_OPCODES];
			
			instruction_buffer m_data;
			cc::size_t m_size;
		};

		class instructions_collection {
		public:
			instructions_collection(const std::initializer_list<cc::x86::instruction>& instructions);
			instructions_collection();

			cc::array<cc::u8> combine();
			void add(const instruction& ins);
			
			typedef cc::array<instruction> arr_type;

			arr_type::iterator begin() { return m_instructions.begin(); }
			arr_type::const_iterator cbegin() const { return m_instructions.cbegin(); }

			arr_type::iterator end() { return m_instructions.end(); }
			arr_type::const_iterator cend() const { return m_instructions.cend(); }

		private:
			cc::array<instruction> m_instructions;
		};
	}
}