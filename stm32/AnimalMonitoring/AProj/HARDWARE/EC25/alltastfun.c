static void SysTask(void *p_arg) // == appCreat
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	
	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OS_CRITICAL_ENTER();	//进入临界区
    //
    OSTaskCreate((OS_TCB *)&MainTaskTCB,                              // 该任务堆栈的开始地址
                 (CPU_CHAR *)"MainTask",                              // 任务分配名字
                 (OS_TASK_PTR)MainTask,                               // 指向任务代码的指针
                 (void *)0,                                           // 指针，第一次执行任务时传递给，任务实体的指针参数*p_arg
                 (OS_PRIO)MAIN_TASK_PRIO,                             // 优先级设置	 参数值越小优先级越高
                 (CPU_STK *)&MainTaskStk[0],                          // 任务堆栈的基地址。基地址通常是分配给该任务的堆栈的最低内存位置
                 (CPU_STK_SIZE)CPU_STK_MAIN_SIZE / 10,                // 第七个参数是地址“水印” ，当堆栈生长到指定位置时就不再允许其生长
                 (CPU_STK_SIZE)CPU_STK_MAIN_SIZE,                     // 任务的堆栈大小
                 (OS_MSG_QTY)0,                                       //
                 (OS_TICK)MAIN_TICK_LEN,                              // 设置任务拥有多少个时间片，当采用时间片轮询调度任务时有效
                 (void *)0,                                           //
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //
                 (OS_ERR *)&err);                                     // 返回当前函数执行的结果OS_ERR_NONE表示成功

    //
    OSTaskCreate((OS_TCB *)&MQTTRECEIVEtcb,
                 (CPU_CHAR *)"MQTTRECEIVE",
                 (OS_TASK_PTR)MQTTRECEIVEtask,
                 (void *)0,
                 (OS_PRIO)MQTTRECEIVE_PRIO,
                 (CPU_STK *)&MQTTRECEIVEstk[0],
                 (CPU_STK_SIZE)CPU_STK_MQTTRECEIVE_SIZE / 10,
                 (CPU_STK_SIZE)CPU_STK_MQTTRECEIVE_SIZE,
                 (OS_MSG_QTY)0,
                 (OS_TICK)MQTTRECEIVE_TICK_LEN,
                 (void *)0,
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR *)&err);
    //
    OSTaskCreate((OS_TCB *)&SIM7100tcb,
                 (CPU_CHAR *)"SIM7100",
                 (OS_TASK_PTR)SIM7100task,
                 (void *)0,
                 (OS_PRIO)SIM7100_PRIO,
                 (CPU_STK *)&SIM7100stk[0],
                 (CPU_STK_SIZE)CPU_STK_SIM7100_SIZE / 10,
                 (CPU_STK_SIZE)CPU_STK_SIM7100_SIZE,
                 (OS_MSG_QTY)0,
                 (OS_TICK)SIM7100_TICK_LEN,
                 (void *)0,
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR *)&err);

    // KeyScan
    OSTaskCreate((OS_TCB *)&KeyScan_TCB,
                 (CPU_CHAR *)"KeyScan",
                 (OS_TASK_PTR)KeyScan_Task,
                 (void *)0,
                 (OS_PRIO)KeyScan_TASK_PRIO,
                 (CPU_STK *)&KeyScan_Stk[0],
                 (CPU_STK_SIZE)CPU_STK_KeyScan_TASK_SIZE / 10,
                 (CPU_STK_SIZE)CPU_STK_KeyScan_TASK_SIZE,
                 (OS_MSG_QTY)0,
                 (OS_TICK)KeyScan_TICK_LEN,
                 (void *)0,
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR *)&err);
    // watchTask
    OSTaskCreate((OS_TCB *)&HELLO_LEDtcb,
                 (CPU_CHAR *)"HELLO_LED",
                 (OS_TASK_PTR)HELLO_LEDtask,
                 (void *)0,
                 (OS_PRIO)HELLO_LED_PRIO,
                 (CPU_STK *)&HELLO_LEDstk[0],
                 (CPU_STK_SIZE)CPU_STK_HELLO_LED_SIZE / 10,
                 (CPU_STK_SIZE)CPU_STK_HELLO_LED_SIZE,
                 (OS_MSG_QTY)0,
                 (OS_TICK)HELLO_LED_TICK_LEN,
                 (void *)0,
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR *)&err);
    // stackMonitoring
    OSTaskCreate((OS_TCB *)&SystemDatasBroadcast_TCB,
                 (CPU_CHAR *)"SystemDatasBroadcast",
                 (OS_TASK_PTR)SystemDatasBroadcast,
                 (void *)0,
                 (OS_PRIO)SystemDatasBroadcast_PRIO,
                 (CPU_STK *)&SystemDatasBroadcast_STK[0],
                 (CPU_STK_SIZE)SystemDatasBroadcast_STK_SIZE / 10, /*栈溢出临界值我设置在栈大小的90%处*/
                 (CPU_STK_SIZE)SystemDatasBroadcast_STK_SIZE,
                 (OS_MSG_QTY)0,
                 (OS_TICK)0,
                 (void *)0,
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR *)&err);
	OS_CRITICAL_EXIT();	//退出临界区
	OSTaskDel((OS_TCB*)0,&err);	//删除start_task任务自身
}

void openUSB(void);
void closeUSB(void);
static void MainTask(void *p_arg) // test fun
{
    OS_ERR err;
    u8 state;
    u8 res;
    u8 flag = 0;
    while (1)
    {
        if (key_down == 1 && flag == 0)
        {
            F407USART1_SendString("key1_down..openUSB...\r\n");
            usb_state_set(1); // 设置标志启动USB
            usbapp_mode_set();
            openUSB();
            res = WaitFindUSBflag(30000);
            if (res == 1) // 正常打开相机
            {
            }
            else
            {
                F407USART1_SendString("Fail WaitDistinguishflag...\r\n");
            }
            key_down = 0;
            flag = 1;
        }
        else if (key_down == 1 && flag == 1)
        {
            F407USART1_SendString("key1_down..closeUSB...\r\n");

            closeUSB();

            res = WaitDiscnctUSBflag(3000);
            if (res == 1) // 正常关闭相机
            {
                F407USART1_SendString("USB_CONNECT_OFF...\r\n");
                USB_CONNECT_OFF();
                usbapp_mode_stop();

                res = mf_send_pics("0:INBOX", "0:ARCH", 1);
                printf("sta=%d\r\n", res);
                mf_scan_files("0:INBOX");
                mf_scan_files("0:ARCH");
                //mysend_allpic();
            }
            else
            {
                F407USART1_SendString("Fail USB_CONNECT_OFF...\r\n");
            }

            key_down = 0;
            flag = 0;
        }

        if (key2_down == 1)
        {
            F407USART1_SendString("key2_down..closeUSB...\r\n");
            //USB_Photograph();
            mysend_picture("0:IMG/IMAG105.JPG", 0105);
            key2_down = 0;
        }

        OSTimeDly(10, OS_OPT_TIME_DLY, &err);
    }
}

// 接收MQTT服务器的指令
static void MQTTRECEIVEtask(void *p_arg)
{
    OS_ERR err;

    int type; // 解析接收的数据值

    //===========================
    unsigned char dup;
    int qos;
    // 保留标志
    unsigned char retained;
    // 包id
    unsigned short packetid;
    // 主题名
    MQTTString topicName;

    // 数据
    unsigned char *payload;
    // 数据长度
    int payloadlen;

    //==============================
    F407USART1_SendString("MQTTRECEIVEtask_run\r\n");
    while (1)
    {
        int i;
        if (UART_TCP_buffLength() != 0)
        {
            F407USART1_SendString("UART_TCP received data\r\n");
            //处理接收到的MQTT消息，并根据不同的消息类型做不同的处理
            type = MQTTPacket_read(MQTT_Receivebuf, MQTT_RECEIVEBUFF_MAXLENGTH, transport_getdata);
            switch (type)
            {
            case CONNECT:
                break;
            case CONNACK:          //连接请求响应
                mqtt_state_set(1); // 设置连接成功
                F407USART1_SendString("MQTT connect successful\r\n");
                break;
            case PUBLISH: //订阅的消息,由别人发布
                if (MQTTDeserialize_publish(&dup, &qos, &retained, &packetid, &topicName, &payload, &payloadlen,
                                            MQTT_Receivebuf, MQTT_RECEIVEBUFF_MAXLENGTH) == 1)
                { // 解析接收到的字符串
                    if (MystringSeek((uint8 *)(topicName.lenstring.data), topicName.lenstring.len, (uint8 *)MY_TOPIC_PICDOWN, strlen(MY_TOPIC_PICDOWN)) != -1)
                    { // 查找指定的主图

                        printf("\r\nTOPIC1=%s,payloadlen=%d\r\n", MY_TOPIC_PICDOWN, payloadlen);
                        for (i = 0; i < payloadlen; i++)
                        {
                            // 打印缓存区域的内容
                        }
                        picdown_Flag_set(1);
                    }
                    else if (MystringSeek((uint8 *)(topicName.lenstring.data), topicName.lenstring.len, (uint8 *)MY_TOPIC_MSGDOWN, strlen(MY_TOPIC_MSGDOWN)) != -1)
                    { //JSON
                        //printf("TOPIC2=%s,payloadlen=%d\r\n",MY_TOPIC_MSGDOWN,payloadlen);
                        printf("TOPIC2=%s,payloadlen=%d\r\n", MY_TOPIC_MSGDOWN, payloadlen);
                    }
                }
                break;
            case PUBACK: //发布消息响应，表示发布成功
                break;
            case PUBREC:
                break;
            case PUBREL:
                break;
            case PUBCOMP:
                break;
            case SUBSCRIBE:
                break;
            case SUBACK: //订阅消息ack
                break;
            case UNSUBSCRIBE:
                break;
            case UNSUBACK:
                break;
            case PINGREQ:
                break;
            case PINGRESP: //Pin返回
                break;
            case DISCONNECT:
                break; //由客户端发送给服务器，表示正常断开连接
            default:
                break;
            }
        }
        OSTimeDly(5, OS_OPT_TIME_DLY, &err);
    }
}

// 如何在使用的时候才创建任务，不用的时候不创建
static void SIM7100task(void *p_arg)
{
    OS_ERR err;

    uint16 time = 0, pic_id = 10001;
    uint8 sss = 0;
    MY_MQTT_ERR mqtterr;
    F407USART1_SendString("SIM7100task run\r\n");
    SIM7100_Init();
    F407USART1_SendString("SIM7100_Init \r\n");
    //transport_open();
    while (1)
    {

        time++;
        time %= 120;

        if (time % 5 == 0)
        { //如果连接失败，每隔5秒会重新尝试连接一次
            if (mqtt_state_get() == 0)
            {
                F407USART1_SendString("Try mqtt_connect\r\n");
                mqtt_connect(MQTT_SEND_buf, MQTT_SEND_BUFF_MAXLENGTH);
            }
        }
        if (time % 10 == 0)
        {
            if (mqtt_state_get() == 1)
            {
                F407USART1_SendString("Data_Packing\r\n");
                Data_Packing_sens(myMessage);
                F407USART1_SendString("Data_Packing_sens\r\n");
                //F407USART1_SendString(myMessage);
                //mqtterr = send_picture(MQTT_SEND_buf,MQTT_SEND_BUFF_MAXLENGTH,MQTT_DATA_buf,MQTT_DATA_BUFF_MAXLENGTH,path_ptr,pic_id,10000,50000);
                //send_data(MQTT_SEND_buf,MQTT_SEND_BUFF_MAXLENGTH,myMessage);
                //Sys_Enter_Standby();
                pic_id++;
            }
        }

        if (mqtt_state_get() == 1)
        {
            HELLO_LED1_ON();
            if (sss == 0)
            { //如果连接MQTT服务器成功，会订阅一个picdown主题消息，可以根据需要决定是否判断响应，判断是否订阅成功
                mqtt_subscribe(MY_TOPIC_PICDOWN);
                mqtt_subscribe(MY_TOPIC_MSGDOWN);
                sss = 1;
            }
            //MY_MQTT_ERR send_data(uint8 *mqttbuff,uint32 mqttbufflength, char* msg)
        }
        else
        {
            HELLO_LED1_OFF();
        }
        OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
    }
}

void  SystemDatasBroadcast (void *p_arg)
{
  OS_ERR err;
  CPU_STK_SIZE free,used;
  (void)p_arg;
  while(DEF_TRUE)
  {
	OSTaskStkChk (&SystemDatasBroadcast_TCB,&free,&used,&err);
	printf("SystemDatasBroadcast  used/free:%d/%d  usage:%%%d\r\n",used,free,(used*100)/(used+free));
		
	OSTaskStkChk (&MainTaskTCB,&free,&used,&err);
	printf("MainTaskTCB             used/free:%d/%d  usage:%%%d\r\n",used,free,(used*100)/(used+free));
		
	OSTaskStkChk (&MQTTRECEIVEtcb,&free,&used,&err);
	printf("MQTTRECEIVEtcb             used/free:%d/%d  usage:%%%d\r\n",used,free,(used*100)/(used+free));
		
	OSTaskStkChk (&SIM7100tcb,&free,&used,&err);
	printf("SIM7100tcb              used/free:%d/%d  usage:%%%d\r\n",used,free,(used*100)/(used+free));
		
	OSTaskStkChk (&KeyScan_TCB,&free,&used,&err);
	printf("KeyScan_TCB          used/free:%d/%d  usage:%%%d\r\n",used,free,(used*100)/(used+free));
		
	OSTaskStkChk (&HELLO_LEDtcb,&free,&used,&err);
	printf("HELLO_LEDtcb             used/free:%d/%d  usage:%%%d\r\n",used,free,(used*100)/(used+free));
	
	printf("\r\n\r\n\r\n");
	OSTimeDlyHMSM(0,0,5,0,(OS_OPT)OS_OPT_TIME_DLY,(OS_ERR*)&err);
   }
}
