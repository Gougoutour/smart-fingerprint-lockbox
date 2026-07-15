#include "stm32f10x.h"                  // Device header
#include "FingerSerial.h"
#include "Delay.h"
#include "Serial.h"
/**
	验证用获取图像
*/
uint8_t GetImage_CMD[] =
{
    0xEF,0x01,

    0xFF,0xFF,0xFF,0xFF,

    0x01,

    0x00,0x03,

    0x01,

    0x00,0x05
};
/**
	生成特征
*/
uint8_t GenChar_CMD[]=
{
	0xEF,0x01,

    0xFF,0xFF,0xFF,0xFF,

    0x01,

    0x00,0x04,

    0x02,
	
	0x01,

    0x00,0x08
};

/**
	验证用获取图像
*/
uint8_t RegModel_CMD[] =
{
    0xEF,0x01,	// 包头

    0xFF,0xFF,0xFF,0xFF, // 设备地址

    0x01,	// 包标识

    0x00,0x03,	// 包长度

    0x05,	//指令码

    0x00,0x09	// 校验和
};

/**
	读有效模板个数
*/
uint8_t ValidTempleteNum_CMD[] =
{
    0xEF,0x01,	// 包头

    0xFF,0xFF,0xFF,0xFF, // 设备地址

    0x01,	// 包标识

    0x00,0x03,	// 包长度

    0x1D,	//指令码

    0x00,0x21	// 校验和
};


/**
	搜索指纹
*/
uint8_t Search_CMD[] =
{
    0xEF,0x01,	// 包头

    0xFF,0xFF,0xFF,0xFF, // 设备地址

    0x01,	// 包标识

    0x00,0x08,	// 包长度

    0x04,	//指令码
	
	0x01,  // 缓冲区号，BufferId

	0x00, 0x00,  // 参数 StartPage
	
	0x00, 0x00,  // 参数 PageNum
	
    0x00, 0x00	// 校验和
};

/**
	储存模板
*/
uint8_t StoreChar_CMD[] =
{
    0xEF,0x01,	// 包头

    0xFF,0xFF,0xFF,0xFF, // 设备地址

    0x01,	// 包标识

    0x00,0x06,	// 包长度

    0x06,	//指令码
	
	0x01,  // 缓冲区号，BufferId

	0x00, 0x00,  // 位置号，PageID
	
    0x00,0x00	// 校验和
};


uint8_t Finger_GetImage(void)
{
    uint8_t rx[12];


    //发送获取图像命令
    Finger_SendArray(
        GetImage_CMD,
        sizeof(GetImage_CMD)
    );


    //接收应答包
    for(uint8_t i=0;i<12;i++)
    {
        rx[i]=Finger_ReadByte();
    }

    return rx[9];
}

/**
	生成特征
	bufferId：缓冲区id
*/
uint8_t Finger_GenChar(uint8_t bufferId)
{
    uint8_t rx[12];

	GenChar_CMD[10]=bufferId;
	uint16_t sum;

    sum =
        GenChar_CMD[6]+
        GenChar_CMD[7]+
        GenChar_CMD[8]+
        GenChar_CMD[9]+
        GenChar_CMD[10];


    GenChar_CMD[11]=sum>>8;
    GenChar_CMD[12]=sum&0xff;
    Finger_SendArray(
        GenChar_CMD,
        sizeof(GenChar_CMD)
    );

    for(uint8_t i=0;i<12;i++)
    {
        rx[i]=Finger_ReadByte();
    }


    return rx[9];
}

/**
 * @brief 生成特征
 *
 * @param bufferId 模板缓冲区编号，通常为1或2
 * @return 模块确认码： 0x00-搜索到
						0x01-收包有错
						0x09-没搜索到，页码与得分为0
						0x17-残留指纹或两次采集之间手指没有移动过
*/
uint8_t Finger_Search(uint8_t bufferId,uint16_t StartPage,uint16_t PageNum)
{
    uint8_t rx[16];
    uint16_t sum;

    /* 设置缓冲区编号 */
    Search_CMD[10] = bufferId;

    /* StartPage和PageNum为两个字节，高字节在前 */
    Search_CMD[11] = (uint8_t)(StartPage >> 8);
    Search_CMD[12] = (uint8_t)(StartPage  & 0xFF);

    Search_CMD[13] = (uint8_t)(StartPage >> 8);
    Search_CMD[14] = (uint8_t)(StartPage  & 0xFF);
    /*
     * 校验和：
     * 包标识 + 包长度 + 指令码 + BufferID + PageID
     */
    sum =
        Search_CMD[6] +
        Search_CMD[7] +
        Search_CMD[8] +
        Search_CMD[9] +
        Search_CMD[10] +
        Search_CMD[11] +
        Search_CMD[12] +
		Search_CMD[13] +
		Search_CMD[14];

    Search_CMD[15] = (uint8_t)(sum >> 8);// 右移8位
    Search_CMD[16] = (uint8_t)(sum & 0xFF);

    Finger_SendArray(Search_CMD, sizeof(Search_CMD));

    for (uint8_t i = 0; i < sizeof(rx); i++)
    {
        rx[i] = Finger_ReadByte();
    }

    /* 第9字节为确认码 */
    return rx[9];
}

/**
 * @brief 合成特征（生成模板）
 */
uint8_t Finger_RegModel(void)
{
	uint8_t rx[12];
	Finger_SendArray(
		RegModel_CMD,
		sizeof(RegModel_CMD)
	);

	for(uint8_t i=0;i<12;i++)
    {
        rx[i]=Finger_ReadByte();
    }

    return rx[9];
}

/**
 * @brief 读取有效指纹模板个数
 * @return >=0：有效模板个数
 * @return -1：读取失败或应答包错误
 */
int32_t Finger_ValidTemplateNum(void)
{
    uint8_t rx[14];
    uint16_t checksum;
    uint16_t receivedChecksum;

    Finger_SendArray(
        ValidTempleteNum_CMD,
        sizeof(ValidTempleteNum_CMD)
    );

    for (uint8_t i = 0; i < 14; i++)
    {
        rx[i] = Finger_ReadByte();
    }

    /* 校验包头、包标识和包长度 */
    if (rx[0] != 0xEF ||
        rx[1] != 0x01 ||
        rx[6] != 0x07 ||
        rx[7] != 0x00 ||
        rx[8] != 0x05)
    {
        return -1;
    }

    /* 校验和：包标识 + 包长度 + 数据 */
    checksum = 0;

    for (uint8_t i = 6; i <= 11; i++)
    {
        checksum += rx[i];
    }

    receivedChecksum = ((uint16_t)rx[12] << 8) | rx[13];

    if (checksum != receivedChecksum)
    {
        return -1;
    }

    /* rx[9]为确认码，0x00表示读取成功 */
    if (rx[9] != 0x00)
    {
        return -1;
    }

    /* 有效模板个数为两个字节，高字节在前 */
    return ((uint16_t)rx[10] << 8) | rx[11];
}

/**
 * @brief 储存指纹模板
 *
 * @param bufferId 模板缓冲区编号，通常为1或2
 * @param pageId 指纹库位置号，范围为0~65535
 * @return 模块确认码： 0x00-储存成功
						0x01-收包有错
						0x0b-PageID超出指纹库范围
						0x18-写FLASH出错
 */
uint8_t Finger_StoreChar(uint8_t bufferId, uint16_t pageId)
{
    uint8_t rx[12];
    uint16_t sum;

    /* 设置缓冲区编号 */
    StoreChar_CMD[10] = bufferId;

    /* PageID为两个字节，高字节在前 */
    StoreChar_CMD[11] = (uint8_t)(pageId >> 8);
    StoreChar_CMD[12] = (uint8_t)(pageId & 0xFF);

    /*
     * 校验和：
     * 包标识 + 包长度 + 指令码 + BufferID + PageID
     */
    sum =
        StoreChar_CMD[6] +
        StoreChar_CMD[7] +
        StoreChar_CMD[8] +
        StoreChar_CMD[9] +
        StoreChar_CMD[10] +
        StoreChar_CMD[11] +
        StoreChar_CMD[12];

    StoreChar_CMD[13] = (uint8_t)(sum >> 8);// 右移8位
    StoreChar_CMD[14] = (uint8_t)(sum & 0xFF);

    Finger_SendArray(StoreChar_CMD, sizeof(StoreChar_CMD));

    /* 应答包固定为12字节 */
    for (uint8_t i = 0; i < sizeof(rx); i++)
    {
        rx[i] = Finger_ReadByte();
    }

    /* 第9字节为确认码 */
    return rx[9];
}

/**
 * @brief 手指检测等待函数
*/
void Finger_WaitRelease(void)
{
    uint8_t result;

    while(1)
    {
        result = Finger_GetImage();

        if(result == 0x02)
        {
            return;
        }

        Delay_ms(300);
    }
}
