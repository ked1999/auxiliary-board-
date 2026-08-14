#ifndef __UART_INTERFACE_H
#define __UART_INTERFACE_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "n32g45x.h"
#include "mb.h"

//modbus��������
#define MODBUS_USART           		USART3
#define MODBUS_USART_GPIO       	GPIOB
#define MODBUS_USART_CLK        	RCC_APB1_PERIPH_USART3
#define MODBUS_USART_GPIO_CLK   	RCC_APB2_PERIPH_GPIOB
#define MODBUS_USART_RxPin      	GPIO_PIN_11
#define MODBUS_USART_TxPin      	GPIO_PIN_10
#define MODBUS_USART_APBxClkCmd 	RCC_EnableAPB1PeriphClk
#define MODBUS_USART_IRQn  				USART3_IRQn
#define MODBUS_USART_IRQHandler  	USART3_IRQHandler

//modbus DMA����
#define MODBUS_DMA					     	DMA1
#define MODBUS_DMA_RX_Channel     DMA1_CH3
#define MODBUS_DMA_TX_Channel     DMA1_CH2
#define MODBUS_DMA_TX_REMAP 			DMA1_REMAP_USART3_TX
#define MODBUS_DMA_RX_REMAP 			DMA1_REMAP_USART3_RX
#define MODBUS_DMA_CLK	 					RCC_AHB_PERIPH_DMA1

//DMA��������ж�����
#define MODBUS_DMA_TX_IRQn 				DMA1_Channel2_IRQn
#define MODBUS_DMA_TX_INT_TXC 		DMA1_INT_TXC2
#define MODBUS_DMA_TX_IRQHandler  DMA1_Channel2_IRQHandler

//485������������
#define RS485_DE_GPIO       			GPIOB
#define RS485_DE_Pin      				GPIO_PIN_0
#define RS485_DE_GPIO_CLK   			RCC_APB2_PERIPH_GPIOB

//485���������ʹ��
#define SET_RS485_RX_ENABLE 			GPIO_ResetBits(RS485_DE_GPIO, RS485_DE_Pin)
#define SET_RS485_TX_ENABLE 			GPIO_SetBits(RS485_DE_GPIO, RS485_DE_Pin)
//��ȡ485ʹ������
#define GET_RS485_DE_STATE        GPIO_ReadOutputDataBit(RS485_DE_GPIO,RS485_DE_Pin)

#define UART_TX_BUFFER_SIZE 64
#define UART_RX_BUFFER_SIZE 64

#define MODBUS_RX_MAXBUFF   256 		// ����������󻺳���
#define MODBUS_TX_MAXBUFF   256 		// ����������󻺳���

// PID�洢��ʼ���
#define UdsPID_STARTINDEX   351
#define UqsPID_STARTINDEX   371
#define TorPID_STARTINDEX   391
#define VelPID_STARTINDEX   411
#define HodPID_STARTINDEX   431
#define PosPID_STARTINDEX   451



extern uint8_t modbus_recv_data[MODBUS_RX_MAXBUFF];			// �������ݻ�����
extern uint8_t modbus_send_data[MODBUS_TX_MAXBUFF];			// �������ݻ�����
extern uint32_t modbus_recv_flag;        // ������ɱ�־λ
extern uint32_t modbus_recv_len;         // ���յ����ݳ���
extern uint32_t modbus_send_flag;        // ������ɱ�־λ
extern uint32_t modbus_send_len;         // ���͵����ݳ���


void Modbus_USART_Init(unsigned int baud);
void Modbus_DMA_Init(void);
void Modbus_DMA_ReEnable(DMA_ChannelType* DMA_Channel,uint16_t len);
void Modbus_Respond(MBModify* modify);
void Update_Modbus_RO_Data(void);
void Modbus_Task(MBModify* modify_);
#ifdef __cplusplus
}
#endif

#endif /*__UART_INTERFACE_H */



