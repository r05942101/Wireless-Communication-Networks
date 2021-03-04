// This is the code of the relay device!

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "autonet.h"

// You can define the period of retransmittion time of the source device
//#define RETRANSMIT_PERIOD 200

int main(void)
{
	//uint8_t src_packet[128] = {0x05, 0x30, 0x00, 0x00, 0x0A};
	uint8_t no = 0x00;
	int nodeACK = 0;
	int rcvd_ACK = 0;
	int status = 0;
	
	uint8_t rcvd_msg[128] = {0};
	uint8_t rcvd_payload[128] = {0};
	uint8_t rcvd_length;
	uint8_t rcvd_payloadLength;
	uint8_t rcvd_rssi;
	uint8_t tx_payload[128] = {0x05, 0x30, 0x00, 0x00, 0x0A};
	
	
	uint8_t Type;
	uint16_t Addr;
	uint8_t radio_channel;
	uint16_t radio_panID;
	
	Type = Type_Light;
	Addr = 0x0001;
	radio_channel = 18;
	radio_panID = 0x00AA;
	
	Initial(Addr, Type, radio_channel, radio_panID);
	setTimer(1,RETRANSMIT_PERIOD,UNIT_MS);
	
	while(1){
		
		if(RF_Rx(rcvd_msg, &rcvd_length, &rcvd_rssi)){
			getPayloadLength(&rcvd_payloadLength, rcvd_msg);
			getPayload(rcvd_payload, rcvd_msg, rcvd_payloadLength);
			status = 1;
			rcvd_ACK = rcvd_payload[3];
			if ((rcvd_ACK==1 && no <= rcvd_payload[2]) || (rcvd_ACK==0 && no < rcvd_payload[2])){
				no = rcvd_payload[2];
				nodeACK = rcvd_ACK;
				tx_payload[0] = rcvd_payload[0];
				tx_payload[1] = rcvd_payload[1];
				tx_payload[2] = rcvd_payload[2];
				tx_payload[3] = rcvd_payload[3];
				tx_payload[4] = rcvd_payload[4];
			}
		}	
		
		if (status ==1){
			setGPIO(1,1);
			RF_Tx(0xFFFF,tx_payload,5);		
		}
		if (no == 0X13 && nodeACK==1){
		setGPIO(1,0);
		}
	}
}
