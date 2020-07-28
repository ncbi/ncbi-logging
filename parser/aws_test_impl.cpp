#include <gtest/gtest.h>

#include "Formatters.hpp"
#include "Classifiers.hpp"
#include "AWS_Interface.hpp"

using namespace std;
using namespace NCBI::Logging;

#define INIT_TSTR(t, s)     do { (t).p = s; (t).n = strlen(s); (t).escaped = false; } while (false)
#define INIT_TSTR_ESC(t, s) do { (t).p = s; (t).n = strlen(s); (t).escaped = true; } while (false)

TEST(LogAWSEventTest, Create)
{
    JsonLibFormatter f;
    LogAWSEvent e ( f );
}

TEST(LogAWSEventTest, Setters)
{
    JsonLibFormatter f;
    LogAWSEvent e ( f );
    
    t_str v;
    INIT_TSTR( v, "i_p");
    e.set( LogLinesInterface::ip, v );
    INIT_TSTR( v, "ref");
    e.set( LogLinesInterface::referer, v );
    INIT_TSTR( v, "unp");
    e.set( LogLinesInterface::unparsed, v );
    t_agent a = { 
        {"o", 1, false },
        {"v_o", 3, false },
        {"v_t", 3, false },
        {"v_r", 3, false },
        {"v_c", 3, false },
        {"v_g", 3, false },
        {"v_s", 3, false },
        {"v_l", 3, false }
    };
    e.setAgent( a );

    t_request r = {
        { "m", 1, false },
        { "p", 1, false },
        { "v", 1, false },
        { "a", 1, false },
        { "f", 1, false },
        { "e", 1, false },
        { "s", 1, false },
        acc_before
    };
    e.setRequest( r );

    INIT_TSTR( v, "own"); e.set(LogAWSEvent::owner, v);
    INIT_TSTR( v, "buc"); e.set(LogAWSEvent::bucket, v);
    INIT_TSTR( v, "tim"); e.set(LogAWSEvent::time, v);
    INIT_TSTR( v, "req"); e.set(LogAWSEvent::requester, v);
    INIT_TSTR( v, "req_id");e.set(LogAWSEvent::request_id, v);
    INIT_TSTR( v, "ope"); e.set(LogAWSEvent::operation, v);
    INIT_TSTR( v, "key"); e.set(LogAWSEvent::key, v);
    INIT_TSTR( v, "cod"); e.set(LogAWSEvent::res_code, v);
    INIT_TSTR( v, "err"); e.set(LogAWSEvent::error, v);
    INIT_TSTR( v, "res"); e.set(LogAWSEvent::res_len, v);
    INIT_TSTR( v, "obj"); e.set(LogAWSEvent::obj_size, v);
    INIT_TSTR( v, "tot"); e.set(LogAWSEvent::total_time, v);
    INIT_TSTR( v, "tur"); e.set(LogAWSEvent::turnaround_time, v);
    INIT_TSTR( v, "ver"); e.set(LogAWSEvent::version_id, v);
    INIT_TSTR( v, "host");e.set(LogAWSEvent::host_id, v);
    INIT_TSTR( v, "sig"); e.set(LogAWSEvent::sig_ver, v);
    INIT_TSTR( v, "cip"); e.set(LogAWSEvent::cipher_suite, v);
    INIT_TSTR( v, "aut"); e.set(LogAWSEvent::auth_type, v);
    INIT_TSTR( v, "hos"); e.set(LogAWSEvent::host_header, v);
    INIT_TSTR( v, "tls"); e.set(LogAWSEvent::tls_version, v);

    stringstream out;
    ASSERT_EQ ( 
        "{\"accession\":\"a\",\"agent\":\"o\",\"auth_type\":\"aut\",\"bucket\":\"buc\",\"cipher_suite\":\"cip\",\"error\":\"err\",\"extension\":\"e\",\"filename\":\"f\",\"host_header\":\"hos\",\"host_id\":\"host\",\"ip\":\"i_p\",\"key\":\"key\",\"method\":\"m\",\"obj_size\":\"obj\",\"operation\":\"ope\",\"owner\":\"own\",\"path\":\"p\",\"referer\":\"ref\",\"request_id\":\"req_id\",\"requester\":\"req\",\"res_code\":\"cod\",\"res_len\":\"res\",\"sig_ver\":\"sig\",\"time\":\"tim\",\"tls_version\":\"tls\",\"total_time\":\"tot\",\"turnaround_time\":\"tur\",\"unparsed\":\"unp\",\"vdb_libc\":\"v_l\",\"vdb_os\":\"v_o\",\"vdb_phid_compute_env\":\"v_c\",\"vdb_phid_guid\":\"v_g\",\"vdb_phid_session_ip\":\"v_s\",\"vdb_release\":\"v_r\",\"vdb_tool\":\"v_t\",\"vers\":\"v\",\"version_id\":\"ver\"}", 
        e.GetFormatter().format( out ).str() );    
}

//TODO: reportField

#if 0
TEST (TestHelper, StringEscaped)
{
    JsonFormatter f;
    t_str v;
    INIT_TSTR_ESC( v, "\\\"");
    string s = f.format( "key", v );
    cout << endl << s << endl;
    ASSERT_EQ ( "{\"key\":\"\\\"\"}", s );
}
TEST (TestHelper, StringEscaped_Multiple)
{
    t_str s = { "abc\\\"\\\"", 7, true };
    ASSERT_EQ( string("abc\"\""), ToString( s ) );
}
TEST (TestHelper, StringEscaped_TrailingSlash)
{
    t_str s = { "\\", 1, true };
    ASSERT_EQ( string("\\"), ToString( s ) );
}
TEST (TestHelper, StringEscaped_NotEscapedCharacter)
{
    t_str s = { "\\n", 2, true };
    ASSERT_EQ( string("\\n"), ToString( s ) );
}
#endif

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}

