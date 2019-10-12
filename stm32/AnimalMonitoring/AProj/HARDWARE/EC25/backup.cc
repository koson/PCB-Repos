//mf_scan_files("0:INBOX");
//	//sta=mf_copy("0:SEND/IMAG102.JPG","0:SEND/IMAG105.JPG",1);
//	//printf("sta=%d\r\n",sta);
//	sta = mf_dcopy("0:IMG","0:INBOX",0);
//	printf("sta=%d\r\n",sta);
//	mf_scan_files("0:INBOX");
//	sta = mf_send_pics("0:INBOX","0:ARCH",1);
//	printf("sta=%d\r\n",sta);
//	mf_scan_files("0:INBOX");
//	mf_scan_files("0:ARCH");
//mf_scan_pictures((TCHAR*)"0:SEND2",  filestest);
//sta=mf_copy("0:SEND/IMAG102.JPG","0:SEND2/IMAG104.JPG",1);
//printf("sta=%d\r\n",sta);
//mf_scan_pictures((TCHAR*)"0:SEND1",  filestest);

ucosiii
    创建任务，并进入临界区

        OS_ERR err;
CPU_SR_ALLOC();

OSInit(&err);        //初始化UCOSIII
OS_CRITICAL_ENTER(); //进入临界区

//OSTaskCreate
OS_CRITICAL_EXIT(); //退出临界区
OSStart(&err);      //开启UCOSIII

// 获取pic的ID并改名！
id_pic = srt2integer_pro(fn); // 获取文件名字的ID ,源，名字不变

dest 的名字非fn
    pic_now = STMFLASH_Read_Num(FLASH_SAVE_ADDR); // 获取当前名字的ID
sprintf(labelnum, "IMAG%05d.JPG", pic_now);       // 获取当前名字的ID

pic_now++;
STMFLASH_Write_Num(FLASH_SAVE_ADDR, pic_now);

// 临时用一个全局变量来控制是否改名字

u8 mf_dcopy(u8 *psrc, u8 *pdst, u8 fwmode)
{
#define MAX_PATHNAME_DEPTH 100 + 1 //最大目标文件路径+文件名深度
    u8 res = 0;
    DIR *srcdir = 0;    // 源目录
    DIR *dstdir = 0;    // 源目录
    FILINFO *finfo = 0; // 文件信息
    u8 *fn = 0;         // 长文件名

    u8 *dstpathname = 0; // 目标文件夹路径+文件名
    u8 *srcpathname = 0; // 源文件夹路径+文件名

    u16 dstpathlen = 0; // 目标路径长度
    u16 srcpathlen = 0; // 源路径长度

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
                    if (res != FR_OK || finfo->fname[0] == 0)
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
                        strcat((char *)dstpathname, (const char *)fn);  // 目标路径加文件名
                        strcat((char *)srcpathname, (const char *)fn);  // 源路径加文件名
                        printf("\r\ncopy file %s to %s\r\n", srcpathname,
                               dstpathname);                       // 拷贝文件
                        mf_copy(srcpathname, dstpathname, fwmode); // 复制文件
						mf_unlink(srcpathname);					   // 删除文件
						printf("rm %s\r\n",srcpathname);
                    }
                    srcpathname[srcpathlen] = 0; // 加入结束符
                    dstpathname[dstpathlen] = 0; // 加入结束符
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

CPU_SR_ALLOC();
OS_CRITICAL_ENTER(); //进入临界区
OS_CRITICAL_EXIT();  

// 申请内存，

释放内存