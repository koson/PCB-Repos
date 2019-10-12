//系统任务相关参数配置和声明
#define SYSTASK_PRIO									20
//任务堆栈大小32bit
#define CPU_STK_SYSTASK_SIZE							1000
//时间片长度
#define SYSTASK_TICK_LEN								0
static  OS_TCB											SysTaskTCB;
static	CPU_STK											SysTaskStk[CPU_STK_SYSTASK_SIZE];
static void  SysTask(void* p_arg);

/* 系统主任务 */
#define MAIN_TASK_PRIO									21
//任务堆栈大小32bit
#define CPU_STK_MAIN_SIZE								1000
//时间片长度
#define MAIN_TICK_LEN									0
static  OS_TCB											MainTaskTCB;
static	CPU_STK											MainTaskStk[CPU_STK_SYSTASK_SIZE];
static void  MainTask(void* p_arg);



/* 接收MQTT指令 */
#define MQTTRECEIVE_PRIO								22
//任务堆栈大小32bit
#define CPU_STK_MQTTRECEIVE_SIZE						3000
//时间片长度
#define MQTTRECEIVE_TICK_LEN							0
static  OS_TCB											MQTTRECEIVEtcb;
static	CPU_STK											MQTTRECEIVEstk[CPU_STK_MQTTRECEIVE_SIZE];
static void  MQTTRECEIVEtask(void* p_arg);

/* 发送MQTT指令 */
#define SIM7100_PRIO									23
//任务堆栈大小32bit
#define CPU_STK_SIM7100_SIZE							3000
//时间片长度
#define SIM7100_TICK_LEN								0
static  OS_TCB											SIM7100tcb;
static	CPU_STK											SIM7100stk[CPU_STK_SIM7100_SIZE];
static void  SIM7100task(void* p_arg);

/* 键盘扫描 */
#define KeyScan_TASK_PRIO								24
#define CPU_STK_KeyScan_TASK_SIZE						300
#define KeyScan_TICK_LEN								0
static  OS_TCB											KeyScan_TCB;
static	CPU_STK											KeyScan_Stk[CPU_STK_KeyScan_TASK_SIZE];

/* Watch task */
#define HELLO_LED_PRIO									25
#define CPU_STK_HELLO_LED_SIZE							3000
#define HELLO_LED_TICK_LEN								0
static  OS_TCB											HELLO_LEDtcb;
static	CPU_STK											HELLO_LEDstk[CPU_STK_HELLO_LED_SIZE];
static void  HELLO_LEDtask(void* p_arg);



#define  SystemDatasBroadcast_PRIO                      30 // 统计任务优先级最低，我这里是12，已经低于其他任务的优先级了
#define  SystemDatasBroadcast_STK_SIZE                  100 // 任务的堆栈大小，做统计一般够了，统计结果出来后不够再加..
#define  SystemDatasBroadcast_LED_TICK_LEN              0
static   OS_TCB                                         SystemDatasBroadcast_TCB;		                // 定义统计任务的TCB
static   CPU_STK                                        SystemDatasBroadcast_STK [SystemDatasBroadcast_STK_SIZE];// 开辟数组作为任务栈给任务使用
static void  SystemDatasBroadcast (void *p_arg);