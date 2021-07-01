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
    mSencondsPerCount = 1.0 / (double)countsPerSec;//ÿ�����������������
}

void GameTime::Tick()
{

    if (isStoped) {
        //�����ǰ��ֹͣ״̬������ʱ��Ϊ0
        mDeltaTime = 0.0;
        return;
    }

    __int64 currentTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
    mCurrentTime = currentTime;
    //���㵱ǰ֡��ǰһ֡ʱ���
    mDeltaTime = (mCurrentTime - mPrevTime) * mSencondsPerCount;
    //׼�����㵱ǰ֡����һ֡��ʱ���
    mPrevTime = mCurrentTime;
    //�ų�ʱ���Ϊ��ֵ
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

    mBaseTime = currTime;	//��ǰʱ����Ϊ��׼ʱ��
    mPrevTime = currTime;	//��ǰʱ����Ϊ��һ֡ʱ�䣬��Ϊ�����ˣ���ʱû����һ֡
    mStopTime = 0;			//����ֹͣ��һ��ʱ��Ϊ0
    isStoped = false;		//���ú��״̬Ϊ��ֹͣ
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