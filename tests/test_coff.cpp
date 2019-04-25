#include <catch2/catch.hpp>

#include <cc/coff/coff.h>

TEST_CASE("CoffObjectFile reads machine type correctly", "[CoffObjectFile]")
{
	using namespace cc;

	CoffObjectFile coff_object_file;
	coff_object_file.ReadFromFile("data/masm1.obj");

	REQUIRE(coff_object_file.Machine() == cc::ECoffMachineType::I386);
}

TEST_CASE("CoffObjectFile reads timestamp correctly", "[CoffObjectFile]")
{
	using namespace cc;

	CoffObjectFile object_file;
	object_file.ReadFromFile("data/masm1.obj");

	// This is Tuesday 23rd April 2019 (00:56:33)
	const u32 DATE_CREATED = 0x5CBE54B1;

	REQUIRE(object_file.TimeStamp().UnixTime() == DATE_CREATED);
}
