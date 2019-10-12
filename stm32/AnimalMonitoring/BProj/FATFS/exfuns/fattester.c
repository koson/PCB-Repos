#include "fattester.h"	 
#include "sdio_sdcard.h"
#include "usmart.h"
#include "usart.h"
#include "exfuns.h"
#include "malloc.h"		  
#include "ff.h"
#include "string.h"
#include "mqttApp.h"
#include "stmflash.h"  // for pic id
#include "iwdg.h" // watchdog
//为磁盘注册工作区	 
//path:磁盘路径，比如"0:"、"1:"
//mt:0，不立即注册（稍后注册）；1，立即注册
//返回值:执行结果
u8 mf_mount(u8* path,u8 mt)
{		   
	return f_mount(fs[2],(const TCHAR*)path,mt); 
}
//打开路径下的文件
//path:路径+文件名
//mode:打开模式
//返回值:执行结果
u8 mf_open(u8*path,u8 mode)
{
	u8 res;	 
	res=f_open(file,(const TCHAR*)path,mode);//打开文件夹
	return res;
} 
//关闭文件
//返回值:执行结果
u8 mf_close(void)
{
	f_close(file);
	return 0;
}
//读出数据
//len:读出的长度
//返回值:执行结果
u8 mf_read(u16 len)
{
	u16 i,t;
	u8 res=0;
	u16 tlen=0;
	printf("\r\nRead file data is:\r\n");
	for(i=0;i<len/512;i++)
	{
		res=f_read(file,fatbuf,512,&br);
		if(res)
		{
			printf("Read Error:%d\r\n",res);
			break;
		}else
		{
			tlen+=br;
			for(t=0;t<br;t++)printf("%c",fatbuf[t]); 
		}
	}
	if(len%512)
	{
		res=f_read(file,fatbuf,len%512,&br);
		if(res)	//读数据出错了
		{
			printf("\r\nRead Error:%d\r\n",res);   
		}else
		{
			tlen+=br;
			for(t=0;t<br;t++)printf("%c",fatbuf[t]); 
		}	 
	}
	if(tlen)printf("\r\nReaded data len:%d\r\n",tlen);//读到的数据长度
	printf("Read data over\r\n");	 
	return res;
}
//写入数据
//dat:数据缓存区
//len:写入长度
//返回值:执行结果
u8 mf_write(u8*dat,u16 len)
{			    
	u8 res;	   					   

	printf("\r\nBegin Write file...\r\n");
	printf("Write data len:%d\r\n",len);	 
	res=f_write(file,dat,len,&bw);
	if(res)
	{
		printf("Write Error:%d\r\n",res);   
	}else printf("Writed data len:%d\r\n",bw);
	printf("Write data over.\r\n");
	return res;
}

//打开目录
 //path:路径
//返回值:执行结果
u8 mf_opendir(u8* path)
{
	return f_opendir(&dir,(const TCHAR*)path);	
}
//关闭目录 
//返回值:执行结果
u8 mf_closedir(void)
{
	return f_closedir(&dir);	
}
//打读取文件夹
//返回值:执行结果
u8 mf_readdir(void)
{
	u8 res;
	char *fn;			 
#if _USE_LFN
 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = mymalloc(SRAMIN,fileinfo.lfsize);
#endif		  
	res=f_readdir(&dir,&fileinfo);//读取一个文件的信息
	if(res!=FR_OK||fileinfo.fname[0]==0)
	{
		myfree(SRAMIN,fileinfo.lfname);
		return res;//读完了.
	}
#if _USE_LFN
	fn=*fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
#else
	fn=fileinfo.fname;;
#endif	
	printf("\r\n DIR info:\r\n");

	printf("dir.id:%d\r\n",dir.id);
	printf("dir.index:%d\r\n",dir.index);
	printf("dir.sclust:%d\r\n",dir.sclust);
	printf("dir.clust:%d\r\n",dir.clust);
	printf("dir.sect:%d\r\n",dir.sect);	  

	printf("\r\n");
	printf("File Name is:%s\r\n",fn);
	printf("File Size is:%d\r\n",fileinfo.fsize);
	printf("File data is:%d\r\n",fileinfo.fdate);
	printf("File time is:%d\r\n",fileinfo.ftime);
	printf("File Attr is:%d\r\n",fileinfo.fattrib);
	printf("\r\n");
	myfree(SRAMIN,fileinfo.lfname);
	return 0;
}			 

 //遍历文件
 //path:路径
 //返回值:执行结果
u8 mf_scan_files(u8 * path)
{
	FRESULT res;	  
    char *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = mymalloc(SRAMIN,fileinfo.lfsize);
#endif		  

    res = f_opendir(&dir,(const TCHAR*)path); //打开一个目录
    if (res == FR_OK) 
	{	
		printf("\r\n"); 
		while(1)
		{
	        res = f_readdir(&dir, &fileinfo);                   //读取目录下的一个文件
	        if (res != FR_OK || fileinfo.fname[0] == 0) break;  //错误了/到末尾了,退出
	        //if (fileinfo.fname[0] == '.') continue;             //忽略上级目录
#if _USE_LFN
        	fn = *fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
#else							   
        	fn = fileinfo.fname;
#endif	                                              /* It is a file. */
			printf("%s/", path);//打印路径	
			printf("%s\r\n",  fn);//打印文件名	  
		} 
    }	  
	myfree(SRAMIN,fileinfo.lfname);
    return res;	  
}
//显示剩余容量
//drv:盘符
//返回值:剩余容量(字节)
u32 mf_showfree(u8 *drv)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //得到磁盘信息及空闲簇数量
    res = f_getfree((const TCHAR*)drv,(DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect = (fs1->n_fatent - 2) * fs1->csize;//得到总扇区数
	    fre_sect = fre_clust * fs1->csize;			//得到空闲扇区数	   
#if _MAX_SS!=512
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		if(tot_sect<20480)//总容量小于10M
		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		    printf("\r\n磁盘总容量:%d KB\r\n"
		           "可用空间:%d KB\r\n",
		           tot_sect>>1,fre_sect>>1);
		}else
		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		    printf("\r\n磁盘总容量:%d MB\r\n"
		           "可用空间:%d MB\r\n",
		           tot_sect>>11,fre_sect>>11);
		}
	}
	return fre_sect;
}		    
//文件读写指针偏移
//offset:相对首地址的偏移量
//返回值:执行结果.
u8 mf_lseek(u32 offset)
{
	return f_lseek(file,offset);
}
//读取文件当前读写指针的位置.
//返回值:位置
u32 mf_tell(void)
{
	return f_tell(file);
}
//读取文件大小
//返回值:文件大小
u32 mf_size(void)
{
	return f_size(file);
} 
//创建目录
//pname:目录路径+名字
//返回值:执行结果
u8 mf_mkdir(u8*pname)
{
	return f_mkdir((const TCHAR *)pname);
}
//格式化
//path:磁盘路径，比如"0:"、"1:"
//mode:模式
//au:簇大小
//返回值:执行结果
u8 mf_fmkfs(u8* path,u8 mode,u16 au)
{
	return f_mkfs((const TCHAR*)path,mode,au);//格式化,drv:盘符;mode:模式;au:簇大小
} 
//删除文件/目录
//pname:文件/目录路径+名字
//返回值:执行结果
u8 mf_unlink(u8 *pname)
{
	return  f_unlink((const TCHAR *)pname);
}

//修改文件/目录名字(如果目录不同,还可以移动文件哦!)
//oldname:之前的名字
//newname:新名字
//返回值:执行结果
u8 mf_rename(u8 *oldname,u8* newname)
{
	return  f_rename((const TCHAR *)oldname,(const TCHAR *)newname);
}
//获取盘符（磁盘名字）
//path:磁盘路径，比如"0:"、"1:"  
void mf_getlabel(u8 *path)
{
	u8 buf[20];
	u32 sn=0;
	u8 res;
	res=f_getlabel ((const TCHAR *)path,(TCHAR *)buf,(DWORD*)&sn);
	if(res==FR_OK)
	{
		printf("\r\n磁盘%s 的盘符为:%s\r\n",path,buf);
		printf("磁盘%s 的序列号:%X\r\n\r\n",path,sn); 
	}else printf("\r\n获取失败，错误码:%X\r\n",res);
}
//设置盘符（磁盘名字），最长11个字符！！，支持数字和大写字母组合以及汉字等
//path:磁盘号+名字，比如"0:ALIENTEK"、"1:OPENEDV"  
void mf_setlabel(u8 *path)
{
	u8 res;
	res=f_setlabel ((const TCHAR *)path);
	if(res==FR_OK)
	{
		printf("\r\n磁盘盘符设置成功:%s\r\n",path);
	}else printf("\r\n磁盘盘符设置失败，错误码:%X\r\n",res);
} 

//从文件里面读取一段字符串
//size:要读取的长度
void mf_gets(u16 size)
{
 	TCHAR* rbuf;
	rbuf=f_gets((TCHAR*)fatbuf,size,file);
	if(*rbuf==0)return  ;//没有数据读到
	else
	{
		printf("\r\nThe String Readed Is:%s\r\n",rbuf);  	  
	}			    	
}
//需要_USE_STRFUNC>=1
//写一个字符到文件
//c:要写入的字符
//返回值:执行结果
u8 mf_putc(u8 c)
{
	return f_putc((TCHAR)c,file);
}
//写字符串到文件
//c:要写入的字符串
//返回值:写入的字符串长度
u8 mf_puts(u8*c)
{
	return f_puts((TCHAR*)c,file);
}

// 能够自动提取字符串中的数字
u32 srt2integer_pro(u8 *str)
{
    u8 len = 0;
    u8 i = 0;
    u8 buf[20] = {0};
    u32 wi = 1;
    u32 sum = 0;

    while (str[i])
    {
        if (str[i] >= '0' && str[i] <= '9')
        {
            buf[len] = str[i];
            //printf("%c",buf[len]);
            len++;
        }
        i++;
    }

    for (i = 0; i < len; i++)
    {
        sum = sum + (buf[len - 1 - i] - '0') * wi;
        wi *= 10;
    }
    return sum;
}



/** 
 * @brief send pictures of assigned path(folder)
 * @param psrc	源文件夹
 * @param pdst	归档文件夹
 * @param fwmode	
 *		<>1<> 覆盖模式
 *		<>0<> 非覆盖模式
 *
 * @return num of files
 * @note  max 20
 */
u8 mf_send_pics(u8 *psrc, u8 *pdst, u8 fwmode)
{
#define MAX_PATHNAME_DEPTH 100 + 1 //最大目标文件路径+文件名深度
	u8 cnt = 0;
	
	u8 res = 0;
    DIR *srcdir = 0;    // 源目录
    DIR *dstdir = 0;    // 源目录
    FILINFO *finfo = 0; // 文件信息
    u8 *fn = 0;         // 长文件名

    u8 *dstpathname = 0; // 目标文件夹路径+文件名
    u8 *srcpathname = 0; // 源文件夹路径+文件名

    u16 dstpathlen = 0; // 目标路径长度
    u16 srcpathlen = 0; // 源路径长度
	u32 id_pic;

    srcdir = (DIR *)mymalloc(SRAMIN, sizeof(DIR)); // 申请内存
    dstdir = (DIR *)mymalloc(SRAMIN, sizeof(DIR));
    finfo = (FILINFO *)mymalloc(SRAMIN, sizeof(FILINFO));

    if (srcdir == NULL || dstdir == NULL || finfo == NULL)
    {
        res = 100;
    }
    if (res == 0)
    {
        finfo->lfsize = _MAX_LFN * 2 + 1;
        finfo->lfname = mymalloc(SRAMIN, finfo->lfsize); // 申请内存
        dstpathname = mymalloc(SRAMIN, MAX_PATHNAME_DEPTH);
        srcpathname = mymalloc(SRAMIN, MAX_PATHNAME_DEPTH);
        if (finfo->lfname == NULL || dstpathname == NULL || srcpathname == NULL)
        {
            res = 101;
        }
        if (res == 0)
        {
            dstpathname[0] = 0;
            srcpathname[0] = 0;
            strcat((char *)srcpathname, (const char *)psrc); // 复制原始源文件路径
            strcat((char *)dstpathname, (const char *)pdst); // 复制原始目标文件路径
            res = f_opendir(srcdir, (const TCHAR *)psrc);    // 打开源目录
            if (res == 0)                                    // 打开目录成功
            {
                res = f_mkdir((const TCHAR *)dstpathname);
                // 如果文件夹已经存在,就不创建.如果不存在就创建新的文件夹.
                if (res == FR_EXIST)
                {
                    res = 0;
                }
                while (res == 0) // 开始复制文件夹里面的东东
                {
                    res = f_readdir(srcdir, finfo); // 读取目录下的一个文件
                    if (res != FR_OK || finfo->fname[0] == 0 || cnt > 3)
                    {
                        break;
                    } // 错误了/到末尾了,退出
                    if (finfo->fname[0] == '.')
                    {
                        continue;
                    } // 忽略上级目录
                    fn = (u8 *)(*finfo->lfname ? finfo->lfname : finfo->fname);
					id_pic=srt2integer_pro(fn);
                    // 得到文件名
                    dstpathlen = strlen((const char *)dstpathname); // 得到当前目标路径的长度
                    srcpathlen = strlen((const char *)srcpathname); // 得到源路径长度

                    strcat((char *)srcpathname, (const char *)"/"); // 源路径加斜杠
                    if (finfo->fattrib & 0X10)                      // 是子目录   文件属性,0X20,归档文件;0X10,子目录;
                    {
                        continue;
                    }    // 忽略子目录
                    else // 非目录
                    {
						u8 sta;
                        strcat((char *)dstpathname, (const char *)"/"); // 目标路径加斜杠
                        strcat((char *)dstpathname, (const char *)fn);  // 目标路径加文件名
                        strcat((char *)srcpathname, (const char *)fn);  // 源路径加文件名
                        printf("\r\nmove file %s to %s\r\n", srcpathname, dstpathname);
						//CPU_SR_ALLOC();
						//OS_CRITICAL_ENTER(); //进入临界区
						sta = mysend_picture(srcpathname, id_pic);	   // 发送文件
						//OS_CRITICAL_EXIT();  
						
                        mf_copy(srcpathname, dstpathname, fwmode); // 复制文件
						mf_unlink(srcpathname);					   // 删除文件
						printf("rm %s\r\n",srcpathname);
						//mycheck_Queue();
						cnt++;
						if(sta != 0) {
							printf("ERROR mysend_picture:%s,%d\r\n",srcpathname,id_pic);
							break;
						}
                    }
                    srcpathname[srcpathlen] = 0; // 加入结束符
                    dstpathname[dstpathlen] = 0; // 加入结束符
					
					// break;
                }
            }
            myfree(SRAMIN, dstpathname);
            myfree(SRAMIN, srcpathname);
            myfree(SRAMIN, finfo->lfname);
        }
    }
    myfree(SRAMIN, srcdir);
    myfree(SRAMIN, dstdir);
    myfree(SRAMIN, finfo);
    return res;
}

static vu8 mf_id_ctrl;

// 文件复制
// 将psrc文件,copy到pdst.
// psrc,pdst:源文件和目标文件
// fwmode:文件写入模式
// 0:不覆盖原有的文件
// 1:覆盖原有的文件
u8 mf_copy(u8 *psrc, u8 *pdst, u8 fwmode)
{
    u8 res;
    u16 br = 0;
    u16 bw = 0;
    FIL *fsrc = 0;
    FIL *fdst = 0;
    u8 *fbuf = 0;
    fsrc = (FIL *)mymalloc(SRAMIN, sizeof(FIL)); // 申请内存
    fdst = (FIL *)mymalloc(SRAMIN, sizeof(FIL));
    fbuf = (u8 *)mymalloc(SRAMIN, 512);
    
	//喂狗
	IWDG_Feed();//喂狗
	//
	
	if (fsrc == NULL || fdst == NULL || fbuf == NULL)
    {
        res = 100;
    } // 前面的值留给fatfs
    else
    {
        if (fwmode == 0)
        {
            fwmode = FA_CREATE_NEW;
        } // 不覆盖
        else
        {
            fwmode = FA_CREATE_ALWAYS;
        } // 覆盖存在的文件

        res = f_open(fsrc, (const TCHAR *)psrc, FA_READ | FA_OPEN_EXISTING);
        // 打开只读文件
        if (res == 0)
        {
            res = f_open(fdst, (const TCHAR *)pdst, FA_WRITE | fwmode);
        }             // 第一个打开成功,才开始打开第二个
        if (res == 0) // 两个都打开成功了
        {
            while (res == 0) // 开始复制
            {
                res = f_read(fsrc, fbuf, 512, (UINT *)&br); // 源头读出512字节
                if (res || br == 0)
                {
                    break;
                }
                res = f_write(fdst, fbuf, (UINT)br, (UINT *)&bw); // 写入目的文件
                if (res || bw < br)
                {
                    break;
                }
            }
            f_close(fsrc);
            f_close(fdst);
        }
    }
    myfree(SRAMIN, fsrc); // 释放内存
    myfree(SRAMIN, fdst);
    myfree(SRAMIN, fbuf);
    return res;
}

// 得到路径下的文件夹
// 返回值:0,路径就是个卷标号.
// 其他,文件夹名字首地址
u8* get_src_dname(u8* dpfn)
{
	u16 temp=0;
 	while(*dpfn!=0)
	{
		dpfn++;
		temp++;	
	}
	printf("temp=%d\r\n",temp);
	if(temp<4)return 0; 
	while((*dpfn!=0x5c)&&(*dpfn!=0x2f)&&(*dpfn!=':')) {	
		dpfn--;
		temp--;	//追述到倒数第一个"\"或者"/"处 
		printf("dpfn[%d]=%c\r\n",temp,*dpfn);
	}
	printf("dpfn=%d\r\n",++temp);
	return ++dpfn;
}

// 文件夹复制
// 将psrc文件夹,copy到pdst文件夹.
// pdst:必须形如"X:"/"X:XX"/"X:XX/XX"之类的.而且要实现确认上一级文件夹存在
// psrc,pdst:源文件夹和目标文件夹
// fwmode:文件写入模式
// 0:不覆盖原有的文件
// 1:覆盖原有的文件
u8 mf_dcopy(u8 *psrc, u8 *pdst, u8 fwmode)
{
#define MAX_PATHNAME_DEPTH 100 + 1 //最大目标文件路径+文件名深度
    u8 cnt=0;
	
	u8 res = 0;
    DIR *srcdir = 0;    // 源目录
    DIR *dstdir = 0;    // 源目录
    FILINFO *finfo = 0; // 文件信息
    u8 *fn = 0;         // 长文件名

    u8 *dstpathname = 0; // 目标文件夹路径+文件名
    u8 *srcpathname = 0; // 源文件夹路径+文件名
	
	u8 *fn_t = 0; // #speical目标文件夹路径+文件名

    u16 dstpathlen = 0; // 目标路径长度
    u16 srcpathlen = 0; // 源路径长度

	u16 id_in_flash; // #special
	id_in_flash = STMFLASH_Read_Num(FLASH_SAVE_ADDR); // ##special
    
	srcdir = (DIR *)mymalloc(SRAMIN, sizeof(DIR)); // 申请内存
    dstdir = (DIR *)mymalloc(SRAMIN, sizeof(DIR));
    finfo = (FILINFO *)mymalloc(SRAMIN, sizeof(FILINFO));

    if (srcdir == NULL || dstdir == NULL || finfo == NULL)
    {
        res = 100;
    }
    if (res == 0)
    {
        finfo->lfsize = _MAX_LFN * 2 + 1;
        finfo->lfname = mymalloc(SRAMIN, finfo->lfsize); // 申请内存
        dstpathname = mymalloc(SRAMIN, MAX_PATHNAME_DEPTH);
        fn_t = mymalloc(SRAMIN, 30); // #speical
		srcpathname = mymalloc(SRAMIN, MAX_PATHNAME_DEPTH);
        if (finfo->lfname == NULL || dstpathname == NULL || srcpathname == NULL)
        {
            res = 101;
        }
        if (res == 0)
        {
            dstpathname[0] = 0;
            srcpathname[0] = 0;
            strcat((char *)srcpathname, (const char *)psrc); // 复制原始源文件路径
            strcat((char *)dstpathname, (const char *)pdst); // 复制原始目标文件路径
            res = f_opendir(srcdir, (const TCHAR *)psrc); // 打开源目录
            if (res == 0)                                 // 打开目录成功
            {
                res = f_mkdir((const TCHAR *)dstpathname);
                // 如果文件夹已经存在,就不创建.如果不存在就创建新的文件夹.
                if (res == FR_EXIST)
                {
                    res = 0;
                }
                while (res == 0) // 开始复制文件夹里面的东东
                {
                    res = f_readdir(srcdir, finfo); // 读取目录下的一个文件
                    if (res != FR_OK || finfo->fname[0] == 0 || cnt>3)
                    {
                        break;
                    } // 错误了/到末尾了,退出
                    if (finfo->fname[0] == '.')
                    {
                        continue;
                    } // 忽略上级目录
                    fn = (u8 *)(*finfo->lfname ? finfo->lfname : finfo->fname);
                    // 得到文件名
                    dstpathlen = strlen((const char *)dstpathname); // 得到当前目标路径的长度
                    srcpathlen = strlen((const char *)srcpathname); // 得到源路径长度
                    strcat((char *)srcpathname, (const char *)"/"); // 源路径加斜杠
                    if (finfo->fattrib & 0X10)                      // 是子目录   文件属性,0X20,归档文件;0X10,子目录;
                    {
                        continue;
                    } // 忽略子目录
                    else // 非目录
                    {
                        strcat((char *)dstpathname, (const char *)"/"); // 目标路径加斜杠
                        strcat((char *)srcpathname, (const char *)fn);  // 源路径加文件名
                        sprintf((char *)fn_t, "IMAG%04d.JPG", id_in_flash); 
						strcat((char *)dstpathname, (const char *)fn_t);  // 目标路径加文件名

                        printf("\r\ncopy file %s to %s\r\n", srcpathname,
                               dstpathname);                       // 拷贝文件
                        mf_copy(srcpathname, dstpathname, fwmode); // 复制文件
						
						id_in_flash++;
						STMFLASH_Write_Num(FLASH_SAVE_ADDR, id_in_flash);
						
						mf_unlink(srcpathname);					   // 删除文件
						
						cnt++;
						printf("rm %s,cnt=%d\r\n",srcpathname,cnt);
						
                    }
                    srcpathname[srcpathlen] = 0; // 加入结束符
                    dstpathname[dstpathlen] = 0; // 加入结束符
                }
            }
            myfree(SRAMIN, dstpathname);
            myfree(SRAMIN, srcpathname);
            myfree(SRAMIN, finfo->lfname);
			myfree(SRAMIN, fn_t);
        }
    }
    myfree(SRAMIN, srcdir);
    myfree(SRAMIN, dstdir);
    myfree(SRAMIN, finfo);
    return res;
}
