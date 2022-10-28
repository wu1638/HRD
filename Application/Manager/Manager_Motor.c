/**
 ******************************************************************************
 * 文件名程: Mannger_Motor.c
 * 作    者: xiao
 * 版    本: V1.0
 * 编写日期: 2022-10-12
 * 功    能: 管理步进电机控制接口
 ******************************************************************************
 */

#include "Manager.h"

#define SlowSpeed 200

extern __IO int32_t step_position; // 当前位置
int32_t CalibrationBuf[8];
int32_t Position_Init = 0, set_location = 0, Issue_TarGetLocation = 0;

int16_t Test_TarGet = 0;
uint8_t entry_count = 0, Trigger_run = 0, Switch_state = 0, switch_num = 0, test_num = 0;

void Manager_Motor_Init(void);     //电机控制初始化
void Manager_Motor_Mainloop(void); //电机控制主循环应用

void MotorRun_perform(void);                  //上位逻辑执行电机
void Entry_CalibrationLocation_Perfrom(void); //执行录入标定位置

uint8_t Continuous_MovementMode(int32_t location, uint8_t mode_id); //模式连续运行切换
void Calibration_LocationSwitch(uint8_t mode_id);                   //位置标定流程

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
    printf("当前值：%d\t\n", step_position);
    printf("设置值：%d\t\n", Issue_TarGetLocation);
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

/*模式定位完成标志处理*/
uint8_t PosCompleteMark(int32_t Location, uint8_t mode_id)
{
  uint8_t CompleteState = FALSE;

  if (mode_id == relative_mode)
  {
    /* 相对定位完成标志  */
    if (Location == (step_position - Position_Init))
    {
      CompleteState = TRUE;
    }
  }
  else if (mode_id == absolute_mode)
  {
    /* 绝对定位完成标志 */
    if (Position_Init && Location == step_position)
    {
      CompleteState = TRUE;
    }
  }
  return CompleteState;
}

/*设置目标位置双字组码*/
int32_t DoubleWord_GroupCode(uint16_t Hdata, uint16_t Ldata)
{
  return (Hdata << 16 | Ldata);
}

/*电机运动模式*/
void MotorRun_perform(void)
{
  /*设置运动目标位置处理*/
  Issue_TarGetLocation = DoubleWord_GroupCode(Mb_Tcpbuf.ServoTargetLocationSetH, Mb_Tcpbuf.ServoTargetLocationSetL);
  /*运行模式判断*/
  if (Mb_Tcpbuf.ServoModeRun == SeekOrigin_mode)
  {
    /* 寻找原点模式 */
    if (Mb_Tcpbuf.ServoWriteData == TRUE)
    {
      DB_HoldingBuf[56] = FALSE;
      STEPMOTOR_AxisHome(Mb_Tcpbuf.ServoModeSpeedSet, SlowSpeed, Mb_Tcpbuf.ServoAddSpeedSet, Mb_Tcpbuf.ServoReduceSpeedSet);
    }
  }
  else if (Mb_Tcpbuf.ServoModeRun == relative_mode)
  {
    /* 相对运动模式 */
    if (Mb_Tcpbuf.ServoWriteData == TRUE)
    {
      DB_HoldingBuf[56] = FALSE;
      Position_Init = step_position;
      STEPMOTOR_AxisMoveRel(Issue_TarGetLocation, Mb_Tcpbuf.ServoAddSpeedSet, Mb_Tcpbuf.ServoReduceSpeedSet, Mb_Tcpbuf.ServoModeSpeedSet);
      /*Trigger_run = 0;
      switch_num = Sta1;*/
    }
    /*寄存器第4位定位完成标志*/
    if (PosCompleteMark(Issue_TarGetLocation, relative_mode))
    {
      Mb_Tcpbuf.Servo_Feedback = Write_WordManage(TRUE, Mb_Tcpbuf.Servo_Feedback, 3);
    }
    else
    {
      Mb_Tcpbuf.Servo_Feedback = Write_WordManage(FALSE, Mb_Tcpbuf.Servo_Feedback, 3);
    }

    /*标定流程*/
    // Calibration_LocationSwitch(relative_mode);
  }
  else if (Mb_Tcpbuf.ServoModeRun == absolute_mode)
  {
    /* 绝对运动模式 */
    if (Mb_Tcpbuf.ServoWriteData == TRUE)
    {
      DB_HoldingBuf[56] = FALSE;
      Position_Init = TRUE;
      STEPMOTOR_AxisMoveAbs(Issue_TarGetLocation, Mb_Tcpbuf.ServoAddSpeedSet, Mb_Tcpbuf.ServoReduceSpeedSet, Mb_Tcpbuf.ServoModeSpeedSet);
      /*Trigger_run = 0;
      switch_num = Sta1;*/
    }
    /*寄存器第4位定位完成标志*/
    if (PosCompleteMark(Issue_TarGetLocation, absolute_mode))
    {
      Mb_Tcpbuf.Servo_Feedback = Write_WordManage(TRUE, Mb_Tcpbuf.Servo_Feedback, 3);
    }
    else
    {
      Mb_Tcpbuf.Servo_Feedback = Write_WordManage(FALSE, Mb_Tcpbuf.Servo_Feedback, 3);
    }
    /*标定流程*/
    // Calibration_LocationSwitch(absolute_mode);
  }
  else if (Mb_Tcpbuf.ServoModeRun == home && !Mb_Tcpbuf.ServoJogRun)
  {
    /* 电机待机停止/急停 */
    STEPMOTOR_StandbyStop();
    Mb_Tcpbuf.Servo_Feedback = Write_WordManage(FALSE, Mb_Tcpbuf.Servo_Feedback, 3);
  }

  /*电机正反向点动运行*/
  if (!Mb_Tcpbuf.ServoModeRun)
  {
    STEMOTOR_JOG(Mb_Tcpbuf.ServoJogRun, Mb_Tcpbuf.ServoJogSpeedSet, Mb_Tcpbuf.ServoAddSpeedSet, Mb_Tcpbuf.ServoReduceSpeedSet);
  }
}

/*录入标定位置*/
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

/*录入标定位置执行条件*/
void Entry_CalibrationLocation_Perfrom(void)
{
  /*标定位置双字高低字*/
  for (uint8_t i = 0; i < 8; i++)
  {
    *(&(Mb_Tcpbuf.CalibrationLocation1H_FeedBack) + i * 2) = (CalibrationBuf[i] >> 16) & 0xffff;
    *(&(Mb_Tcpbuf.CalibrationLocation1L_FeedBack) + i * 2) = CalibrationBuf[i] & 0xffff;
  }
  /*录入位置执行*/
  if (Mb_Tcpbuf.ServoWriteData == ConState2H)
  {
    DB_HoldingBuf[56] = FALSE;
    entry_Calibration_location(step_position, CalibrationBuf, 8);
  }
}

/*模式连续运动切换*/
uint8_t Continuous_MovementMode(int32_t location, uint8_t mode_id)
{
  uint8_t complete = 0;

  if (mode_id == relative_mode)
  {
    /* 连续相对运动 */
    if (!Trigger_run)
    {
      Position_Init = step_position;
      STEPMOTOR_AxisMoveRel(location, Mb_Tcpbuf.ServoAddSpeedSet, Mb_Tcpbuf.ServoReduceSpeedSet, Mb_Tcpbuf.ServoModeSpeedSet);
      Trigger_run = TRUE;
    }
    if (location == (step_position - Position_Init))
    {
      /* 定位完成标志 */
      complete = TRUE;
    }
  }
  else if (mode_id == absolute_mode)
  {
    /* 连续绝对运动 */
    if (!Trigger_run)
    {
      STEPMOTOR_AxisMoveAbs(location, Mb_Tcpbuf.ServoAddSpeedSet, Mb_Tcpbuf.ServoReduceSpeedSet, Mb_Tcpbuf.ServoModeSpeedSet);
      Trigger_run = TRUE;
    }
    if (location == step_position)
    {
      /* 定位完成标志 */
      complete = TRUE;
    }
  }

  return complete;
}

/*触发运行条件*/
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
/*多位置标定流程运行*/
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
        printf("一次定位完成\t\n");
        printf("当前位置：%d\t\n", step_position);
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
        printf("二次定位完成\t\n");
        printf("当前位置：%d\t\n", step_position);
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
        printf("三次定位完成\t\n");
        printf("当前位置：%d\t\n", step_position);
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
        printf("四次定位完成\t\n");
        printf("当前位置：%d\t\n", step_position);
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
        printf("五次定位完成\t\n");
        printf("当前位置：%d\t\n", step_position);
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
        printf("六次定位完成\t\n");
        printf("当前位置：%d\t\n", step_position);
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
        printf("七次定位完成\t\n");
        printf("当前位置：%d\t\n", step_position);
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
      printf("八次定位完成\t\n");
      printf("当前位置：%d\t\n", step_position);
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
