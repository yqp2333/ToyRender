#pragma once
#include <Windows.h>

class  GameTime{

public:
    static GameTime& GetInstance()
    {
        return instance;
    }

    float TotalTime()const;//游戏运行的总时间
    float DeltaTIme()const;//获取mDeltaTime变量
    bool IsStoped();   //获取isStoped变量

    void Reset(); 
    void Start();
    void Stop();
    void Tick();

private:
    GameTime();
    ~GameTime();

    static GameTime instance;
    double mSencondsPerCount; //计数/s
    double mDeltaTime; //每帧时间
    __int64 mCurrentTime; //本帧时间
    __int64 mBaseTime;//重置后的基准时间
    __int64 mPauseTime;
    __int64 mStopTime;//停止那一刻的时间
    __int64 mPrevTime;//上一帧时间

    bool isStoped;       //是否为停止状态
};
