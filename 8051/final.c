/*-----------------------------------------------
  名稱：串口通信（使用換行符判斷數據結束）
  網站：www.doflye.net
  編寫：shifang（修改後）
  日期：2009.5（修改後）
  內容：
    當UART接收到一條以換行符 ('\n') 結尾的數據，例如 "29.77\n"， 
    程式將自動解析數據，將其轉換成數碼管段碼，
    並持續刷新顯示在數碼管上。
------------------------------------------------*/

#include <reg52.h>
#include <string.h>

#define DataPort P0
#define MAX 10

sbit LATCH1 = P2^2;
sbit LATCH2 = P2^3;

// 定義數碼管段碼：0~9和小數點（共 11 個元素）
unsigned char code dofly_DuanMa[11] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F, // 9
    0x80  // 小數點
};

// 定義位碼（根據你的硬件接線，這裡假設有8位數碼管）
unsigned char code dofly_WeiMa[] = {
    0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F
};

unsigned char TempData[8]; // 數碼管顯示緩衝區

// UART接收緩衝區以及相關全局變量
unsigned char buf[MAX];
unsigned char head = 0;
bit recv_flag = 0;  // 當接收到換行符後設置此標誌

/*------------------------------------------------
    函數聲明
------------------------------------------------*/
void InitUART(void);
void SendByte(unsigned char dat);
void SendStr(unsigned char *s);
void Delay(unsigned int t);
void Display(unsigned char FirstBit, unsigned char Num);

/*------------------------------------------------
    串口初始化 (9600 波特率, 11.0592MHz)
------------------------------------------------*/
void InitUART(void)
{
    SCON  = 0x50;    // 工作模式1：8位 UART，允許接收
    TMOD |= 0x20;    // Timer1，模式2（8位自動重載）
    TH1   = 0xFD;    // 9600 波特率（11.0592MHz晶振）
    TR1   = 1;       // 啟動定時器1
    EA    = 1;       // 全中斷使能
    ES    = 1;       // 打開串口中斷
}

/*------------------------------------------------
    主函數
------------------------------------------------*/
void main(void)
{
    unsigned int j; // 定義用於解析接收到資料的臨時變數
    char ten, unit, dot, tenth, hundredth;

    InitUART();
    ES = 1;  // 打開串口中斷

    while (1)
    {
        // 當接收緩衝區滿 5 個字符時進行處理
        if (head == 5)
        {
            buf[head] = '\0'; // 加入字串結束符
            head = 0;         // 重置接收緩衝區的指針
            
            // 從接收緩衝區讀取當前命令的數據
            ten       = buf[0];  // 十位數字
            unit      = buf[1];  // 個位數字
            dot       = buf[2];  // 小數點
            tenth     = buf[3];  // 十分位
            hundredth = buf[4];  // 百分位

            // 將字符轉換為對應段碼，並存入顯示緩衝區
            if (ten >= '0' && ten <= '9')
                TempData[0] = dofly_DuanMa[ten - '0'];
            else
                TempData[0] = 0x00;

            if (unit >= '0' && unit <= '9')
                TempData[1] = dofly_DuanMa[unit - '0'];
            else
                TempData[1] = 0x00;

            // 小數點處理：只有當字符為 '.' 才顯示小數點段碼
            if (dot == '.')
                TempData[2] = dofly_DuanMa[10];  // 使用定義好的小數點段碼
            else
                TempData[2] = 0x00;

            if (tenth >= '0' && tenth <= '9')
                TempData[3] = dofly_DuanMa[tenth - '0'];
            else
                TempData[3] = 0x00;

            if (hundredth >= '0' && hundredth <= '9')
                TempData[4] = dofly_DuanMa[hundredth - '0'];
            else
                TempData[4] = 0x00;

            // 其他位保持空白
            TempData[5] = TempData[6] = TempData[7] = 0x00;
        }
        
        // 刷新顯示：這裡用一個 for 迴圈持續掃描數碼管，
        // 每次顯示會依次輸出每一位的位碼與段碼
        for (j = 0; j < 100; j++)
        {
            Display(0, 8);  // 顯示 8 位，根據實際需要調整顯示位數
        }
    }


}

/*------------------------------------------------
    串口中斷服務程序
------------------------------------------------*/
void UART_SER(void) interrupt 4
{
    unsigned char Temp;
    
    if (RI)  // 如果發生接收中斷
    {
        RI = 0;       // 清除接收中斷標誌
        Temp = SBUF;  // 讀取接收緩衝器資料
        buf[head] = Temp;
        head++;
        if (head == MAX)
            head = 0;  // 防止緩衝區溢出，超出 MAX 後重新歸零
    }


}

/*------------------------------------------------
    發送一個字節 (UART)
------------------------------------------------*/
void SendByte(unsigned char dat)
{
    SBUF = dat;
    while (!TI);
    TI = 0;
}

/*------------------------------------------------
    發送一個字符串 (UART)
------------------------------------------------*/
void SendStr(unsigned char *s)
{
    while (*s != '\0')
    {
        SendByte(*s);
        s++;
    }
}

/*------------------------------------------------
    毫秒延時函數
------------------------------------------------*/
void Delay(unsigned int t)
{
    while(t--);
}

/*------------------------------------------------
    數碼管顯示掃描函數
    參數：FirstBit：起始位；Num：顯示位數
------------------------------------------------*/
void Display(unsigned char FirstBit, unsigned char Num)
{
    unsigned char i;
    for (i = 0; i < Num; i++)
    {
        DataPort = 0;             // 清空數據口
        LATCH1 = 1;
        LATCH1 = 0;
      
        // 輸出位選信號
        DataPort = dofly_WeiMa[i + FirstBit];
        LATCH2 = 1;
        LATCH2 = 0;
      
        // 輸出段碼數據
        DataPort = TempData[i];
        LATCH1 = 1;
        LATCH1 = 0;
      
        Delay(200);  // 此延時根據硬件需求調整
    }
}
