// 定时读取传感器数据
fopen();
f_write(mes);
fclose();

// 定时读取传感器消息
fopen();
size = read_size();
f_read(size)

    send_pic();
waitresponse();
if (ok)
    本分数据信息
else
{
    resent();
    while (1)
    {
        cnt++ if (cnt > 3)
        {
            if (flag == 0)
            {
                flag = 1;
                restart();
                cnt = 0;
            }
            else
            {
                return Network_anomaly;
                // 下一步就要备份缓存区
                // 更新下次唤醒时间

                // check 是否配置，手动控制！
                // 44444444/DebugChangle
                // analy(json)
                // not sleep induction 10min
                // takephoto() & updata
                // take sen() & updata
            }
        }
    }
}

send_pic();
// 每次传送结束后，则将张片转移至另一个文件夹中
// 如果传输过程网络异常，做多reset硬件一次
// 若仍然失败
return Network_anomaly;
//然后保存现场
//1
将所有“暂存区”转移到“重发区”
    //2
    save_resent_area()
{
    f_open(cache_file.data)
        len = queue_leng(Q_resent)
            visit() : f_printf(uid, pid, packid\r\n);
    Dequeue();
}
//3
下个整点唤醒；整点唤醒的效果是显而易见的，最大程度避免冲突！

// 对应的，启动读取缓存区
if (f_open == 0)
{
    f_scanf()
        ENQueue()
    //入队列
}

1.原因A 清除flag 2.原因B 采集sensor 3.原因C 网络心跳包；电压 4.原因D 获取对时！ 3.原因C 发送传感器数据，简单传感器数据最好整点 4.原因D 发送传感器数据

    加入一些flag,
    根据情景判断是否发送 1.立即发送 2.转存发送

    合适check
        写入的时候check
    ?

    // ps 无论是否成功，文件将被写入到backfile

    //每天一个文件


// 关于resent部分
每次读取下一张图片之前，还是发送一张图片之时！

需要一个变量保存当前的一些环境信息：
u16 now_pic_id
u16 now_pack_id

now_pic_id 

if(Q_resent == full)
{
    d=get_head_of_Q(resent_Q)
    if(now_pic_id == d.pic_id)
    {
        
        
    }
    else
    {
        f_close(now_pic_id);
        f_open(d.pic_id);
    }

    offset = caculate_offset(d.pack_id);
    pack_len = caculate_pack_len(d.pack_id)
        change_pp(offset);
    f_read(file, pp, len);

    send();
    // 此处有重发机制

    if(success)
        DeQueue(d);
        EnQueue(stage)
;
EnQueue(stage)
            check Q_resent
}

then

    fn 是通过扫描后获得的！
    id_pic = srt2integer_pro(fn);
"0:INBOX/

    strcat((char *)srcpathname, (const char *)psrc); // ???????????????·??

DIR *srcdir = 0; // 源目录

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

srcdir = (DIR *)mymalloc(SRAMIN, sizeof(DIR)); // 申请内存

if (srcdir == NULL || dstdir == NULL || finfo == NULL)
{
    res = 100;
}

if (res == 0)
{

    dstpathname = mymalloc(SRAMIN, MAX_PATHNAME_DEPTH);
    fn_t = mymalloc(SRAMIN, 30); // #speical


    fn_t = mymalloc(SRAMIN, 30); // #speical

    u8 mf_dcopy(u8 * psrc, u8 * pdst, u8 fwmode)
    {
        u8 cnt = 0;

        u8 res = 0;
        DIR *srcdir = 0;    // 源目录

        FILINFO *finfo = 0; // 文件信息
        u8 *fn = 0;         // 长文件名

        u8 *dstpathname = 0; // 目标文件夹路径+文件名
        u8 *srcpathname = 0; // 源文件夹路径+文件名

        u8 *fn_t = 0; // #speical目标文件夹路径+文件名

        u16 dstpathlen = 0; // 目标路径长度
        u16 srcpathlen = 0; // 源路径长度



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
                srcpathname[0] = 0;

                strcat((char *)srcpathname, "0:INBOX"); // 复制原始源文件路径
                strcat((char *)srcpathname, (const char *)"/"); // 源路径加斜杠
                sprintf((char *)fn_t, "IMAG%05d.JPG", id_in_flash);
                strcat((char *)srcpathname, (const char *)fn_t); // 目标路径加文件名
                printf("file name: %s\r\n", srcpathname);

                

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
