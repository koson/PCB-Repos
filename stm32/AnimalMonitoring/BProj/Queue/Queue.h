#ifndef QUEUE_H
#define QUEUE_H

#define ERROR   0
#define SUCCESS 1

#define MAXSIZE 200 /* 存储空间初始分配量 */

typedef int Status; 

typedef struct
{
	unsigned int uid;  // unique id
	unsigned int pic_id;  // picture id
	unsigned int pack_id;  // pack id
}QElemType;   /* QElemType类型 */

/* 循环队列的顺序存储结构 */
typedef struct
{
	QElemType data[MAXSIZE];
	int front;    	/* 头指针 */
	int rear;		/* 尾指针，若队列不空，指向队列尾元素的下一个位置 */
}SqQueue;

Status InitQueue(SqQueue *Q);
Status ClearQueue(SqQueue *Q);
Status QueueEmpty(SqQueue Q);
int QueueLength(SqQueue Q);
Status GetHead(SqQueue Q,QElemType *e);
Status EnQueue(SqQueue *Q,QElemType e);
Status DeQueue(SqQueue *Q,QElemType *e);


#endif
