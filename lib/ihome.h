/*
 * Copyright (c) Jack's Lab (www.jackslab.org)
 * Author: Jack Tan <jiankemeng@gmail.com>
*/

#ifndef __HSG_CMD__
#define __HSG_CMD__

/*
 * Packet Frame Format:
 *
 * | Type 1Byte | DST_SYS_ID 1B | DST_DEV_ID 1B | SRC_SYS_ID | SRC_DEV_ID | Data Size 1B | Data 24B | Frame No. 1B | Frame Count 1B |
 *
 * Forwarding Frame Format:
 *
 * | Type 1Byte | DST_SYS_ID 1B | DST_DEV_ID 1B | DST_SYS_ID 1B | DST_DEV_ID 1B | Forward Count 1B | Normal Packet Frame Format |
 *
*/
#define	T_CMD		0x01
#define	T_DATA		0x02
#define	T_ALARM		0x04
#define	T_FORWARD	0x08
#define	T_ACK		0x80

/*
 *
 *
 * Main System ID:
 *
 *  Light system	0xF0
 *  Plant system	0xE0   (include plant, flower, fish...)
 *  Home Theatre system	0xD0
 *  
 *  Alarm system	0x80
 *  Curtain System	0x90
 *
 *  Water System	0xA0
 *  Air   System	0xB0
 *  Heating System	0xC0
 * 
*/
#define	CENTRAL_GW	0x10

#define	ALARM_SYS	0x80
#define	CURTAIN_SYS	0x90

#define WATER_SYS	0xA0
#define	AIR_SYS		0xB0
//maybe heating system is related to air system
#define	HEATING_SYS	0xC0

#define	LIGHT_SYS	0xD0
#define	HT_SYS		0xE0
#define	PLANT_SYS	0xF0


// Subsystem ID of Light system
#define	PARLOR_LIGHT	(LIGHT_SYS | 0x01)
#define KITCHEN_LIGHT	(LIGHT_SYS | 0x02)
#define	BEDROOM_LIGHT	(LIGHT_SYS | 0x03)
#define DINING_LIGHT	(LIGHT_SYS | 0x04)
#define	WASHROOM_LIGHT	(LIGHT_SYS | 0x05)
#define HALL_LIGHT	(LIGHT_SYS | 0x06)

// Subsystem ID of Plant system
#define	PARLOR_PLANT	(PLANT_SYS | 0x01)

// Subsystem ID of Home Theatre system
// These are all in IR station :)
#define	HT_TV		(HT_SYS | 0x01)
#define HT_TOPBOX	(HT_SYS | 0x02)
#define	HT_PLAYER	(HT_SYS | 0x03)
#define HT_STEREO	(HT_SYS | 0x0A)

#define	HT_TV_HALL		((TV << 8) | 0x01)
#define HT_TV_BEDROOM		((TV << 8) | 0x02)
#define HT_TV_HALL_PROJECTOR	((TV << 8) | 0x03)

#define HT_TOPBOX_HALL		((TOPBOX << 8) | 0x01)
#define HT_TOPBOX_BEDROOM	((TOPBOX << 8) | 0x02)

#define	HT_PLAYER_DVD			((PLAYER << 8) | 0x01)
#define	HT_PLAYER_CD			((PLAYER << 8) | 0x02)
#define	HT_PLAYER_VCD			((PLAYER << 8) | 0x03)
#define	HT_PLAYER_BLUERAY		((PLAYER << 8) | 0x04)


// plan to add a USB serial wireless
#define	HTPC			((PLAYER << 8) | 0x05)


// Subsystem ID of Alarm system


// Subsystem ID of Curtain System


// Command ID
#define	SET_SWITCH_ON		0xAF
#define	SET_SWITCH_OFF		0xAE
#define	QUERY_SWITCH_STAT	0xAD

#define	PREFETCH_DATA		0xA9
#define	GET_DATA		0xA8

#define	SCAN_DEV		0xA0

#endif
