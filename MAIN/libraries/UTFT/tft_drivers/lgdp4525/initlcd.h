case LGDP4525:
	delay(150);
    //*********************Initial code******************************   
LCD_Write_COM_DATA(0x0007,0x0000);   
LCD_Write_COM_DATA(0x0012,0x0000);   
LCD_Write_COM_DATA(0x0013,0x0000);   
LCD_Write_COM_DATA(0x0014,0x0331);   
delay(50);   
  
LCD_Write_COM_DATA(0x0011,0x0101);   
LCD_Write_COM_DATA(0x0012,0x000A);   
LCD_Write_COM_DATA(0x0013,0x0D45); 
LCD_Write_COM_DATA(0x0010,0x0164); 
LCD_Write_COM_DATA(0x0012,0x001A); 
delay(50);   
LCD_Write_COM_DATA(0x0013,0x2D45);   
LCD_Write_COM_DATA(0x0010,0x0160);   
delay(50);   
LCD_Write_COM_DATA(0x0060,0x2000);   
LCD_Write_COM_DATA(0x0060,0x0000);   
LCD_Write_COM_DATA(0x0061,0x0002);   
delay(50);   
LCD_Write_COM_DATA(0x0001,0x011B);   
LCD_Write_COM_DATA(0x0002,0x0700);   
LCD_Write_COM_DATA(0x0003,0x1030);   
LCD_Write_COM_DATA(0x0008,0x0504);
LCD_Write_COM_DATA(0x0009,0x0000);   
LCD_Write_COM_DATA(0x000B,0x0C30);   
LCD_Write_COM_DATA(0x0021,0x0000);   
LCD_Write_COM_DATA(0x0030,0x0606);   
LCD_Write_COM_DATA(0x0031,0x0606);   
LCD_Write_COM_DATA(0x0032,0x0606);   
LCD_Write_COM_DATA(0x0033,0x0803);   
LCD_Write_COM_DATA(0x0034,0x0202);   
LCD_Write_COM_DATA(0x0035,0x0404);   
LCD_Write_COM_DATA(0x0036,0x0404);   
LCD_Write_COM_DATA(0x0037,0x0404);   
LCD_Write_COM_DATA(0x0038,0x0402);   
LCD_Write_COM_DATA(0x0039,0x100C);   
LCD_Write_COM_DATA(0x003A,0x0101);   
LCD_Write_COM_DATA(0x003B,0x0101);   
LCD_Write_COM_DATA(0x003C,0x0101);   
LCD_Write_COM_DATA(0x003D,0x0101);   
LCD_Write_COM_DATA(0x003E,0x0001);   
LCD_Write_COM_DATA(0x003F,0x0001);   
LCD_Write_COM_DATA(0x0042,0xDB00);   
LCD_Write_COM_DATA(0x0044,0xAF00);   
LCD_Write_COM_DATA(0x0045,0xDB00);   
delay(50);   
LCD_Write_COM_DATA(0x0071,0x0040);   
LCD_Write_COM_DATA(0x0072,0x0002);   
delay(50);   
LCD_Write_COM_DATA(0x0010,0x6060);     
LCD_Write_COM_DATA(0x0007,0x0005);   
delay(50);   
LCD_Write_COM_DATA(0x0007,0x0025);   
LCD_Write_COM_DATA(0x0007,0x0027);   
delay(50);   
LCD_Write_COM_DATA(0x0007,0x0037);   
LCD_Write_COM_DATA(0x0021,0x0000);

LCD_Write_COM(0x22);

	break;