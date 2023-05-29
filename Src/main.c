/**
 ******************************************************************************
 * File Name          : main.c
 * Description        : Main program body
 ******************************************************************************
 *
 * COPYRIGHT(c) 2015 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "zlg7290.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
#define ZLG_READ_ADDRESS1 0x01
#define ZLG_READ_ADDRESS2 0x10
#define ZLG_WRITE_ADDRESS1 0x10
#define ZLG_WRITE_ADDRESS2 0x11
#define BUFFER_SIZE1 (countof(Tx1_Buffer))
#define BUFFER_SIZE2 (countof(Rx2_Buffer))
#define countof(a) (sizeof(a) / sizeof(*(a)))

uint8_t flag;	   // 不同的按键有不同的标志位值
uint8_t flag1 = 0; // 中断标志位，每次按键产生一次中断，并开始读取8个数码管的值
uint8_t Rx2_Buffer[8] = {0};
uint8_t Tx1_Buffer[8] = {0};
uint8_t Rx1_Buffer[1] = {0};

//op
uint8_t Rx1_Buffer1[1]={0};
uint8_t Rx1_Buffer2[1]={0};
uint8_t Rx1_Buffer3[1]={0};
uint8_t num1[0x20] = {0, 13, 14, 15, 16, 0, 0, 0, 0, 12, 9, 8, 7, 0, 0, 0,\
					  0, 11, 6, 5, 4, 0, 0, 0, 0, 10, 3, 2, 1, 0, 0, 0};


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void swtich_key(void);
void switch_flag(void);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

// checkSum
typedef struct {
	uint8_t* mArray;
	uint8_t mLen;
	uint8_t checkSumCode;
}checkSumArr;
checkSumArr initCheckSumArrF(uint8_t* marr, uint8_t mlen);
void restoreBackupArray(uint8_t* marr, uint8_t* marrBackup, uint8_t mlen);
void checkStatic();
uint8_t checkSumArrF(checkSumArr mstruct);
uint8_t flagCheckSum;

// restart
uint16_t mflagInit = 0;
void minit();

// music
#define NOTE_DO 762
#define NOTE_RE 680
#define NOTE_MI 606
#define NOTE_FA 572
#define NOTE_SO 510
#define NOTE_LA 454
#define NOTE_SI 404
uint32_t music[4] = {NOTE_DO, NOTE_RE, NOTE_MI, NOTE_FA};
void Delay_us(uint32_t us);
void Note(uint32_t us);

// coffer
#define UNLOGINED 0	// 未登录
#define LOGINED 1	// 已登录
#define CHANGEPASSWORD 2	// 修改密码
#define ADDLOCKCONTAINER 3	// 向保险箱中添加物品
#define OPENLOCKCONTAINER 4	// 打开保险箱

#define FLAG_KEY_STAR 16	// *
#define FLAG_KEY_SHARP 14	// #
#define FLAG_KEY_A 10		// A
#define FLAG_KEY_B 11		// B
#define FLAG_KEY_C 12		// C
#define FLAG_KEY_D 13		// D

#define FLAG_KEY_0 15	// 0
#define FLAG_KEY_1 1	// 1
#define FLAG_KEY_2 2	// 2
#define FLAG_KEY_3 3	// 3
#define FLAG_KEY_4 4	// 4
#define FLAG_KEY_5 5	// 5
#define FLAG_KEY_6 6	// 6
#define FLAG_KEY_7 7	// 7
#define FLAG_KEY_8 8	// 8
#define FLAG_KEY_9 9	// 9

int myPassword[6] = {1, 1, 4, 5, 1, 4};
int tempPassword[6] = {0};
int p = 0;
int state = 0;
int noe = 0;
int mySecret = 6;
uint8_t check_sum = 0;

int checkPassword();
void setPassword();
void myReset();
void clearTmp();
void displayHelp();
uint8_t calculateCheckSum(uint8_t *data, uint8_t len, uint8_t start_sum);
uint8_t calculateAllCheckSum();	// TODO

//LED
uint8_t PASS_Buffer[8] = {0xCE, 0xEE, 0xB6, 0xB6, 0, 0, 0, 0};
uint8_t ERROR_Buffer[8] = {0x9E, 0xEE, 0xEE, 0xFC, 0xEE, 0, 0, 0};
//unsigned char seg7code[10]={ 0xFC,0x0C,0xDA,0xF2,0x66,0xB6,0xBE,0xE0,0xFE,0xE6}; //数码管字根
uint8_t ZERO_Buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
void ledPass();
void ledError();
void ledClean();

//hot start
void hotStart();


//backup
uint8_t num1Backup[0x20] = {0, 13, 14, 15, 16, 0, 0, 0, 0, 12, 9, 8, 7, 0, 0, 0,\
					  0, 11, 6, 5, 4, 0, 0, 0, 0, 10, 3, 2, 1, 0, 0, 0};
uint8_t PASS_BufferBackup[8] = {0xCE, 0xEE, 0xB6, 0xB6, 0, 0, 0, 0};
uint8_t ERROR_BufferBackup[8] = {0x9E, 0xEE, 0xEE, 0xFC, 0xEE, 0, 0, 0};
//unsigned char seg7codeBackup[10]={ 0xFC,0x0C,0xDA,0xF2,0x66,0xB6,0xBE,0xE0,0xFE,0xE6}; //数码管字根
uint8_t ZERO_BufferBackup[8] = {0, 0, 0, 0, 0, 0, 0, 0};


int main(void)
{

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	// ++
	HAL_Delay(200);
	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_I2C1_Init();
	MX_USART1_UART_Init();

	// checksum
	checkSumArr mArrNum1 = initCheckSumArrF(num1, 32);
	checkSumArr mArrNum1Backup = initCheckSumArrF(num1Backup, 32);
	checkSumArr mArrPASS_Buffer = initCheckSumArrF(PASS_Buffer, 8);
	checkSumArr mArrPASS_BufferBackup = initCheckSumArrF(PASS_BufferBackup, 8);
	checkSumArr mArrERROR_Buffer = initCheckSumArrF(ERROR_Buffer, 8);
	checkSumArr mArrERROR_BufferBackup = initCheckSumArrF(ERROR_BufferBackup, 8);
	checkSumArr mArrZERO_Buffer = initCheckSumArrF(ZERO_Buffer, 8);
	checkSumArr mArrZERO_BufferBackup = initCheckSumArrF(ZERO_BufferBackup, 8);
	flagCheckSum = 2;

	ledClean();
	/* USER CODE BEGIN 2 */
	printf("\n\r");
	printf("\n\r-------------------------------------------------\r\n");
	printf("\n\r 音乐保险箱测试例程 \r\n");
	printf("\n请输入密码:\n");
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		if (flag1 == 1)
		{
			flag1 = 0;
			mflagInit = 0; // 用户有输入就不会记时重启

			I2C_ZLG7290_Read(&hi2c1,0x71,0x01,Rx1_Buffer1,1);//读键值1
			I2C_ZLG7290_Read(&hi2c1,0x71,0x01,Rx1_Buffer2,1);//读键值2
			I2C_ZLG7290_Read(&hi2c1,0x71,0x01,Rx1_Buffer3,1);//读键值3
			//printf("\n\r111按键键值 = %#x\r\n\n\r222按键键值 = %#x\r\n\n\r333按键键值 = %#x\r\n",Rx1_Buffer1[0],Rx1_Buffer2[0],Rx1_Buffer3[0]);
			if(Rx1_Buffer1[0] != Rx1_Buffer2[0] && Rx1_Buffer2[0] != Rx1_Buffer3[0] && Rx1_Buffer1[0] != Rx1_Buffer3[0])
			{
				goto endNow;
			}
			else if(Rx1_Buffer2[0] == Rx1_Buffer3[0]){
				Rx1_Buffer[0] = Rx1_Buffer2[0];
			}
			else
			{
					Rx1_Buffer[0] = Rx1_Buffer1[0];
			}

			//校验码检验模块
			if(flagCheckSum++ == 12) {
				checkStatic();
				flagCheckSum = 2;
			}

			//I2C_ZLG7290_Read(&hi2c1, 0x71, 0x01, Rx1_Buffer, 1); // 读键值	FIXME 读取多次
			printf("\n\r按键键值 = %#x\r\n", Rx1_Buffer[0]); // 想串口发送键值
			swtich_key();									 // 扫描键值，写标志位

			switch(state){
				case UNLOGINED:	// 未登录状态，只能输入密码，重置建回到这个状态，确定键进入判断
					switch(flag){
						case FLAG_KEY_STAR:	// 重置键 *
							myReset();
							printf("重置完成,请从第一位开始重新输入密码：\n");
							break;
						case FLAG_KEY_SHARP:	// 确认键 #
							if(checkPassword()){
								state = LOGINED;
								printf("密码正确，已登录\n");
								ledPass();
								for (uint8_t i = 0; i < 3; i++)
								{
									Note(music[i]);
								}
								clearTmp();
							}
							else{
								printf("密码错误，请重新输入\n");
								clearTmp();
								ledError();
								for (uint8_t i = 0; i < 3; i++)
							{
								Note(music[2]);
							}
								// p = 0;
							}
							p = 0;	// 重置密码输入位数
							break;
						case FLAG_KEY_A:	// 退格键 A
							if(p > 0){
								p--;	// 退格
								tempPassword[p] = 0;
								printf("已输入密码前%d位数\n当前密码为: ", p);
								for (uint8_t i = 0; i < p; i++)
								{
									printf("%d", tempPassword[i]);
								}
								printf("\n");
							}
							else{
								printf("已经退到最前面了\n");
							}
							break;
						case FLAG_KEY_B:	
						case FLAG_KEY_C:
						case FLAG_KEY_D:
							break;
						default:	// 数字键
						if(p >=6){
								p=5;
								printf("more than 6\n");
							}
							tempPassword[p] = flag;
							p++;
							printf("已输入密码前%d位数\n当前密码为: ", p);
							for (uint8_t i = 0; i < p; i++)
							{
								printf("%d", tempPassword[i]);
							}
							printf("\n");
							break;
						}
						break;
				case LOGINED:	// 登录状态，可以输入密码，重置键回到未登录状态，确认键进入修改密码状态
					switch(flag){
						case FLAG_KEY_1: // 输出帮助
							displayHelp();
							break;
						case FLAG_KEY_2:	// 修改密码
							state = CHANGEPASSWORD;
							printf("进入修改密码状态，需要输入6位密码，输入完成后按#确认\n");
							clearTmp();
							break;
						case FLAG_KEY_3:	// 打开保险箱
						case FLAG_KEY_4:	// 关闭保险箱
							break;
						case FLAG_KEY_5:	// 查询密码
							printf("当前密码为: ");
							for(uint8_t i = 0; i < 6; i++)
							{
								printf("%d", myPassword[i]);
							}
							printf("\n");
							break;
						case FLAG_KEY_6:	// 查询保险箱状态
						case FLAG_KEY_7:	// 查询保险箱内物品
						case FLAG_KEY_8:	// 放入物品
						case FLAG_KEY_9:	// 取出物品
							break;
						case FLAG_KEY_0:	// 退出登录
							state = UNLOGINED;
							myReset();
							printf("已退出登录\n");
							for(uint8_t i = 0; i < 3; i++)
							{
								Note(music[3-i]);
							}
						case FLAG_KEY_STAR:	// 重置键
							myReset();
							printf("重置完成,请从第一位开始重新输入密码：\n");
						  	clearTmp();
							break;
						default:
							printf("输入错误，请重新输入\n");
							clearTmp();
						}
						break;
				case CHANGEPASSWORD:	// 修改密码状态，可以输入密码，重置键回到未登录状态，确认键进入登录状态
					switch(flag){
					case FLAG_KEY_STAR:	// 重置键
						myReset();
						printf("重置完成,请从第一位开始重新输入密码：\n");
					  	clearTmp();
						break;
					case FLAG_KEY_SHARP:	// 确认键,如果位数不够6位，不会修改密码
						if(p == 6){
							setPassword();
							printf("密码修改完成\n");
							ledClean();
							for (uint8_t i = 0; i < 3; i++)
			  				{
			  					Note(music[i]);
			  				}
							state = UNLOGINED;
							clearTmp();
						}
						else{
							printf("密码位数不够6位，请重新输入\n");
							clearTmp();
			
						}
						p = 0;	// 重置密码输入位数
						break;
					case FLAG_KEY_B: // B 退出回到登录状态
						state = LOGINED;
						printf("退出修改密码状态，已进入登录状态\n");
						clearTmp();
						p = 0;	// 重置密码输入位数
						displayHelp();
						break;
					case FLAG_KEY_A:	// 退格键 A
						if(p > 0){
							p--;	// 退格
							tempPassword[p] = 0;
							printf("已输入密码前%d位数\n当前密码为: ", p);
							for (uint8_t i = 0; i < p; i++)
							{
								printf("%d", tempPassword[i]);
							}
							printf("\n");
						}
						else{
							printf("已经退到最前面了\n");
						}
						break;
					default:	// 数字键
						if(p>=6){
							printf("more than 6\n");
							p=5;
						}
						tempPassword[p] = flag;
						p++;
						printf("已输入密码前%d位数\n当前密码为: ", p);
						for (uint8_t i = 0; i < p; i++)
						{
							printf("%d", tempPassword[i]);
						}
						printf("\n");
						break;
					}
			    		break;
				case ADDLOCKCONTAINER:	// 添加物品
						break;
			}
			//I2C_ZLG7290_Read(&hi2c1, 0x71, 0x10, Rx2_Buffer, 8); // 读8位数码管
			//switch_flag();	// 扫描到相应的按键并且向数码管写进数值
			endNow:		

		}

		//计时重启模块
		HAL_Delay(1);
		//mflagInit++;
		//printf("%d\n", mflagInit);
		if(++mflagInit == 65535){
			printf("time to restart\n\n");
			minit();
			printf("restart completed\n\n");
		}
	}
		
	/* USER CODE END 3 */
}

uint8_t calculateCheckSum(uint8_t *data, uint8_t len, uint8_t start_sum)
{
	for(uint8_t i = 0; i < len; i++)	// 异或校验
	{
		start_sum ^= data[i];
	}
	return start_sum;
}
uint8_t calculateAllCheckSum()
{
	uint8_t check_sum = 0;
}
void displayHelp(){
	printf("当前为登录状态\n\n");
	printf("输入1：输出帮助\n\n");
	printf("输入2：修改密码\n\n");
	printf("输入3：打开保险箱\n\n");
	printf("输入4：关闭保险箱\n\n");
	printf("输入5：查询密码\n\n");
	printf("输入6：查询保险箱状态\n\n");
	printf("输入7：查询保险箱内物品 \n\n");
	printf("输入8：放入物品 \n\n");
	printf("输入9：取出物品 \n\n");
	printf("输入10：退出登录\n");
}
// coffer
int checkPassword()
{
	int i = 0;
	for (i = 0; i < 6; i++)
	{
		if (myPassword[i] != tempPassword[i])
		{
			return 0;
		}
	}
	return 1;
}

void setPassword()
{
	int i = 0;
	for(; i < 6; i++)
	{
		myPassword[i] = tempPassword[i];
	}
}
void clearTmp(){
	int i = 0;
	for(; i < 6; i++)
	{
		tempPassword[i] = 0;
	}
}

void myReset()
{
	state = 0;
	p = 0;
	clearTmp();
	ledClean();
}

// LED
void ledPass(){
	I2C_ZLG7290_Write(&hi2c1,0x70,ZLG_WRITE_ADDRESS1,PASS_Buffer,8);	
}

void ledError(){
	I2C_ZLG7290_Write(&hi2c1,0x70,ZLG_WRITE_ADDRESS1,ERROR_Buffer,8);	
}

void ledClean(){
	I2C_ZLG7290_Write(&hi2c1,0x70,ZLG_WRITE_ADDRESS1,ZERO_Buffer,8);
}

// checkSum
checkSumArr initCheckSumArrF(uint8_t* marr, uint8_t mlen){
	uint8_t i;
	uint8_t	msum = 0;
	for(i = 0; i < mlen; i++) {
		msum ^= marr[i];
	}
	checkSumArr mstruct;
	mstruct.mArray = marr;
	mstruct.mLen = mlen;
	mstruct.checkSumCode = msum;
	return mstruct;
}

uint8_t checkSumArrF(checkSumArr mstruct){
	uint8_t i, mflag;
	uint8_t	msum = 0;
	for(i = 0; i < mstruct.mLen; i++) {
		msum ^= mstruct.mArray[i];
	}
	mflag = msum == mstruct.checkSumCode ? 0 : 1;
	return mflag;
}

void restoreBackupArray(uint8_t* marr, uint8_t* marrBackup, uint8_t mlen){
	uint8_t i = 0;
	for(i = 0; i < mlen; i++) {
		marr[i] = marrBackup[i];
	}
}

void checkStatic(){
	flagCheckSum = checkSumArrF(mArrNum1);
	if (flagCheckSum == 1){
		printf("checkSumErrorMArr1");
		restoreBackupArray(num1, num1Backup, 32);
	}
	flagCheckSum = checkSumArrF(mArrNum1Backup);
	if (flagCheckSum == 1){
		printf("checkSumErrorMArr1");
		restoreBackupArray(num1Backup, num1, 32);
	}
	flagCheckSum = checkSumArrF(mArrPASS_Buffer);
	if (flagCheckSum == 1){
		printf("checkSumErrorMArr1");
		restoreBackupArray(PASS_Buffer, PASS_BufferBackup, 8);
	}
	flagCheckSum = checkSumArrF(mArrPASS_BufferBackup);
	if (flagCheckSum == 1){
		printf("checkSumErrorMArr1");
		restoreBackupArray(PASS_BufferBackup, PASS_Buffer, 8);
	}
	flagCheckSum = checkSumArrF(mArrERROR_Buffer);
	if (flagCheckSum == 1){
		printf("checkSumErrorMArr1");
		restoreBackupArray(ERROR_Buffer, ERROR_BufferBackup, 8);
	}
	flagCheckSum = checkSumArrF(mArrERROR_BufferBackup);
	if (flagCheckSum == 1){
		printf("checkSumErrorMArr1");
		restoreBackupArray(ERROR_BufferBackup, ERROR_Buffer, 8);
	}
	flagCheckSum = checkSumArrF(mArrZERO_Buffer);
	if (flagCheckSum == 1){
		printf("checkSumErrorMArr1");
		restoreBackupArray(ZERO_Buffer, ZERO_BufferBackup, 8);
	}
	flagCheckSum = checkSumArrF(mArrZERO_BufferBackup);
	if (flagCheckSum == 1){
		printf("checkSumErrorMArr1");
		restoreBackupArray(ZERO_BufferBackup, ZERO_Buffer, 8);
	}
}

// restart
void minit(){
	HAL_Init();
	HAL_Delay(200);

  /* Configure the system clock */
  //SystemClock_Config();

  /* Initialize all configured peripherals */
  	MX_GPIO_Init();
	MX_I2C1_Init();
	MX_USART1_UART_Init();

	ledClean();
	mflagInit = 0;
	flagCheckSum = 2;
}

// music
void Delay_us(uint32_t us)
{
	uint32_t Delay = us * 168 / 4;
	do
	{
		__NOP();
	} while (Delay--);
}
void Note(uint32_t us)
{
	uint8_t wait = 0;
	while (wait++ < 250)
	{
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);
		Delay_us(us);
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
		Delay_us(us);
	}
}

//hot start
void hotStart(){
	minit();
	//todo

}

/** System Clock Configuration
 */
void SystemClock_Config(void)
{

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	__PWR_CLK_ENABLE();

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */
void swtich_key(void)	// 使用buf向switch 赋值
{
	//flag = num1[Rx1_Buffer[0]];
	switch (Rx1_Buffer[0])
	{
	//case 0x03: // 0
		//flag = 0;
		//break;
	case 0x1C:	
		flag = 1;
		break;
	case 0x1B:
		flag = 2;
		break;
	case 0x1A:
		flag = 3;
		break;
	case 0x14:
		flag = 4;
		break;
	case 0x13:
		flag = 5;
		break;
	case 0x12:
		flag = 6;
		break;
	case 0x0C:
		flag = 7;
		break;
	case 0x0B:
		flag = 8;
		break;
	case 0x0A:
		flag = 9;
		break;
	case 0x03:
		flag = 15;
		break;
	case 0x19:
		flag = 10;
		break;
	case 0x11:
		flag = 11;
		break;
	case 0x09:
		flag = 12;
		break;
	case 0x01:
		flag = 13;
		break;
	case 0x02:
		flag = 14;
		break;
	case 0x04:	// *
		flag = 16;
		break;
	default:
		break;
	}
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	flag1 = 1;
}
int fputc(int ch, FILE *f)
{
	uint8_t tmp[1] = {0};
	tmp[0] = (uint8_t)ch;
	HAL_UART_Transmit(&huart1, tmp, 1, 10);
	return ch;
}
/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
 * @brief Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}

#endif

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
