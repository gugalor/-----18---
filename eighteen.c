#include <intrins.h> 
#include <REG51.h>                                                       
#define uchar unsigned char
#define uint unsigned int

/*Declare SFR associated with the IAP */
sfr IAP_DATA = 0xC2; //Flash data register
sfr IAP_ADDRH = 0xC3; //Flash address HIGH
sfr IAP_ADDRL = 0xC4; //Flash address LOW
sfr IAP_CMD = 0xC5; //Flash command register
sfr IAP_TRIG = 0xC6; //Flash command trigger
sfr IAP_CONTR = 0xC7; //Flash control register
sfr P0M0        =   0x94;   //0000,0000 端口0模式寄存器0
sfr P0M1        =   0x93;   //0000,0000 端口0模式寄存器1
sfr P1M0        =   0x92;   //0000,0000 端口1模式寄存器0
sfr P1M1        =   0x91;   //0000,0000 端口1模式寄存器1
sfr P3M0        =   0xB2;   //0000,0000 端口3模式寄存器0
sfr P3M1        =   0xB1;   //0000,0000 端口3模式寄存器1
/*Define ISP/IAP/EEPROM command*/
#define CMD_IDLE 0 //Stand-By
#define CMD_READ 1 //Byte-Read
#define CMD_PROGRAM 2 //Byte-Program
#define CMD_ERASE 3 //Sector-Erase
#define ENABLE_IAP 0x82 //if SYSCLK<20MHz
#define IAP_ADDRESS 0x0000
#define  PORTD 	P3	

void IapIdle();
uchar IapReadByte(uint addr);
void IapProgramByte(uint addr, uchar dat);
void IapEraseSector(uint addr);

sbit wei1=P0^1; 
sbit wei2=P1^0; 
sbit wei3=P1^1; 
sbit IN1=P1^2; 
sbit k1=P1^3; 
sbit k2=P1^4; 
sbit k3=P1^5;
sbit OUT=P0^0; 
bit GNselect;
uchar GN;
uchar CN;
uint ms05;
uint dingshi;
bit fenshan;
uint  GNT[28];
uint CycleNum[2];
uint CycleN;
bit yanshi;
bit biaozhi,xianshi;
bit flag;
bit flag1;
bit flag5;
bit GNchanged;
bit ShanShou;
bit init;
bit visionflag;		//版本‘分’，‘秒’标志
bit on;
bit flag_17;
bit flag_17_1;
bit biaozhi1;
uchar off;
uchar vision;
uchar Key_Num;
uchar flag4;
uchar NUM;
uchar LED=1;
uchar i; 
void Write();
void keystate();
void  saomiao();
void Key();
void delay(uint z);
void temptoseg();
void GNseparate();
void separate();
void outfuction();
uchar  display_data[3];
uchar ledcode[]={0x84,0xb7,0x2c,0x25,0x17,0x45,0x44,0xa7,0x04,0x05,0x5c,0x76,0x06,0x54};	//0-9,t,n,a,b

//*********************************************************

 void main()
{ 		
   init=IapReadByte(IAP_ADDRESS+121);
   if(init)				//eeprom为空,初始化
   {
	 for(i=0;i<28;i++)
		GNT[i]=10;	
	 GN=0;
	 CN=0;
	 on=0;
	 vision=0;
     CycleNum[0]=10;
	 CycleNum[1]=10;
	 Write();			
    }
    else
	{
		for (i=0; i<56; i++) 
        {
          GNT[i/2]=IapReadByte(IAP_ADDRESS+i)<<8|IapReadByte(IAP_ADDRESS+i+1);
          i++;
        } 
        GN=IapReadByte(IAP_ADDRESS+120);
        CycleNum[0]=IapReadByte(IAP_ADDRESS+122)<<8|IapReadByte(IAP_ADDRESS+123);
        CycleNum[1]=IapReadByte(IAP_ADDRESS+124)<<8|IapReadByte(IAP_ADDRESS+125);
		    vision=IapReadByte(IAP_ADDRESS+126);
				on=IapReadByte(IAP_ADDRESS+127);
        if(GN==6||GN==7)
           CN=GN-6;
     }   						             	      
		   P0M0=0x01;
		   P3M0=0Xff;	    
	        TMOD=0x11;//工作模式设置
		    EA=1;
			TR0=1;
			ET0=1;
			TH0=(65536-50000)/256;
			TL0=(65536-50000)%256;
			dingshi=GNT[GN];
			biaozhi=1;
			if(GN>7)
			{
			  OUT=0;
			  biaozhi=0;
			}else
		      OUT=(GN+1)%2;
 while(1)
  {	
	 uchar k,g;			
	 k++;
	 if(k>=20)
     {
		k=0;
		xianshi=!xianshi;	   //修改时数字闪烁
	 }
	 if(((GN>7&&biaozhi)||GN<8)&&dingshi>0&&vision){
	 g++;
	 if(g>=100){
	  g=0;
	  fenshan=!fenshan;
	 }
	 }
  	 Key();
	 temptoseg();
	 saomiao();	   
	 if(GN==16)//功能17
	 {
			if(flag_17_1==0&&IN1==0)		 //功能17
			{
				biaozhi1=!biaozhi1;
			 OUT=biaozhi1;
				flag_17_1=1;
			}
			if(IN1==1)
			flag_17_1=0;
			if((k2==0)&&(flag_17==0)) 
		{
			 biaozhi1=!biaozhi1;
			 OUT=biaozhi1;
				flag_17=1;
		}
		if(k2==1)
		flag_17=0;
	 }
	 else if(GN==8||GN==9||GN==12||GN==13)
	 {
		if(biaozhi==0)		 //延时中触发无效
		if((IN1==0))
		{
		   for(i=0;i<10;i++)
			   saomiao();
		   if(IN1==0)
			{	 		 
			   flag=0;  
			   dingshi=GNT[GN];
			   yanshi=0;
			   biaozhi=1;
			   if(GN<12)
			   OUT=(GN+1)%2;
			   else
			   OUT=GN%2;
			   if(GNchanged)
			   {
			     GNchanged=0;
           Write();  
			   }
			}
		}
	}else if((IN1==0))
	{
		for(i=0;i<10;i++)
			   saomiao();
		if(IN1==0)
		{
			 if(GN>23)
			  {
			    GN=GN-12;
			  }	
			  else if(GN>17)
			  {
			    GN=GN-16;
			  }	 	
			if(GN!=17)	  //功能18不处理
			{
			   if(GN==6||GN==7)	  //功能7,8；触发后循环次数清零
			   {
			   	  CycleN=0;
			   } 	   	
			   dingshi=GNT[GN];
			   yanshi=0;
			   biaozhi=1;
			   flag=0;
			   if(GN<12)
			   OUT=(GN+1)%2;
			   else
			   OUT=GN%2;
			   if(GNchanged)
			   {
			    GNchanged=0;
                Write(); 
			   } 
			}
		}
    }
		
	}			
}


void Key()
{
  keystate();	 
  if(yanshi==1&&(Key_Num==0))
  {	
	   if(k3==0)// 加键检测
	   {
		   for(i=0;i<10;i++)
			   saomiao();		   
		   if(k3==0)
		   {
		   flag1=1;
		   GNselect=1;		    
		   NUM++;
		   if(NUM==10)
		     NUM=0;	
		   if(flag5==1)	 //显示循环次数
		   {
		   	  switch(LED)
				 {
				    case 1:
			               CycleNum[CN]=NUM*100+CycleNum[CN]%100;
					       break;
					case 2:
					       CycleNum[CN]=NUM*10+(CycleNum[CN]/100)*100+CycleNum[CN]%10;
					       break;
					case 3:
					       CycleNum[CN]=NUM+CycleNum[CN]-CycleNum[CN]%10;
					       break;
				    default:
			               break;
				}	
		   }else		 //显示时间
		   {	
			  switch(LED)
				 {
				    case 1:
			               GNT[GN]=NUM*100+GNT[GN]%100;
					       break;
					case 2:
					       GNT[GN]=NUM*10+(GNT[GN]/100)*100+GNT[GN]%10;
					       break;
					case 3:
					       GNT[GN]=NUM+GNT[GN]-GNT[GN]%10;
					       break;
				    default:
			               break;
				}
			}			   						
		}					
	}
	   if(k2==0)//		数码管选择键检测
	   {
		  for(i=0;i<10;i++)
			saomiao();						
		  if(k2==0)
		  {
		    GNselect=1;
	    	flag1=1;
		    LED++; 
			if(LED==4)
			  LED=1;
			if(flag5==1)  //显示循环次数
			{
			  switch(LED)
				 {
				    case 1:
			               NUM=CycleNum[CN]/100;
					       break;
					case 2:
					       NUM=CycleNum[CN]%100/10;
					       break;
					case 3:
					       NUM=CycleNum[CN]%10;
					       break;
				    default:
			               break;
				}	
			}else		   //显示时间
			{
			switch(LED)
				 {
				    case 1:
			               NUM=GNT[GN]/100;
					       break;
					case 2:
					       NUM=GNT[GN]%100/10;
					       break;
					case 3:
				       	   NUM=GNT[GN]%10;
				           break;
				    default:
			               break;
				}
			 }		
		   }
		}
	}			
}

 void temptoseg()
 {	
    if(Key_Num==2)
	 {
	     display_data[0]=0xff;
		 display_data[1]=0xff;
		 display_data[2]=ledcode[vision+12];
	 }
    else if(yanshi)
   {
   	 if(GNselect==0)
	 {
	   if(Key_Num==0)
	   {
		 if(flag5==1)			 //显示功能循环次数
		 {		 
		   display_data[0]=0xff;
		   display_data[1]=0xff;
		   display_data[2]=ledcode[11];
		  }
		  else if(GN==0||GN==1||(GN>7&&GN<12))	   //只有一个时间
		  {	 
		    display_data[0] =0xff;
	        display_data[1] = ledcode[10];
		    display_data[2] = 0xff;				
		  }
		  else if(GN>17)		//T2
		  {					 
		    display_data[0]=0xff;
		    display_data[1]=ledcode[10];
		    display_data[2]=ledcode[2];		
		  }
		  else if(GN<16)  //T1
		  {			 
		    display_data[0]=0xff;
		    display_data[1]=ledcode[10];
		    display_data[2]=ledcode[1];
		  }
		}
		else
		{
		  GNseparate();	
		}							 				
	  }
      else
	  {
		if(flag5)
		  dingshi=CycleNum[CN];
		else
		  dingshi=GNT[GN];

		if(!ShanShou)
		{
		if(xianshi)
		{
		  display_data[LED-1] = 0xff;	 //选择的数码管闪烁
		}
		else
		{									
		  separate();	
		}
	   }
	  }
	}
	else
	{
	 if(GN==16||GN==17)
	 {
	   GNseparate();
	 }
	 else
	 {
		separate();
	 }
	}											 	
 }
//*********************************************************

void timer0() interrupt 1  //定时器0中断，该中断完成数码管的动态扫描
 { 
	   TH0=(65536-50000)/256;
       TL0=(65536-50000)%256;      //每中断一次的时间为100毫秒

	  if(k2==0)
			off++;
		else
			off=0;
		if(off>=30){
			on=!on;
			off=0;
			Write(); 
		}
	  if(GN==17&&IN1==0)	   //功能18
	  {
		 OUT=1;
	  }
	  else if(GN==17&&IN1==1)
	  {
	   OUT=0;
	  }
	  if(biaozhi)// 开机状态
	  {
	    
	      ms05++;
		  if(vision==1)
	    {
		   if(ms05>=600)
		   {
			 outfuction();
		   }
	    }else
		{
		  if(ms05>=10)//1秒
			{
			  outfuction();
			}
		}
	  }
 } 

  void  saomiao()
     {
			 if(on==0){
            PORTD=~display_data[0];
			wei1=0;
	    	delay(3);
			wei1=1;

            PORTD=~display_data[1];
			wei2=0;
	    	delay(3);
			wei2=1;

			 if(vision&&yanshi==0&&fenshan){
			 PORTD=~(display_data[2]&0Xfb);	
			 }		 
			  else
               PORTD=~display_data[2];
			wei3=0;
			delay(3);
			wei3=1;		  
      }
		}

void separate(){
                       if(dingshi<10)
						{
					   	     display_data[0] = ledcode[0];
		                     display_data[1] = ledcode[0];
						 	 display_data[2] = ledcode[dingshi%10];
						}
						else if(dingshi<100)
						{
					   	     display_data[0] = ledcode[0];
		                     display_data[1] = ledcode[dingshi/10%10];
						 	 display_data[2] = ledcode[dingshi%10];								 
						}
					     else {
						   display_data[0] = ledcode[dingshi/100];
		                   display_data[1] = ledcode[dingshi/10%10];
						   display_data[2] = ledcode[dingshi%10];	
						   }  			 	    
}		
	
	void GNseparate(){
                        dingshi=GN+1;
                     	if(dingshi<10)
						{
					   	     display_data[0] = 0xff;
		                     display_data[1] = 0xff;
						 	 display_data[2] = ledcode[dingshi%10];
						}
						else if(dingshi<100)
						{
					   	     display_data[0] = 0xff;
		                     display_data[1] = ledcode[dingshi/10%10];
						 	 display_data[2] = ledcode[dingshi%10];								 
						}
					     else{
						   display_data[0] = ledcode[dingshi/100];
		                   display_data[1] = ledcode[dingshi/10%10];
						   display_data[2] = ledcode[dingshi%10];
						   }	
			}
 void delay(uint z)//1MS 延时
{
  unsigned char i, j;

	_nop_();
	_nop_();
	while(z--){
	i = 6;
	j = 210;
	do
	{
		while (--j);
	} while (--i);
	}
}
void keystate(){
   uchar nn=0;                                                    //“按下”时长计数器清零
   uchar mm=0;                                                    //“释放”时长计数器清零
    if(k1==0)						     //如按键set0按下
    {
          delay(30);                                       //延时（消抖）
          if(k1==0)					     //如按键set0按下
	  {
               do{                                           //“按下”计数器开始计数
                      nn++;
                      delay(10);
                 }while(k1==0);
               if(nn<50)                                     //如“按下”计数小于设定值（短击的最大限制值）
               {
                    delay(30);                             //延时（消抖）
                    do{                                      //“释放”计数器开始计数
                          mm++;
                          delay(10);
                      }while((mm<40)&&(k1==1));            //当超出设定值或再次有“按下”事件发生，结束计数
                    delay(25);                             //延时（消抖）
                    do{                                      //待到第二次的“按下”事件结束
                          
		      }while(k1==0);
                    if(mm<40) 				     //在释放期间提前结束的为双击，否则为短击
		   {
			Key_Num=0;	   //双击   

		    ShanShou=0;
			flag1=1;  
			GNselect=0;
			if(GN>23)
			{
			  GN=GN-12;
			}	
			else if(GN>17)
			{
			   GN=GN-16;
			}						
			if(GN==6||GN==7)	 //功能7,8有循环次数
			{	
			   if(flag4==0)
			   {
			     flag5=0;
			     flag4=1;
			   }else if(flag4==1)
			   {
			     GN=GN+16;
			     flag4=2;
			   }else if(flag4==2)
			   {
			      flag4=0;
			      flag5=1;
			   }
		     }else if(GN>1&&GN<6) //功能2-8	T2在数组中的位置是（功能号码+16）
			 {
		   	   if(flag==0)
			   {
			     flag=1;
			   }else if(flag)
			   {
			     GN=GN+16;
			     flag=0;
			   }
		     }else if(GN>11&&GN<16)	 //功能13-16 T2在数组中的位置是 （功能号码+12） 
			 {
			   if(flag==0)
			   {
			      flag=1;
			   }else if(flag)
			   {
			     GN=GN+12;
			     flag=0;
			   }		
			 }
			}
			else
		    {
			  OUT=0;
			  flag=0;
			  flag5=0;
		      ShanShou=0;
		      if(flag1==1)	  //修改后确定
			  { 
			    flag1=0;
				GNchanged=0;
			    ShanShou=1;	
				CycleN=0;				            						
			    
			  if(GN>23)
			  {
			    GN=GN-12;
			  }	
			  else if(GN>17)
			  {
			    GN=GN-16;
			  }	
			  OUT=(GN+1)%2;
				if(GN>7)
				OUT=0;
			  if(GN!=16&&GN!=17)   //功能17,18不处理
			  {
			    dingshi=GNT[GN];
			    if(GN>7)
			       biaozhi=0;
			    else
			       biaozhi=1; 			
	            yanshi=0;
			    Write();
			  }
			  }else{
			     if(Key_Num==2&&yanshi)
				 {
					GN--;
				 }
			     if(yanshi)
		            if(GN>16)
		               GN=0;
		            else
		               GN++;				
			        if(GN==6||GN==7)
			        CN=GN-6;
			        GNchanged=1;
			        GNselect=0;
                    biaozhi=0; 
	                yanshi=1;
					if(GN==16||GN==17)
					{
					   Write();
					}	   						  
		       }
			   			  Key_Num=1;	 //短击
			}
         }
         else
               {
                    Key_Num=2;                               //”按下”计数大于设定值为长击
					if(visionflag==0)
					{
					  vision=1;
					  visionflag=1;
					}
					else
					{
					  vision=0;
					  visionflag=0;
					}
						
               }
          }
    }
}		

 void Write(){
IapEraseSector(IAP_ADDRESS); //Erase current sector
for (i=0; i<56; i++)
{
IapProgramByte(IAP_ADDRESS+i,GNT[i/2]>>8);
IapProgramByte(IAP_ADDRESS+i+1, GNT[i/2]&0xff);
i++;
}
IapProgramByte(IAP_ADDRESS+120, GN);
IapProgramByte(IAP_ADDRESS+121,0);
IapProgramByte(IAP_ADDRESS+122,CycleNum[0]>>8);
IapProgramByte(IAP_ADDRESS+123,CycleNum[0]&0xff);
IapProgramByte(IAP_ADDRESS+124,CycleNum[1]>>8);
IapProgramByte(IAP_ADDRESS+125,CycleNum[1]&0xff);
IapProgramByte(IAP_ADDRESS+126,vision);
IapProgramByte(IAP_ADDRESS+127,on);
}

  void outfuction()
  {
     ms05=0;
		      dingshi--;
			  if(dingshi==0)
			  {
				   switch(GN)
				   {
				    case 0:
		                   	OUT=0;
							biaozhi=0;										
		                   	break;
		             case 1:
			                OUT=1;
							biaozhi=0;
			                break;
		   	         case 2:
					        if(0==flag){
			        		OUT=0;
							dingshi=GNT[GN+16];
							flag=1;
							}else{
							OUT=1;
							flag=0;
							biaozhi=0;
							}
			            	break;
				 	 case 3:
			              if(flag==0){
			        		OUT=1;
							dingshi=GNT[GN+16];
							flag=1;
							}else{
							OUT=0;
							flag=0;
							biaozhi=0;
							}
			            	break;
					case 5:
			                if(flag==0){
			        		OUT=1;
							dingshi=GNT[GN+16];
							flag=1;
							}else{
							OUT=0;
							dingshi=GNT[GN];
							flag=0;
							}
			            	break;
		   	         case 4:
					        if(0==flag){
			        		OUT=0;
							dingshi=GNT[GN+16];
							flag=1;
							}else{
							OUT=1;
							dingshi=GNT[GN];
							flag=0;
							}
			            	break;
				 	 case 7:
			              if(flag==0){
			        		OUT=1;
							dingshi=GNT[GN+16];
							flag=1;
							}else{
							OUT=0;
							dingshi=GNT[GN];
							flag=0;
							CycleN++;
							if(CycleN==CycleNum[1]||CycleNum[1]==0){
							CycleN=0;
							biaozhi=0;
							}
							}
			            	break;
					  case 6:
		                   	if(0==flag){
			        		OUT=0;
							dingshi=GNT[GN+16];
							flag=1;
							}else{
							OUT=1;
							dingshi=GNT[GN];
							flag=0;
							CycleN++;
							if(CycleN==CycleNum[0]||CycleNum[0]==0){
							CycleN=0;
							biaozhi=0;
							OUT=0;
							}
							}
							break;
				     case 8:
		                   	OUT=0;
							biaozhi=0;										
		                   	break;
		             case 9:
			                OUT=1;
							biaozhi=0;
			                break;
							 
		             case 10:
			                OUT=0;
							biaozhi=0;
			                break;
					 case 11:
		                   	OUT=1;
							biaozhi=0;										
		                   	break;
		            
		             case 12:
			                if(flag==0){
			        		OUT=1;
							dingshi=GNT[GN+12];
							flag=1;
							}else{
							OUT=0;
							flag=0;
							biaozhi=0;
							}
			            	break;
		   	         case 13:
					        if(0==flag){
			        		OUT=0;
							dingshi=GNT[GN+12];
							flag=1;
							}else{
							OUT=1;
							flag=0;
							biaozhi=0;
							}
			            	break;
				 	 case 14:
			              if(flag==0){
			        		OUT=1;
							dingshi=GNT[GN+12];
							flag=1;
							}else{
							OUT=0;
							flag=0;
							biaozhi=0;
							}
			            	break;
					  case 15:
		                   	if(0==flag){
			        		OUT=0;
							dingshi=GNT[GN+12];
							flag=1;
							}else{
							OUT=1;
							flag=0;
							biaozhi=0;
							}
							break;
		             default:
			                 ;	
				    }	  				   
			     }
  }
void IapIdle()
{
IAP_CONTR = 0; //Close IAP function
IAP_CMD = 0; //Clear command to standby
IAP_TRIG = 0; //Clear trigger register
IAP_ADDRH = 0x80; //Data ptr point to non-EEPROM area
IAP_ADDRL = 0; //Clear IAP address to prevent misuse
}
/*----------------------------
Read one byte from ISP/IAP/EEPROM area
Input: addr (ISP/IAP/EEPROM address)
Output:Flash data
----------------------------*/
uchar IapReadByte(uint addr)
{
uchar dat; //Data buffer
IAP_CONTR = ENABLE_IAP; //Open IAP function, and set wait time
IAP_CMD = CMD_READ; //Set ISP/IAP/EEPROM READ command
IAP_ADDRL = addr; //Set ISP/IAP/EEPROM address low
IAP_ADDRH = addr >> 8; //Set ISP/IAP/EEPROM address high
IAP_TRIG = 0x5a; //Send trigger command1 (0x5a)
IAP_TRIG = 0xa5; //Send trigger command2 (0xa5)
_nop_(); //MCU will hold here until ISP/IAP/EEPROM
//operation complete
dat = IAP_DATA; //Read ISP/IAP/EEPROM data
IapIdle(); //Close ISP/IAP/EEPROM function
return dat; //Return Flash data
}
/*----------------------------
Program one byte to ISP/IAP/EEPROM area
Input: addr (ISP/IAP/EEPROM address)
dat (ISP/IAP/EEPROM data)
Output:-
----------------------------*/

void IapProgramByte(uint addr, uchar dat)
{
IAP_CONTR = ENABLE_IAP; //Open IAP function, and set wait time
IAP_CMD = CMD_PROGRAM; //Set ISP/IAP/EEPROM PROGRAM command
IAP_ADDRL = addr; //Set ISP/IAP/EEPROM address low
IAP_ADDRH = addr >> 8; //Set ISP/IAP/EEPROM address high
IAP_DATA = dat; //Write ISP/IAP/EEPROM data
IAP_TRIG = 0x5a; //Send trigger command1 (0x5a)
IAP_TRIG = 0xa5; //Send trigger command2 (0xa5)
_nop_(); //MCU will hold here until ISP/IAP/EEPROM
//operation complete
IapIdle();
}
/*----------------------------
Erase one sector area
Input: addr (ISP/IAP/EEPROM address)
Output:-
----------------------------*/
void IapEraseSector(uint addr)
{
IAP_CONTR = ENABLE_IAP; //Open IAP function, and set wait time
IAP_CMD = CMD_ERASE; //Set ISP/IAP/EEPROM ERASE command
IAP_ADDRL = addr; //Set ISP/IAP/EEPROM address low
IAP_ADDRH = addr >> 8; //Set ISP/IAP/EEPROM address high
IAP_TRIG = 0x5a; //Send trigger command1 (0x5a)
IAP_TRIG = 0xa5; //Send trigger command2 (0xa5)
_nop_(); //MCU will hold here until ISP/IAP/EEPROM
//operation complete
IapIdle();
}		   
