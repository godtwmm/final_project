#include<reg52.h> //包含頭文件，一般情況不需要改動，頭文件包含特殊功能寄存器的定義                        
#define DataPort P0 
#define MAX 10
unsigned char buf[MAX];
unsigned char head = 0;
//unsigned char recv_flag = 0;  // 接收完成標誌


sbit LATCH1 = P2^2;  
sbit LATCH2 = P2^3;  

unsigned char code dofly_DuanMa[11]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x80};// 顯示段碼值0~9
unsigned char code dofly_WeiMa[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};//分別對應相應的數碼管點亮,即位碼
unsigned char TempData[8];
/*------------------------------------------------
                   函數聲明
------------------------------------------------*/
void SendStr(unsigned char *s);
void SendByte(unsigned char dat);
void delay_ms(unsigned int ms);
void Display(unsigned char FirstBit, unsigned char Num);
void ProcessReceivedData(void);
/*------------------------------------------------
                    串口初始化
------------------------------------------------*/
void InitUART  (void)
{

    SCON  = 0x50;		        // SCON: 模式 1, 8-bit UART, 使能接收  
    TMOD |= 0x20;               // TMOD: timer 1, mode 2, 8-bit 重裝
    TH1   = 0xFD;               // TH1:  重裝值 9600 波特率 晶振 11.0592MHz  
    TR1   = 1;                  // TR1:  timer 1 打開                         
    EA    = 1;                  //打開總中斷
   // ES    = 1;                  //打開串口中斷
}                            
/*------------------------------------------------
                    主函數
------------------------------------------------*/
void main (void)
{

InitUART();

ES    = 1;                  //打開串口中斷
while (1)                       
    {
	    if(head == 5) {
		    buf[head] = '\0'; 
			head = 0;
            ProcessReceivedData();
        }
        Display(0, 5);  // 顯示前5位數碼管    
    }
}

/*------------------------------------------------
                    發送一個字節
------------------------------------------------*/
void SendByte(unsigned char dat)
{
 SBUF = dat;
 while(!TI);
      TI = 0;
}
/*------------------------------------------------
                    發送一個字符串
------------------------------------------------*/
void SendStr(unsigned char *s)
{
 while(*s!='\0')// \0 表示字符串結束標誌，通過檢測是否字符串末尾
  {
  SendByte(*s);
  s++;
  }
}
/*------------------------------------------------
                     串口中斷程序
------------------------------------------------*/
void UART_SER (void) interrupt 4 //串行中斷服務程序
{
    unsigned char Temp;          //定義臨時變量 
   
   if(RI)                        //判斷是接收中斷產生
     {
	  	RI=0;                      //標誌位清零
	  	Temp=SBUF;                 //讀入緩衝區的值
	//  P1=Temp;                   //把值輸出到P1口，用於觀察
    //  SBUF=Temp;                 //把接收到的值再發回電腦端
	  	buf[head] = Temp;
		head++;
		if (head == MAX) head = 0;
	 }
//   if(TI)                        //如果是發送標誌位，清零
//     TI=0;
}

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for(i = 0; i < ms; i++)
    {
        for(j = 0; j < 120; j++)
        {
            ;  // 空循環，用來耗時
        }
    }
}

void Display(unsigned char FirstBit, unsigned char Num)
{
    unsigned char i;

    for(i = 0; i < Num; i++)
    { 
        DataPort = 0;   
        LATCH1 = 1;     
        LATCH1 = 0;

        DataPort = dofly_WeiMa[i + FirstBit];  
        LATCH2 = 1;     
        LATCH2 = 0;

        DataPort = TempData[i];  
        LATCH1 = 1;     
        LATCH1 = 0;
       
        delay_ms(200);     
    }
}

void ProcessReceivedData(void)
{
    // 直接拆分前5個字符
    char ten = buf[0];      // 十位數字
    char unit = buf[1];     // 個位數字
    char dot = buf[2];      // 小數點
    char tenth = buf[3];    // 十分位
    char hundredth = buf[4];// 百分位
    
    // 十位數字
    if(ten >= '0' && ten <= '9') {
        TempData[0] = dofly_DuanMa[ten - '0'];
    } else {
        TempData[0] = 0x00; // 空白
    }
    
    // 個位數字
    if(unit >= '0' && unit <= '9') {
        TempData[1] = dofly_DuanMa[unit - '0'];
    } else {
        TempData[1] = 0x00; // 空白
    }
    
    // 小數點
    if(dot == '.') {
        TempData[2] = dofly_DuanMa[10]; // 小數點段碼
    } else {
        TempData[2] = 0x00; // 空白
    }
    
    // 十分位
    if(tenth >= '0' && tenth <= '9') {
        TempData[3] = dofly_DuanMa[tenth - '0'];
    } else {
        TempData[3] = 0x00; // 空白
    }
    
    // 百分位
    if(hundredth >= '0' && hundredth <= '9') {
        TempData[4] = dofly_DuanMa[hundredth - '0'];
    } else {
        TempData[4] = 0x00; // 空白
    }

}
