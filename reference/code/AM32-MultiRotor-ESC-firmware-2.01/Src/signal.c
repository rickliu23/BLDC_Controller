/*
 * IO.c
 *
 *  Created on: Sep. 26, 2020
 *      Author: Alka
 */

#include "targets.h"
#include "signal.h"
#include "IO.h"
#include "dshot.h"
#include "serial_telemetry.h"
#include "functions.h"
#include "sounds.h"
#include "common.h"

int max_servo_deviation = 200;
int servorawinput;

uint8_t enter_calibration_count = 0;
uint8_t calibration_required = 0;
uint8_t high_calibration_counts = 0;
uint8_t high_calibration_set = 0;
uint16_t last_high_threshold = 0;
uint8_t low_calibration_counts = 0;
uint16_t last_input = 0;

void computeMSInput()
{

	int lastnumber = dma_buffer[0];
	for (int j = 1; j < 2; j++)
	{

		if (((dma_buffer[j] - lastnumber) < 1500) && ((dma_buffer[j] - lastnumber) > 0))
		{ // blank space

			newinput = map((dma_buffer[j] - lastnumber), 243, 1200, 0, 2000);
			break;
		}
		lastnumber = dma_buffer[j];
	}
}

void computeServoInput()
{

	if (((dma_buffer[1] - dma_buffer[0]) > 800) && ((dma_buffer[1] - dma_buffer[0]) < 2200))
	{
		if (calibration_required)
		{
			if (!high_calibration_set)
			{
				if (high_calibration_counts == 0)
				{
					last_high_threshold = dma_buffer[1] - dma_buffer[0];
				}
				high_calibration_counts++;
				if (getAbsDif(last_high_threshold, servo_high_threshold) > 50)
				{
					calibration_required = 0;
				}
				else
				{
					servo_high_threshold = ((7 * servo_high_threshold + (dma_buffer[1] - dma_buffer[0])) >> 3);
					if (high_calibration_counts > 50)
					{
						servo_high_threshold = servo_high_threshold - 25;
						eepromBuffer[33] = (servo_high_threshold - 1750) / 2;
						high_calibration_set = 1;
						playDefaultTone();
					}
				}
				last_high_threshold = servo_high_threshold;
			}
			if (high_calibration_set)
			{
				if (dma_buffer[1] - dma_buffer[0] < 1250)
				{
					low_calibration_counts++;
					servo_low_threshold = ((7 * servo_low_threshold + (dma_buffer[1] - dma_buffer[0])) >> 3);
				}
				if (low_calibration_counts > 75)
				{
					servo_low_threshold = servo_low_threshold + 25;
					eepromBuffer[32] = (servo_low_threshold - 750) / 2;
					calibration_required = 0;
					saveEEpromSettings();
					low_calibration_counts = 0;
					playChangedTone();
				}
			}
			signaltimeout = 0;
		}
		else
		{
			if (bi_direction)
			{
				if (dma_buffer[1] - dma_buffer[0] <= servo_neutral)
				{
					servorawinput = map((dma_buffer[1] - dma_buffer[0]), servo_low_threshold, servo_neutral, 0, 1000);
				}
				else
				{
					servorawinput = map((dma_buffer[1] - dma_buffer[0]), servo_neutral + 1, servo_high_threshold, 1001, 2000);
				}
			}
			else
			{
				servorawinput = map((dma_buffer[1] - dma_buffer[0]), servo_low_threshold, servo_high_threshold, 47, 2047);
				if (servorawinput == 47)
				{
					servorawinput = 0;
				}
			}
			signaltimeout = 0;
		}
	}
	else
	{
		zero_input_count = 0; // reset if out of range
	}

#ifdef SLOW_RAMP_DOWN
	if (forward)
	{
		if ((servorawinput - newinput) > max_servo_deviation)
		{
			newinput += max_servo_deviation;
		}
		else if ((newinput - servorawinput) > (max_servo_deviation >> 2))
		{
			newinput -= (max_servo_deviation >> 2);
		}
		else
		{
			newinput = servorawinput;
		}
	}
	else
	{
		if ((servorawinput - newinput) > max_servo_deviation >> 2)
		{
			newinput += max_servo_deviation >> 2;
		}
		else if ((newinput - servorawinput) > (max_servo_deviation))
		{
			newinput -= (max_servo_deviation);
		}
		else
		{
			newinput = servorawinput;
		}
	}
#else
	if ((servorawinput - newinput) > max_servo_deviation)
	{
		newinput += max_servo_deviation;
	}
	else if ((newinput - servorawinput) > max_servo_deviation)
	{
		newinput -= max_servo_deviation;
	}
	else
	{
		newinput = servorawinput;
	}
#endif
}

// 外部输入信号捕获完成
void transfercomplete()
{
	if (armed && dshot_telemetry) // 已解锁且双向
	{
		if (out_put)
		{
			// 发送完毕，切换回接受
			receiveDshotDma();

			return;
		}
		else
		{
			// 先发送上一帧数据，再准备当前帧数据？
			sendDshotDma();
			make_dshot_package();

			computeDshotDMA(); // 解码飞控发过来的 DShot 命令

			return; // 直接返回
		}
	}

	if (inputSet == 0) // 还不知道是什么协议格式
	{
		detectInput();	   // 分析协议
		receiveDshotDma(); // 启动下一次DMA接收
		return;
	}

	if (inputSet == 1) // 已经知道是什么协议
	{
		if (dshot_telemetry) // 双向协议
		{
			if (out_put) // 当前是输出模式
			{
				//    	TIM17->CNT = 0;
				make_dshot_package(); // this takes around 10us !!
				computeDshotDMA();	  // this is slow too..
				receiveDshotDma();	  // holy smokes.. reverse the line and set up dma again
				return;
			}
			else
			{
				sendDshotDma();
				return;
			}
		}
		else // 单向协议
		{
			if (dshot == 1) // DShot协议
			{
				computeDshotDMA(); // 解码飞控发过来的 DShot 命令
				if (send_telemetry)
				{
					// done in 10khz routine
				}
				receiveDshotDma(); // 继续接收
			}

			if (servoPwm == 1) // servoPwm协议
			{
				// 确保当前 PWM 脉冲已经结束，避免在引脚还是高电平时重新配置定时器，导致捕获错误
				while ((INPUT_PIN_PORT->IDR & INPUT_PIN))
				{ // if the pin is high wait
				}

				// 计算脉宽
				computeServoInput();

				// 重置捕获极性为上升沿
				LL_TIM_IC_SetPolarity(IC_TIMER_REGISTER, IC_TIMER_CHANNEL, LL_TIM_IC_POLARITY_RISING); // setup rising pin trigger.

				// 重新启动DMA接收
				receiveDshotDma();

				// 使能半传输中断
				// 第一次捕获到上升沿时，DMA 搬完 1 个数据，触发半传输中断。
				// 中断里把极性切换成 下降沿触发。
				// 第二次捕获到下降沿时，DMA 搬完第 2 个数据，触发传输完成中断，进入 transfercomplete()。
				LL_DMA_EnableIT_HT(DMA1, INPUT_DMA_CHANNEL);
			}
		}

		if (!armed) // 还没解锁
		{
			if (adjusted_input < 0)
			{
				adjusted_input = 0;
			}
			if (adjusted_input == 0 && calibration_required == 0)
			{ // note this in input..not newinput so it will be adjusted be main loop
				zero_input_count++;
			}
			else
			{
				zero_input_count = 0;
				if (adjusted_input > 1500)
				{
					if (getAbsDif(adjusted_input, last_input) > 50)
					{
						enter_calibration_count = 0;
					}
					else
					{
						enter_calibration_count++;
					}

					if (enter_calibration_count > 50 && (!high_calibration_set))
					{
						playBeaconTune3();
						calibration_required = 1;
						enter_calibration_count = 0;
					}
					last_input = adjusted_input;
				}
			}
		}
	}
}
