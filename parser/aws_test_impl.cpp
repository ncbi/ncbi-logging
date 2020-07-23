#include <gtest/gtest.h>

#include "Formatters.hpp"
#include "Classifiers.hpp"
#include "AWS_Interface.hpp"

using namespace std;
using namespace NCBI::Logging;

#define INIT_TSTR(t, s)     do { (t).p = s; (t).n = strlen(s); (t).escaped = false; } while (false)
#define INIT_TSTR_ESC(t, s) do { (t).p = s; (t).n = strlen(s); (t).escaped = true; } while (false)

class TestLogLinesInterface : public LogLinesInterface
{
public:
    TestLogLinesInterface( uint16_t p_memCount ) : memCount ( p_memCount )
    {
    }

    virtual void beginLine() {}
    virtual void endLine() {}
    virtual Category GetCategory() const { return cat_ugly; }
    virtual void failedToParse( const t_str & source ) {}
    virtual void set( Members m, const t_str & v ) {}
    virtual void set( Members m, int64_t v ) {}
    virtual void setAgent( const t_agent & a ) {}
    virtual void setRequest( const t_request & r ) {}
    virtual void setTime( const t_timepoint & t ) {}
    virtual bool nextMember( Member & m ) 
    {
        ++curMember;
        if ( curMember > memCount )
        {
            return false;
        }
        switch (curMember)
        {
        case 1:
            INIT_TSTR( m . name, "a");
            m . type = Member::t_string;
            EMPTY_TSTR( m.u.s );
            m . u . s . p = "str";
            m . u . s . n = 3;
            return true;
        case 2:
            INIT_TSTR( m . name, "b");
            m . type = Member::t_int;
            m . u . i = 123;
            return true;
        default:
            return false;
        }
    }
    uint16_t curMember = 0;
    uint16_t memCount;

    virtual ReportFieldResult reportField( Members field, const char * value, const char * message, ReportFieldType type ) { return abort; }
};

TEST(JsonFormatterTest, Iterate_NoEscape)
{
    TestLogLinesInterface line(2);
    JsonFormatter f;
    ASSERT_EQ ( "{\"a\":\"str\",\"b\":123}", f.format( line ) );
}

TEST(JsonFormatterTest, Iterate_NoMembers)
{
    TestLogLinesInterface line(0);
    JsonFormatter f;
    ASSERT_EQ ( "{}", f.format( line ) );
}
TEST(JsonFormatterTest, SingleString)
{
    JsonFormatter f;
    t_str v;
    INIT_TSTR( v, "abc");
    ASSERT_EQ ( "{\"key\":\"abc\"}", f.format( "key", v ) );
}

TEST(JsonFormatterTest, Escaping)
{
    JsonFormatter f;
    t_str v;
    INIT_TSTR( v, "abc");
    ASSERT_EQ ( "{\"key\":\"abc\"}", f.format( "key", v ) );
}


TEST(LogAWSEventTest, Create)
{
    LogAWSEvent e;
}

TEST(LogAWSEventTest, Setters)
{
    LogAWSEvent e;
    
    t_str v;
    INIT_TSTR( v, "ip");
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

    INIT_TSTR( v, "own");
    e.set(AWS_LogLinesInterface::owner, v);
    INIT_TSTR( v, "buc");
    e.set(AWS_LogLinesInterface::bucket, v);
    t_timepoint t = {1, 2, 3, 4, 5, 6, 7};
    e.setTime(t);
    INIT_TSTR( v, "req");e.set(AWS_LogLinesInterface::requester, v);
    INIT_TSTR( v, "req_id");e.set(AWS_LogLinesInterface::request_id, v);
    INIT_TSTR( v, "ope");e.set(AWS_LogLinesInterface::operation, v);
    INIT_TSTR( v, "key");e.set(AWS_LogLinesInterface::key, v);
    INIT_TSTR( v, "cod");e.set(AWS_LogLinesInterface::res_code, v);
    INIT_TSTR( v, "err");e.set(AWS_LogLinesInterface::error, v);
    INIT_TSTR( v, "res");e.set(AWS_LogLinesInterface::res_len, v);
    INIT_TSTR( v, "obj");e.set(AWS_LogLinesInterface::obj_size, v);
    INIT_TSTR( v, "tot");e.set(AWS_LogLinesInterface::total_time, v);
    INIT_TSTR( v, "tur");e.set(AWS_LogLinesInterface::turnaround_time, v);
    INIT_TSTR( v, "ver");e.set(AWS_LogLinesInterface::version_id, v);
    INIT_TSTR( v, "host");e.set(AWS_LogLinesInterface::host_id, v);
    INIT_TSTR( v, "sig");e.set(AWS_LogLinesInterface::sig_ver, v);
    INIT_TSTR( v, "cip");e.set(AWS_LogLinesInterface::cipher_suite, v);
    INIT_TSTR( v, "aut");e.set(AWS_LogLinesInterface::auth_type, v);
    INIT_TSTR( v, "hos");e.set(AWS_LogLinesInterface::host_header, v);
    INIT_TSTR( v, "tls");e.set(AWS_LogLinesInterface::tls_version, v);

    JsonFormatter f;
    ASSERT_EQ ( 
        "{blah}", 
        f.format( e ) );    
}

//TODO: add tests for t_timepoint LogLinesInterface::Member::u.t

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


// Classifiers

TEST(FileClassifier, Write)
{
    {
        FileClassifier f( "test" );
        f.write( LogLinesInterface::cat_review, "review");
        f.write( LogLinesInterface::cat_good, "good");
        f.write( LogLinesInterface::cat_bad, "bad");
        f.write( LogLinesInterface::cat_ugly, "ugly");
    }

    {
        ifstream in("test.review"); ASSERT_TRUE( in.good() );
        string s; in >> s;
        ASSERT_EQ( string("review"), s);
    }
    remove("test.review");

    {
        ifstream in("test.good"); ASSERT_TRUE( in.good() );
        string s; in >> s;
        ASSERT_EQ( string("good"), s);
    }
    remove("test.good");

    {
        ifstream in("test.bad"); ASSERT_TRUE( in.good() );
        string s; in >> s;
        ASSERT_EQ( string("bad"), s);
    }
    remove("test.bad");

    {
        ifstream in("test.unrecog"); ASSERT_TRUE( in.good() );
        string s; in >> s;
        ASSERT_EQ( string("ugly"), s);
    }
    remove("test.unrecog");
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}

