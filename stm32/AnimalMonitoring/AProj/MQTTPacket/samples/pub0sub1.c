/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Sergio R. Caprile - clarifications and/or documentation extension
 *******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "delay.h"
#include "MQTTPacket.h"
#include "transport.h"
#define KEEPALIVE_INTERVAL 20
/* This is in order to get an asynchronous signal to stop the sample,
as the code loops waiting for msgs on the subscribed topic.
Your actual code will depend on your hw and approach*/
extern char* mytopic;
int subscrib(void)
{
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	int rc = 0;
	unsigned char buf[200];
	int buflen = sizeof(buf);
	int msgid = 1;
	MQTTString topicString = MQTTString_initializer;
	int req_qos[1] = {0};
	char* payload = "mypayload";
	int payloadlen = strlen(payload);
	int len = 0;
	int i=0;
	int cnt=0;

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
			printf("Unable to connect, return code %d\n", connack_rc);
			goto exit;
		}
	}
	else
		goto exit;

	printf("MQTT connected\n");

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
	cnt=0;

	/* subscribe */
	topicString.cstring = "/public/TEST/RWET"; /* mytopic */
	/*int MQTTSerialize_subscribe(unsigned char* buf, int buflen, unsigned char dup, unsigned short packetid, int count,
	      MQTTString topicFilters[], int requestedQoSs[])*/
	len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, req_qos);

	rc = transport_sendPacketBuffer(buf, len);
	if (MQTTPacket_read(buf, buflen, transport_getdata) == SUBACK) 	/* wait for suback */
	{
		unsigned short submsgid;
		int subcount;
		int granted_qos[1];

		rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, granted_qos, buf, buflen);
		if (granted_qos[0] != 0)
		{
			printf("Errsetp2:granted qos != 0, %d\n", granted_qos[0]);
			goto exit;
		}
		printf("setp2:subscribe to MQTT server successful\r\n");
	}
	else
	{
		printf("Errsetp1.5:can't receive SUBACK\r\n");
		goto exit;
	}

	/* loop getting msgs on subscribed topic */
	topicString.cstring = mytopic;
	while (cnt<100)
	{
		/* transport_getdata() has a built-in 1 second timeout,
		your mileage will vary */
		if (MQTTPacket_read(buf, buflen, transport_getdata) == PUBLISH)
		{
			unsigned char dup;
			int qos;
			unsigned char retained;
			unsigned short msgid;
			int payloadlen_in;
			unsigned char* payload_in;
			int rc;
			MQTTString receivedTopic;

			rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
					&payload_in, &payloadlen_in, buf, buflen);
			printf("message arrived %.*s\n", payloadlen_in, payload_in);
		}

		printf("publishing reading [%d] times\n",cnt);
		len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);
		rc = transport_sendPacketBuffer(buf, len);
		
		delay_ms(1000);
	}

exit:	
	
	printf("disconnecting\n");
	len = MQTTSerialize_disconnect(buf, buflen);
	rc = transport_sendPacketBuffer(buf, len);


	transport_close();
	delay_ms(1000);

	return 0;
}
