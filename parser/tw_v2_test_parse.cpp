#include <gtest/gtest.h>

#include <sstream>
#include <vector>

#include "TW_Interface.hpp"
#include "Formatters.hpp"
#include "CatWriters.hpp"
#include "parse_test_fixture.hpp"

using namespace std;
using namespace NCBI::Logging;

#define INIT_TSTR(t, s)     do { (t).p = s; (t).n = strlen(s); (t).escaped = false; } while (false)
#define INIT_TSTR_ESC(t, s) do { (t).p = s; (t).n = strlen(s); (t).escaped = true; } while (false)

TEST( LogTWEventTest, Create )
{
    std::unique_ptr<FormatterInterface> f = make_unique<JsonLibFormatter>();
    TWReceiver e ( f );
}

TEST( LogTWEventTest, Setters )
{
    std::unique_ptr<FormatterInterface> f = make_unique<JsonLibFormatter>();
    TWReceiver e ( f );

    t_str v;

    INIT_TSTR( v, "77619/000/0000/R" );     e.set( TWReceiver::id1, v );
    INIT_TSTR( v, "CC952F33EE2FDBD1" );     e.set( TWReceiver::id2, v );
    INIT_TSTR( v, "0008/0008" );            e.set( TWReceiver::id3, v );
    INIT_TSTR( v, "2020-06-11T23:59:58.079949" ); e.set( TWReceiver::time, v );
    INIT_TSTR( v, "traceweb22" );           e.set( TWReceiver::server, v );
    INIT_TSTR( v, "185.151.196.174" );      e.set( ReceiverInterface::ip, v );
    INIT_TSTR( v, "CC952F33EE2FDBD1_0000SID" );   e.set( TWReceiver::sid, v );
    INIT_TSTR( v, "sra" );                  e.set( TWReceiver::service, v );
    INIT_TSTR( v, "extra" );               e.set( TWReceiver::event, v );
    INIT_TSTR( v, "issued_subhit=m_1" );    e.set( TWReceiver::msg, v );

    ASSERT_EQ (
        "{\"event\":\"extra\",\"id1\":\"77619/000/0000/R\",\"id2\":\"CC952F33EE2FDBD1\",\"id3\":\"0008/0008\",\"ip\":\"185.151.196.174\",\"msg\":\"issued_subhit=m_1\",\"server\":\"traceweb22\",\"service\":\"sra\",\"sid\":\"CC952F33EE2FDBD1_0000SID\",\"time\":\"2020-06-11T23:59:58.079949\"}",
        e.GetFormatter().format() );
}

class TWTestFixture : public ParseTestFixture< TWParseBlockFactory >
{
};

TEST_F( TWTestFixture, Setters_BadUTF8_lib )
{
    std::unique_ptr<FormatterInterface> f = make_unique<JsonLibFormatter>();
    TWReceiver e ( f );

    t_str v;
    INIT_TSTR( v, "issued_su\xe2\x28\xa1hit=m_1" );
    e.set( TWReceiver::id1, v );
    ASSERT_EQ( ReceiverInterface::cat_review, e.GetCategory() );
    string text = e . GetFormatter() . format();
    ASSERT_EQ( "badly formed UTF-8 character in 'id1'", extract_value( text, "_error" ) );
    ASSERT_EQ( "issued_su\\uffffffe2(\\uffffffa1hit=m_1", extract_value( text, "id1" ) );
}

TEST_F( TWTestFixture, Setters_BadUTF8_fast )
{
    std::unique_ptr<FormatterInterface> f = make_unique<JsonFastFormatter>();
    TWReceiver e ( f );

    t_str v;
    INIT_TSTR( v, "issued_su\xe2\x28\xa1hit=m_1" );
    e.set( TWReceiver::id1, v );
    ASSERT_EQ( ReceiverInterface::cat_review, e.GetCategory() );
    string text = e . GetFormatter() . format();
    ASSERT_EQ( "badly formed UTF-8 character in 'id1'", extract_value( text, "_error" ) );
    ASSERT_EQ( "issued_su\\uffffffe2(\\uffffffa1hit=m_1", extract_value( text, "id1" ) );
}

TEST_F( TWTestFixture, Setters_GoodUTF8_lib )
{
    std::unique_ptr<FormatterInterface> f = make_unique<JsonLibFormatter>();
    TWReceiver e ( f );

    t_str v;
    auto s = u8"попробуем 产品公司求购 ";
    INIT_TSTR( v, s );
    e.set( TWReceiver::msg, v );
    string text = e . GetFormatter() . format();
    ASSERT_EQ( s, extract_value( text, "msg" ) );
}

TEST_F( TWTestFixture, Setters_GoodUTF8_fast )
{
    std::unique_ptr<FormatterInterface> f = make_unique<JsonFastFormatter>();
    TWReceiver e ( f );

    t_str v;
    auto s = u8"попробуем 产品公司求购 ";
    INIT_TSTR( v, s );
    e.set( TWReceiver::msg, v );
    string text = e . GetFormatter() . format();
    ASSERT_EQ( s, extract_value( text, "msg" ) );
}

TEST_F( TWTestFixture, LineRejecting )
{
    std::string res = try_to_parse_review( "line1 blah\nline2\nline3\n" );
    ASSERT_EQ(
        "{\"_error\":\"invalid ID1\",\"_line_nr\":1,\"_unparsed\":\"line1 blah\"}\n"
        "{\"_error\":\"invalid ID1\",\"_line_nr\":2,\"_unparsed\":\"line2\"}\n"
        "{\"_error\":\"invalid ID1\",\"_line_nr\":3,\"_unparsed\":\"line3\"}\n", res );
}

TEST_F( TWTestFixture, OneGoodLine )
{
    try_to_parse( "77619/000/0000/R  CC952F33EE2FDBD1 0008/0008 2020-06-11T23:59:58.079949 traceweb22      185.151.196.174 CC952F33EE2FDBD1_0000SID sra extra         issued_subhit=m_1\n" );
    ASSERT_EQ( "{\"event\":\"extra\",\"id1\":\"77619/000/0000/R\",\"id2\":\"CC952F33EE2FDBD1\",\"id3\":\"0008/0008\",\"ip\":\"185.151.196.174\",\"msg\":\"issued_subhit=m_1\",\"server\":\"traceweb22\",\"service\":\"sra\",\"sid\":\"CC952F33EE2FDBD1_0000SID\",\"time\":\"2020-06-11T23:59:58.079949\"}\n",
                s_outputs.get_good() );
}

TEST_F( TWTestFixture, ErrorRecovery )
{
    try_to_parse(
        "line1 blah\n"
        "77619/000/0000/R  CC952F33EE2FDBD1 0008/0008 2020-06-11T23:59:58.079949 traceweb22      185.151.196.174 CC952F33EE2FDBD1_0000SID sra extra         issued_subhit=m_1\n" );
    ASSERT_EQ( "{\"_error\":\"invalid ID1\",\"_line_nr\":1,\"_unparsed\":\"line1 blah\"}\n",
                s_outputs.get_review() );
    ASSERT_EQ( "{\"event\":\"extra\",\"id1\":\"77619/000/0000/R\",\"id2\":\"CC952F33EE2FDBD1\",\"id3\":\"0008/0008\",\"ip\":\"185.151.196.174\",\"msg\":\"issued_subhit=m_1\",\"server\":\"traceweb22\",\"service\":\"sra\",\"sid\":\"CC952F33EE2FDBD1_0000SID\",\"time\":\"2020-06-11T23:59:58.079949\"}\n",
                s_outputs.get_good() );
}

TEST_F( TWTestFixture, unrecognized_char_escaped_in_json_ouput )
{
    std::string res = try_to_parse_review( "line1 \07" );
    ASSERT_EQ( "{\"_error\":\"invalid ID1\",\"_line_nr\":1,\"_unparsed\":\"line1 \07\"}\n", res );
}

TEST_F( TWTestFixture, NoMessage )
{
    std::string res = try_to_parse_good( "77619/000/0000/R  CC952F33EE2FDBD1 0008/0008 2020-06-11T23:59:58.079949 traceweb22      185.151.196.174 CC952F33EE2FDBD1_0000SID sra extra         \n" );
    ASSERT_EQ( "", extract_value( res, "msg" ) );
}

TEST_F( TWTestFixture, NoMessage2 )
{
    std::string res = try_to_parse_good( "77619/000/0000/R  CC952F33EE2FDBD1 0008/0008 2020-06-11T23:59:58.079949 traceweb22      185.151.196.174 CC952F33EE2FDBD1_0000SID sra extra\n" );
    ASSERT_EQ( "", extract_value( res, "msg" ) );
}

TEST_F( TWTestFixture, NoEvent )
{
    try_to_parse( "77619/000/0000/R  CC952F33EE2FDBD1 0008/0008 2020-06-11T23:59:58.079949 traceweb22      185.151.196.174 CC952F33EE2FDBD1_0000SID sra \n" );
    ASSERT_FALSE( s_outputs.get_ugly().empty() );
}

TEST_F( TWTestFixture, NoService )
{
    try_to_parse( "77619/000/0000/R  CC952F33EE2FDBD1 0008/0008 2020-06-11T23:59:58.079949 traceweb22      185.151.196.174 CC952F33EE2FDBD1_0000SID \n" );
    ASSERT_FALSE( s_outputs.get_ugly().empty() );
}

TEST_F( TWTestFixture, NoSID )
{
    try_to_parse( "77619/000/0000/R  CC952F33EE2FDBD1 0008/0008 2020-06-11T23:59:58.079949 traceweb22      185.151.196.174 \n" );
    ASSERT_FALSE( s_outputs.get_ugly().empty() );
}

TEST_F( TWTestFixture, NoIP )
{
    try_to_parse( "77619/000/0000/R  CC952F33EE2FDBD1 0008/0008 2020-06-11T23:59:58.079949 traceweb22      \n" );
    ASSERT_FALSE( s_outputs.get_ugly().empty() );
}

TEST_F( TWTestFixture, NoServer )
{
    try_to_parse( "77619/000/0000/R  CC952F33EE2FDBD1 0008/0008 2020-06-11T23:59:58.079949 \n" );
    ASSERT_FALSE( s_outputs.get_ugly().empty() );
}

TEST_F( TWTestFixture, NoTime )
{
    try_to_parse( "77619/000/0000/R  CC952F33EE2FDBD1 0008/0008 \n" );
    ASSERT_FALSE( s_outputs.get_ugly().empty() );
}

TEST_F( TWTestFixture, NoID3 )
{
    try_to_parse( "77619/000/0000/R  CC952F33EE2FDBD1 \n" );
    ASSERT_FALSE( s_outputs.get_ugly().empty() );
}

TEST_F( TWTestFixture, NoID2 )
{
    try_to_parse( "77619/000/0000/R\n" );
    ASSERT_FALSE( s_outputs.get_ugly().empty() );
}
TEST_F( TWTestFixture, NoID2_after_SEP )
{
    try_to_parse( "77619/000/0000/R  \n" );
    ASSERT_FALSE( s_outputs.get_ugly().empty() );
}
TEST_F( TWTestFixture, BadID2_after_SEP )
{
    std::string res = try_to_parse_review( "77619/000/0000/R  .\n" );
    ASSERT_EQ( "invalid ID2", extract_value( res, "_error" ) );
}
TEST_F( TWTestFixture, BadID2 )
{
    std::string res = try_to_parse_review( "77619/000/0000/R.\n" );
    ASSERT_EQ( "invalid ID1", extract_value( res, "_error" ) );
}

TEST_F( TWTestFixture, NoID1 )
{
    try_to_parse( "\n" );
    ASSERT_FALSE( s_outputs.get_ugly().empty() );
}

TEST_F( TWTestFixture, BadId1 )
{
    const char * txt =
"214533/000/X/R  CC954605EF0486E1 0009/0009 2020-06-22T01:58:06.783792 traceweb22      58.250.174.76   02C52D95FC7A04E6_74E0SID run_selector Warning: CONNECT(313.4) \"ncbi_localip.c\", line 191: UNK_FUNC --- [214533] Local IP spec at line 111, '10.65/16' overlaps with already defined one: 10.65.0.0-10.65.1.255";
    std::string res = try_to_parse_review( "txt" );
    ASSERT_EQ( "invalid ID1", extract_value( res, "_error" ) );
}

 /*TODO: Add tests for error handliong of all fields, a la id2 */

TEST_F( TWTestFixture, problem1 )
{
    const char * txt =
"214533/000/0000/R  CC954605EF0486E1 0009/0009 2020-06-22T01:58:06.783792 traceweb22      58.250.174.76   02C52D95FC7A04E6_74E0SID run_selector Warning: CONNECT(313.4) \"ncbi_localip.c\", line 191: UNK_FUNC --- [214533] Local IP spec at line 111, '10.65/16' overlaps with already defined one: 10.65.0.0-10.65.1.255";
    std::string res = try_to_parse_good( txt );
    ASSERT_EQ( "Warning:", extract_value( res, "event" ) );
}

TEST_F( TWTestFixture, problem2 )
{
    const char * txt =
"222026/000/0000/R  CC95634AEF04AE01 0008/0008 2020-06-22T02:08:32.559587 traceweb22      61.144.145.31   396A25C583BEBAA1_0091SID TR_SraSub Error:  \"app.cpp\", line 187: CXApp::GetSelfUrl() --- [222026] Cannot take self url by key trace.ncbi.nlm.nih.gov/export/home/web/public/htdocs/Traces/sra_sub/index.cgi from [CGI_SELF_URL_MAP]";
    std::string res = try_to_parse_good( txt );
    ASSERT_EQ( "TR_SraSub", extract_value( res, "service" ) );
}

TEST_F( TWTestFixture, problem3 )
{
    const char * txt =
"184295/000/0000/RB CC95CFE7F3655F01 0004/0004 2020-08-14T05:14:24.564335 traceweb22      2001:981:83ac:1:f449:e4d2:b6d9:57b A9F5023FF361C403_11555SID tracequery request-start fields=1&ncbi_role=production&ncbi_location=be-md";
    std::string res = try_to_parse_good( txt );
    ASSERT_EQ( "A9F5023FF361C403_11555SID", extract_value( res, "sid" ) );
}

TEST_F( TWTestFixture, errorLine )
{
    const char * txt =
"55032/000/0000/R  CC95D6F8EF06E6B1 0010/0010 2020-06-22T04:40:13.429395 traceweb22      193.144.35.5    C7A484EAEE3230D1_0000SID sra Note[E]:  \"helper_sra.cpp\", line 56: ncbi::CSraHelper::CheckSignal() --- Signal SIGPIPE was cought";
    std::string res = try_to_parse_good( txt );
    ASSERT_EQ( "Note[E]:", extract_value( res, "event" ) );
    ASSERT_EQ( "\"helper_sra.cpp\", line 56: ncbi::CSraHelper::CheckSignal() --- Signal SIGPIPE was cought", extract_value( res, "msg" ) );
}

;
TEST_F( TWTestFixture, UTF8 )
{
    const char * txt =
u8"55032/000/0000/R  CC95D6F8EF06E6B1 0010/0010 2020-06-22T04:40:13.429395 traceweb22      193.144.35.5    C7A484EAEE3230D1_0000SID sra Note[E]: попробуем 产品公司求购 ";
    std::string res = try_to_parse_good( txt );
    ASSERT_EQ( u8"попробуем 产品公司求购 ", extract_value( res, "msg" ) );
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
