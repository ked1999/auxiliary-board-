#include "canfliter_init.h"



void canfilter_init()
{
    CAN_FilterTypeDef canfilter1;
		CAN_FilterTypeDef canfilter2;

    /* CAN1 Filter 0 Configuration*/
   canfilter1.FilterIdHigh = 0x0000;
   canfilter1.FilterIdLow  = 0x0000;
   canfilter1.FilterMaskIdHigh = 0x0000;
   canfilter1.FilterMaskIdLow  =  0x0000;
    canfilter1.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    canfilter1.FilterBank = 0;
    canfilter1.FilterMode = CAN_FILTERMODE_IDMASK;
    canfilter1.FilterScale = CAN_FILTERSCALE_16BIT;
    canfilter1.FilterActivation = ENABLE;
    canfilter1.SlaveStartFilterBank = 14;

    HAL_CAN_ConfigFilter(&hcan1,&canfilter1);
	  HAL_CAN_Start(&hcan1);
    HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);

	
    canfilter2.FilterIdHigh = 0x0000;
    canfilter2.FilterIdLow  = 0x0000;
    canfilter2.FilterMaskIdHigh = 0x0000;
    canfilter2.FilterMaskIdLow  =  0x0000;
    canfilter2.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    canfilter2.FilterBank = 15;
    canfilter2.FilterMode = CAN_FILTERMODE_IDMASK;
    canfilter2.FilterScale = CAN_FILTERSCALE_16BIT;
    canfilter2.FilterActivation = ENABLE;
    canfilter2.SlaveStartFilterBank = 14;
		HAL_CAN_ConfigFilter(&hcan2,&canfilter2);
    HAL_CAN_ActivateNotification(&hcan2,CAN_IT_RX_FIFO0_MSG_PENDING);
    HAL_CAN_Start(&hcan2);

}

extern void CAN1_RE_Int_CALLBACK(CAN_HandleTypeDef *hcan);
extern void CAN2_RE_Int_CALLBACK(CAN_HandleTypeDef *hcan);

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	  if(&hcan1 == hcan)
    {
      CAN1_RE_Int_CALLBACK(hcan);
    }
    else
    {
      CAN2_RE_Int_CALLBACK(hcan);
    }
}

