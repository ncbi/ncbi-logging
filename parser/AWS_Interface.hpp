#pragma once

#include "LogLinesInterface.hpp"

namespace NCBI
{
    namespace Logging
    {
        struct LogAWSEvent : public LogLinesInterface
        {
            using LogLinesInterface::set;
            
            LogAWSEvent( FormatterInterface & fmt );

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
            } AWS_Members; // all are t_str values
            virtual void set( AWS_Members m, const t_str & ); // will invoke set( LogLinesInterface::Members ) if necessary

            virtual void reportField( const char * message );

        private:
        };

        class AWSParser : public ParserInterface
        {
        public:
            AWSParser( std::istream & input,  
                       LogAWSEvent & receiver,
                       ClassificationInterface & outputs ) 
            :   ParserInterface ( input, receiver, outputs )
            {
            }

            virtual void parse();
        };

        class AWSMultiThreadedParser : public ParserInterface
        {
        public:
        };
#if 0
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

