/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "wiz820io.h"
#include "config.h"
#include "w5200.h"
#include "socket.h"
#include <stdio.h>

/* Private macro -------------------------------------------------------------*/

/* tick -----------------*/
#define tick_second 1
/* ----------------------*/

/* Private variables ---------------------------------------------------------*/
// Debug
uint8_t lis = 0;

// Strng parse flag
int E1Flag = false;
int E2Flag = false;
int PCFlag = false;

// typedef initialize
CONFIG_MSG Config_Msg;
CHCONFIG_TYPE_DEF Chconfig_Type_Def; 

// Configuration Network Information of W5200
uint8 Enable_DHCP = OFF;
#ifdef USE_EQDAS01
  uint8 MAC[6] = {0x45, 0x51, 0x44, 0x41, 0x53, 0x01};  //MAC Address : EQDAS1
  uint8 IP[4] = {192, 168, 0, 10};                      //IP Address
#elif defined USE_EQDAS02
  uint8 MAC[6] = {0x45, 0x51, 0x44, 0x41, 0x53, 0x02};  //MAC Address : EQDAS2
  uint8 IP[4] = {192, 168, 0, 15};                      //IP Address
#elif defined USE_EQDAS_SERVER
  uint8 MAC[6] = {0x50, 0x43, 0x4F, 0x55, 0x54, 0x52};  //MAC Address : PCOUTR
  uint8 IP[4] = {192, 168, 0, 25};                      //IP Address  
#endif
uint8 GateWay[4] = {192, 168, 0, 1};                  //Gateway Address
uint8 SubNet[4] = {255, 255, 255, 0};                 //SubnetMask Address

//TX MEM SIZE- SOCKET 0:8KB, SOCKET 1:2KB, SOCKET2-7:1KB
//RX MEM SIZE- SOCKET 0:8KB, SOCKET 1:2KB, SOCKET2-7:1KB
uint8 txsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};
uint8 rxsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};

//FOR TCP Client
//Configuration Network Information of TEST PC
uint8 Dest_IP[4] = {192, 168, 0, 25}; //DST_IP Address 

/* Setup Port for each board 
 * Please open config.h file to select board configuration
 */
#ifdef USE_EQDAS01
  uint16 Dest_PORT = 5050; //DST_IP port
#elif defined USE_EQDAS02
  uint16 Dest_PORT = 6060; //DST_IP port
#elif defined USE_EQDAS_SERVER
  uint16 Dest_PORT = 7070; //DST_IP port
#endif

uint8 ch_status[MAX_SOCK_NUM] = { 0, };	/** 0:close, 1:ready, 2:connected */

uint8 TX_BUF[TX_RX_MAX_BUF_SIZE]; // TX Buffer for applications
uint8 RX_BUF[TX_RX_MAX_BUF_SIZE]; // RX Buffer for applications

// Time criteria
extern uint32_t my_time;
extern uint32_t presentTime;


/* Private functions ---------------------------------------------------------*/

void WIZ820io_SPI1_Configuration(void) { // one that is on left one
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;

  /* RCC configuration -------------------------------------------------------*/  
  /* Enable GPIOA for SPI1 & RESET pin */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
  /* Enable the SPI1 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  
  /* GPIO configuration ------------------------------------------------------*/
  /* RESET pin configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* SPI1 NSS pin configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* SPI1 SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* SPI1  MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* SPI1  MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* SPI configuration -------------------------------------------------------*/
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);
  
  /* Enable the SPI peripheral */
  SPI_Cmd(SPI1, ENABLE);
  
  /* RESET Device ------------------------------------------------------------*/
  GPIO_ResetBits(GPIOA, GPIO_Pin_3);
  delay_us(2);
  GPIO_SetBits(GPIOA, GPIO_Pin_3);
  delay_ms(150);
}

void Set_network(void) {
  uint8 i;
  
  // MAC ADDRESS
  for (i = 0 ; i < 6; i++) Config_Msg.Mac[i] = MAC[i];
  
  // Local IP ADDRESS
  Config_Msg.Lip[0] = IP[0]; Config_Msg.Lip[1] = IP[1]; 
  Config_Msg.Lip[2] = IP[2]; Config_Msg.Lip[3] = IP[3];
  
  // GateWay ADDRESS
  Config_Msg.Gw[0] = GateWay[0]; Config_Msg.Gw[1] = GateWay[1]; 
  Config_Msg.Gw[2] = GateWay[2]; Config_Msg.Gw[3] = GateWay[3];
  
  // Subnet Mask ADDRESS
  Config_Msg.Sub[0] = SubNet[0]; Config_Msg.Sub[1] = SubNet[1]; 
  
  Config_Msg.Sub[2] = SubNet[2]; Config_Msg.Sub[3] = SubNet[3];
  
  setSHAR(Config_Msg.Mac);
  saveSUBR(Config_Msg.Sub);
  setSUBR();
  setGAR(Config_Msg.Gw);
  setSIPR(Config_Msg.Lip);

  // Set DHCP
  Config_Msg.DHCP = Enable_DHCP;    
  //Destination IP address for TCP Client
  Chconfig_Type_Def.destip[0] = Dest_IP[0]; Chconfig_Type_Def.destip[1] = Dest_IP[1];
  Chconfig_Type_Def.destip[2] = Dest_IP[2]; Chconfig_Type_Def.destip[3] = Dest_IP[3];
  Chconfig_Type_Def.port = Dest_PORT;

  //Set PTR and RCR register	
  setRTR(6000);
  setRCR(3);

  //Init. TX & RX Memory size
  sysinit(txsize, rxsize);
}

void printSysCfg(void) {
  uint8 tmp_array[6];
  
  printf("\r\n----------------------------------------- \r\n");
  printf("W5200-Cortex M4                      \r\n");        
  printf("Network Configuration Information \r\n");        
  printf("----------------------------------------- ");         		
  
  printf("\r\nMAC : %.2X.%.2X.%.2X.%.2X.%.2X.%.2X", IINCHIP_READ(SHAR0+0),IINCHIP_READ(SHAR0+1),IINCHIP_READ(SHAR0+2),
                                                                                                                                                             IINCHIP_READ(SHAR0+3),IINCHIP_READ(SHAR0+4),IINCHIP_READ(SHAR0+5));
  
  getSIPR (tmp_array);
  printf("\r\nIP : %d.%d.%d.%d", tmp_array[0],tmp_array[1],tmp_array[2],tmp_array[3]);
  
  getSUBR(tmp_array);
  printf("\r\nSN : %d.%d.%d.%d", tmp_array[0],tmp_array[1],tmp_array[2],tmp_array[3]);
  
  getGAR(tmp_array);
  printf("\r\nGW : %d.%d.%d.%d", tmp_array[0],tmp_array[1],tmp_array[2],tmp_array[3]);
}

uint32_t time_return(void) {
  return my_time;
}

/*----------- TCP Server Routine ------------*/
void ProcessTcpServer(SOCKET mSocket, uint16 port) {
  // if len is not volatile, it will overflow
  // and causes 'cstack' overstack error
  volatile uint16_t len;
  
  switch (getSn_SR(mSocket)) {
    case SOCK_ESTABLISHED:
      if(ch_status[mSocket] == 0) {
        printf("\r\n------------------------");
        printf("\r\nSocket No. %d : Connected", mSocket);
        
        printf("\r\n - Peer IP : %d.%d.%d.%d", 
               IINCHIP_READ(Sn_DIPR0(mSocket)+0), IINCHIP_READ(Sn_DIPR0(mSocket)+1), 
               IINCHIP_READ(Sn_DIPR0(mSocket)+2), IINCHIP_READ(Sn_DIPR0(mSocket)+3));
        
        printf("\r\n - Peer Port : %d", 
               ( (uint16)(IINCHIP_READ(Sn_DPORT0(mSocket)+0)<<8) + 
                 (uint16)IINCHIP_READ(Sn_DPORT0(mSocket)+1)) );
        
        printf("\r\n - Source Port : %d", IINCHIP_READ(Sn_PORT0(mSocket)));
        
        ch_status[mSocket] = 1;
      }
      
      /* check Rx data and set flags only when there's valid character recognized */
      if ((len = getSn_RX_RSR(mSocket)) > 0) {
        /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
        /* the data size to read is MAX_BUF_SIZE. */
        if (len > TX_RX_MAX_BUF_SIZE) len = TX_RX_MAX_BUF_SIZE;
        
        /* prevent from overflowing */
        len = recv(mSocket, RX_BUF, len);
        
        switch(mSocket) {
        case SOCK_ZERO : E1Flag = true; break;  // EQ-DAQ-01 board
        case SOCK_ONE : E2Flag = true; break;   // EQ-DAQ-02 board
        case SOCK_TWO : PCFlag = true; break;   // PC Client
        default : break;
        }
      }
      
      break;
      
  case SOCK_CLOSE_WAIT:
      printf("\r\nSocket No. %d : CLOSE_WAIT", mSocket);
      
      // disconnect and set status
      disconnect(mSocket);
      ch_status[mSocket] = 0;
      
      E1Flag = false; // Reset E1 flag
      E2Flag = false; // Reset E2 flag
      PCFlag = false; // Reset PC flag
      break;
    
  case SOCK_INIT:
      lis = listen(mSocket);
      break;
      
  case SOCK_CLOSED:      
    //reinitialize the socket
    if(socket(mSocket, Sn_MR_TCP, port ,0x00) == 0) {
      printf("Fail to create socket.");
    } else {
      E1Flag = false; // Reset E1 flag
      E2Flag = false; // Reset E2 flag
      PCFlag = false; // Reset PC flag
      lis = listen(mSocket);
    }
    break;
  }
}

/* ------------ TCP Client Routine ------------ */
void ProcessTcpClient(SOCKET mSocket) {
  // if len is not volatile, it will overflow
  // and causes 'cstack' overstack error
  volatile uint16_t len;
  
  switch (getSn_SR(mSocket)) {  
    case SOCK_ESTABLISHED:
      if(ch_status[mSocket]==1) {
        printf("\r\n%d : Connected", mSocket);
        ch_status[mSocket] = 2;
      }
      
#ifdef USE_EQDAS01
      if(E1Flag != true) E1Flag = true; // Set E1 flag
#elif defined USE_EQDAS02
      if(E2Flag != true) E2Flag = true; // Set E2 flag
#endif
      
      break;
      
    case SOCK_CLOSE_WAIT: /* If the client request to close */
      E1Flag = false; // Reset E1 flag
      E2Flag = false; // Reset E2 flag
      PCFlag = false;  // Reset PC flag
      printf("\r\n%d : CLOSE_WAIT", mSocket);

      /* check Rx data */
      if ((len = getSn_RX_RSR(mSocket)) > 0) {
        /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
        /* the data size to read is MAX_BUF_SIZE. */
        if (len > TX_RX_MAX_BUF_SIZE) len = TX_RX_MAX_BUF_SIZE;
        /* read the received data */
        len = recv(mSocket, RX_BUF, len);
      }
      disconnect(mSocket);
      ch_status[mSocket] = 0;
      break;
      
    case SOCK_INIT: /* if a socket is initiated */
      /* For TCP client's connection request delay : 3 sec */
      if(time_return() - presentTime >= (tick_second * 3)) {
        /* Try to connect to TCP server(Socket, DestIP, DestPort) */
        printf("\r\n[ Attempt to connect on socket %d ]", mSocket);
        connect(mSocket, Chconfig_Type_Def.destip, Chconfig_Type_Def.port);
        presentTime = time_return();
      }
      break;
        
    case SOCK_CLOSED: /* if a socket is closed */
      if(!ch_status[mSocket]) {
        printf("\r\n%d : TCP Client Started. port: %d", mSocket, Chconfig_Type_Def.port);
        ch_status[mSocket] = 1;
      }
      
      /* reinitialize the socket */
      if(socket(mSocket, Sn_MR_TCP, Chconfig_Type_Def.port, 0x00) == 0) {
        printf("\a%d : Fail to create socket.", mSocket);
        ch_status[mSocket] = 0;
      }      
      break;
  }
}

