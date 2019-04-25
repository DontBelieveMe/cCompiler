#include <catch2/catch.hpp>

#include <cc/buffer.h>

TEST_CASE("ReadBuffer ReadNext() will read consequtive data from stream", "[ReadBuffer]")
{
	cc::u8 data[] = {
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A
	};

	cc::ReadBuffer read_buffer(data, sizeof(data));

	SECTION("Can read one byte from stream")
	{
		read_buffer.ResetToStart();
		REQUIRE(read_buffer.ReadNext<cc::u8>() == 0x01);
	}

	SECTION("Can read multiple consequtive bytes from stream")
	{
		read_buffer.ResetToStart();

		REQUIRE(read_buffer.ReadNext<cc::u8>() == 0x01);
		REQUIRE(read_buffer.ReadNext<cc::u8>() == 0x02);
		REQUIRE(read_buffer.ReadNext<cc::u8>() == 0x03);
	}

	SECTION("Can read one 16 bit integer from stream")
	{
		read_buffer.ResetToStart();

		// todo: this relies on the system the tests are running on
		//       is little endian
		REQUIRE(read_buffer.ReadNext<cc::u16>() == 0x0201);
	}

	SECTION("Can read multiple consequtive 16 bit integers from stream")
	{
		read_buffer.ResetToStart();

		REQUIRE(read_buffer.ReadNext<cc::u16>() == 0x0201);
		REQUIRE(read_buffer.ReadNext<cc::u16>() == 0x0403);
		REQUIRE(read_buffer.ReadNext<cc::u16>() == 0x0605);
	}

	SECTION("Can read one 32 bit integer from stream")
	{
		read_buffer.ResetToStart();

		REQUIRE(read_buffer.ReadNext<cc::u32>() == 0x04030201);
	}

	SECTION("Can read multiple consequtive 32 bit integers from stream")
	{
		read_buffer.ResetToStart();

		REQUIRE(read_buffer.ReadNext<cc::u32>() == 0x04030201);
		REQUIRE(read_buffer.ReadNext<cc::u32>() == 0x08070605);
	}

	SECTION("Can read a mix of fixed sized integers from stream")
	{
		read_buffer.ResetToStart();

		REQUIRE(read_buffer.ReadNext<cc::u16>() == 0x0201);
		REQUIRE(read_buffer.ReadNext<cc::u8>() == 0x03);
		REQUIRE(read_buffer.ReadNext<cc::u32>() == 0x07060504);
	}
}

TEST_CASE("ReadBuffer PeekNext() can read data without advancing", "[ReadBuffer]")
{
	cc::u8 buffer[] = {
		0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
	};

	cc::ReadBuffer read_buff(buffer, sizeof(buffer));

	SECTION("Can peek 8 bit integer from start of stream")
	{
		REQUIRE(read_buff.PeekNext<cc::u8>() == 0x0A);

		// To verify that the buffer has not advanced
		REQUIRE(read_buff.ReadNext<cc::u8>() == 0x0A);
	}

	SECTION("Can read 16 bit integer from middle of stream")
	{
		read_buff.ResetToStart();

		// Move buffer forward by 3 bytes so it's roughly in
		// the middle of the stream
		read_buff.Advance(3);

		REQUIRE(read_buff.PeekNext<cc::u16>() == 0x0E0D);
		REQUIRE(read_buff.ReadNext<cc::u16>() == 0x0E0D);
	}
}
