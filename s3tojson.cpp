#include "json.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <time.h>
#include <unordered_map>

// g++ --std=c++14 -Wall -O3 -Wextra -o logtojson logtojson.cpp

using namespace std;
using json = nlohmann::json;


struct sess {
    string ip;
    string agent;
    string domain;
    //    string url;
    string acc;
    set<string> cmds;
    set<string> status;
    size_t bytecount;
    size_t cnt;
    double start;
    double end;
};

// Key is ip, url, agent, domain

[[nodiscard]] size_t linefind ( const string &line, char c, size_t s )
{
    size_t e = line.find ( c, s );
    if ( e == string::npos ) {
        cerr << "can't find '" << c << "' in '" << line << "'\n";
    }
    return e;
}


int main ( int argc, char *argv[] )
{
    if ( argc != 1 ) {
        cerr << "Usage: " << argv[0] << " stdin only\n";
        return 1;
    }

    unordered_map<string, struct sess> sessions;
    sessions.reserve ( 20000000 );

    // https://docs.aws.amazon.com/AmazonS3/latest/dev/LogFormat.html
    /*
     * 922194806485875312b252374a3644f1feecd16802a50d4729885c1d11e1fd37
     * sra-pub-run-6
     * [10/Jul/2019:22:38:15 +0000]
     * 34.239.179.151
     * arn:aws:sts::783971887864:assumed-role/sra-developer-instance-profile-role/i-06cc8fd81834818e4
     * 2930B19B0B66BDC1
     * REST.GET.BUCKET
     * -
     * "GET
     * /?list-type=2&delimiter=%2F&prefix=SRR9657204%2FSRR9657204.1&encoding-type=url
     * HTTP/1.1" 200
     * -
     * 323
     * -
     * 25
     * 24
     * "-"
     * "aws-cli/1.16.102 Python/2.7.16 Linux/4.14.109-80.92.amzn1.x86_64
     * botocore/1.12.92"
     * -
     * 6Ja8F2JEXsJlnML7JfCHl06BzrlwbSzuKCoU2SYYeQu45f4YjOad8mIarilEeyICv8S/LzfH8fg=
     * SigV4
     * ECDHE-RSA-AES128-GCM-SHA256
     * AuthHeader
     * sra-pub-run-6.s3.amazonaws.com
     * TLSv1.2
     */

    regex accre (
        ".*([DES]RR[\\d\\.]{6,10})", std::regex::icase | std::regex::optimize );

    smatch matches;
    std::string test ( "fooSRR9668056" );
    regex_search ( test, matches, accre );
    if ( matches.empty () ) return 1;
    size_t linecount = 0;
    const bool debug = false;
    string line;
    // longest seen in wild is 2124 bytes
    char buf[4096];
    line.reserve ( sizeof buf );
    size_t successes = 0;
    while ( fgets ( buf, sizeof ( buf ), stdin ) ) {
        line.assign ( buf );

        //    while ( getline ( cin, line ) ) {
        ++linecount;
        if ( line.size () < 10 ) {
            cerr << "short line: " << line << "\n";
            continue;
        }

        size_t s = 0, e = 0;
        e = linefind ( line, ' ', s );
        if ( e == string::npos ) {
            cerr << " no ip: " << line << "\n";
            continue;
        }

        if constexpr ( debug ) cout << "line is:" << line << "\n";


        string bucket_owner = line.substr ( s, e - s );
        if constexpr ( debug )
            cout << "bucket_owner is:" << bucket_owner << ".\n";
        s = e + 1;

        e = linefind ( line, ' ', s );
        string bucket = line.substr ( s, e - s );
        if constexpr ( debug ) cout << "bucket is:" << bucket << ".\n";
        s = e + 1;

        e = linefind ( line, ']', s );
        string time = line.substr ( s + 1, e - s );
        if constexpr ( debug ) cout << "time is:" << time << ".\n";
        s = e + 2;

        e = linefind ( line, ' ', s );
        string remote_ip = line.substr ( s, e - s );
        if constexpr ( debug ) cout << "remote_ip is:" << remote_ip << ".\n";
        s = e + 1;

        e = linefind ( line, ' ', s );
        string requester = line.substr ( s, e - s );
        if constexpr ( debug ) cout << "requester is:" << requester << ".\n";
        s = e + 1;

        e = linefind ( line, ' ', s );
        string request_id = line.substr ( s, e - s );
        if constexpr ( debug ) cout << "request_id is:" << request_id << ".\n";
        s = e + 1;

        e = linefind ( line, ' ', s );
        string operation = line.substr ( s, e - s );
        if constexpr ( debug ) cout << "operation is:" << operation << ".\n";
        s = e + 1;

        e = linefind ( line, ' ', s );
        string key = line.substr ( s, e - s );
        if constexpr ( debug ) cout << "key is:" << key << ".\n";
        s = e + 1;

        e = linefind ( line, '"', s + 1 );
        string request_uri = line.substr ( s + 1, e - s - 1 );
        if constexpr ( debug )
            cout << "request_uri is:" << request_uri << ".\n";
        s = e + 2;

        e = linefind ( line, ' ', s );
        string http_status = line.substr ( s, e - s );
        if constexpr ( debug )
            cout << "http_status is:" << http_status << ".\n";
        s = e + 1;

        e = linefind ( line, ' ', s );
        string error_code = line.substr ( s, e - s );
        if constexpr ( debug ) cout << "error_code is:" << error_code << ".\n";
        s = e + 1;

        e = linefind ( line, ' ', s );
        string bytes_sent = line.substr ( s, e - s );
        if constexpr ( debug ) cout << "bytes_sent is:" << bytes_sent << ".\n";
        s = e + 1;

        e = linefind ( line, ' ', s );
        string object_size = line.substr ( s, e - s );
        if constexpr ( debug )
            cout << "object_size is:" << object_size << ".\n";
        s = e + 1;

        e = linefind ( line, ' ', s );
        string total_time = line.substr ( s, e - s );
        if constexpr ( debug ) cout << "total_time is:" << total_time << ".\n";
        s = e + 1;

        e = linefind ( line, ' ', s );
        string turn_around_time = line.substr ( s, e - s );
        if ( debug )
            cout << "turn_around_time is:" << turn_around_time << ".\n";
        s = e + 1;

        e = linefind ( line, '"', s + 1 );
        string referrer = line.substr ( s + 1, e - s - 1 );
        if constexpr ( debug ) cout << "referrer is:" << referrer << ".\n";
        s = e + 2;

        e = linefind ( line, '"', s + 1 );
        string user_agent = line.substr ( s + 1, e - s - 1 );
        if constexpr ( debug ) cout << "user_agent is:" << user_agent << ".\n";
        s = e + 2;

        e = linefind ( line, ' ', s );
        string version_id = line.substr ( s, e - s );
        if constexpr ( debug ) cout << "version_id is:" << version_id << ".\n";
        s = e + 1;

        e = linefind ( line, ' ', s );
        string host_id = line.substr ( s, e - s );
        if constexpr ( debug ) cout << "host_id is:" << host_id << ".\n";
        s = e + 1;

        e = linefind ( line, ' ', s );
        string signature_version = line.substr ( s, e - s );
        if ( debug )
            cout << "signature_version is:" << signature_version << ".\n";
        s = e + 1;

        e = linefind ( line, ' ', s );
        string cipher_suite = line.substr ( s, e - s );
        if constexpr ( debug )
            cout << "cipher_suite is:" << cipher_suite << ".\n";
        s = e + 1;

        e = linefind ( line, ' ', s );
        string authentication_type = line.substr ( s, e - s );
        if ( debug )
            cout << "authenticaiton_type is:" << authentication_type << ".\n";
        s = e + 1;

        e = linefind ( line, ' ', s );
        string host_header = line.substr ( s, e - s );
        if constexpr ( debug )
            cout << "host_header is:" << host_header << ".\n";
        s = e + 1;

        string tls_version = line.substr ( s );
        if constexpr ( debug )
            cout << "tls_version is:" << tls_version << ".\n";
        s = e + 1;

        struct tm tm;
        memset ( (void *)&tm, 0, sizeof tm );
        if ( !strptime ( time.data (), "%d/%b/%Y:%H:%M:%S %z", &tm ) ) {
            cerr << "Couldn't parse time" << time << "\n";
            continue;
        }
        time_t ltm = timegm ( &tm );
        if constexpr ( debug ) cout << "time is: " << ltm << "\n";

        if ( total_time == "-" ) total_time = "0";


        s = request_uri.find ( " " );
        string cmd = request_uri.substr ( 0, s );
        request_uri = request_uri.substr ( s + 1 );

        if constexpr ( debug ) cerr << "request was\n " + request_uri;
        smatch sm;
        if ( regex_search ( request_uri, sm, accre ) ) {
            if constexpr ( debug ) {
                cerr << "hits:" << sm.size () << "\n";
                cerr << "0: " << sm[0] << "\n";
                cerr << "1: " << sm[1] << "\n";
            }
            request_uri = sm[sm.size () - 1];
        }
        if ( debug )
            cerr << "\n now is " + request_uri + "\n" << sm.size () << "\n";
        long bytecount = 0;
        try {
            if ( bytes_sent != "-" ) bytecount = stol ( bytes_sent );
        } catch ( const invalid_argument &a ) {
            cerr << "Not a number: " << bytes_sent << "\n";
            bytecount = 0;
        }

        // VDB-3822
        // fastq-dump.2.10-head
        if ( user_agent.find ( "-head" ) != string::npos ) { cmd = "HEAD"; }

        struct sess sess;
        sess.ip = remote_ip;
        sess.agent = user_agent.substr ( 0, 64 );
        sess.domain = host_header;
        // sess.url = request_uri;
        sess.acc = request_uri;
        sess.status.emplace ( http_status );
        sess.cmds.emplace ( cmd );
        sess.bytecount = bytecount;
        sess.cnt = 1;
        sess.start = (double)ltm;
        sess.end = sess.start + stol ( total_time ) / 1000.0;

        if constexpr ( debug ) cout << "end is:" << sess.end << "\n";

        string sesskey = remote_ip + request_uri + user_agent + bucket;
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
        for ( const auto &stat : sess.status ) statuses += stat + " ";
        statuses.erase ( statuses.find_last_not_of ( " " ) + 1 );
        j["status"] = statuses;

        string cmds;
        for ( const auto &cmd : sess.cmds ) cmds += cmd + " ";
        cmds.erase ( cmds.find_last_not_of ( " " ) + 1 );
        j["cmds"] = cmds;

        cout << j << "\n";
    }
}
