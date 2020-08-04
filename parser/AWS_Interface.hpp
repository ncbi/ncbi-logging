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
                       CatWriterInterface & outputs );

            virtual void parse();

        private:
            LogAWSEvent & m_receiver;
        };

        class AWSMultiThreadedParser : public ParserInterface
        {
        public:
            AWSMultiThreadedParser( 
                std::istream & input,  
                CatWriterInterface & outputs,
                size_t queueLimit,
                size_t threadNum
            );

            virtual void parse();

        private:
            size_t m_queueLimit;
            size_t m_threadNum;
        };

#if 0   
            class ParseThread
            {
            public:
                Thread(AWS_LogLinesInterface_Factory &loglinesF, 
                       Formatter & formatter,
                       CatWriterInterface & outputs) {}

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
                       CatWriterInterface & outputs )
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

