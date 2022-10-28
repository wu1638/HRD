/**
 ******************************************************************************
 * �ļ�����: Mannger_GPIO.c
 * ��    ��: xiao
 * ��    ��: V1.0
 * ��д����: 2022-10-12
 * ��    ��: ��������GPIO��
 ******************************************************************************
 */

#include "Manager.h"

uint32_t motor_speed = 500, motor_accel = 150, motor_decel = 150;
int32_t motor_step = 0;
uint8_t run_num = 0;

void Manager_GPIO_Init(void);
void Manager_GPIO_Mainloop(void);

void Key_Manage(void);         //�ڲ�����
void Peripherals_Input(void);  //�ⲿ����
void Peripherals_Output(void); //�ⲿ���

void Manager_GPIO_Init(void)
{
    // GPIO��ʼ��
    SuperGpio_Init();
    SuperGpio_WriteOutput(1, 1);
    SuperGpio_WriteOutput(0, 0);

    // GPIO�жϼ���
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
    //����1
    if (!SuperGpio_ReadInput(0))
    {
        SuperGpio_WriteOutput(1, 1);
        STEPMOTOR_AxisMoveRel(motor_step, motor_accel, motor_decel, motor_speed); //����˶�ִ��
    }
    //����2
    if (!SuperGpio_ReadInput(1))
    {
        SuperGpio_WriteOutput(1, 0);
        run_num = 0x01;
    }
    else
    {
        run_num = 0x00;
    }
    //����3������д��
    if (!SuperGpio_ReadInput(2))
    {
        motor_step = 5 * SPR * POSITIVE;
        run_num = 0x02;
    }
    // STEMOTOR_JOG(run_num, motor_speed, motor_accel, motor_decel);
    //����4������д��
    if (!SuperGpio_ReadInput(3))
    {
        motor_step = 5 * SPR * NEGATIVE;
    }
    //����5ִ�л�ԭ��
    if (!SuperGpio_ReadInput(4))
    {
        STEPMOTOR_AxisMoveAbs(0, motor_accel, motor_decel, motor_speed);
    }
}

/*��������*/
void Peripherals_Input(void)
{
    /*�ŷ�׼����*/
    if (!SuperGpio_ReadInput(5))
    {
        Mb_Tcpbuf.Servo_Feedback = Write_WordManage(TRUE, Mb_Tcpbuf.Servo_Feedback, 0);
    }
    else
    {
        Mb_Tcpbuf.Servo_Feedback = Write_WordManage(FALSE, Mb_Tcpbuf.Servo_Feedback, 0);
    }
    /*��λ��� */
    if (!SuperGpio_ReadInput(6))
    {
        /*��ȷ��*/
    }
    /*�����ת*/
    if (!SuperGpio_ReadInput(7))
    {
        Mb_Tcpbuf.Servo_Feedback = Write_WordManage(TRUE, Mb_Tcpbuf.Servo_Feedback, 2);
    }
    else
    {
        Mb_Tcpbuf.Servo_Feedback = Write_WordManage(FALSE, Mb_Tcpbuf.Servo_Feedback, 2);
    }
    /*����������*/
    if (!SuperGpio_ReadInput(8))
    {
        Mb_Tcpbuf.Servo_Feedback = Write_WordManage(TRUE, Mb_Tcpbuf.Servo_Feedback, 4);
    }
    else
    {
        Mb_Tcpbuf.Servo_Feedback = Write_WordManage(FALSE, Mb_Tcpbuf.Servo_Feedback, 4);
    }
    /*ԭ��������*/
    if (!SuperGpio_ReadInput(9))
    {
        Mb_Tcpbuf.Servo_Feedback = Write_WordManage(TRUE, Mb_Tcpbuf.Servo_Feedback, 1);
    }
    else
    {
        Mb_Tcpbuf.Servo_Feedback = Write_WordManage(FALSE, Mb_Tcpbuf.Servo_Feedback, 1);
    }
    /*�������/ά��*/
    if (!SuperGpio_ReadInput(10))
    {
        Mb_Tcpbuf.Run_Service_Mode = TRUE;
    }
    else
    {
        Mb_Tcpbuf.Run_Service_Mode = FALSE;
    }
    /*��弱ͣ��ť*/
    if (!SuperGpio_ReadInput(11))
    {
        Mb_Tcpbuf.Button_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.Button_FeedBackState, 0);
    }
    else
    {
        Mb_Tcpbuf.Button_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.Button_FeedBackState, 0);
    }
    /*���������ť*/
    if (!SuperGpio_ReadInput(12))
    {
        Mb_Tcpbuf.Button_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.Button_FeedBackState, 2);
    }
    else
    {
        Mb_Tcpbuf.Button_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.Button_FeedBackState, 2);
    }
    /*���ֹͣ��ť*/
    if (!SuperGpio_ReadInput(13))
    {
        Mb_Tcpbuf.Button_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.Button_FeedBackState, 1);
    }
    else
    {
        Mb_Tcpbuf.Button_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.Button_FeedBackState, 1);
    }
    /*��帴λ��ť*/
    if (!SuperGpio_ReadInput(14))
    {
        Mb_Tcpbuf.Button_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.Button_FeedBackState, 3);
    }
    else
    {
        Mb_Tcpbuf.Button_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.Button_FeedBackState, 3);
    }
    /*����ֶ�/�Զ��л�*/
    if (!SuperGpio_ReadInput(15))
    {
        Mb_Tcpbuf.Manual_Automatic_Mode = TRUE;
    }
    else
    {
        Mb_Tcpbuf.Manual_Automatic_Mode = FALSE;
    }
    /*�����������λ*/
    if (!SuperGpio_ReadInput(16))
    {
        Mb_Tcpbuf.CylinderDetection_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.CylinderDetection_FeedBackState, 0);
    }
    else
    {
        Mb_Tcpbuf.CylinderDetection_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.CylinderDetection_FeedBackState, 0);
    }
}

/*�������*/
void Peripherals_Output(void)
{
    /*�ŷ�������λ*/
    if (Mb_Tcpbuf.ServoErrorReset)
    {
        SuperGpio_WriteOutput(3, 0);
        DB_HoldingBuf[72] = 3;
    }
    else
    {
        SuperGpio_WriteOutput(3, 1);
    }
    /*��ɫ��_��*/
    if (Read_WordManage(Mb_Tcpbuf.TricolourLight_BuuzerControl, 1))
    {
        SuperGpio_WriteOutput(4, 0);
        DB_HoldingBuf[72] = 4;
    }
    else
    {
        SuperGpio_WriteOutput(4, 1);
    }
    /*��ɫ��_��*/
    if (Read_WordManage(Mb_Tcpbuf.TricolourLight_BuuzerControl, 2))
    {
        SuperGpio_WriteOutput(5, 0);
        DB_HoldingBuf[72] = 5;
    }
    else
    {
        SuperGpio_WriteOutput(5, 1);
    }
    /*��ɫ��_��*/
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
