#include "json.hpp"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <curl/curl.h>
#include <fstream>
#include <immintrin.h>
#include <iostream>
//#include <re2/re2.h>
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
    string version;
    string sessid;
    string phid;
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
        cerr << "can't find '" << c << "' in '" << line << "'" << "after " << s << "\n";
        // exit ( 0 );
    }
    return e;
}

[[nodiscard]] bool parse_url ( const string &request_uri, string &acc,
    string &version )
{
    CURLUcode rc;
    CURLU *url = curl_url ();
    acc = "";
    version = "";
    string spath;
    string squery;

    // TODO(vartanianmh): mike handle %00 nulls in CURL API
    rc = curl_url_set ( url, CURLUPART_URL, request_uri.data (), 0 );
    if ( rc == 0u ) {
        char *path = nullptr;
        rc = curl_url_get ( url, CURLUPART_PATH, &path, CURLU_URLDECODE );
        if ( rc == 0u ) {
            char *query = nullptr;
            spath = path;
            rc = curl_url_get ( url, CURLUPART_QUERY, &query, CURLU_URLDECODE );
            if ( rc == 0u ) {
                squery = query;
                curl_free ( query );
            }
            curl_free ( path );
        } else {
            cerr << "curl parse error " << rc << ":" << request_uri << "\n";
            curl_url_cleanup ( url );
            return false;
        }
    } else {
        cerr << "curl set error " << rc << ":" << request_uri << "\n";
        curl_url_cleanup ( url );
        return false;
    }
    curl_url_cleanup ( url );

    for ( const auto c : squery) {
        if ( isascii ( c ) == 0 ) {
            cerr << "Non ascii in squery:" << squery << "\n";
            return false;
        }
    }

    for ( const auto c : spath ) {
        if ( isascii ( c ) == 0 ) {
            cerr << "Non ascii in spath:" << spath << "\n";
            return false;
        }
    }

    size_t ls = spath.find_last_of ( '/' );
    if ( ls != string::npos ) {
        acc = spath.substr ( ls + 1 );
        if ( ( acc[0] == 'D' || acc[0] == 'E' || acc[0] == 'S' )
            && acc[1] == 'R' && acc[2] == 'R' ) {
            size_t d = 3;
            while ( d < acc.size () && ( isdigit ( acc[d] ) != 0 ) ) { ++d; }
            if ( d + 1 < acc.size () && acc[d] == '.'
                && ( isdigit ( acc[d + 1] ) != 0 ) ) {
                version = acc.substr ( d + 1 );
            }
            acc = acc.substr ( 0, d );
        }
    }

    return true;
}

int main ( int argc, char *argv[] )
{
    if ( argc != 1 ) {
        cerr << "Usage: " << argv[0] << " stdin only\n";
        return 1;
    }

    unordered_map<string, struct sess> sessions;
    // sessions.reserve ( 1000000 ); // Most are < 200,000, max 513,000

    size_t linecount = 0;
    const bool debug = false;
    string line;
    // longest seen in wild is 2124 bytes
    char buf[4096];
    line.reserve ( sizeof buf );


    //    RE2 re("([DES]RR[\\.0-9]{6,12})");
    //    assert(re.ok());

    //    assert(RE2::PartialMatch("SRR1234567", re, &acc));
    // cerr << "acc is " << acc << "\n";

    size_t successes = 0;
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
         * HTTP/1.1"
         * 206 131072 0.000 "-"
         * "linux64 sra-toolkit fastq-dump.2.8"
         * "-"
         * port=443
         * rl=358
         */

        /* 132.249.107.67
         * -
         * -
         * [23/Mar/2017:08:52:14 -0400]
         * sra-download.ncbi.nlm.nih.gov:443
         * "GET
         * /srapub/SRR1609033
         * HTTP/1.1"
         * 206 2986 "-"
         * "linux64 sra-toolkit fastq-dump.2.8"
         * xff="-"
         * rt=0.000
         * rl=276
         */

        /*
         * 130.14.24.58
         * -
         * -
         * [23/Mar/2017:00:00:04 -0400]
         * sra-download.ncbi.nlm.nih.gov:443
         * "HEAD /srapub/DRR034175 HTTP/1.1"
         * 200 0 "-"
         * "Python-urllib/2.7"
         * xff="-"
         * rt=0.000
         * rl=133
         */

        /*
          $remote_addr - $remote_user [$time_local] "$server_name" "$request" $status $body_bytes_sent $request_time "$http_referer" "$http_user_agent" "$http_x_forwarded_for" port=$server_port rl=$request_length
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
        s = e + 2;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue; }
        string host_header = line.substr ( s, e - s  );
        host_header.erase(std::remove(host_header.begin(), host_header.end(), '"'), host_header.end());
        if constexpr ( debug ) {
            cout << "host_header is:" << host_header << ".\n";
        }
        s = e + 1;

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

        e = linefind ( line, '"', s + 1);
        string user_agent;
        if ( e == string::npos ) { continue; }
        user_agent = line.substr ( s + 1, e - s - 1 );
        if constexpr ( debug ) {
            cout << "user_agent is:" << user_agent << ".\n";
        }
/*
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

*/
        struct tm tm {
        };
        if ( strptime ( time.data (), "%d/%b/%Y:%H:%M:%S %z", &tm )
            == nullptr ) {
            cerr << "Couldn't parse time" << time << "\n";
            continue;
        }
        time_t ltm = timegm ( &tm );
        if constexpr ( debug ) { cout << "time is now: " << ltm << "\n"; }

        if ( total_time == "-" || total_time == "\"-\"" ) { total_time = "0"; }


        s = request_uri.find ( ' ' );
        string cmd = request_uri.substr ( 0, s );
        if ( s != string::npos ) { request_uri = request_uri.substr ( s + 1 ); }

        size_t sp = request_uri.find ( ' ' ); // Remove optional _HTTP/1...
        if ( sp != string::npos ) { request_uri.resize ( sp ); }

        if constexpr ( debug ) { cerr << "request was '" + request_uri + "'\n"; }

        string version, sessid, phid, acc;

        string url;
        if (request_uri.rfind("http",0)==0)
        {
            url=request_uri;
        } else
        {
            url="https://";
            url+= host_header ;
            url+= request_uri;
        }

        if ( !parse_url ( url, acc, version ) ) { continue; }

        uint64_t bytecount = 0;
        try {
            if ( bytes_sent != "-" ) { bytecount = stoull ( bytes_sent ); }
        } catch ( const invalid_argument &a ) {
            cerr << "bytes_sent not a number: " << bytes_sent << "\n";
            bytecount = 0;
        } catch ( const out_of_range &a ) {
            cerr << "out_of_range : " << bytes_sent << "\n";
            bytecount = 0;
        }

        // TODO(vartanianmh): Check largest accession
        if (bytecount > 1000000000000)
        {
            cerr << "bytecount too large: " << bytecount << ":" << bytes_sent << "\n";
            continue;
        }

        // VDB-3961
        // TODO(vartanianmh): Fix parsing
        size_t phloc=user_agent.find("(phid=");
        if ( phloc != string::npos ) {
            phid=user_agent.substr(phloc+6,10);
        }


        struct sess sess;
        sess.ip = remote_ip;
        sess.agent = user_agent.substr ( 0, 70 );
        sess.domain = host_header;
        sess.acc = acc;
        sess.status.emplace ( http_status );
        sess.cmds.emplace ( cmd );
        sess.bytecount = bytecount;
        sess.version = version;
        sess.sessid = sessid;
        sess.phid = phid;
        sess.cnt = 1;
        sess.start = static_cast<double> ( ltm );
        try {
            sess.end = sess.start + stod ( total_time );
        } catch ( const invalid_argument &a ) {
            //cerr << "total_time not a number: " << total_time << "\n";
            sess.end = sess.start;
        } catch ( const out_of_range &a ) {
            cerr << "out_of_range: " << total_time << "\n";
            sess.end = sess.start;
        }

        if constexpr ( debug ) { cout << "end is:" << sess.end << "\n"; }

        string sesskey = remote_ip;
        sesskey += acc;
        sesskey += user_agent;
        // Combine requests that were load-balanced to different servers
        //sesskey += host_header;
        //sesskey += version;
        //sesskey += sessid;
        //sesskey += phid;

        // VDB-3889
        if ( cmd == "PUT" || cmd == "POST" ) {
            sesskey += "PUTPOST";
        } else if ( cmd == "DELETE" || cmd == "OPTIONS" ) {
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
        string cmds;
        string statuses;
        try {
            json j;
            j["ip"] = sess.ip;
            j["agent"] = sess.agent;
            j["domain"] = sess.domain;
            j["acc"] = sess.acc;
            j["bytecount"] = sess.bytecount;
            j["cnt"] = sess.cnt;
            j["start"] = sess.start;
            j["end"] = sess.end;
            j["version"] = sess.version;
            j["sessid"] = sess.sessid;
            j["phid"] = sess.phid;

            for ( const auto &stat : sess.status ) { statuses += stat + " "; }
            statuses.erase ( statuses.find_last_not_of ( ' ' ) + 1 );
            j["status"] = statuses;

            for ( const auto &cmd : sess.cmds ) { cmds += cmd + " "; }
            cmds.erase ( cmds.find_last_not_of ( ' ' ) + 1 );
            j["cmds"] = cmds;

            cout << j << "\n";
        } catch ( json::type_error &e ) {
            cerr << "type error:" << sess.ip << "\n";
            cerr << sess.ip << "\n";
            cerr << sess.agent << "\n";
            cerr << sess.domain << "\n";
            cerr << sess.acc << "\n";
            cerr << sess.bytecount << "\n";
            cerr << sess.cnt << "\n";
            cerr << sess.start << "\n";
            cerr << sess.end << "\n";
            cerr << sess.version << "\n";
            cerr << sess.sessid << "\n";
            cerr << sess.phid << "\n";
            cerr << statuses << "\n";
            cerr << cmds << "\n";
        }
    }
}
