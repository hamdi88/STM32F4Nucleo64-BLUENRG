/**
 ******************************************************************************
 * File Name          : app_x-cube-ble1.c
 * Description        : Implementation file
 *
 ******************************************************************************
 *
 * COPYRIGHT 2019 STMicroelectronics
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_X_CUBE_BLE1_C
#define __APP_X_CUBE_BLE1_C
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "app_x-cube-ble1.h"

#include "hci.h"
#include "hci_le.h"
#include "hci_tl.h"
#include "link_layer.h"
#include "sensor.h"
#include "gatt_db.h"

#include "bluenrg_utils.h"
#include "stm32f4_nucleo_f411re.h"
#include "bluenrg_gap.h"
#include "bluenrg_gap_aci.h"
#include "bluenrg_gatt_aci.h"
#include "bluenrg_hal_aci.h"
#include "sm.h"
#include "stm32f4xx_hal_tim.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private defines -----------------------------------------------------------*/
/**
 * 1 to send environmental and motion data when pushing the user button
 * 0 to send environmental and motion data automatically (period = 1 sec)
 */
#define USE_BUTTON 0
#define JIG1 "79004C9466"
#define JIG2 "79004C6023"

/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
extern AxesRaw_t x_axes;
extern AxesRaw_t g_axes;
extern AxesRaw_t m_axes;
extern AxesRaw_t q_axes;

extern volatile uint8_t set_connectable;
extern volatile int     connected;
extern UART_HandleTypeDef hComHandle[COMn];
extern uint8_t str[64*4];

/* at startup, suppose the X-NUCLEO-IDB04A1 is used */
uint8_t bnrg_expansion_board = IDB04A1; 
uint8_t user_button_init_state;
uint8_t bdaddr[BDADDR_SIZE];
uint8_t volatile UPADATE_JIG_RFID = 0 , ADV_LED =0;
volatile uint8_t jig_rfid [10];
volatile uint8_t JIG_FINISHED = 0 ;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void User_Process(void);
static void User_Init(void);
static void Set_Random_Environmental_Values(float *data_t, float *data_p);
static void Set_Random_Motion_Values(uint32_t cnt);
static void Reset_Motion_Values(void);
static void Set_Random_Address(uint8_t* bdaddr, uint8_t hwVersion, uint16_t fwVersion);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

#if PRINT_CSV_FORMAT
extern volatile uint32_t ms_counter;
/**
 * @brief  This function is a utility to print the log time
 *         in the format HH:MM:SS:MSS (DK GUI time format)
 * @param  None
 * @retval None
 */
void print_csv_time(void){
	uint32_t ms = HAL_GetTick();
	PRINT_CSV("%02d:%02d:%02d.%03d", ms/(60*60*1000)%24, ms/(60*1000)%60, (ms/1000)%60, ms%1000);
}
#endif

void MX_X_CUBE_BLE1_Init(void)
{
	/* USER CODE BEGIN SV */

	/* USER CODE END SV */

	/* USER CODE BEGIN X_CUBE_BLE1_Init_PreTreatment */

	/* USER CODE END X_CUBE_BLE1_Init_PreTreatment */

	/* Initialize the peripherals and the BLE Stack */
	const char *name = "BSCH";
	uint16_t service_handle, dev_name_char_handle, appearance_char_handle;

	uint8_t  hwVersion;
	uint16_t fwVersion;
	int ret;

	User_Init();

	hci_init(user_notify, NULL);

	/* get the BlueNRG HW and FW versions */
	getBlueNRGVersion(&hwVersion, &fwVersion);

	/*
	 * Reset BlueNRG again otherwise we won't
	 * be able to change its MAC address.
	 * aci_hal_write_config_data() must be the first
	 * command after reset otherwise it will fail.
	 */
	hci_reset();
	HAL_Delay(100);

	PRINTF("HWver %d\nFWver %d\n", hwVersion, fwVersion);
	if (hwVersion > 0x30) { /* X-NUCLEO-IDB05A1 expansion board is used */
		bnrg_expansion_board = IDB05A1;
	}

	/*
	 * Change the MAC address to avoid issues with Android
	 * cache if different boards have the same MAC address
	 */
	Set_Random_Address(bdaddr, hwVersion, fwVersion);

	ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET,
			CONFIG_DATA_PUBADDR_LEN,
			bdaddr);
	if (ret) {
		PRINTF("Setting BD_ADDR failed.\n");
	}

	/* GATT Init */
	ret = aci_gatt_init();
	if(ret){
		PRINTF("GATT_Init failed.\n");
	}

	/* GAP Init */
	if (bnrg_expansion_board == IDB05A1) {
		ret = aci_gap_init_IDB05A1(GAP_PERIPHERAL_ROLE_IDB05A1, 0, 0x07, &service_handle, &dev_name_char_handle, &appearance_char_handle);
	}
	else {
		ret = aci_gap_init_IDB04A1(GAP_PERIPHERAL_ROLE_IDB04A1, &service_handle, &dev_name_char_handle, &appearance_char_handle);
	}
	if (ret != BLE_STATUS_SUCCESS) {
		PRINTF("GAP_Init failed.\n");
	}

	/* Update device name */
	ret = aci_gatt_update_char_value(service_handle, dev_name_char_handle, 0,
			strlen(name), (uint8_t *)name);
	if (ret) {
		PRINTF("aci_gatt_update_char_value failed.\n");
		while(1);
	}

	ret = aci_gap_set_auth_requirement(MITM_PROTECTION_REQUIRED,
			OOB_AUTH_DATA_ABSENT,
			NULL,
			7,
			16,
			USE_FIXED_PIN_FOR_PAIRING,
			123456,
			BONDING);
	if (ret) {
		PRINTF("aci_gap_set_authentication_requirement failed.\n");
		while(1);
	}

	PRINTF("BLE Stack Initialized\n");

	//	ret = Add_HWServW2ST_Service();
	//	if(ret == BLE_STATUS_SUCCESS) {
	//		PRINTF("BlueMS HW service added successfully.\n");
	//	} else {
	//		PRINTF("Error while adding BlueMS HW service: 0x%02x\r\n", ret);
	//		while(1);
	//	}
	//
	//	ret = Add_SWServW2ST_Service();
	//	if(ret == BLE_STATUS_SUCCESS) {
	//		PRINTF("BlueMS SW service added successfully.\n");
	//	} else {
	//		PRINTF("Error while adding BlueMS HW service: 0x%02x\r\n", ret);
	//		while(1);
	//	}

	ret = Add_DeviceInfo_Service();
	if(ret == BLE_STATUS_SUCCESS)
	{
		strcat(str, "DEVICEINFO Service added \n\r");
	}
	else
	{
		//strcat(str, "MRBT Service add failed\n\r");
		sprintf(str, "%sDEVICEINFO Service add failed error: 0x%02x\n\r", str, ret);

	}

	ret = Add_MRBT_Service();
	if(ret == BLE_STATUS_SUCCESS)
	{
		strcat(str, "MRBT Service added \n\r");
	}
	else
	{
		//strcat(str, "MRBT Service add failed\n\r");
		sprintf(str, "%sMRBT Service add failed error: 0x%02x\n\r", str, ret);

	}


	/* Set output power level */
	ret = aci_hal_set_tx_power_level(1,4);

	/* USER CODE BEGIN X_CUBE_BLE1_Init_PostTreatment */

	/* USER CODE END X_CUBE_BLE1_Init_PostTreatment */
}

/*
 * BlueNRG-MS background task
 */
void MX_X_CUBE_BLE1_Process(void)
{
	/* USER CODE BEGIN X_CUBE_BLE1_Process_PreTreatment */

	/* USER CODE END X_CUBE_BLE1_Process_PreTreatment */

	User_Process();
	hci_user_evt_proc();

	/* USER CODE BEGIN X_CUBE_BLE1_Process_PostTreatment */

	/* USER CODE END X_CUBE_BLE1_Process_PostTreatment */
}

/**
 * @brief  Initialize User process.
 *
 * @param  None
 * @retval None
 */
static void User_Init(void)
{
	BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);
	BSP_LED_Init(LED2);

	BSP_COM_Init(COM1);
}

/**
 * @brief  Process user input (i.e. pressing the USER button on Nucleo board)
 *         and send the updated acceleration data to the remote client.
 *
 * @param  None
 * @retval None
 */
static void User_Process(void)
{
	float data_t;
	float data_p;
	static uint32_t counter = 0;
	uint8_t my_adv_data_name [6] =  {5, 0x09, 'B', 'S', 'C', 'H'};
	uint8_t my_adv_data_jig  [16] = {15, 0x16, 0x22, 0x22, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41};
	uint8_t my_adv_data_ext  [4] =  {2, 0x02, 0x22};
	uint8_t my_adv_data_all  [] = 	{15, 0x16, 0x22, 0x22, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41};

	//strncpy(my_adv_data_jig+4, JIG2, strlen(JIG2));
	if (set_connectable && UPADATE_JIG_RFID)
	{
		Set_DeviceConnectable();
		set_connectable = FALSE;
		user_button_init_state = BSP_PB_GetState(BUTTON_KEY);
		UPADATE_JIG_RFID = 0 ;
		ADV_LED = 1 ;
		strncpy(my_adv_data_jig+4, jig_rfid, strlen(jig_rfid));
		if (JIG_FINISHED == 1)
		{
			JIG_FINISHED = 0 ;
			my_adv_data_jig[14] = 0x42;
			my_adv_data_jig[15] = 0x42;

		}


		//		if (!aci_gap_update_adv_data(sizeof(my_adv_data_all), my_adv_data_all))
		//		{
		//			PRINTF("UPDATE SUCESS");
		//			strcpy(str, "UPDATE ADV data success!\n\r");
		//		}
		//		else
		//		{
		//			strcpy(str, "UPDATE ADV data failed!\n\r");
		//		}


		//		if (!aci_gap_update_adv_data(sizeof(my_adv_data_name), my_adv_data_name))
		//		{
		//			PRINTF("UPDATE SUCESS");
		//			strcpy(str, "UPDATE ADV_NAME data success!\n\r");
		//		}
		//		else
		//		{
		//			strcpy(str, "UPDATE ADV_NAME data failed!\n\r");
		//		}
		//
		//
		//		if (!aci_gap_update_adv_data(sizeof(my_adv_data_ext), my_adv_data_ext))
		//		{
		//			strcat(str, "UPDATE ADV_EXT  success!\n\r");
		//		}
		//		else
		//		{
		//			strcat(str, "UPDATE ADV_EXT failed!\n\r");
		//		}

		if (!aci_gap_update_adv_data(sizeof(my_adv_data_jig), my_adv_data_jig))
		{
			PRINTF("UPDATE SUCESS");
			strcat(str, "UPDATE ADV_JIG data success!\n\r");
		}
		else
		{
			strcat(str, "UPDATE ADV_JIG data failed!\n\r");
		}
		//BSP_LED_Toggle(LED2);
	}

#if USE_BUTTON  
	/* Check if the user has pushed the button */
	if (BSP_PB_GetState(BUTTON_KEY) == !user_button_init_state)
	{
		while (BSP_PB_GetState(BUTTON_KEY) == !user_button_init_state);
#endif


		if (connected)
		{
			BSP_LED_On(LED2);
			ADV_LED = 0 ;
			/* Set a random seed */
			//			srand(HAL_GetTick());
			//
			//			/* Update emulated Environmental data */
			//			Set_Random_Environmental_Values(&data_t, &data_p);
			//			BlueMS_Environmental_Update((int32_t)(data_p *100), (int16_t)(data_t * 10));
			//
			//			/* Update emulated Acceleration, Gyroscope and Sensor Fusion data */
			//			Set_Random_Motion_Values(counter);
			//			Acc_Update(&x_axes, &g_axes, &m_axes);
			//			Quat_Update(&q_axes);
			//
			//			counter ++;
			//			if (counter == 40) {
			//				counter = 0;
			//				Reset_Motion_Values();
			//			}
#if !USE_BUTTON      
			HAL_Delay(1000); /* wait 1 sec before sending new data */
#endif
		}
		if (ADV_LED && !connected)
		{
			BSP_LED_Toggle(LED2);
		}
		else if ( !ADV_LED && !connected)
		{
			BSP_LED_Off(LED2);

		}
		{

		}
#if USE_BUTTON    
	}
#endif  

	if (UPADATE_JIG_RFID && !connected)
	{
		UPADATE_JIG_RFID = 0 ;
		strncpy(my_adv_data_jig+4, jig_rfid, strlen(jig_rfid));
		if (!aci_gap_update_adv_data(sizeof(my_adv_data_jig), my_adv_data_jig))
		{
			PRINTF("UPDATE SUCESS");
			strcat(str, "UPDATE ADV_JIG data success!\n\r");
		}
		else
		{
			strcat(str, "UPDATE ADV_JIG data failed!\n\r");
		}

	}
}

/**
 * @brief  Set random values for all environmental sensor data
 * @param  float pointer to temperature data
 * @param  float pointer to pressure data
 * @retval None
 */
static void Set_Random_Environmental_Values(float *data_t, float *data_p)
{ 
	*data_t = 27.0 + ((uint64_t)rand()*5)/RAND_MAX;     /* T sensor emulation */
	*data_p = 1000.0 + ((uint64_t)rand()*80)/RAND_MAX; /* P sensor emulation */
}

/**
 * @brief  Set random values for all motion sensor data
 * @param  uint32_t counter for changing the rotation direction
 * @retval None
 */
static void Set_Random_Motion_Values(uint32_t cnt)
{ 
	/* Update Acceleration, Gyroscope and Sensor Fusion data */
	if (cnt < 20) {
		x_axes.AXIS_X +=  (10  + ((uint64_t)rand()*3*cnt)/RAND_MAX);
		x_axes.AXIS_Y += -(10  + ((uint64_t)rand()*5*cnt)/RAND_MAX);
		x_axes.AXIS_Z +=  (10  + ((uint64_t)rand()*7*cnt)/RAND_MAX);
		g_axes.AXIS_X +=  (100 + ((uint64_t)rand()*2*cnt)/RAND_MAX);
		g_axes.AXIS_Y += -(100 + ((uint64_t)rand()*4*cnt)/RAND_MAX);
		g_axes.AXIS_Z +=  (100 + ((uint64_t)rand()*6*cnt)/RAND_MAX);
		m_axes.AXIS_X +=  (3  + ((uint64_t)rand()*3*cnt)/RAND_MAX);
		m_axes.AXIS_Y += -(3  + ((uint64_t)rand()*4*cnt)/RAND_MAX);
		m_axes.AXIS_Z +=  (3  + ((uint64_t)rand()*5*cnt)/RAND_MAX);

		q_axes.AXIS_X -= (100  + ((uint64_t)rand()*3*cnt)/RAND_MAX);
		q_axes.AXIS_Y += (100  + ((uint64_t)rand()*5*cnt)/RAND_MAX);
		q_axes.AXIS_Z -= (100  + ((uint64_t)rand()*7*cnt)/RAND_MAX);
	}
	else {
		x_axes.AXIS_X += -(10  + ((uint64_t)rand()*3*cnt)/RAND_MAX);
		x_axes.AXIS_Y +=  (10  + ((uint64_t)rand()*5*cnt)/RAND_MAX);
		x_axes.AXIS_Z += -(10  + ((uint64_t)rand()*7*cnt)/RAND_MAX);
		g_axes.AXIS_X += -(100 + ((uint64_t)rand()*2*cnt)/RAND_MAX);
		g_axes.AXIS_Y +=  (100 + ((uint64_t)rand()*4*cnt)/RAND_MAX);
		g_axes.AXIS_Z += -(100 + ((uint64_t)rand()*6*cnt)/RAND_MAX);
		m_axes.AXIS_X += -(3  + ((uint64_t)rand()*7*cnt)/RAND_MAX);
		m_axes.AXIS_Y +=  (3  + ((uint64_t)rand()*9*cnt)/RAND_MAX);
		m_axes.AXIS_Z += -(3  + ((uint64_t)rand()*3*cnt)/RAND_MAX);

		q_axes.AXIS_X += (200 + ((uint64_t)rand()*7*cnt)/RAND_MAX);
		q_axes.AXIS_Y -= (150 + ((uint64_t)rand()*3*cnt)/RAND_MAX);
		q_axes.AXIS_Z += (10  + ((uint64_t)rand()*5*cnt)/RAND_MAX);
	}

}

/**
 * @brief  Reset values for all motion sensor data
 * @param  None
 * @retval None
 */
static void Reset_Motion_Values(void)
{
	x_axes.AXIS_X = (x_axes.AXIS_X)%2000 == 0 ? -x_axes.AXIS_X : 10;
	x_axes.AXIS_Y = (x_axes.AXIS_Y)%2000 == 0 ? -x_axes.AXIS_Y : -10;
	x_axes.AXIS_Z = (x_axes.AXIS_Z)%2000 == 0 ? -x_axes.AXIS_Z : 10;
	g_axes.AXIS_X = (g_axes.AXIS_X)%2000 == 0 ? -g_axes.AXIS_X : 100;
	g_axes.AXIS_Y = (g_axes.AXIS_Y)%2000 == 0 ? -g_axes.AXIS_Y : -100;
	g_axes.AXIS_Z = (g_axes.AXIS_Z)%2000 == 0 ? -g_axes.AXIS_Z : 100;
	m_axes.AXIS_X = (g_axes.AXIS_X)%2000 == 0 ? -m_axes.AXIS_X : 3;
	m_axes.AXIS_Y = (g_axes.AXIS_Y)%2000 == 0 ? -m_axes.AXIS_Y : -3;
	m_axes.AXIS_Z = (g_axes.AXIS_Z)%2000 == 0 ? -m_axes.AXIS_Z : 3;
	q_axes.AXIS_X = -q_axes.AXIS_X;
	q_axes.AXIS_Y = -q_axes.AXIS_Y;
	q_axes.AXIS_Z = -q_axes.AXIS_Z;
}

/**
 * @brief  Set a random BLE MAC address
 * @param  Pointer to array where to save the BLE MAC address 
 * @param  Hardware version
 * @param  Firmware version
 * @retval None
 */
static void Set_Random_Address(uint8_t* bdaddr, uint8_t hwVersion, uint16_t fwVersion)
{  
	uint8_t i;

	/* Initialize a random seed */
	srand (HAL_GetTick() + hwVersion + fwVersion);

	for (i=0; i<5; i++) {
		bdaddr[i] = rand()&0xFF;
	}
	bdaddr[i] = 0xD0;
}

#ifdef __cplusplus
}
#endif
#endif /* __APP_X_CUBE_BLE1_C */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
