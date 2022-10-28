/**
 ******************************************************************************
 * 文件名程: Mannger_GPIO.c
 * 作    者: xiao
 * 版    本: V1.0
 * 编写日期: 2022-10-12
 * 功    能: 管理外设GPIO口
 ******************************************************************************
 */

#include "Manager.h"

uint32_t motor_speed = 500, motor_accel = 150, motor_decel = 150;
int32_t motor_step = 0;
uint8_t run_num = 0;

void Manager_GPIO_Init(void);
void Manager_GPIO_Mainloop(void);

void Key_Manage(void);         //内部按键
void Peripherals_Input(void);  //外部输入
void Peripherals_Output(void); //外部输出

void Manager_GPIO_Init(void)
{
    // GPIO初始化
    SuperGpio_Init();
    SuperGpio_WriteOutput(1, 1);
    SuperGpio_WriteOutput(0, 0);

    // GPIO中断激活
    // EXTI_useCallBack(GPIO_EXTIues);
}

void Manager_GPIO_Mainloop(void)
{
    Key_Manage();
    Peripherals_Input();
    Peripherals_Output();
}

void Key_Manage(void)
{
    //按键1
    if (!SuperGpio_ReadInput(0))
    {
        SuperGpio_WriteOutput(1, 1);
        STEPMOTOR_AxisMoveRel(motor_step, motor_accel, motor_decel, motor_speed); //电机运动执行
    }
    //按键2
    if (!SuperGpio_ReadInput(1))
    {
        SuperGpio_WriteOutput(1, 0);
        run_num = 0x01;
    }
    else
    {
        run_num = 0x00;
    }
    //按键3正方向写入
    if (!SuperGpio_ReadInput(2))
    {
        motor_step = 5 * SPR * POSITIVE;
        run_num = 0x02;
    }
    // STEMOTOR_JOG(run_num, motor_speed, motor_accel, motor_decel);
    //按键4反方向写入
    if (!SuperGpio_ReadInput(3))
    {
        motor_step = 5 * SPR * NEGATIVE;
    }
    //按键5执行回原点
    if (!SuperGpio_ReadInput(4))
    {
        STEPMOTOR_AxisMoveAbs(0, motor_accel, motor_decel, motor_speed);
    }
}

/*外设输入*/
void Peripherals_Input(void)
{
    /*伺服准备好*/
    if (!SuperGpio_ReadInput(5))
    {
        Mb_Tcpbuf.Servo_Feedback = Write_WordManage(TRUE, Mb_Tcpbuf.Servo_Feedback, 0);
    }
    else
    {
        Mb_Tcpbuf.Servo_Feedback = Write_WordManage(FALSE, Mb_Tcpbuf.Servo_Feedback, 0);
    }
    /*定位完成 */
    if (!SuperGpio_ReadInput(6))
    {
        /*待确定*/
    }
    /*电机旋转*/
    if (!SuperGpio_ReadInput(7))
    {
        Mb_Tcpbuf.Servo_Feedback = Write_WordManage(TRUE, Mb_Tcpbuf.Servo_Feedback, 2);
    }
    else
    {
        Mb_Tcpbuf.Servo_Feedback = Write_WordManage(FALSE, Mb_Tcpbuf.Servo_Feedback, 2);
    }
    /*电机故障输出*/
    if (!SuperGpio_ReadInput(8))
    {
        Mb_Tcpbuf.Servo_Feedback = Write_WordManage(TRUE, Mb_Tcpbuf.Servo_Feedback, 4);
    }
    else
    {
        Mb_Tcpbuf.Servo_Feedback = Write_WordManage(FALSE, Mb_Tcpbuf.Servo_Feedback, 4);
    }
    /*原点回零完成*/
    if (!SuperGpio_ReadInput(9))
    {
        Mb_Tcpbuf.Servo_Feedback = Write_WordManage(TRUE, Mb_Tcpbuf.Servo_Feedback, 1);
    }
    else
    {
        Mb_Tcpbuf.Servo_Feedback = Write_WordManage(FALSE, Mb_Tcpbuf.Servo_Feedback, 1);
    }
    /*面板运行/维修*/
    if (!SuperGpio_ReadInput(10))
    {
        Mb_Tcpbuf.Run_Service_Mode = TRUE;
    }
    else
    {
        Mb_Tcpbuf.Run_Service_Mode = FALSE;
    }
    /*面板急停按钮*/
    if (!SuperGpio_ReadInput(11))
    {
        Mb_Tcpbuf.Button_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.Button_FeedBackState, 0);
    }
    else
    {
        Mb_Tcpbuf.Button_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.Button_FeedBackState, 0);
    }
    /*面板启动按钮*/
    if (!SuperGpio_ReadInput(12))
    {
        Mb_Tcpbuf.Button_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.Button_FeedBackState, 2);
    }
    else
    {
        Mb_Tcpbuf.Button_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.Button_FeedBackState, 2);
    }
    /*面板停止按钮*/
    if (!SuperGpio_ReadInput(13))
    {
        Mb_Tcpbuf.Button_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.Button_FeedBackState, 1);
    }
    else
    {
        Mb_Tcpbuf.Button_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.Button_FeedBackState, 1);
    }
    /*面板复位按钮*/
    if (!SuperGpio_ReadInput(14))
    {
        Mb_Tcpbuf.Button_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.Button_FeedBackState, 3);
    }
    else
    {
        Mb_Tcpbuf.Button_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.Button_FeedBackState, 3);
    }
    /*面板手动/自动切换*/
    if (!SuperGpio_ReadInput(15))
    {
        Mb_Tcpbuf.Manual_Automatic_Mode = TRUE;
    }
    else
    {
        Mb_Tcpbuf.Manual_Automatic_Mode = FALSE;
    }
    /*检测气缸下限位*/
    if (!SuperGpio_ReadInput(16))
    {
        Mb_Tcpbuf.CylinderDetection_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.CylinderDetection_FeedBackState, 0);
    }
    else
    {
        Mb_Tcpbuf.CylinderDetection_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.CylinderDetection_FeedBackState, 0);
    }
}

/*外设输出*/
void Peripherals_Output(void)
{
    /*伺服报警复位*/
    if (Mb_Tcpbuf.ServoErrorReset)
    {
        SuperGpio_WriteOutput(3, 0);
        DB_HoldingBuf[72] = 3;
    }
    else
    {
        SuperGpio_WriteOutput(3, 1);
    }
    /*三色灯_绿*/
    if (Read_WordManage(Mb_Tcpbuf.TricolourLight_BuuzerControl, 1))
    {
        SuperGpio_WriteOutput(4, 0);
        DB_HoldingBuf[72] = 4;
    }
    else
    {
        SuperGpio_WriteOutput(4, 1);
    }
    /*三色灯_黄*/
    if (Read_WordManage(Mb_Tcpbuf.TricolourLight_BuuzerControl, 2))
    {
        SuperGpio_WriteOutput(5, 0);
        DB_HoldingBuf[72] = 5;
    }
    else
    {
        SuperGpio_WriteOutput(5, 1);
    }
    /*三色灯_红*/
    if (Read_WordManage(Mb_Tcpbuf.TricolourLight_BuuzerControl, 0))
    {
        SuperGpio_WriteOutput(6, 0);
        DB_HoldingBuf[72] = 6;
    }
    else
    {
        SuperGpio_WriteOutput(6, 1);
    }
}
