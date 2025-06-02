#include<reg52.h> //�]�t�Y���A�@�뱡�p���ݭn��ʡA�Y���]�t�S��\��H�s�����w�q                        
#define DataPort P0 
#define MAX 10
unsigned char buf[MAX];
unsigned char head = 0;
//unsigned char recv_flag = 0;  // ���������лx


sbit LATCH1 = P2^2;  
sbit LATCH2 = P2^3;  

unsigned char code dofly_DuanMa[11]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x80};// ��ܬq�X��0~9
unsigned char code dofly_WeiMa[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};//���O�����������ƽX���I�G,�Y��X
unsigned char TempData[8];
/*------------------------------------------------
                   ����n��
------------------------------------------------*/
void SendStr(unsigned char *s);
void SendByte(unsigned char dat);
void delay_ms(unsigned int ms);
void Display(unsigned char FirstBit, unsigned char Num);
void ProcessReceivedData(void);
/*------------------------------------------------
                    ��f��l��
------------------------------------------------*/
void InitUART  (void)
{

    SCON  = 0x50;		        // SCON: �Ҧ� 1, 8-bit UART, �ϯ౵��  
    TMOD |= 0x20;               // TMOD: timer 1, mode 2, 8-bit ����
    TH1   = 0xFD;               // TH1:  ���˭� 9600 �i�S�v ���� 11.0592MHz  
    TR1   = 1;                  // TR1:  timer 1 ���}                         
    EA    = 1;                  //���}�`���_
   // ES    = 1;                  //���}��f���_
}                            
/*------------------------------------------------
                    �D���
------------------------------------------------*/
void main (void)
{

InitUART();

ES    = 1;                  //���}��f���_
while (1)                       
    {
	    if(head == 5) {
		    buf[head] = '\0'; 
			head = 0;
            ProcessReceivedData();
        }
        Display(0, 5);  // ��ܫe5��ƽX��    
    }
}

/*------------------------------------------------
                    �o�e�@�Ӧr�`
------------------------------------------------*/
void SendByte(unsigned char dat)
{
 SBUF = dat;
 while(!TI);
      TI = 0;
}
/*------------------------------------------------
                    �o�e�@�Ӧr�Ŧ�
------------------------------------------------*/
void SendStr(unsigned char *s)
{
 while(*s!='\0')// \0 ��ܦr�Ŧ굲���лx�A�q�L�˴��O�_�r�Ŧ꥽��
  {
  SendByte(*s);
  s++;
  }
}
/*------------------------------------------------
                     ��f���_�{��
------------------------------------------------*/
void UART_SER (void) interrupt 4 //��椤�_�A�ȵ{��
{
    unsigned char Temp;          //�w�q�{���ܶq 
   
   if(RI)                        //�P�_�O�������_����
     {
	  	RI=0;                      //�лx��M�s
	  	Temp=SBUF;                 //Ū�J�w�İϪ���
	//  P1=Temp;                   //��ȿ�X��P1�f�A�Ω��[��
    //  SBUF=Temp;                 //�Ⱶ���쪺�ȦA�o�^�q����
	  	buf[head] = Temp;
		head++;
		if (head == MAX) head = 0;
	 }
//   if(TI)                        //�p�G�O�o�e�лx��A�M�s
//     TI=0;
}

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for(i = 0; i < ms; i++)
    {
        for(j = 0; j < 120; j++)
        {
            ;  // �Ŵ`���A�ΨӯӮ�
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
    // ��������e5�Ӧr��
    char ten = buf[0];      // �Q��Ʀr
    char unit = buf[1];     // �Ӧ�Ʀr
    char dot = buf[2];      // �p���I
    char tenth = buf[3];    // �Q����
    char hundredth = buf[4];// �ʤ���
    
    // �Q��Ʀr
    if(ten >= '0' && ten <= '9') {
        TempData[0] = dofly_DuanMa[ten - '0'];
    } else {
        TempData[0] = 0x00; // �ť�
    }
    
    // �Ӧ�Ʀr
    if(unit >= '0' && unit <= '9') {
        TempData[1] = dofly_DuanMa[unit - '0'];
    } else {
        TempData[1] = 0x00; // �ť�
    }
    
    // �p���I
    if(dot == '.') {
        TempData[2] = dofly_DuanMa[10]; // �p���I�q�X
    } else {
        TempData[2] = 0x00; // �ť�
    }
    
    // �Q����
    if(tenth >= '0' && tenth <= '9') {
        TempData[3] = dofly_DuanMa[tenth - '0'];
    } else {
        TempData[3] = 0x00; // �ť�
    }
    
    // �ʤ���
    if(hundredth >= '0' && hundredth <= '9') {
        TempData[4] = dofly_DuanMa[hundredth - '0'];
    } else {
        TempData[4] = 0x00; // �ť�
    }

}
