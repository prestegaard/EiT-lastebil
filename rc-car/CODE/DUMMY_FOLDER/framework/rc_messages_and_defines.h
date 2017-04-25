#ifndef __RC_MESSAGES_AND_DEFINES_H
#define __RC_MESSAGES_AND_DEFINES_H

// STATES FOR REMOTE CONTROLLER
#define STATE_REMOTE_WAIT_FOR_CHANNEL_SELECT 0
#define STATE_REMOTE_ADVERTISE_AVAILABLE 1
#define STATE_REMOTE_SINGLE_MODE 2
#define STATE_REMOTE_SMART_MODE 3

// MESSAGE TYPES FOR REMOTE CONTROLLER
#define MSG_REMOTE_TYPE_ADVERTISE_AVAILABLE 0
#define MSG_REMOTE_TYPE_SINGLE_MODE_STEERING 1
#define MSG_REMOTE_TYPE_SMART_MODE_STEERING 2
#define MSG_REMOTE_TYPE_TRUCK_POOLING_START 3
#define MSG_REMOTE_TYPE_TRUCK_POOLING_STOP 4

// STATES FOR CAR
#define STATE_CAR_WAIT_FOR_REMOTE 10
#define STATE_CAR_SINGLE_MODE 11
#define STATE_CAR_SMART_MODE 12

// MESAGE TYPES FOR CAR
#define MSG_CAR_TYPE_CONNECTED_TO 10
#define MSG_CAR_TYPE_ACKNOWLEDGE 11
#define MSG_CAR_TYPE_SMART_ACK 12





typedef struct 
{
    uint8_t senderID;
    uint8_t type;
    uint32_t x;
    uint32_t y;
    uint8_t button;
     
 } remote_packet_t;

typedef struct 
{
	uint8_t senderID;
	uint8_t type;
	
} car_packet_t;

typedef struct 
{
	uint8_t senderID;
	uint8_t type;
	int8_t speed_info;
	
} master_packet_t;




#endif // MESSAGES_AND_DEFINES
/** @} */
