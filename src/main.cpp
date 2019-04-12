#include <stdio.h>

#include <cc/file_io.h>
#include <cc/logging.h>

int main(int argc, char* argv[])
{
	cc::logger::startup();
	
	CINFO("hello world!");

	cc::file myfile = cc::read_file("todo.txt", cc::file::read_mode::binary);
	myfile.close();

	return 0;
}
