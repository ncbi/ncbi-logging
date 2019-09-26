#include "json.hpp"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <immintrin.h>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>

using std::cerr;
using std::cout;
using std::invalid_argument;
using std::max;
using std::min;
using std::out_of_range;
using std::set;
using std::string;
using std::unordered_map;
using json = nlohmann::json;


struct sess {
    string ip;
    string agent;
    string domain;
    string acc;
    set<string> cmds;
    set<string> status;
    uint64_t bytecount = 0;
    size_t cnt = 0;
    double start = 0.0;
    double end = 0.0;
};

// gcc >=6 has CPUID function dispatch, so should invoke
// AVX2 (Haswell, 256-bit) version of memchr() if supported,
// otherwise use almost as fast SSE (128-bit) vesion
#if 0
const char *sse_memchr_r ( const char *cur, const char *end )
{
    __m128i pattern = _mm_set1_epi8 ( 'R' );
    __m128i ld, cmp;

    while ( cur < end ) {
        ld = _mm_lddqu_si128 ( reinterpret_cast<const __m128i *> ( cur ) );
        cmp = _mm_cmpeq_epi8 ( ld, pattern );
        int z = _mm_movemask_epi8 ( cmp );
        if ( z != 0 ) { // found
            const char *r = cur + __builtin_ffs ( z ) - 1;
            return r < end ? r : end;
        }
        cur += 16;
    }

    return end;
}
#endif

// Key is ip, url, agent, domain

[[nodiscard]] size_t linefind ( const string &line, char c, size_t s )
{
    size_t e = line.find ( c, s );
    if ( e == string::npos ) {
        cerr << "can't find '" << c << "' in '" << line << "'\n";
        // exit ( 0 );
    }
    return e;
}

[[nodiscard]] string findacc ( const string &str )
{
    size_t sz = str.size ();
    if ( sz < 8 ) { return ""; }
    const char *s = str.data ();

    const char *r = static_cast<const char *> (
        memchr ( static_cast<const char *> ( s + 1 ), 'R', sz - 6 ) );
    if ( r != nullptr ) {
        size_t start = r - s - 1;
        if ( str[start + 2] == 'R' ) {
            if ( str[start] == 'S' || str[start] == 'D' || str[start] == 'E' ) {
                size_t d = 3;
                while ( isdigit ( str[start + d] ) != 0 ) { ++d; }
                if ( str[start + d] == '.'
                    && ( isdigit ( str[start + d + 1] ) != 0 ) ) {
                    d += 2;
                }
                if ( d > 6 ) {
                    return str.substr ( start, d );
                } 
                    return "";
                
            }
        }
    }
    return "";
}

int main ( int argc, char *argv[] )
{
    if ( argc != 1 ) {
        cerr << "Usage: " << argv[0] << " stdin only\n";
        return 1;
    }

    unordered_map<string, struct sess> sessions;
    sessions.reserve ( 1000000 ); // Most are < 200,000, max 513,000

    size_t linecount = 0;
    const bool debug = false;
    string line;
    // longest seen in wild is 2124 bytes
    char buf[4096];
    line.reserve ( sizeof buf );
    size_t successes = 0;
    string sesskey;
    while ( fgets ( buf, sizeof ( buf ), stdin ) != nullptr ) {
        line.assign ( buf );

        bool utf8 = true;
        for ( const auto c : line ) {
            if ( isascii ( c ) == 0 ) {
                cerr << "Non ascii in line\n";
                utf8 = false;
            }
        }
        if ( !utf8 ) { continue; }


        ++linecount;
        if ( line.size () < 10 ) {
            cerr << "short line: " << line << "\n";
            continue;
        }

        size_t s = 0, e = 0;

        if constexpr ( debug ) { cout << "line is:" << line << "\n"; }
        /*
         * 132.204.81.18
         * -
         * -
         * [01/Mar/2019:23:59:51 -0500]
         * "gap-download.be-md.ncbi.nlm.nih.gov"
         * "GET
         * /sragap/A032F554-501A-428B-B7C0-0427AE242198/SRR1406330?tic=75D2D46C-AACE-457D-B02C-4AD6D4A17447
         * HTTP/1.1" 206 131072 0.000
         * "-"
         * "linux64 sra-toolkit fastq-dump.2.8"
         * "-"
         * port=443
         * rl=358
         */

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue; }
        string remote_ip = line.substr ( s, e - s );
        if constexpr ( debug ) {
            cout << "remote_ip is:" << remote_ip << ".\n";
        }
        s = e + 1;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue; }
        string dash = line.substr ( s, e - s );
        if constexpr ( debug ) { cout << "dash is:" << dash << ".\n"; }
        s = e + 1;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue; }
        string user = line.substr ( s, e - s );
        if constexpr ( debug ) { cout << "user is:" << user << ".\n"; }
        s = e + 1;

        e = linefind ( line, ']', s );
        if ( e == string::npos ) { continue; }
        string time = line.substr ( s + 1, e - s - 1 );
        if constexpr ( debug ) { cout << "time is:" << time << ".\n"; }
        s = e + 3;

        e = linefind ( line, '"', s );
        if ( e == string::npos ) { continue; }
        string host_header = line.substr ( s, e - s );
        if constexpr ( debug ) {
            cout << "host_header is:" << host_header << ".\n";
        }
        s = e + 2;

        e = linefind ( line, '"', s + 1 );
        if ( e == string::npos ) { continue; }
        string request_uri = line.substr ( s + 1, e - s - 1 );
        if constexpr ( debug ) {
            cout << "request_uri is:" << request_uri << ".\n";
        }
        s = e + 2;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue; }
        string http_status = line.substr ( s, e - s );
        if constexpr ( debug ) {
            cout << "http_status is:" << http_status << ".\n";
        }
        s = e + 1;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue; }
        string bytes_sent = line.substr ( s, e - s );
        if constexpr ( debug ) {
            cout << "bytes_sent is:" << bytes_sent << ".\n";
        }
        s = e + 1;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue; }
        string total_time = line.substr ( s, e - s );
        if constexpr ( debug ) {
            cout << "total_time is:" << total_time << ".\n";
        }
        s = e + 1;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue; }
        string turn_around_time = line.substr ( s, e - s );
        if ( debug ) {
            cout << "turn_around_time is:" << turn_around_time << ".\n";
        }
        s = e + 1;

        e = linefind ( line, '"', s + 1 );
        if ( e == string::npos ) { continue; }
        string user_agent = line.substr ( s + 1, e - s - 1 );
        if constexpr ( debug ) {
            cout << "user_agent is:" << user_agent << ".\n";
        }
        s = e + 2;

        e = linefind ( line, '"', s + 1 );
        if ( e == string::npos ) { continue; }
        string referrer = line.substr ( s + 1, e - s - 1 );
        if constexpr ( debug ) { cout << "referrer is:" << referrer << ".\n"; }
        s = e + 2;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue; }
        string port = line.substr ( s, e - s );
        if constexpr ( debug ) { cout << "port is:" << port << ".\n"; }
        s = e + 1;

        string rl = line.substr ( s, string::npos );
        if constexpr ( debug ) { cout << "rl is:" << rl << ".\n"; }
        // s = e + 1;


        struct tm tm {
        };
        if ( strptime ( time.data (), "%d/%b/%Y:%H:%M:%S %z", &tm )
            == nullptr ) {
            cerr << "Couldn't parse time" << time << "\n";
            continue;
        }
        time_t ltm = timegm ( &tm );
        if constexpr ( debug ) { cout << "time is: " << ltm << "\n"; }

        if ( total_time == "-" ) { total_time = "0"; }


        s = request_uri.find ( ' ' );
        string cmd = request_uri.substr ( 0, s );
        if ( s != string::npos ) { request_uri = request_uri.substr ( s + 1 ); }

        if constexpr ( debug ) { cerr << "request was\n " + request_uri; }

#if 0
        size_t sz = request_uri.size ();
        if ( sz > 8 ) {
            const char *uri = request_uri.data ();

            const char *p = static_cast<char *> (
                memchr ( const_cast<char *> ( uri + 1 ), 'R', sz - 6 ) );
            if ( p != nullptr ) {
                --p;
                std::string m ( p, 15 );
                //                cerr << "fast matching:" << m << " : ";
                if ( p[2] == 'R' ) {
                    if ( p[0] == 'S' || p[0] == 'D' || p[0] == 'E' ) {
                        //                        cerr << " likely ";
                        int d = 3;
                        while ( isdigit ( p[d] ) != 0 ) { ++d; }
                        if ( p[d] == '.' && ( isdigit ( p[d + 1] ) != 0 ) ) {
                            d += 2;
                        }
                        //                        cerr << "found " << d;
                    }
                }
                //                cerr << "\n";
            }
        }
#endif

        string acc = findacc ( request_uri );
        if ( !acc.empty () ) { request_uri = acc; }

        uint64_t bytecount = 0;
        try {
            if ( bytes_sent != "-" ) { bytecount = stol ( bytes_sent ); }
        } catch ( const invalid_argument &a ) {
            cerr << "Not a number: " << bytes_sent << "\n";
            bytecount = 0;
        } catch ( const out_of_range &a ) {
            cerr << "out_of_range : " << bytes_sent << "\n";
            bytecount = 0;
        }


        struct sess sess;
        sess.ip = remote_ip;
        sess.agent = user_agent.substr ( 0, 64 );
        sess.domain = host_header;
        sess.acc = request_uri;
        sess.status.emplace ( http_status );
        sess.cmds.emplace ( cmd );
        sess.bytecount = bytecount;
        sess.cnt = 1;
        sess.start = static_cast<double> ( ltm );
        try {
            sess.end = sess.start + stod ( total_time );
        } catch ( const invalid_argument &a ) {
            cerr << "Not a number: " << total_time << "\n";
            sess.end = sess.start;
        } catch ( const out_of_range &a ) {
            cerr << "out_of_range: " << total_time << "\n";
            sess.end = sess.start;
        }

        if constexpr ( debug ) { cout << "end is:" << sess.end << "\n"; }

        sesskey = remote_ip;
        sesskey += request_uri;
        sesskey += user_agent;
        sesskey += host_header;

        // VDB-3889
        if (cmd=="PUT" || cmd=="POST") {
            sesskey += "PUTPOST";
        } else if (cmd=="DELETE" || cmd=="OPTIONS") {
            sesskey += "DELOPT";
        }

        if ( sessions.count ( sesskey ) == 0 ) {
            sessions.emplace ( sesskey, sess );
        } else {
            struct sess sess2 = sessions[sesskey];
            sess2.start = min ( sess.start, sess2.start );
            sess2.end = max ( sess2.end, sess2.end );
            sess2.bytecount += sess.bytecount;
            sess2.cnt += sess.cnt;

            sess2.cmds.insert ( sess.cmds.begin (), sess.cmds.end () );
            sess2.status.insert ( sess.status.begin (), sess.status.end () );

            sessions.erase ( sesskey );
            sessions.emplace ( sesskey, sess2 );
        }
        ++successes;
    }

    size_t errorcount = linecount - successes;
    cerr << "Read in " << linecount << " lines\n";
    cerr << "Had     " << errorcount << " errors\n";
    cerr << "Have    " << sessions.size () << " sessions\n";

    for ( const auto &s : sessions ) {
        string key = s.first;
        struct sess sess = s.second;
        json j;
        j["ip"] = sess.ip;
        j["agent"] = sess.agent;
        j["domain"] = sess.domain;
        j["acc"] = sess.acc;
        j["bytecount"] = sess.bytecount;
        j["cnt"] = sess.cnt;
        j["start"] = sess.start;
        j["end"] = sess.end;

        string statuses;
        for ( const auto &stat : sess.status ) { statuses += stat + " "; }
        statuses.erase ( statuses.find_last_not_of ( ' ' ) + 1 );
        j["status"] = statuses;

        string cmds;
        for ( const auto &cmd : sess.cmds ) { cmds += cmd + " "; }
        cmds.erase ( cmds.find_last_not_of ( ' ' ) + 1 );
        j["cmds"] = cmds;

        cout << j << "\n";
    }
}
