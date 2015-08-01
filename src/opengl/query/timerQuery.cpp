#include "saiga/opengl/query/timerQuery.h"



TimerQuery::TimerQuery()
{


}

TimerQuery::~TimerQuery()
{
    if(queryFrontBuffer)
        glDeleteQueries(1, &queryFrontBuffer);
    if(queryBackBuffer)
        glDeleteQueries(1, &queryBackBuffer);
}

void TimerQuery::create()
{
    if(!queryFrontBuffer)
        glGenQueries(1, &queryFrontBuffer);
    if(!queryBackBuffer)
        glGenQueries(1, &queryBackBuffer);

    //initialize both queries
    //without that the first stopTimers call would gernerate the following OpenGL error:
    //Message : GL_INVALID_OPERATION error generated. Query object not found.

    glBeginQuery(GL_TIME_ELAPSED,queryFrontBuffer);
    glEndQuery(GL_TIME_ELAPSED);

    glBeginQuery(GL_TIME_ELAPSED,queryBackBuffer);
    glEndQuery(GL_TIME_ELAPSED);
}

void TimerQuery::swapQueries()
{
    std::swap(queryBackBuffer,queryFrontBuffer);
}


void TimerQuery::startTimer()
{
    glBeginQuery(GL_TIME_ELAPSED,queryBackBuffer);
}

void TimerQuery::stopTimer()
{
    glEndQuery(GL_TIME_ELAPSED);

    glGetQueryObjectui64v(queryFrontBuffer,GL_QUERY_RESULT, &time);
    swapQueries();
}

float TimerQuery::getTimeMS()
{
    return time/1000000.0f;
}

GLuint64 TimerQuery::getTimeNS()
{
    return time;
}

