/**
 ******************************************************************************
 * �ļ�����: Mannger_Motor.c
 * ��    ��: xiao
 * ��    ��: V1.0
 * ��д����: 2022-10-12
 * ��    ��: ������������ƽӿ�
 ******************************************************************************
 */

#include "Manager.h"

#define SlowSpeed 200

extern __IO int32_t step_position; // ��ǰλ��
int32_t CalibrationBuf[8];
int32_t Position_Init = 0, set_location = 0, Issue_TarGetLocation = 0;

int16_t Test_TarGet = 0;
uint8_t entry_count = 0, Trigger_run = 0, Switch_state = 0, switch_num = 0, test_num = 0;

void Manager_Motor_Init(void);     //������Ƴ�ʼ��
void Manager_Motor_Mainloop(void); //���������ѭ��Ӧ��

void MotorRun_perform(void);                  //��λ�߼�ִ�е��
void Entry_CalibrationLocation_Perfrom(void); //ִ��¼��궨λ��

uint8_t Continuous_MovementMode(int32_t location, uint8_t mode_id); //ģʽ���������л�
void Calibration_LocationSwitch(uint8_t mode_id);                   //λ�ñ궨����

void Manager_Motor_Init(void)
{
  STEPMOTOR_TIMx_Init();
}

void Manager_Motor_Mainloop(void)
{
  MotorRun_perform();
  Entry_CalibrationLocation_Perfrom();
  if (DB_HoldingBuf[71])
  {
    Mb_Tcpbuf.Servo_Feedback = Write_WordManage(TRUE, Mb_Tcpbuf.Servo_Feedback, 8);
    printf("��ǰֵ��%d\t\n", step_position);
    printf("����ֵ��%d\t\n", Issue_TarGetLocation);
    DB_HoldingBuf[71] = FALSE;
  }
  if (DB_HoldingBuf[70])
  {
    /* code */
    Mb_Tcpbuf.Servo_Feedback = Write_WordManage(TRUE, Mb_Tcpbuf.Servo_Feedback, 5);
  }
  else
  {
    Mb_Tcpbuf.Servo_Feedback = Write_WordManage(FALSE, Mb_Tcpbuf.Servo_Feedback, 2);
  }
}

/*ģʽ��λ��ɱ�־����*/
uint8_t PosCompleteMark(int32_t Location, uint8_t mode_id)
{
  uint8_t CompleteState = FALSE;

  if (mode_id == relative_mode)
  {
    /* ��Զ�λ��ɱ�־  */
    if (Location == (step_position - Position_Init))
    {
      CompleteState = TRUE;
    }
  }
  else if (mode_id == absolute_mode)
  {
    /* ���Զ�λ��ɱ�־ */
    if (Position_Init && Location == step_position)
    {
      CompleteState = TRUE;
    }
  }
  return CompleteState;
}

/*����Ŀ��λ��˫������*/
int32_t DoubleWord_GroupCode(uint16_t Hdata, uint16_t Ldata)
{
  return (Hdata << 16 | Ldata);
}

/*����˶�ģʽ*/
void MotorRun_perform(void)
{
  /*�����˶�Ŀ��λ�ô���*/
  Issue_TarGetLocation = DoubleWord_GroupCode(Mb_Tcpbuf.ServoTargetLocationSetH, Mb_Tcpbuf.ServoTargetLocationSetL);
  /*����ģʽ�ж�*/
  if (Mb_Tcpbuf.ServoModeRun == SeekOrigin_mode)
  {
    /* Ѱ��ԭ��ģʽ */
    if (Mb_Tcpbuf.ServoWriteData == TRUE)
    {
      DB_HoldingBuf[56] = FALSE;
      STEPMOTOR_AxisHome(Mb_Tcpbuf.ServoModeSpeedSet, SlowSpeed, Mb_Tcpbuf.ServoAddSpeedSet, Mb_Tcpbuf.ServoReduceSpeedSet);
    }
  }
  else if (Mb_Tcpbuf.ServoModeRun == relative_mode)
  {
    /* ����˶�ģʽ */
    if (Mb_Tcpbuf.ServoWriteData == TRUE)
    {
      DB_HoldingBuf[56] = FALSE;
      Position_Init = step_position;
      STEPMOTOR_AxisMoveRel(Issue_TarGetLocation, Mb_Tcpbuf.ServoAddSpeedSet, Mb_Tcpbuf.ServoReduceSpeedSet, Mb_Tcpbuf.ServoModeSpeedSet);
      /*Trigger_run = 0;
      switch_num = Sta1;*/
    }
    /*�Ĵ�����4λ��λ��ɱ�־*/
    if (PosCompleteMark(Issue_TarGetLocation, relative_mode))
    {
      Mb_Tcpbuf.Servo_Feedback = Write_WordManage(TRUE, Mb_Tcpbuf.Servo_Feedback, 3);
    }
    else
    {
      Mb_Tcpbuf.Servo_Feedback = Write_WordManage(FALSE, Mb_Tcpbuf.Servo_Feedback, 3);
    }

    /*�궨����*/
    // Calibration_LocationSwitch(relative_mode);
  }
  else if (Mb_Tcpbuf.ServoModeRun == absolute_mode)
  {
    /* �����˶�ģʽ */
    if (Mb_Tcpbuf.ServoWriteData == TRUE)
    {
      DB_HoldingBuf[56] = FALSE;
      Position_Init = TRUE;
      STEPMOTOR_AxisMoveAbs(Issue_TarGetLocation, Mb_Tcpbuf.ServoAddSpeedSet, Mb_Tcpbuf.ServoReduceSpeedSet, Mb_Tcpbuf.ServoModeSpeedSet);
      /*Trigger_run = 0;
      switch_num = Sta1;*/
    }
    /*�Ĵ�����4λ��λ��ɱ�־*/
    if (PosCompleteMark(Issue_TarGetLocation, absolute_mode))
    {
      Mb_Tcpbuf.Servo_Feedback = Write_WordManage(TRUE, Mb_Tcpbuf.Servo_Feedback, 3);
    }
    else
    {
      Mb_Tcpbuf.Servo_Feedback = Write_WordManage(FALSE, Mb_Tcpbuf.Servo_Feedback, 3);
    }
    /*�궨����*/
    // Calibration_LocationSwitch(absolute_mode);
  }
  else if (Mb_Tcpbuf.ServoModeRun == home && !Mb_Tcpbuf.ServoJogRun)
  {
    /* �������ֹͣ/��ͣ */
    STEPMOTOR_StandbyStop();
    Mb_Tcpbuf.Servo_Feedback = Write_WordManage(FALSE, Mb_Tcpbuf.Servo_Feedback, 3);
  }

  /*���������㶯����*/
  if (!Mb_Tcpbuf.ServoModeRun)
  {
    STEMOTOR_JOG(Mb_Tcpbuf.ServoJogRun, Mb_Tcpbuf.ServoJogSpeedSet, Mb_Tcpbuf.ServoAddSpeedSet, Mb_Tcpbuf.ServoReduceSpeedSet);
  }
}

/*¼��궨λ��*/
uint8_t entry_Calibration_location(int32_t location, int32_t *CalibrationBuf, uint8_t lent)
{
  uint8_t State = FALSE;

  *(CalibrationBuf + entry_count) = location;

  if (entry_count < 8)
  {
    if (*(CalibrationBuf + entry_count - 1) == *(CalibrationBuf + entry_count))
    {
      if (entry_count)
      {
        entry_count--;
      }
      *(CalibrationBuf + entry_count) = location;
    }
    entry_count++;
    State = TRUE;
  }
  else
  {
    entry_count = FALSE;
    State = FALSE;
  }
  return State;
}

/*¼��궨λ��ִ������*/
void Entry_CalibrationLocation_Perfrom(void)
{
  /*�궨λ��˫�ָߵ���*/
  for (uint8_t i = 0; i < 8; i++)
  {
    *(&(Mb_Tcpbuf.CalibrationLocation1H_FeedBack) + i * 2) = (CalibrationBuf[i] >> 16) & 0xffff;
    *(&(Mb_Tcpbuf.CalibrationLocation1L_FeedBack) + i * 2) = CalibrationBuf[i] & 0xffff;
  }
  /*¼��λ��ִ��*/
  if (Mb_Tcpbuf.ServoWriteData == ConState2H)
  {
    DB_HoldingBuf[56] = FALSE;
    entry_Calibration_location(step_position, CalibrationBuf, 8);
  }
}

/*ģʽ�����˶��л�*/
uint8_t Continuous_MovementMode(int32_t location, uint8_t mode_id)
{
  uint8_t complete = 0;

  if (mode_id == relative_mode)
  {
    /* ��������˶� */
    if (!Trigger_run)
    {
      Position_Init = step_position;
      STEPMOTOR_AxisMoveRel(location, Mb_Tcpbuf.ServoAddSpeedSet, Mb_Tcpbuf.ServoReduceSpeedSet, Mb_Tcpbuf.ServoModeSpeedSet);
      Trigger_run = TRUE;
    }
    if (location == (step_position - Position_Init))
    {
      /* ��λ��ɱ�־ */
      complete = TRUE;
    }
  }
  else if (mode_id == absolute_mode)
  {
    /* ���������˶� */
    if (!Trigger_run)
    {
      STEPMOTOR_AxisMoveAbs(location, Mb_Tcpbuf.ServoAddSpeedSet, Mb_Tcpbuf.ServoReduceSpeedSet, Mb_Tcpbuf.ServoModeSpeedSet);
      Trigger_run = TRUE;
    }
    if (location == step_position)
    {
      /* ��λ��ɱ�־ */
      complete = TRUE;
    }
  }

  return complete;
}

/*������������*/
uint8_t Location_RunCondition(void)
{
  uint8_t movementState = FALSE;

  if (!Switch_state)
  {
    Trigger_run = FALSE;
    Switch_state = TRUE;
    movementState = TRUE;
  }
  return movementState;
}
/*��λ�ñ궨��������*/
void Calibration_LocationSwitch(uint8_t mode_id)
{
  switch (switch_num)
  {
  case Sta1:

    set_location = CalibrationBuf[0];
    Location_RunCondition();
    if (Continuous_MovementMode(set_location, mode_id))
    {

      if (CalibrationBuf[0] == CalibrationBuf[1])
      {
        test_num = Sta0;
        switch_num = Sta0;
      }
      else
      {
        printf("һ�ζ�λ���\t\n");
        printf("��ǰλ�ã�%d\t\n", step_position);
        HAL_Delay(1);
        Switch_state = FALSE;
        switch_num = Sta2;
      }
    }
    break;
  case Sta2:

    set_location = CalibrationBuf[1];
    Location_RunCondition();
    if (Continuous_MovementMode(set_location, mode_id))
    {
      if (CalibrationBuf[1] == CalibrationBuf[2])
      {
        test_num = Sta0;
        switch_num = Sta0;
      }
      else
      {
        printf("���ζ�λ���\t\n");
        printf("��ǰλ�ã�%d\t\n", step_position);
        HAL_Delay(1);
        Switch_state = FALSE;
        switch_num = Sta3;
      }
    }
    break;
  case Sta3:

    set_location = CalibrationBuf[2];
    Location_RunCondition();
    if (Continuous_MovementMode(set_location, mode_id))
    {
      if (CalibrationBuf[2] == CalibrationBuf[3])
      {
        test_num = Sta0;
        switch_num = Sta0;
      }
      else
      {
        printf("���ζ�λ���\t\n");
        printf("��ǰλ�ã�%d\t\n", step_position);
        HAL_Delay(1);
        Switch_state = FALSE;
        switch_num = Sta4;
      }
    }
    break;
  case Sta4:

    set_location = CalibrationBuf[3];
    Location_RunCondition();
    if (Continuous_MovementMode(set_location, mode_id))
    {
      if (CalibrationBuf[3] == CalibrationBuf[4])
      {
        test_num = Sta0;
        switch_num = Sta0;
      }
      else
      {
        printf("�Ĵζ�λ���\t\n");
        printf("��ǰλ�ã�%d\t\n", step_position);
        HAL_Delay(1);
        Switch_state = FALSE;
        switch_num = Sta5;
      }
    }
    break;
  case Sta5:

    set_location = CalibrationBuf[4];
    Location_RunCondition();
    if (Continuous_MovementMode(set_location, mode_id))
    {
      if (CalibrationBuf[4] == CalibrationBuf[5])
      {
        test_num = Sta0;
        switch_num = Sta0;
      }
      else
      {
        printf("��ζ�λ���\t\n");
        printf("��ǰλ�ã�%d\t\n", step_position);
        HAL_Delay(1);
        Switch_state = FALSE;
        switch_num = Sta6;
      }
    }
    break;
  case Sta6:

    set_location = CalibrationBuf[5];
    Location_RunCondition();
    if (Continuous_MovementMode(set_location, mode_id))
    {
      if (CalibrationBuf[5] == CalibrationBuf[6])
      {
        test_num = Sta0;
        switch_num = Sta0;
      }
      else
      {
        printf("���ζ�λ���\t\n");
        printf("��ǰλ�ã�%d\t\n", step_position);
        HAL_Delay(1);
        Switch_state = FALSE;
        switch_num = Sta7;
      }
    }
    break;
  case Sta7:

    set_location = CalibrationBuf[6];
    Location_RunCondition();
    if (Continuous_MovementMode(set_location, mode_id))
    {
      if (CalibrationBuf[6] == CalibrationBuf[7])
      {
        test_num = Sta0;
        switch_num = Sta0;
      }
      else
      {
        printf("�ߴζ�λ���\t\n");
        printf("��ǰλ�ã�%d\t\n", step_position);
        HAL_Delay(1);
        Switch_state = FALSE;
        switch_num = Sta8;
      }
    }
    break;
  case Sta8:

    set_location = CalibrationBuf[7];
    Location_RunCondition();
    if (Continuous_MovementMode(set_location, mode_id))
    {
      printf("�˴ζ�λ���\t\n");
      printf("��ǰλ�ã�%d\t\n", step_position);
      HAL_Delay(1);
      Switch_state = FALSE;
      test_num = Sta0;
      switch_num = Sta0;
    }
    break;
  default:
    break;
  }
}
