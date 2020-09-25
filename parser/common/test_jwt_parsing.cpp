#include <gtest/gtest.h>

#include "JWT_Interface.hpp"
#include "Formatters.hpp"

using namespace NCBI::Logging;
using namespace std;

TEST( JWT_Parsing, Create )
{
    JWTReceiver receiver( make_shared<JsonFastFormatter>() );
    JWTParseBlock pb( receiver );
}

TEST( JWT_Parsing, EmptyString )
{
    JWTReceiver receiver( make_shared<JsonFastFormatter>() );
    JWTParseBlock pb( receiver );
    pb.format_specific_parse( "", 0 );
    ASSERT_EQ( "{}", receiver.GetFormatter().format() );
}

TEST( JWT_Parsing, NoJwt )
{
    JWTReceiver receiver( make_shared<JsonFastFormatter>() );
    JWTParseBlock pb( receiver );
    pb.format_specific_parse( "blah blah blah", 0 );
    ASSERT_EQ( "{}", receiver.GetFormatter().format() );
}

TEST( JWT_Parsing, Jwt )
{
    JWTReceiver receiver( make_shared<JsonFastFormatter>() );
    JWTParseBlock pb( receiver );

    const string jwt = "eyJhbGciOiJSUzI1NiIsImtpZCI6Ijc0NGY2MGU5ZmI1MTVhMmEwMWMxMWViZWIyMjg3MTI4NjA1NDA3MTEiLCJ0eXAiOiJKV1QifQ.eyJhdWQiOiJodHRwczovL3d3dy5uY2JpLm5sbS5uaWguZ292IiwiYXpwIjoiMTA5MDI3MzA5Mzc3ODQwMDg5NTA2IiwiZW1haWwiOiIxMDc2Nzc1Njk3Mjg0LWNvbXB1dGVAZGV2ZWxvcGVyLmdzZXJ2aWNlYWNjb3VudC5jb20iLCJlbWFpbF92ZXJpZmllZCI6dHJ1ZSwiZXhwIjoxNTk2ODk1OTUyLCJnb29nbGUiOnsiY29tcHV0ZV9lbmdpbmUiOnsiaW5zdGFuY2VfY3JlYXRpb25fdGltZXN0YW1wIjoxNTk1MTQ5OTg0LCJpbnN0YW5jZV9pZCI6IjI5MTE2NjU1NjAyNjEwMTM4NTMiLCJpbnN0YW5jZV9uYW1lIjoiaW5zdGFuY2UtMSIsInByb2plY3RfaWQiOiJnb2xkLW9wdGljcy0yNjI0MDciLCJwcm9qZWN0X251bWJlciI6MTA3Njc3NTY5NzI4NCwiem9uZSI6InVzLWNlbnRyYWwxLWEifX0sImlhdCI6MTU5Njg5MjM1MiwiaXNzIjoiaHR0cHM6Ly9hY2NvdW50cy5nb29nbGUuY29tIiwic3ViIjoiMTA5MDI3MzA5Mzc3ODQwMDg5NTA2In0.TaaroLHnzskyEzb1gavhvzWAVBT3_gjZ2n5FV36OEHPKwE0tazIx7vcdvA2CEhKLiI7Gfcr-ilRnrPBeucZfxXMiceO5C6ddwnBHyF05d6iw4kDVFm0-k39qgt8XIRLK6vHDs1sswhoBGJ1lrJrGr4-JbwwXwcEYZ2wqh9XQmwX7wOszNtWV-InGVEjp5Dvwyi2R64J6efvR6M_ZI9PGbqsdTHueKqeezqdRxYG0DA8yNOT18Z8CHm2y2ZIODJv7cF18iviShv2AA5KMRkXiy_aySyUBdZDuPoXqMd2cnmMsmb0xfus_8LYl42r5wk7JO5tmDyQYPqG6CZGia-o55A";
    string str = string("ident=") + jwt;
    pb.format_specific_parse( str.c_str(), str.size() );
    ASSERT_EQ( string ("{\"jwt1\":\"" ) + jwt + "\"}", receiver.GetFormatter().format() );
}

TEST( JWT_Parsing, Multiple_Jwt )
{
    JWTReceiver receiver( make_shared<JsonFastFormatter>() );
    JWTParseBlock pb( receiver );

    const string jwt1 = "eyJ.eyJ.T1";
    const string jwt2 = "eyJ.eyJ.T2";
    const string jwt3 = "eyJ.eyJ.T3";

    string str = string("ident=") + jwt1 + "&jwt1=" + jwt2 + "&jwt2=" + jwt3;
    pb.format_specific_parse( str.c_str(), str.size() );
    ASSERT_EQ( string ("{\"jwt1\":\"" ) + jwt1 + "\",\"jwt2\":\"" + jwt2 + "\",\"jwt3\":\"" + jwt3 + "\"}",
                receiver.GetFormatter().format() );
}

