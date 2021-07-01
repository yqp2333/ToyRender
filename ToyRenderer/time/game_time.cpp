#include"game_time.h"

GameTime::GameTime() :
    mSencondsPerCount(0.0),
    mDeltaTime(-1.0),
    mBaseTime(0),
    mPauseTime(0),
    mStopTime(0),
    mPrevTime(0),
    mCurrentTime(0),
    isStoped(false)
{
    __int64 countsPerSec;
    QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
    mSencondsPerCount = 1.0 / (double)countsPerSec;//每个计数所代表的秒数
}

void GameTime::Tick()
{

    if (isStoped) {
        //如果当前是停止状态，则间隔时间为0
        mDeltaTime = 0.0;
        return;
    }

    __int64 currentTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
    mCurrentTime = currentTime;
    //计算当前帧和前一帧时间差
    mDeltaTime = (mCurrentTime - mPrevTime) * mSencondsPerCount;
    //准备计算当前帧和下一帧的时间差
    mPrevTime = mCurrentTime;
    //排除时间差为负值
    if (mDeltaTime < 0)
    {
        mDeltaTime = 0;
    }
}

float GameTime::DeltaTIme()const
{
    return (float)mDeltaTime;
}

void GameTime::Reset()
{
    __int64 currTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

    mBaseTime = currTime;	//当前时间作为基准时间
    mPrevTime = currTime;	//当前时间作为上一帧时间，因为重置了，此时没有上一帧
    mStopTime = 0;			//重置停止那一刻时间为0
    isStoped = false;		//重置后的状态为不停止
}

void GameTime::Stop() {
    if (isStoped)
    {
        __int64 currTime;
        QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
        mStopTime = currTime;
        isStoped = true;
    }
}

void GameTime::Start()
{
    __int64 startTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
    if (isStoped)
    {
        mPauseTime += (startTime - mStopTime);
        mPrevTime = startTime;
        mStopTime = 0;
        isStoped = false;
    }
}

float GameTime::TotalTime()const
{
    if (isStoped)
    {
        return (float)((mStopTime - mPauseTime - mBaseTime) * mSencondsPerCount);
    }
    else
    {
        return (float)((mCurrentTime - mPauseTime - mBaseTime) * mSencondsPerCount);
    }
}

bool GameTime::IsStoped()
{
    return isStoped;
}