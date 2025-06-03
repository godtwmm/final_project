/*-----------------------------------------------
  �W�١G��f�q�H�]�ϥδ���ŧP�_�ƾڵ����^
  �����Gwww.doflye.net
  �s�g�Gshifang�]�ק��^
  ����G2009.5�]�ק��^
  ���e�G
    ��UART������@���H����� ('\n') �������ƾڡA�Ҧp "29.77\n"�A 
    �{���N�۰ʸѪR�ƾڡA�N���ഫ���ƽX�ެq�X�A
    �ë����s��ܦb�ƽX�ޤW�C
------------------------------------------------*/

#include <reg52.h>
#include <string.h>

#define DataPort P0
#define MAX 10

sbit LATCH1 = P2^2;
sbit LATCH2 = P2^3;

// �w�q�ƽX�ެq�X�G0~9�M�p���I�]�@ 11 �Ӥ����^
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
    0x80  // �p���I
};

// �w�q��X�]�ھڧA���w�󱵽u�A�o�̰��]��8��ƽX�ޡ^
unsigned char code dofly_WeiMa[] = {
    0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F
};

unsigned char TempData[8]; // �ƽX����ܽw�İ�

// UART�����w�İϥH�ά��������ܶq
unsigned char buf[MAX];
unsigned char head = 0;
bit recv_flag = 0;  // �����촫��ū�]�m���лx

/*------------------------------------------------
    ����n��
------------------------------------------------*/
void InitUART(void);
void SendByte(unsigned char dat);
void SendStr(unsigned char *s);
void Delay(unsigned int t);
void Display(unsigned char FirstBit, unsigned char Num);

/*------------------------------------------------
    ��f��l�� (9600 �i�S�v, 11.0592MHz)
------------------------------------------------*/
void InitUART(void)
{
    SCON  = 0x50;    // �u�@�Ҧ�1�G8�� UART�A���\����
    TMOD |= 0x20;    // Timer1�A�Ҧ�2�]8��۰ʭ����^
    TH1   = 0xFD;    // 9600 �i�S�v�]11.0592MHz�����^
    TR1   = 1;       // �Ұʩw�ɾ�1
    EA    = 1;       // �����_�ϯ�
    ES    = 1;       // ���}��f���_
}

/*------------------------------------------------
    �D���
------------------------------------------------*/
void main(void)
{
    unsigned int j; // �w�q�Ω�ѪR�������ƪ��{���ܼ�
    char ten, unit, dot, tenth, hundredth;

    InitUART();
    ES = 1;  // ���}��f���_

    while (1)
    {
        // �����w�İϺ� 5 �Ӧr�Ůɶi��B�z
        if (head == 5)
        {
            buf[head] = '\0'; // �[�J�r�굲����
            head = 0;         // ���m�����w�İϪ����w
            
            // �q�����w�İ�Ū����e�R�O���ƾ�
            ten       = buf[0];  // �Q��Ʀr
            unit      = buf[1];  // �Ӧ�Ʀr
            dot       = buf[2];  // �p���I
            tenth     = buf[3];  // �Q����
            hundredth = buf[4];  // �ʤ���

            // �N�r���ഫ�������q�X�A�æs�J��ܽw�İ�
            if (ten >= '0' && ten <= '9')
                TempData[0] = dofly_DuanMa[ten - '0'];
            else
                TempData[0] = 0x00;

            if (unit >= '0' && unit <= '9')
                TempData[1] = dofly_DuanMa[unit - '0'];
            else
                TempData[1] = 0x00;

            // �p���I�B�z�G�u����r�Ŭ� '.' �~��ܤp���I�q�X
            if (dot == '.')
                TempData[2] = dofly_DuanMa[10];  // �ϥΩw�q�n���p���I�q�X
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

            // ��L��O���ť�
            TempData[5] = TempData[6] = TempData[7] = 0x00;
        }
        
        // ��s��ܡG�o�̥Τ@�� for �j����򱽴y�ƽX�ޡA
        // �C����ܷ|�̦���X�C�@�쪺��X�P�q�X
        for (j = 0; j < 100; j++)
        {
            Display(0, 8);  // ��� 8 ��A�ھڹ�ڻݭn�վ���ܦ��
        }
    }


}

/*------------------------------------------------
    ��f���_�A�ȵ{��
------------------------------------------------*/
void UART_SER(void) interrupt 4
{
    unsigned char Temp;
    
    if (RI)  // �p�G�o�ͱ������_
    {
        RI = 0;       // �M���������_�лx
        Temp = SBUF;  // Ū�������w�ľ����
        buf[head] = Temp;
        head++;
        if (head == MAX)
            head = 0;  // ����w�İϷ��X�A�W�X MAX �᭫�s�k�s
    }


}

/*------------------------------------------------
    �o�e�@�Ӧr�` (UART)
------------------------------------------------*/
void SendByte(unsigned char dat)
{
    SBUF = dat;
    while (!TI);
    TI = 0;
}

/*------------------------------------------------
    �o�e�@�Ӧr�Ŧ� (UART)
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
    �@���ɨ��
------------------------------------------------*/
void Delay(unsigned int t)
{
    while(t--);
}

/*------------------------------------------------
    �ƽX����ܱ��y���
    �ѼơGFirstBit�G�_�l��FNum�G��ܦ��
------------------------------------------------*/
void Display(unsigned char FirstBit, unsigned char Num)
{
    unsigned char i;
    for (i = 0; i < Num; i++)
    {
        DataPort = 0;             // �M�żƾڤf
        LATCH1 = 1;
        LATCH1 = 0;
      
        // ��X���H��
        DataPort = dofly_WeiMa[i + FirstBit];
        LATCH2 = 1;
        LATCH2 = 0;
      
        // ��X�q�X�ƾ�
        DataPort = TempData[i];
        LATCH1 = 1;
        LATCH1 = 0;
      
        Delay(200);  // �����ɮھڵw��ݨD�վ�
    }
}
