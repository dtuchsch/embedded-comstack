#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Packet.h"

TEST_CASE( "Boolean", "[bool]" ) 
{
    Packet< 2 > packet;
    boolean in = false;
    packet << in;
    in = true;
    packet << in;

    boolean out = false;
    packet >> out;
    REQUIRE( out == false );
    packet >> out;
    REQUIRE( out == true );
}

TEST_CASE( "Unsigned Byte", "[unsigned-byte]" ) 
{
    Packet< 1 > packet;
    uint8 in = 'a';
    packet << in;
    uint8 out = 0x00U;
    packet >> out;
    REQUIRE( out == 'a' );
}

TEST_CASE( "Signed Byte", "[signed-byte]" ) 
{
    Packet< 1 > packet;
    sint8 in = -120;
    packet << in;
    sint8 out = 0x00;
    packet >> out;
    REQUIRE( out == -120 );
}

TEST_CASE( "Unsigned Word", "[unsigned-word]" ) 
{
    Packet< 2 > packet;
    uint16 in = 0x1234U;
    packet << in;
    uint16 out = 0x0000U;
    packet >> out;
    REQUIRE( out == 0x1234U );
}

TEST_CASE( "Signed Word", "[signed-word]" ) 
{
    Packet< 2 > packet;
    sint16 in = -23000;
    packet << in;
    sint16 out = 0;
    packet >> out;
    REQUIRE( out == -23000 );
}

TEST_CASE( "Unsigned Double Word", "[unsigned-dword]" ) 
{
    Packet< 4 > packet;
    uint32 in = 0x12345678U;
    packet << in;
    uint32 out = 0x00000000UL;
    packet >> out;
    REQUIRE( out == 0x12345678U );
}

TEST_CASE( "Signed Double Word", "[signed-dword]" ) 
{
    Packet< 4 > packet;
    sint32 in = -1012397;
    packet << in;
    sint32 out = 0x00000000L;
    packet >> out;
    REQUIRE( out == -1012397 );
}

TEST_CASE( "Unsigned Quad Word", "[unsigned-qword]" ) 
{
    Packet< 8 > packet;
    uint64 in = 0x1234567811776688ULL;
    packet << in;
    uint64 out = 0x00ULL;
    packet >> out;
    REQUIRE( out == 0x1234567811776688U );
}

TEST_CASE( "Floating Point 32 Bit", "[float32]" ) 
{
    Packet< 4 > packet;
    float32 in = -2.56F;
    packet << in;
    float32 out = 0.0F;
    packet >> out;
    REQUIRE( out == -2.56F );
}

TEST_CASE( "Floating Point 64 Bit", "[float64]" ) 
{
    Packet< 8 > packet;
    float64 in = -90.66F;
    packet << in;
    float64 out = 0.0F;
    packet >> out;
    REQUIRE( out == -90.66F );
}

TEST_CASE( "C-String", "[c-string]" ) 
{
    Packet< 11 > packet;
    const char* in = "packet";
    packet << in;
    char out[6] = "";
    packet >> out;
    REQUIRE( std::strcmp(in, out) == 0 );
}

