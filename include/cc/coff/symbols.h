#pragma once

#include <cc/stdlib.h>

namespace cc {
	namespace coff {
		enum EBaseDataType {
			kImageSymTypeNull = 0,
			kImageSymTypeVoid = 1,
			kImageSymTypeChar = 2,
			kImageSymTypeShort = 3,
			kImageSymTypeInt = 4,
			kImageSymTypeLong = 5,
			kImageSymTypeFloat = 6,
			kImageSymTypeDouble = 7,
			kImageSymTypeStruct = 8,
			kImageSymTypeUnion = 9,
			kImageSymTypeEnum = 10,
			kImageSymTypeMOE = 11,
			kImageSymTypeByte = 12,
			kImageSymTypeWord = 13,
			kImageSymTypeUint = 14,
			kImageSymTypeDWORD = 15
		};

		enum EComplexType {
			kImageSymDtypeNull = 0,
			kImageSymDtypePointer = 1,
			kImageSymDtypeFunction = 2,
			kImageSymDtypeArray = 3
		};

		enum EStorageClass {
			kImageSymClassEndOfFunction = 0xFF,
			kImageSymClassNull = 0,
			kImageSymClassAutomatic = 1,
			kImageSymClassExternal = 2,
			kImageSymClassStatic = 3,
			kImageSymClassRegister = 4,
			kImageSymClassExternalDef = 5,
			kImageSymClassLabel = 6,
			kImageSymClassUndefinedLabel = 7,
			kImageSymClassMemberOfStruct = 8,
			kImageSymClassArgument = 9,
			kImageSymClassStructTag = 10,
			kImageSymClassMemberOfUnion = 11,
			kImageSymClassUnionTag = 12,
			kImageSymClassTypeDefinition = 13,
			kImageSymClassUndefinedStatic = 14,
			kImageSymClassEnumTag = 15,
			kImageSymClassMemberOfEnum = 16,
			kImageSymClassRegisterParam = 17,
			kImageSymClassBitField = 18,
			kImageSymClassBlock = 100,
			kImageSymClassFunction = 101,
			kImageSymClassEndOfStruct = 102,
			kImageSymClassFile = 103,
			kImageSymClassSection = 104,
			kImageSymClassWeakExternal = 105,
			kImageSymClassCLRToken = 106
		};

		struct SymbolType {
			cc::u8 complex_type;
			cc::u8 base_type;

			SymbolType(const cc::u8& base_type, const cc::u8& complex_type)
				: base_type(base_type), complex_type(complex_type) {}

			SymbolType()
				: base_type(0), complex_type(0) {}

			SymbolType(const cc::u16& type);

			cc::u16 get_type();
		};

		class SymbolName {
		public:
			static SymbolName from_buff(cc::u8* buff);
			
			SymbolName(cc::String str);
			SymbolName(){}

			bool uses_short_name();
			
			cc::String get_short_name();
			cc::u32    get_long_name_string_table_offset();

		private:
			union {
				cc::u8 short_name[8];
				struct {
					cc::u32	 zeroes;
					cc::u32 offset;
				} long_name;
			} m_data;
		};

		struct Symbol {
			static Symbol from_buff(cc::u8* buff);

			SymbolName name;
			cc::u32 value;
			cc::u16 section_number;
			SymbolType type;
			cc::u8 storage_clss;
			cc::u8 number_of_aux_symbols;
			bool is_aux;

			cc::Array<cc::u8> write_to_buffer();
		};

		class SymbolTable {
		private:
			cc::Array<Symbol> m_symbols;

		public:
			SymbolTable(cc::u8* read_buff, cc::u32 nsymbols);
			SymbolTable(){}

			void add_symbol(const Symbol& sym) { m_symbols.push_back(sym); }

			cc::Array<cc::u8> write_to_buffer();
			cc::Array<Symbol>& symbols() { return m_symbols; }
		};
	}
}