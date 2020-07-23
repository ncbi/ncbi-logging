#include "AWS_Interface.hpp"

using namespace NCBI::Logging;

void LogAWSEvent::beginLine() {}
void LogAWSEvent::endLine()  {}

LogLinesInterface::Category 
LogAWSEvent::GetCategory() const 
{ 
    return cat_ugly; 
}

void LogAWSEvent::failedToParse( const t_str & source ) {}
void LogAWSEvent::set( Members m, const t_str & v ) {}
void LogAWSEvent::set( Members m, int64_t v ) {}
void LogAWSEvent::setAgent( const t_agent & a ) {}
void LogAWSEvent::setRequest( const t_request & r ) {}
void LogAWSEvent::setTime( const t_timepoint & t ) {}
bool LogAWSEvent::nextMember( Member & value ) { return false; }

LogLinesInterface::ReportFieldResult 
LogAWSEvent::reportField( Members field, const char * value, const char * message, ReportFieldType type ) 
{
    return proceed;
}

void LogAWSEvent::set( AWS_Members m, const t_str & ) {}
