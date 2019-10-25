#include "json.hpp"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <curl/curl.h>
#include <fstream>
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

// Key is ip, url, agent, domain

[[nodiscard]] size_t linefind ( const string &line, char c, size_t s )
{
    size_t e = line.find ( c, s );
    if ( e == string::npos ) {
        cerr << "can't find '" << c << "' in '" << line << "'\n";
        //exit ( 0 );
    }
    return e;
}


[[nodiscard]] bool parse_url ( const string &request_uri, string &acc,
    string &version, string &sessid, string &phid )
{
    CURLUcode rc;
    CURLU *url = curl_url ();
    acc = "";
    version = "";
    sessid = "";
    phid = "";
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
        return __LINE__;
    }

    unordered_map<string, struct sess> sessions;
    //    sessions.reserve ( 20000000 );

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

    size_t linecount = 0;
    const bool debug = false;
    string line;
    // longest seen in wild is 2124 bytes
    char buf[4096];
    line.reserve ( sizeof buf );

    /*
    RE2 re("([DES]RR[\\.0-9]{6,12})");
    assert(re.ok());

    string acc;
    assert(RE2::PartialMatch("SRR1234567", re, &acc));
    */

    size_t successes = 0;
    string sesskey;
    while ( nullptr != fgets ( buf, sizeof ( buf ), stdin ) ) {
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
        e = linefind ( line, ' ', s );
        if ( e == string::npos ) {
            cerr << " no ip: " << line << "\n";
            continue;
        }

        if constexpr ( debug ) { cout << "line is:" << line << "\n"; }


        string bucket_owner = line.substr ( s, e - s );
        if constexpr ( debug ) {
            cout << "bucket_owner is:" << bucket_owner << ".\n";
        }
        s = e + 1;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue;
}
        string bucket = line.substr ( s, e - s );
        if constexpr ( debug ) { cout << "bucket is:" << bucket << ".\n"; }
        s = e + 1;

        e = linefind ( line, ']', s );
        if ( e == string::npos ) { continue;
}
        string time = line.substr ( s + 1, e - s );
        if constexpr ( debug ) { cout << "time is:" << time << ".\n"; }
        s = e + 2;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue;
}
        string remote_ip = line.substr ( s, e - s );
        if constexpr ( debug ) {
            cout << "remote_ip is:" << remote_ip << ".\n";
        }
        s = e + 1;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue;
}
        string requester = line.substr ( s, e - s );
        if constexpr ( debug ) {
            cout << "requester is:" << requester << ".\n";
        }
        s = e + 1;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue;
}
        string request_id = line.substr ( s, e - s );
        if constexpr ( debug ) {
            cout << "request_id is:" << request_id << ".\n";
        }
        s = e + 1;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue;
}
        string operation = line.substr ( s, e - s );
        if constexpr ( debug ) {
            cout << "operation is:" << operation << ".\n";
        }
        s = e + 1;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue;
}
        string key = line.substr ( s, e - s );
        if constexpr ( debug ) { cout << "key is:" << key << ".\n"; }
        s = e + 1;

        e = linefind ( line, '"', s + 1 );
        if ( e == string::npos ) { continue;
}
        string request_uri = line.substr ( s + 1, e - s - 1 );
        if constexpr ( debug ) {
            cout << "request_uri is:" << request_uri << ".\n";
        }
        s = e + 2;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue;
}
        string http_status = line.substr ( s, e - s );
        if constexpr ( debug ) {
            cout << "http_status is:" << http_status << ".\n";
        }
        s = e + 1;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue;
}
        string error_code = line.substr ( s, e - s );
        if constexpr ( debug ) {
            cout << "error_code is:" << error_code << ".\n";
        }
        s = e + 1;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue;
}
        string bytes_sent = line.substr ( s, e - s );
        if constexpr ( debug ) {
            cout << "bytes_sent is:" << bytes_sent << ".\n";
        }
        s = e + 1;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue;
}
        string object_size = line.substr ( s, e - s );
        if constexpr ( debug ) {
            cout << "object_size is:" << object_size << ".\n";
        }
        s = e + 1;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue;
}
        string total_time = line.substr ( s, e - s );
        if constexpr ( debug ) {
            cout << "total_time is:" << total_time << ".\n";
        }
        s = e + 1;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue;
}
        string turn_around_time = line.substr ( s, e - s );
        if ( debug ) {
            cout << "turn_around_time is:" << turn_around_time << ".\n";
        }
        s = e + 1;

        e = linefind ( line, '"', s + 1 );
        if ( e == string::npos ) { continue;
}
        string referrer = line.substr ( s + 1, e - s - 1 );
        if constexpr ( debug ) { cout << "referrer is:" << referrer << ".\n"; }
        s = e + 2;

        e = linefind ( line, '"', s + 1 );
        if ( e == string::npos ) { continue;
}
        string user_agent = line.substr ( s + 1, e - s - 1 );
        if constexpr ( debug ) {
            cout << "user_agent is:" << user_agent << ".\n";
        }
        s = e + 2;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue;
}
        string version_id = line.substr ( s, e - s );
        if constexpr ( debug ) {
            cout << "version_id is:" << version_id << ".\n";
        }
        s = e + 1;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue;
}
        string host_id = line.substr ( s, e - s );
        if constexpr ( debug ) { cout << "host_id is:" << host_id << ".\n"; }
        s = e + 1;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue;
}
        string signature_version = line.substr ( s, e - s );
        if ( debug ) {
            cout << "signature_version is:" << signature_version << ".\n";
        }
        s = e + 1;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue;
}
        string cipher_suite = line.substr ( s, e - s );
        if constexpr ( debug ) {
            cout << "cipher_suite is:" << cipher_suite << ".\n";
        }
        s = e + 1;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue;
}
        string authentication_type = line.substr ( s, e - s );
        if ( debug ) {
            cout << "authenticaiton_type is:" << authentication_type << ".\n";
        }
        s = e + 1;

        e = linefind ( line, ' ', s );
        if ( e == string::npos ) { continue;
}
        string host_header = line.substr ( s, e - s );
        if constexpr ( debug ) {
            cout << "host_header is:" << host_header << ".\n";
        }
        s = e + 1;

        string tls_version = line.substr ( s );
        if constexpr ( debug ) {
            cout << "tls_version is:" << tls_version << ".\n";
        }
        // s = e + 1;
        if ( tls_version.rfind ( "TLSv1" ) != 0 ) {
            cerr << " no TLS version: " << tls_version << "\n";
            continue;
        }

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
        request_uri = request_uri.substr ( s + 1 );

        size_t sp = request_uri.find ( ' ' ); // Remove optional _HTTP/1...
        if ( sp != string::npos ) { request_uri.resize ( sp ); }

        if constexpr ( debug ) { cerr << "request was\n " + request_uri; }

        string version, sessid, phid, acc;

        string url = "https://";
        url += host_header;
        url += request_uri;
        if ( !parse_url ( url, acc, version, sessid, phid ) ) { continue; }

        //if (acc.empty()) cerr << "Empty: " << request_uri << "\n";
        uint64_t bytecount = 0;
        try {
            if ( bytes_sent != "-" ) { bytecount = stoull ( bytes_sent ); }
        } catch ( const invalid_argument &a ) {
            cerr << "Not a number: " << bytes_sent << "\n";
            bytecount = 0;
        } catch ( const out_of_range &a ) {
            cerr << "out_of_range : " << bytes_sent << "\n";
            bytecount = 0;
        }

        if (bytecount > 1000000000000)
        {
            cerr << "bytecount too large: " << bytecount << "\n";
            continue;
        }

        // VDB-3822
        // fastq-dump.2.10-head
        if ( user_agent.find ( "-head" ) != string::npos ) {
            cmd = "HEAD";
            user_agent.resize(user_agent.size()-5);
        }

        struct sess sess;
        sess.ip = remote_ip;
        sess.agent = user_agent.substr ( 0, 64 );
        sess.domain = host_header;
        sess.acc = acc;
        sess.status.emplace ( http_status );
        sess.cmds.emplace ( cmd );
        sess.bytecount = bytecount;
        sess.bytecount = bytecount;
        sess.version = version;
        sess.sessid = sessid;
        sess.phid = phid;

        sess.cnt = 1;
        sess.start = static_cast<double> ( ltm );
        try {
            sess.end = sess.start + stol ( total_time ) / 1000.0;
        } catch ( const invalid_argument &a ) {
            cerr << "Not a number: " << total_time << "\n";
            sess.end = 0;
        } catch ( const out_of_range &a ) {
            cerr << "out_of_range: " << total_time << "\n";
            sess.end = 0;
        }

        if constexpr ( debug ) { cout << "end is:" << sess.end << "\n"; }

        sesskey = remote_ip;
        sesskey += acc;
        sesskey += user_agent;
        sesskey += bucket;

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
        j["version"] = sess.version;
        j["sessid"] = sess.sessid;
        j["phid"] = sess.phid;

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
