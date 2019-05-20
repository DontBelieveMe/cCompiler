#include <catch2/catch.hpp>

#include <cc/buffer.h>

TEST_CASE("WriteBuffer constructs correctly", "[WriteBuffer]")
{
	using namespace cc;

	SECTION("Constructs correctly with default args")
	{
		WriteBuffer write_buffer;

		REQUIRE(write_buffer.Capacity() == 0);
		REQUIRE(write_buffer.Size() == 0);
		REQUIRE(write_buffer.Data() == nullptr);
	}

	SECTION("Constructs correctly with given initial capacity")
	{
		WriteBuffer write_buffer(4);

		REQUIRE(write_buffer.Capacity() == 4);
		REQUIRE(write_buffer.Size() == 0);
		REQUIRE(write_buffer.Data() != nullptr);
	}
}