#include <cc/parsing/scanner.h>
#include <cc/file_io.h>

using namespace cc::parsing;

scanner scanner::from_file(const cc::string& filepath) {
	scanner scan;
	
	cc::file src_file(filepath);
	src_file.read(cc::kFileModeText);

	cc::array<cc::u8> arr = src_file.data_array();
	scan.m_string = cc::string(arr.begin(), arr.end()); 

	return scan;
}
	
