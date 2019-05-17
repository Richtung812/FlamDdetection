/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
��ƽ    ̨�������������ܿƼ�MK66FX1M0VLQ18���İ�
����    д��CHIUSIR
����    ע��
������汾��V1.0
�������¡�2016��08��20��
�������Ϣ�ο����е�ַ��
����    վ��http://www.lqist.cn
���Ա����̡�http://shop36265907.taobao.com
���������䡿chiusir@163.com
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
#include "include.h"
vuint16 ADV[128]={0,0};         //�������飬���ڴ�Ųɼ���������ֵ
vuint16 LCDD[128]={0,0};        //ת��ΪLCD��ʾ����ֵ

vint16 piancha=0;

#define TSL_SI   PTE3_OUT    //�������Դ������Ķ˿� SI
#define TSL_CLK  PTE2_OUT    //�������Դ������Ķ˿� CLK

//---------------PID����------------------------------------------------
float err=0.0; //����ƫ��ֵ  
float err_next=0.0; //������һ��ƫ��ֵ   
float err_last=0.0; //��������ǰ��ƫ��ֵ   
float Kp=14.55,Ki=6.00,Kd=2.0; //������������֡�΢��ϵ��   ������27������6������


/*   
2m:17.55,6.00,2.0   
5m:14.55,6.00,2.0   
*/   
float incrementSpeed; //�����ٶ�  
float ActualSpeed=0.0;//ʵ���ٶ�
float index;
float integral; //�������ֵ
float voltage; //�����ѹֵ������ִ�����ı�����
float pa;  
//----------------------------------------------------------------------
float E[100]={0};
//-------------------------------------------------------------------------*
//������: void Init_TSL1401(void)                                                     
//��  ��: ��ʼ��TSL1401                                                  
//��  ��: ��                                                                               
//��  ��: ��                                                              
//��  ��: Init_TSL1401() ;              
//-------------------------------------------------------------------------*
void Init_TSL1401(void)
{
  GPIO_Init(PTE,3,GPO,0); //�������Դ������Ķ˿� SI
  GPIO_Init(PTE,2,GPO,0); //�������Դ������Ķ˿� CLK
  ADC_Init(ADC_0);        //AD�ڳ�ʼ��               
  ADC_Start(ADC_0,ADC0_SE16,ADC_12bit);   //AD�ڳ�ʼ��   
}

//-------------------------------------------------------------------------*
//������: void Dly_us(int us)                                                    
//��  ��: ������ʱ                                                    
//��  ��: us �൱��΢�뼶�����ʱ                                                                               
//��  ��: ��                                                              
//��  ��: Dly_us(1) ;              
//-------------------------------------------------------------------------*
//-------------------------------------------------------------------------*
void Dly_us(int us)
{   
   uint16 i,j;    
   for(i=0;i<us;i++)
   {
     for(j=0;j<500;j++);  
   } 
}
//-------------------------------------------------------------------------*
//������: void Read_TSL1401(void)                                                  
//��  ��: ��ȡ����ͷ����                                                     
//��  ��: ��                                                                               
//��  ��: ��                                                              
//��  ��: Read_TSL1401() ;              
//-------------------------------------------------------------------------*
void Read_TSL1401(void) 
{
  u8 i=0,tslp=0;
  
  TSL_CLK=1;//��ʼ��ƽ�� 
  TSL_SI=0; //��ʼ��ƽ��
  Dly_us(1); //�������ʱ
      
  TSL_SI=1; //������
  TSL_CLK=0;//�½���
  Dly_us(1); //������ʱ
      
  TSL_CLK=1;//������
  TSL_SI=0; //�½���
  Dly_us(1); //������ʱ      
  for(i=0;i<64;i++)
  { 
    TSL_CLK=0;//�½���    
    Dly_us(8-i/8+1); //������ʱ    
    ADV[tslp]=ADC_Ave(ADC_0,ADC0_SE16,ADC_12bit,10);;  //AD�ɼ�
    ++tslp;
    TSL_CLK=1;//������ 
    Dly_us(8-i/8+1); //������ʱ    
  }
  for(i=0;i<64;i++)
  { 
    TSL_CLK=0;//�½���    
    Dly_us(i/8+1); //������ʱ   
    ADV[tslp]=ADC_Ave(ADC_0,ADC0_SE16,ADC_12bit,10);;  //AD�ɼ�
    ++tslp;
    TSL_CLK=1;//������ 
    Dly_us(i/8+1); //������ʱ    
  }    
}
//-------------------------------------------------------------------------*
//������: void Show_TSL1401(void)                                                       
//��  ��: ��OLED��Ļ����ʾ��ֵ                                                     
//��  ��: ��                                                                               
//��  ��: ��                                                              
//��  ��: Show_TSL1401() ;              
//-------------------------------------------------------------------------*
float Show_TSL1401(void) 
{    
  u16 i=0,maxv=0,minv=0xFFFF;
  u16 evv=0,tslp=0;
  u32 addall=0;
  int num_L=0,num_M=0,num_R,num=0;//��������Ҹ���,�׸���
  char txt[16]="";   
  for(i=0;i<128;i++)
  { 
    addall+=ADV[i];           
  }
  evv=addall/128;    

  for(i=0;i<128;i++)
  {     
    if(minv>ADV[i])  minv=ADV[i];
    if(maxv<ADV[i])  maxv=ADV[i];               
  }

  sprintf(txt,"%04d %04d %04d",maxv,minv,evv);
  LCD_P6x8Str(20,6,(uint8*)txt);
 
  LCD_Set_Pos(0,7);    
  for(i=0;i<128;i++)  //��ʾ��ɫ�߽�
  {   
    if(ADV[i]<= evv-evv/5)
    {  
      tslp=0xfe; //��ɫ   
      num++;//ͳ�ư׵ĸ���
    }
    else
    {
      num_L=num;
      tslp=0x80; //��ɫ
      num_M++;//ͳ�ƺڵĸ���
    }      
    LCD_WrDat(tslp);           
  }
  num_R=128-num_L-num_M;

  err=num_L-num_R;//�������ߵ����
  incrementSpeed=Kp*(err-err_next)+Ki*err+Kd*(err-2*err_next+err_last);
  err_last=err_next;
  err_next=err;
  for (int i=100;i>0;i--)
  {
    E[i]=err;
  }
  if (E[99]>-3 && E[99]<3)
  {
    LED_Ctrl(LED0,ON);
  }
  else
  {
    LED_Ctrl(LEDALL,OFF);
  }
  
  sprintf(txt,"%04d,%04d,%04d",num_L,num_M,num_R);
  LCD_P6x8Str(5,3,(uint8*)txt);

  sprintf(txt,"err: %5.2f",err);//��ӡerr
  LCD_P6x8Str(5,2,(uint8*)txt);

  sprintf(txt,"inS: %5.2f",incrementSpeed);//��ӡincrementSpeed
  LCD_P6x8Str(5,1,(uint8*)txt);

  return incrementSpeed;
  //return voltage;
}
