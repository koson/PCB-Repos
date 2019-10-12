#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "delay.h"

#include "MQTTPacket.h"
#include "transport.h"

#define KEEPALIVE_INTERVAL 20
//void test1234(void)
//{
//	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
//	int rc = 0;
//	unsigned char buf[200];
//	int buflen = sizeof(buf);
//	int msgid = 1;
//	MQTTString topicString = MQTTString_initializer;
//	int req_qos[1] = {0};
//	char* payload = "mypayload";
//	int payloadlen = strlen(payload);
//	int len = 0;
//	int i=0;
//	int cnt=0;
//	/* subscribe */
//	topicString.cstring = "/public/TEST/RWET"; /* mytopic */
//	/*int MQTTSerialize_subscribe(unsigned char* buf, int buflen, unsigned char dup, unsigned short packetid, int count,
//		  MQTTString topicFilters[], int requestedQoSs[])*/
//	len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, req_qos);

//	rc = transport_sendPacketBuffer(buf, len);
//	if (MQTTPacket_read(buf, buflen, transport_getdata) == SUBACK) 	/* wait for suback */
//	{
//		unsigned short submsgid;
//		int subcount;
//		int granted_qos[1];

//		rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, granted_qos, buf, buflen);
//		if (granted_qos[0] != 0)
//		{
//			printf("Errsetp2:granted qos != 0, %d\n", granted_qos[0]);
//			goto exit;
//		}
//		printf("setp2:subscribe to MQTT server successful\r\n");
//	}
//	else
//	{
//		printf("Errsetp1.5:can't receive SUBACK\r\n");
//		goto exit;
//	}

//	exit:
//		printf("err******************\r\n");

//}

void test1111(void)
{
 	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
 	/*表示接收状态*/
 	int rc = 0;
 	unsigned char buf[200];
 	int buflen = sizeof(buf);
 	int msgid = 1;
 	MQTTString topicString = MQTTString_initializer;
 	int req_qos = 0;
	int len;


 	/* subscribe */
 	topicString.cstring = "/public/TEST/RWET";
 	len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);

 	rc = transport_sendPacketBuffer(buf, len);
   if(rc != len){
     printf("connect transport_sendPacketBuffer error\n\r");
     goto exit;
   }
 	if (MQTTPacket_read(buf, buflen, transport_getdata) == SUBACK) 	/* wait for suback */
 	{
 		unsigned short submsgid=0;
 		int subcount=0;
 		int granted_qos=0;

 		rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
 		if (granted_qos != 0)
 		{
 			printf("granted qos != 0, %d\r\n", granted_qos);
 			goto exit;
 		}
 	}
 	else
 		goto exit;
	
	exit:
		printf("err******************\r\n");
}

int ping_test(void)
{
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	
	unsigned char buf[200];
	int buflen = sizeof(buf);
	int rc = 0;
	int len = 0;
	int i=0;

	unsigned char cnt=0;
	
	while(transport_open() == TRANSPORT_ERROR)
	{
		i++;
		printf("try %d time(s) to open socket\r\n",i);
		if(i>5)
			goto exit;
	}
//	printf("Sending to hostname %s port %d\n", host, port);

	data.clientID.cstring = "me";
	data.keepAliveInterval = KEEPALIVE_INTERVAL;
	data.cleansession = 1;
//	data.username.cstring = "testuser";
//	data.password.cstring = "testpassword";

	len = MQTTSerialize_connect(buf, buflen, &data);
	rc = transport_sendPacketBuffer(buf, len);

	printf("Sent MQTT connect\n");
	/* wait for connack */
	if (MQTTPacket_read(buf, buflen, transport_getdata) == CONNACK)
	{
		unsigned char sessionPresent, connack_rc;

		if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0)
		{
			printf("Unable to connect, return code %d\r\n", connack_rc);
			goto exit;
		}
	}
	else
		goto exit;

	printf("MQTT connected\r\n");

	while (cnt<3)
	{
		len = MQTTSerialize_pingreq(buf, buflen);
		transport_sendPacketBuffer(buf, len);
		printf("Ping...");
		if (MQTTPacket_read(buf, buflen, transport_getdata) == PINGRESP){
			printf("Pong\n");
		}
		else {
			printf("OOPS\n");
			goto exit;
		}
		delay_ms(1000);
		
		cnt++;
	}
//订阅
//test1111();
/////////	
	printf("disconnecting\n");
	len = MQTTSerialize_disconnect(buf, buflen);
	rc = transport_sendPacketBuffer(buf, len);

exit:
	transport_close();
	delay_ms(1000);
	return 0;
}
