#include "queue.h"  
#include "stdio.h"    
//#include "stdlib.h"   

// #include "io.h"  
// #include "math.h"  
// #include "time.h"



/* 初始化一个空队列Q */
Status InitQueue(SqQueue *Q)
{
	Q->front=0;
	Q->rear=0;
	return  SUCCESS;
}

/* 将Q清为空队列 */
Status ClearQueue(SqQueue *Q)
{
	Q->front=Q->rear=0;
	return SUCCESS;
}

/* 若队列Q为空队列,则返回SUCCESS,否则返回ERROR */
Status QueueEmpty(SqQueue Q)
{ 
	if(Q.front==Q.rear) /* 队列空的标志 */
		return SUCCESS;
	else
		return ERROR;
}

/* 返回Q的元素个数，也就是队列的当前长度 */
int QueueLength(SqQueue Q)
{
	return  (Q.rear-Q.front+MAXSIZE)%MAXSIZE;
}

/* 若队列不空,则用e返回Q的队头元素,并返回SUCCESS,否则返回ERROR */
Status GetHead(SqQueue Q,QElemType *e)
{
	if(Q.front==Q.rear) /* 队列空 */
		return ERROR;
	*e=Q.data[Q.front];
	return SUCCESS;
}

/* 若队列未满，则插入元素e为Q新的队尾元素 */
Status EnQueue(SqQueue *Q,QElemType e)
{
	if ((Q->rear+1)%MAXSIZE == Q->front)	/* 队列满的判断 */
		return ERROR;
	Q->data[Q->rear]=e;			/* 将元素e赋值给队尾 */
	Q->rear=(Q->rear+1)%MAXSIZE;/* rear指针向后移一位置， */
								/* 若到最后则转到数组头部 */
	return  SUCCESS;
}

/* 若队列不空，则删除Q中队头元素，用e返回其值 */
Status DeQueue(SqQueue *Q,QElemType *e)
{
	if (Q->front == Q->rear)			/* 队列空的判断 */
		return ERROR;
	*e=Q->data[Q->front];				/* 将队头元素赋值给e */
	Q->front=(Q->front+1)%MAXSIZE;	/* front指针向后移一位置， */
									/* 若到最后则转到数组头部 */
	return  SUCCESS;
}

// 遍历，另有他用 
//Status visit(QElemType c)
//{
//	printf("%d ",c);
//	return SUCCESS;
//}
///* 从队头到队尾依次对队列Q中每个元素输出 */
//Status QueueTraverse(SqQueue Q)
//{ 
//	int i;
//	i=Q.front;
//	while((i+Q.front)!=Q.rear)
//	{
//		visit(Q.data[i]);
//		i=(i+1)%MAXSIZE;
//	}
//	printf("\n");
//	return SUCCESS;
//}
