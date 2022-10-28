/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * File Name          : LWIP.c
  * Description        : This file provides initialization code for LWIP
  *                      middleWare.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "lwip/dhcp.h"
#include "tcp_server.h"
#include "app_ethernet.h"
#include "ethernetif.h"

#ifdef USE_DHCP
#define MAX_DHCP_TRIES  4
uint32_t DHCPfineTimer = 0;
__IO uint8_t DHCP_state = DHCP_OFF;
__IO uint8_t DHCP_flag=0;
Lwip_strut Lwip_data;
#endif

uint32_t EthernetLinkTimer;

/* ��չ���� ------------------------------------------------------------------*/

/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
/**
  * ��������: ��ȡPHY�Ĵ���ֵ
  * �������: reg��Ҫ��ȡ�ļĴ�����ַ
  * �� �� ֵ: 0����ȡ�ɹ���-1����ȡʧ��
  * ˵    ��: ��
  */
int32_t ETH_PHY_ReadReg(uint16_t reg,uint32_t *regval)
{
    if(HAL_ETH_ReadPHYRegister(&EthHandle,reg,regval)!=HAL_OK)
        return -1;
    return 0;
}

/**
  * ��������: ��LAN8720Aָ���Ĵ���д��ֵ
  * �������: reg��Ҫ��ȡ�ļĴ�����ַ ��value:Ҫд���ֵ
  * �� �� ֵ: 0��д��������-1��д��ʧ��
  * ˵    ��: ��
  */
int32_t ETH_PHY_WriteReg(uint16_t reg,uint16_t value)
{
    uint32_t temp=value;
    if(HAL_ETH_WritePHYRegister(&EthHandle,reg,temp)!=HAL_OK)
        return -1;
    return 0;
}

/**
  * ��������: ����LAN8720A����Э�̹���
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void LAN8720A_StartAutoNego(void)
{
    uint32_t readval=0;
    ETH_PHY_ReadReg(LAN8720A_BCR,&readval);
    readval|=LAN8720A_BCR_AUTONEGO_EN;
    ETH_PHY_WriteReg(LAN8720A_BCR,readval);
}

/**
  * ��������: ��ȡLAN8720A������״̬
  * �������: ��
  * �� �� ֵ: LAN8720A_STATUS_LINK_DOWN           �� ���ӶϿ�        
  * ˵    ��: ��
  */
uint32_t check_link_state(void)
{
	
	uint32_t readval=0;
	
	/* ��ȡ���飬ȷ����ȡ��ȷ������ */
	ETH_PHY_ReadReg(LAN8720A_BSR,&readval);
	ETH_PHY_ReadReg(LAN8720A_BSR,&readval);
	
	/* ��ȡ����״̬(Ӳ�������ߵ����ӣ�����TCP��UDP��������ӣ�) */
	if((readval&LAN8720A_BSR_LINK_STATUS)==0)
	{

		return LAN8720A_STATUS_LINK_DOWN;
	}
  else 	
	return LAN8720A_STATUS_OK;
}	

/**
  * ��������: ��ȡLAN8720A������״̬
  * �������: ��
  * �� �� ֵ: LAN8720A_STATUS_LINK_DOWN           �� ���ӶϿ� 
              LAN8720A_STATUS_AUTONEGO_NOTDONE    �� �Զ�Э�����
              LAN8720A_STATUS_100MBITS_FULLDUPLEX �� 100Mȫ˫��
              LAN8720A_STATUS_100MBITS_HALFDUPLEX �� 100M��˫��
              LAN8720A_STATUS_10MBITS_FULLDUPLEX  �� 10Mȫ˫��
              LAN8720A_STATUS_10MBITS_HALFDUPLEX  �� 10M��˫��              
  * ˵    ��: ��
  */
uint32_t ETH_link_check_state(void)
{
    uint32_t readval=0;
    
    /* ��ȡ���飬ȷ����ȡ��ȷ������ */
    ETH_PHY_ReadReg(LAN8720A_BSR,&readval);
    ETH_PHY_ReadReg(LAN8720A_BSR,&readval);
    
    /* ��ȡ����״̬(Ӳ�������ߵ����ӣ�����TCP��UDP��������ӣ�) */
    if((readval&LAN8720A_BSR_LINK_STATUS)==0)
    {

      return LAN8720A_STATUS_LINK_DOWN;
    }
        
    /* ��ȡ�Զ�Э��״̬ */
    ETH_PHY_ReadReg(LAN8720A_BCR,&readval);
    if((readval&LAN8720A_BCR_AUTONEGO_EN)!=LAN8720A_BCR_AUTONEGO_EN)  /* δʹ���Զ�Э�� */
    {
        if(((readval&LAN8720A_BCR_SPEED_SELECT)==LAN8720A_BCR_SPEED_SELECT)&&
                ((readval&LAN8720A_BCR_DUPLEX_MODE)==LAN8720A_BCR_DUPLEX_MODE)) 
            return LAN8720A_STATUS_100MBITS_FULLDUPLEX;
        else if((readval&LAN8720A_BCR_SPEED_SELECT)==LAN8720A_BCR_SPEED_SELECT)
            return LAN8720A_STATUS_100MBITS_HALFDUPLEX;
        else if((readval&LAN8720A_BCR_DUPLEX_MODE)==LAN8720A_BCR_DUPLEX_MODE)
            return LAN8720A_STATUS_10MBITS_FULLDUPLEX;
        else
            return LAN8720A_STATUS_10MBITS_HALFDUPLEX;
    }
    else                                                             /* ʹ�����Զ�Э�� */
    {
        ETH_PHY_ReadReg(LAN8720A_PHYSCSR,&readval);
        if((readval&LAN8720A_PHYSCSR_AUTONEGO_DONE)==0)
            return LAN8720A_STATUS_AUTONEGO_NOTDONE;
        if((readval&LAN8720A_PHYSCSR_HCDSPEEDMASK)==LAN8720A_PHYSCSR_100BTX_FD)
            return LAN8720A_STATUS_100MBITS_FULLDUPLEX;
        else if ((readval&LAN8720A_PHYSCSR_HCDSPEEDMASK)==LAN8720A_PHYSCSR_100BTX_HD)
            return LAN8720A_STATUS_100MBITS_HALFDUPLEX;
        else if ((readval&LAN8720A_PHYSCSR_HCDSPEEDMASK)==LAN8720A_PHYSCSR_10BT_FD)
            return LAN8720A_STATUS_10MBITS_FULLDUPLEX;
        else
            return LAN8720A_STATUS_10MBITS_HALFDUPLEX;         
    }
}

/**
  * ��������: ETH�жϷ�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void ETH_IRQHandler(void)
{
  lwip_IT_handle();
	__HAL_ETH_DMA_CLEAR_IT(&EthHandle,ETH_DMA_IT_NIS);    //���DMA�жϱ�־λ
	__HAL_ETH_DMA_CLEAR_IT(&EthHandle,ETH_DMA_IT_R);      //���DMA�����жϱ�־λ
}



#ifdef USE_DHCP

/**
  * ��������: DHCP��ȡ����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void DHCP_Process(struct netif *netif)
{
  uint32_t ipaddr=0,netmask=0,gw=0;
  struct dhcp *dhcp;   

  uint8_t iptxt[20];
  
  switch (DHCP_state)
  {
    case DHCP_START:
    {
      DHCP_state = DHCP_WAIT_ADDRESS;
      dhcp_start(netif);
      printf ("  State: Looking for DHCP server ...\n");
    }
    break;
    
  case DHCP_WAIT_ADDRESS:
    {
      printf ("DHCP_state=%d\n",DHCP_state);
      if (dhcp_supplied_address(netif)) 
      {
        DHCP_state = DHCP_ADDRESS_ASSIGNED; 
        /* ��ȡIP��ַ */
        ipaddr=netif->ip_addr.addr;
        
        Lwip_data.MAC[3]=MAC_ADDR3;
        Lwip_data.MAC[2]=MAC_ADDR2;
        Lwip_data.MAC[1]=MAC_ADDR1;
        Lwip_data.MAC[0]=MAC_ADDR0;
        printf (" MAC  ��ַ:         %d %d %d %d\n", Lwip_data.MAC[0],Lwip_data.MAC[1],Lwip_data.MAC[2],Lwip_data.MAC[3]);  
               
        Lwip_data.IP[3]=(uint8_t)(ipaddr>>24);
        Lwip_data.IP[2]=(uint8_t)(ipaddr>>16);
        Lwip_data.IP[1]=(uint8_t)(ipaddr>>8);
        Lwip_data.IP[0]=(uint8_t)(ipaddr);
        printf (" DHCP GET IP:      %d %d %d %d\n", Lwip_data.IP[0],Lwip_data.IP[1],Lwip_data.IP[2],Lwip_data.IP[3]);  
        /* ��ȡ���������ַ */
        netmask=netif->netmask.addr;
        Lwip_data.NetMask[3]=(uint8_t)(netmask>>24);
        Lwip_data.NetMask[2]=(uint8_t)(netmask>>16);
        Lwip_data.NetMask[1]=(uint8_t)(netmask>>8);
        Lwip_data.NetMask[0]=(uint8_t)(netmask);
        printf (" DHCP GET ��������: %d %d %d %d\n", Lwip_data.NetMask[0],Lwip_data.NetMask[1],Lwip_data.NetMask[2],Lwip_data.NetMask[3]);  
        /* ��ȡĬ�����ص�ַ */
        gw=netif->gw.addr;
        Lwip_data.GateWay[3]=(uint8_t)(gw>>24);
        Lwip_data.GateWay[2]=(uint8_t)(gw>>16);
        Lwip_data.GateWay[1]=(uint8_t)(gw>>8);
        Lwip_data.GateWay[0]=(uint8_t)(gw);
        printf (" DHCP GET Ĭ������: %d %d %d %d\n", Lwip_data.GateWay[0],Lwip_data.GateWay[1],Lwip_data.GateWay[2],Lwip_data.GateWay[3]);  

				YSF4_TCP_SENDData();
//        sprintf((char *)iptxt, "%s", ip4addr_ntoa((const ip4_addr_t *)&netif->ip_addr));        
//        printf (" IP address assigned by a DHCP server: %s\n", iptxt);        
      }
      else
      {
        dhcp = (struct dhcp *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
    
        /* DHCP timeout */
        if (dhcp->tries > MAX_DHCP_TRIES)
        {
          DHCP_state = DHCP_TIMEOUT;
          
          /* Stop DHCP */
          dhcp_stop(netif);
          
          /* Static address used */
          IP_ADDR4(&(netif->ip_addr), IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
          IP_ADDR4(&(netif->netmask), NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
          IP_ADDR4(&(netif->gw), GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
      
          sprintf((char *)iptxt, "%s", ip4addr_ntoa((const ip4_addr_t *)&netif->ip_addr));
          printf ("DHCP Timeout !! \n");
          printf ("Static IP address: %s\n", iptxt);
				
					YSF4_TCP_SENDData();
        }
      }
    }
    break;
  case DHCP_LINK_DOWN:
    {
      /* Stop DHCP */
      dhcp_stop(netif);
      DHCP_state = DHCP_OFF; 
    }
    break;
  default: break;
  }
}

/**
  * ��������: DHCP��ѯ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void DHCP_Periodic_Handle(struct netif *netif)
{  
  /* Fine DHCP periodic process every 500ms */
  if (HAL_GetTick() - DHCPfineTimer >= DHCP_FINE_TIMER_MSECS)
  {
    DHCPfineTimer =  HAL_GetTick();
    /* process DHCP state machine */
    DHCP_Process(netif);    
  }
}
#endif

#if LWIP_NETIF_LINK_CALLBACK
/**
  * @brief  Ethernet Link periodic check
  * @param  netif
  * @retval None
  */
void Ethernet_Link_Periodic_Handle(struct netif *netif)
{
  /* Ethernet Link every 100ms */
  if (HAL_GetTick() - EthernetLinkTimer >= 100)
  {
    EthernetLinkTimer = HAL_GetTick();
    ethernet_link_check_state(netif);
  }
}
#endif

/**
  * ��������: ��������ӿ�
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void Netif_Config(void)
{
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;
  
  /* ��ʼ��LWIP�ں� */
  lwip_init();	
  /* Initializes the dynamic memory heap defined by MEM_SIZE.*/
//  mem_init(); 
//  /* Initializes the memory pools defined by MEMP_NUM_x.*/
//  memp_init();  
#ifdef USE_DHCP
  ip_addr_set_zero_ip4(&ipaddr);
  ip_addr_set_zero_ip4(&netmask);
  ip_addr_set_zero_ip4(&gw);
#else
  IP_ADDR4(&ipaddr,IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
  IP_ADDR4(&netmask,NETMASK_ADDR0,NETMASK_ADDR1,NETMASK_ADDR2,NETMASK_ADDR3);
  IP_ADDR4(&gw,GW_ADDR0,GW_ADDR1,GW_ADDR2,GW_ADDR3);
  
	printf("��̬IP��ַ........................%d.%d.%d.%d\r\n",IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
	printf("��������..........................%d.%d.%d.%d\r\n",NETMASK_ADDR0,NETMASK_ADDR1,NETMASK_ADDR2,NETMASK_ADDR3);
	printf("Ĭ������..........................%d.%d.%d.%d\r\n",GW_ADDR0,GW_ADDR1,GW_ADDR2,GW_ADDR3);
	
#endif /* USE_DHCP */
  
  /* Add the network interface */    
  netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);
  
  /* Registers the default network interface */
  netif_set_default(&gnetif);
  
  ethernet_link_status_updated(&gnetif);

#if LWIP_NETIF_LINK_CALLBACK
      netif_set_link_callback(&gnetif, ethernet_link_status_updated);
#endif		
}

void ethernet_link_status_updated(struct netif *netif)
{
  if (netif_is_link_up(netif))
  {
    printf("�ɹ���������\n");
    /* �����������ɹ�ʱ�����ô˺��� */
    netif_set_up(&gnetif);	

#ifdef USE_DHCP
    /* ��־λ����Ϊ DHCP_START*/
    DHCP_state = DHCP_START;
	  dhcp_start(&gnetif);
#else
    uint8_t iptxt[20];
    sprintf((char *)iptxt, "%s", ip4addr_ntoa((const ip4_addr_t *)&gnetif.ip_addr));
    printf ("Static IP address: %s\n", iptxt);
		
		Lwip_data.IP[3]=IP_ADDR3;
		Lwip_data.IP[2]=IP_ADDR2;
		Lwip_data.IP[1]=IP_ADDR1;
		Lwip_data.IP[0]=IP_ADDR0;	
		YSF4_TCP_SENDData();
#endif /* USE_DHCP */
  }
  else
  {
    /* ����������ʧ��ʱ�����ô˺��� */
    netif_set_down(&gnetif);		
#ifdef USE_DHCP
    /* ��־λ����Ϊ DHCP_LINK_DOWN */
    DHCP_state = DHCP_LINK_DOWN;
#endif  /* USE_DHCP */
    printf ("The network cable is not connected \n");

  }

	
}

