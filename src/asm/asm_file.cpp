#include <cc/asm/asm_file.h>
#include <cc/asm/asm_parser.h>

#include <cc/timing.h>

using namespace cc::assembly;

asm_file::asm_file(const cc::string& filepath) {
	asm_parser parser = asm_parser::from_file(filepath);
	
	{	
		time_block perf([](float s){
			CINFO("Tokenizing took {0}s", s);
		});
		
		token tok = parser.parse_next_token();
		while(tok.get_type() != kTok_EOF) {
			cc::string str(tok.get_data(), tok.get_data() + tok.get_data_length());
			if(str == "\n") str = "\\n";
			CDEBUG("Token [{0}] '{1}'", tok.get_type(), str);
			tok = parser.parse_next_token();
		}
	}


	cc::unique_ptr<coff::object_file> object = cc::make_unique<coff::object_file>();
	
	m_object_file = std::move(object);
}

