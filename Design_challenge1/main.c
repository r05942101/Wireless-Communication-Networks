#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "autonet.h"

int main(void)
{
	// sender parameters
	uint8_t tx_msg[256];
	uint8_t tx_length;
  uint8_t tx_flag_msg[256];
	uint8_t tx_flag_length;
	uint8_t tx_data_msg[256];
	uint8_t tx_data_length;
	
	uint16_t temp_Addr;
	uint8_t count = 0;
	uint8_t Type;
	uint16_t Addr;
	uint8_t radio_channel;
	uint16_t radio_panID;
	
	uint8_t ID = 0;
	uint8_t temp_ID = 0;
	//uint8_t ABC = 1;
	
	uint8_t timer;
	uint8_t timer_1 =0;
  uint8_t timer_2 =0;
	uint8_t i =0;
	uint16_t r;
	
	// receiver parameters
	uint8_t rcvd_msg[20];
	uint8_t rcvd_payload[20];
	uint8_t temp_rcvd_payload[20];
	uint8_t tt_rcvd_payload[20];
	uint16_t route_table;
	uint8_t route_length;
	uint8_t rcvd_length;
	uint8_t rcvd_payloadLength;
	uint8_t rcvd_rssi;
	uint8_t output_array[60];
//	uint8_t State = 0;
	
	int tmp2 = 0;
	
	Type = Type_Light;
	Addr = 0x0007;
	radio_channel = 25;
	radio_panID = 0x0006;
	
	
	
	Initial(Addr, Type, radio_channel, radio_panID);
	//Initial(Addr, Type, radio_channel, radio_panID);
	setTimer(1,1000,UNIT_MS);
	setTimer(2,1000,UNIT_MS);
	setTimer(3,100,UNIT_MS);
  setGPIO(1,1);
	
	 tx_msg[0] = 0; // ID message
	 tx_msg[1] = ID;
	 //r=(rand()%8)+1;
	 for(i=2;i<10;i++){
				tx_msg[i] = 0;
	 }
	
	
	// listen for 5s to find ID
	while(timer_1<=5){
		if(RF_Rx(rcvd_msg, &rcvd_length, &rcvd_rssi)){
		  getPayloadLength(&rcvd_payloadLength, rcvd_msg);
		  getPayload(temp_rcvd_payload, rcvd_msg, rcvd_payloadLength);
		  if(temp_rcvd_payload[1]>ID && temp_rcvd_payload[0] == 0){
			  ID = temp_rcvd_payload[1]+1;
				getPayload(tx_msg,rcvd_msg,rcvd_payloadLength);
				tx_msg[1] =  ID;
		  }
		}
		if(checkTimer(1)){
			timer_1 ++;
		}
	}
	
	if(ID==0){ 
	  ID=1;
		tx_msg[1] = ID;
	}
	
	/*
  // choose address
	while(1){
		
		r=(rand()%8)+1;
		if (tx_msg[r+1]==0){
			Addr = r;
			tx_msg[Addr+1] =1;
			break;
		}
	}*/
	
	
	
	// LED lights
	setTimer(4,1000,UNIT_MS);
	temp_ID = ID;
	while(temp_ID>0){
		if(checkTimer(4)){
			setGPIO(1,0);
			temp_ID--;
			Delay(500);
		}

		setGPIO(1,1);
		//sprintf((char *)output_array,"%d\r\n",ID);
	}
	
	// initialize
	Initial(Addr, Type, radio_channel, radio_panID);
	
	
if(ID<8){
	tx_data_length = 3; // add 1 at first addr at second and ID at last 
	tx_data_msg[0] = 1;
	tx_data_msg[1] = Addr;
	tx_data_msg[2] = ID; 
	
	tx_flag_msg[0] = 2;
	tx_flag_msg[1] = ID;
	tx_flag_msg[2] = Addr;
	tx_flag_length = 3;

	//broadcast ID until it listen someone's ID is higher OR "flag" message
	while(1){
	  if(RF_Rx(rcvd_msg, &rcvd_length, &rcvd_rssi)){
		  getPayloadLength(&rcvd_payloadLength, rcvd_msg);
		  getPayload(tt_rcvd_payload, rcvd_msg, rcvd_payloadLength);
			if((tt_rcvd_payload[1]>ID && tt_rcvd_payload[0]==0)|| tt_rcvd_payload[0] ==2){
			  break;
			}
		}
	  tx_length = 10;
		RF_Tx(0xFFFF,tx_msg,tx_length);
		
	  
  }  
	
	// a flag from H
	while(1){
	  if(RF_Rx(rcvd_msg, &rcvd_length, &rcvd_rssi)){
			getPayloadLength(&rcvd_payloadLength, rcvd_msg);
		  getPayload(rcvd_payload, rcvd_msg, rcvd_payloadLength);
			if(ID!=1){
	      if(rcvd_payload[0] ==2 && rcvd_payload[1]>ID ){							// receive flag and broadcast flag first time
			    route_table = rcvd_payload[rcvd_payloadLength-1];
					route_length = 1;
					
					while(1){
						if(checkTimer(1)){
							tmp2 += 1;
							if(tmp2 >=7){
								break;
							}
						}						
						RF_Tx(0xFFFF,tx_flag_msg,tx_flag_length);
							//State = 1;
					}
					break;
			  }
		  }else if(ID==1){
				if(rcvd_payload[0] ==2){
					route_table = rcvd_payload[rcvd_payloadLength-1];
					route_length = 1;
					Delay(7000);
					break;
				}
			} 
	  }
	}
	
	setGPIO(1,0);
  
	// transmit data
	while(1){
		RF_Tx(route_table, tx_data_msg, tx_data_length);
	
		if(RF_Rx(rcvd_msg, &rcvd_length, &rcvd_rssi)){
			getPayloadLength(&rcvd_payloadLength, rcvd_msg);
		  getPayload(rcvd_payload, rcvd_msg, rcvd_payloadLength);
			if(rcvd_payload[0] ==1){
				RF_Tx(route_table,rcvd_payload,rcvd_payloadLength);
			}
		}
	}
}//A~G
//device H
else if(ID==8){
	  float h_t[8];
		float plus_t = 0;
		uint8_t h_addr[8];
		uint8_t reID = 0;
		int coco = 0;
		int co_msg = 0;
		for (i=0;i<8;i++){
			h_t[i] =0;
			h_addr[i]=0;
		}
		
		timer = 0;
		tx_msg[0] =2;
		tx_msg[1] = ID;
		tx_msg[2] = Addr;
		tx_length =3;
		while(timer <2){
			RF_Tx(0xFFFF,tx_msg,tx_length);
			if(checkTimer(1)){
				timer ++;
			}
		}//tx 1 second
		timer =0;
		while(timer <5){
			if(checkTimer(1)){
				timer ++;
			}
		}//sleep 5 second	
		
		setGPIO(1,0);
    		
		while(1){
			if(coco>=7){
			break;
			}
			if(checkTimer(3)){
				plus_t += 0.01;
			}
			if(RF_Rx(rcvd_msg, &rcvd_length, &rcvd_rssi)){
				getPayloadLength(&rcvd_payloadLength, rcvd_msg);
				getPayload(rcvd_payload, rcvd_msg, rcvd_payloadLength);
				co_msg+=1;
				if(rcvd_payload[0] == 1){
					reID = rcvd_payload[2];
					if(h_addr[reID-1] ==0){
						h_addr[reID-1] = rcvd_payload[1]; // put addresss
						h_t[reID-1] = plus_t;
						coco +=1;
					}//store 
				}
			}//receive msg
		}
		sprintf((char *)output_array,"Total number of messages: %d\r\n",co_msg);
		COM2_Tx(output_array,60);
		for(i=0;i<8;i++){
			sprintf((char *)output_array,"ID:%d  Address: %d time: %f s\r\n",i+1,h_addr[i],h_t[i]);
			COM2_Tx(output_array,60);
		}
	}//H
}


