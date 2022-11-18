/*
 * @Description:
 * @Author: xiao
 * @Date: 2022-10-15 14:01:01
 * @LastEditTime: 2022-10-20 14:13:00
 * @LastEditors: xiao
 */
#ifndef _VARABLE_DEFINE_H
#define _VARABLE_DEFINE_H

#include "main.h"

typedef enum
{
    home = 0x00,
    SeekOrigin_mode,
    relative_mode,
    absolute_mode,
} mode_t;

typedef enum
{
    ConState0H = 0,
    ConState1H,
    ConState2H,
    ConState3H
} State_t;

typedef enum
{
    Sta0 = 0,
    Sta1,
    Sta2,
    Sta3,
    Sta4,
    Sta5,
    Sta6,
    Sta7,
    Sta8,
} switch_sta;

typedef struct
{
    /*写入寄存器数据*/
    uint16_t Heartcount;                      // 0心跳包
    uint16_t Servo_Feedback;                  //伺服反馈状态
    uint16_t Manual_Automatic_Mode;           //手/自动模式
    uint16_t Run_Service_Mode;                //运行/维修模式
    uint16_t Button_FeedBackState;            //按钮反馈状态
    uint16_t CylinderDetection_FeedBackState; //气缸检测反馈状态
    uint16_t SensorDetection_FeedBackState;   //光电开关检测反馈状态
    uint16_t Fan_ElectricDrum_FeedBackState;  // 7风扇/电棍筒故障反馈状态
    uint16_t CalibrationLocation1H_FeedBack;  //标定位置1高字反馈
    uint16_t CalibrationLocation1L_FeedBack;  //标定位置1低字反馈
    uint16_t CalibrationLocation2H_FeedBack;  //标定位置2高字反馈
    uint16_t CalibrationLocation2L_FeedBack;  //标定位置2低字反馈
    uint16_t CalibrationLocation3H_FeedBack;  //标定位置3高字反馈
    uint16_t CalibrationLocation3L_FeedBack;  //标定位置3低字反馈
    uint16_t CalibrationLocation4H_FeedBack;  //标定位置4高字反馈
    uint16_t CalibrationLocation4L_FeedBack;  //标定位置4低字反馈
    uint16_t CalibrationLocation5H_FeedBack;  //标定位置5高字反馈
    uint16_t CalibrationLocation5L_FeedBack;  //标定位置5低字反馈
    uint16_t CalibrationLocation6H_FeedBack;  //标定位置6高字反馈
    uint16_t CalibrationLocation6L_FeedBack;  //标定位置6低字反馈
    uint16_t CalibrationLocation7H_FeedBack;  //标定位置7高字反馈
    uint16_t CalibrationLocation7L_FeedBack;  //标定位置7低字反馈
    uint16_t CalibrationLocation8H_FeedBack;  //标定位置8高字反馈
    uint16_t CalibrationLocation8L_FeedBack;  //标定位置8低字反馈
    uint16_t WriteReserved9;                  //写入预留
    uint16_t WriteReserved10;                 //写入预留
    uint16_t WriteReserved11;                 //写入预留
    uint16_t WriteReserved12;                 //写入预留
    uint16_t WriteReserved13;                 //写入预留
    uint16_t WriteReserved14;                 //写入预留
    uint16_t WriteReserved15;                 //写入预留
    uint16_t WriteReserved16;                 //写入预留
    uint16_t WriteReserved17;                 //写入预留
    uint16_t WriteReserved18;                 //写入预留
    uint16_t WriteReserved19;                 //写入预留
    uint16_t WriteReserved20;                 //写入预留
    uint16_t WriteReserved21;                 //写入预留
    uint16_t WriteReserved22;                 //写入预留
    uint16_t WriteReserved23;                 //写入预留
    uint16_t WriteReserved24;                 //写入预留
    uint16_t WriteReserved25;                 //写入预留
    uint16_t WriteReserved26;                 //写入预留
    uint16_t WriteReserved27;                 //写入预留
    uint16_t WriteReserved28;                 //写入预留
    uint16_t WriteReserved29;                 //写入预留
    uint16_t WriteReserved30;                 //写入预留
    uint16_t WriteReserved31;                 //写入预留
    uint16_t WriteReserved32;                 //写入预留
    uint16_t WriteReserved33;                 //写入预留
    uint16_t WriteReserved34;                 // 49写入预留34

    /*读取寄存器数据*/
    uint16_t TricolourLight_BuuzerControl; // 50三色灯/蜂鸣器控制
    uint16_t CylinderAction_Control;       //气缸动作控制
    uint16_t ElectricRun_Control;          //电棍筒运行控制
    uint16_t ServoErrorReset;              //伺服报错复位
    uint16_t ServoJogRun;                  //伺服点动运行控制
    uint16_t ServoModeRun;                 //伺服模式选择运行
    uint16_t ServoWriteData;               //伺服写入参数
    uint16_t ServoJogSpeedSet;             //伺服点动运行速度设置
    uint16_t ServoModeSpeedSet;            //伺服模式运行速度设置
    uint16_t ServoAddSpeedSet;             //伺服加速度设置
    uint16_t ServoReduceSpeedSet;          //伺服减速度设置
    uint16_t ServoTargetLocationSetH;      //伺服目标移动位置高字设置
    uint16_t ServoTargetLocationSetL;      // 62伺服目标移动位置低字设置
} MB_tcpbuf_t;

extern MB_tcpbuf_t Mb_Tcpbuf;

/*读写寄存器位操作函数调用*/
extern uint16_t Write_WordManage(uint8_t State, uint16_t data, uint8_t n);
extern uint8_t Read_WordManage(uint16_t data, uint8_t n);

#endif
