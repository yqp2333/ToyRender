#pragma once
#include <Windows.h>

class  GameTime{

public:
    GameTime();

    float TotalTime()const;//��Ϸ���е���ʱ��
    float DeltaTIme()const;//��ȡmDeltaTime����
    bool IsStoped();   //��ȡisStoped����

    void Reset(); 
    void Start();
    void Stop();
    void Tick();

private:
    double mSencondsPerCount; //����/s
    double mDeltaTime; //ÿ֡ʱ��
    __int64 mCurrentTime; //��֡ʱ��
    __int64 mBaseTime;//���ú�Ļ�׼ʱ��
    __int64 mPauseTime;
    __int64 mStopTime;//ֹͣ��һ�̵�ʱ��
    __int64 mPrevTime;//��һ֡ʱ��


    bool isStoped;       //�Ƿ�Ϊֹͣ״̬
};
