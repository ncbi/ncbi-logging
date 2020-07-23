#pragma once

#include "LogLinesInterface.hpp"

namespace NCBI
{
    namespace Logging
    {
        struct AWS_LogLinesInterface : public LogLinesInterface
        {
            typedef enum { 
                owner = LastMemberId+1,
                bucket,
                time,
                requester,
                request_id,
                operation,
                key,
                res_code,
                error,
                res_len,
                obj_size,
                total_time,
                turnaround_time,
                version_id,
                host_id,
                sig_ver,
                cipher_suite,
                auth_type,
                host_header,
                tls_version,
                AWS_LastMemberId = tls_version
            } AWS_Members;
            virtual void set( AWS_Members m, const t_str & ) = 0;
        };

        struct LogAWSEvent : public AWS_LogLinesInterface, public CommonLogEvent
        {
            t_str       owner;
            t_str       bucket;
            t_timepoint time;
            t_str       requester;
            t_str       request_id;
            t_str       operation;
            t_str       key;
            t_str       res_code;
            t_str       error;
            t_str       res_len;
            t_str       obj_size;
            t_str       total_time;
            t_str       turnaround_time;
            t_str       version_id;
            t_str       host_id;
            t_str       sig_ver;
            t_str       cipher_suite;
            t_str       auth_type;
            t_str       host_header;
            t_str       tls_version;

            LogAWSEvent()
            {
                EMPTY_TSTR( owner );
                EMPTY_TSTR( bucket );
                memset( &time, 0, sizeof time );
                EMPTY_TSTR( requester );
                EMPTY_TSTR( request_id );
                EMPTY_TSTR( operation );
                EMPTY_TSTR( key );
                EMPTY_TSTR( res_code );
                EMPTY_TSTR( error );
                EMPTY_TSTR( res_len );
                EMPTY_TSTR( obj_size );
                EMPTY_TSTR( total_time );
                EMPTY_TSTR( turnaround_time );
                EMPTY_TSTR( version_id );
                EMPTY_TSTR( host_id );
                EMPTY_TSTR( sig_ver );
                EMPTY_TSTR( cipher_suite );
                EMPTY_TSTR( auth_type );
                EMPTY_TSTR( host_header );
                EMPTY_TSTR( tls_version );
            }

            virtual void beginLine();
            virtual void endLine();
            virtual Category GetCategory() const;
            virtual void failedToParse( const t_str & source );
            virtual void set( Members m, const t_str & v );
            virtual void set( Members m, int64_t v );
            virtual void setAgent( const t_agent & a );
            virtual void setRequest( const t_request & r );
            virtual bool nextMember( Member & value );
            virtual ReportFieldResult reportField( Members field, const char * value, const char * message, ReportFieldType type );

            virtual void set( AWS_Members m, const t_str & );
            virtual void setTime( const t_timepoint & t );

        private:
            // typedef vector<Member> Members;
            // Members members;
        };
#if 0
        class AWSParser
        {
        public:
            AWSParser(  ) : m_lines( loglines ), m_input( input ) {}

            void parseSingleThreaded(AWS_LogLinesInterface &loglines, 
                       std::istream &input,
                       FormatterInterface & formatter,
                       ClassificationInterface & outputs ) 
            { 
                string source;
                while ( m_input.getline( source ) )
                {
                    loglines.reset();
                    scan_init(source);
                    if ( ! yyparse( loglines ) ) 
                        outputs . write( cat_ugly, formatter.format(source) );
                    else 
                    {
                        outputs. write ( loglines.Category(), formatter . format ( loglines ) );
                    }
                }
            }

            class OneReaderQueue
            {   //TODO: instrument to report the maximum queue size reached

                void enqueue( const string & s ) 
                { 
                    locker(lock); 
                    queue.push(s);  // copy
                }

                bool dequeue( string & s ) 
                { 
                    if ( ! queue.empty() )
                    {
                        locker(lock); 
                        s = queue.pop(); 
                        return true;
                    }

                    while ( open.get() )
                    {
                        if ( queue.empty() ) 
                            sleep();
                        else
                        {
                            locker(lock); 
                            s = queue.pop(); 
                            return true;
                        }
                    }
                    return false;
                }

                void close() 
                { 
                    open.set( false ); 
                }

                bool is_open() const { return open; }

                vector<string> queue;
                atomic<bool> open;
                lock lock;
            }
            
            class ParseThread
            {
            public:
                Thread(AWS_LogLinesInterface_Factory &loglinesF, 
                       Formatter & formatter,
                       ClassificationInterface & outputs) {}

                OneReaderQueue queue; 

                void run()
                {
                    string line;
                    while ( queue.dequeue( line ) )
                    {   //TODO: keep track fo line numbers (have queue carry tuples <line, line_no>)
                        AWS_LogLinesInterface receiver = loglinesF.create(); //TODO: add line_no to receiver
                        scan_reset( line );
                        if ( ! yyparse( receiver, scanner ) ) 
                            outputs . write( cat_ugly, formatter.format(line) );
                        else 
                        {
                            outputs. write ( receiver.Category(), formatter . format ( receiver ) );
                        }
                    }
                }

                void close() 
                { 
                    queue.close(); 
                    queue.join(); 
                }
            }

            class OutputThread
            {
            public:
                OutputThread( ostream& out);

                OneReaderQueue queue; 

                void run()
                {
                    string line;
                    while ( queue.dequeue( line ) )
                    {
                        out << line << endl;
                    }
                }        
            };

            class ThreadPool
            {
            public:
                ThreadPool( numTreads, 
                       AWS_LogLinesInterface_Factory &loglinesF, 
                       formatter,
                       ClassificationInterface & outputs )
                {
                    for( i = 1..numThreads )
                    {
                        workers.push(new ParseThread(loglinesF, formatter, outputs ))
                    }
                }
                ~ThreadPool()
                {
                }

                void enqueueLine( string str )
                {   
                    //round robin
                    workers[cur].enqueue(str); //TODO: if the queues are bounded, block if full
                    cur++;
                    if ( cur >= numThreads )
                    {
                        cur = 0;
                    }
                }

                void wait()
                {
                    for( i = 1..numThreads )
                    {
                        workers[i].close();
                    }
                }
            }

            void parseMultiThreaded 
              ( AWS_LogLinesInterface_Factory &loglinesF, 
                std::istream &input,
                formatter,
                strng baseName,
                uint numTreads ) 
            { 
                ThreadFileClassifier outputs ( baseName );
                ThreadPool threadPool(numTreads, 
                                     loglinesF, 
                                     formatter,
                                     outputs);
                string source;
                while ( m_input.getline( source ) )
                {
                    threadPool.enqueueLine( source );
                }
                threadPool.wait();
            }

            void setDebug( bool on ) {}
            void setLineFilter( unsigned long int line_nr ) {}

        private:
            AWS_LogLinesInterface &  m_lines;
            std::istream &  m_input;
            unsigned long int line_filter = 0;            
        };
#endif
    }
}

