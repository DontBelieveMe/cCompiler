#include <cc/parsing/scanner.h>
#include <cc/file_io.h>

using namespace cc::parsing;

scanner scanner::from_file(const cc::String& filepath) {
	scanner scan;
	
	cc::File src_file(filepath);
	src_file.read(cc::kFileModeText);

	cc::Array<cc::u8> arr = src_file.data_array();
	scan.m_string = cc::String(arr.begin(), arr.end()); 

	return scan;
}
	
