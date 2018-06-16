//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: APPBLUETOOTH.H
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#ifndef __APPBLUETOOTH_H__
#define __APPBLUETOOTH_H__

 
#if BRAKEBOARD

#define MAX_GENERAL_BUFFER 1040
extern uint8_t general_buffer[MAX_GENERAL_BUFFER]; 
//---------------------GLOBAL VARIABLES--------------------------
 
extern uint8_t bluetoothAwake;  
//---------------------GLOBAL PROTOTYPES--------------------------
uint8_t BluetoothMicrochipConfig(void); 
uint8_t BluetoothWakeUp(void);
uint8_t BluetoothSleep(void); 
#endif
#endif
