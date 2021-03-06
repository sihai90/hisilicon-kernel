/*
********************************************************************************
* File Name	 :   	Stv6110A.c                                                                                                 
* Author		 :     Louhq                                                                                                  
* Project    	 :   	GXAPI 
* Type      	 :     Driver
********************************************************************************
* Purpose   :  Stv6110A  driver
********************************************************************************
* Release History:
* VERSION   Date              AUTHOR          Description
*    1.00      2010.4.6      	Louhq           Original version   
********************************************************************************
*Abbreviation
*   GX		--	GUOXIN 
*   RF		--  radiate frequency
*   SNR		--	signal to noise ratio
*   OSC		--	oscillate
*   SPEC		--	spectrum
*   FREQ		--	frequency
********************************************************************************/
#include "GX1132.h"
#include <linux/delay.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/time.h>
#include <linux/module.h>
#include <asm/io.h>
#include <linux/kthread.h>

#ifdef TUNER_STV6110A 
 
#define GX_TUNER_ADDRESS 0xC0

static u8 deviceAddr = 0xC0;

GX1132_STATE STV6110AInit(void);
/*******************************************************************************
*Function:	GX1132_Set_Tuner
*Input:
*		RFfreq:	 		Fcenter at L band, unit:MHz
*		Symbol_Rate:		symbol rate, unit:kBaud
*Output:
*		SUCCESS or FAILURE
********************************************************************************/
GX1132_STATE GX_Set_STV6110A(u32 RFfreq,u32 Symbol_Rate)
{     
	u8 	P=0;
	u8   Presc=0;  
	u8   PD=0;
	u8 	Fosc=16;
	u8	A=0,N=0,K=0;
	u32 Bandwidth=0;
	u8 	R=0,R_DIV=0;
//	u8	i=0;
	u8 	CO=0;
	u8	REF=1;
	u8 	BB_Gain=8;	
	static u8 tuner_initial =0;
	u32 	freq_tmp=0;
	u8 	regdata[8]={0};
	GX1132_STATE 	temp;
//	GX1132_STATE state = SUCCESS;
/*
	u8 regaddr[]={
		0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};
*/
	if(tuner_initial==0)
		{
			STV6110AInit();
			tuner_initial =1;
		}
	if (RFfreq<950)
		return FAILURE;
	else if(RFfreq<=1023)
	{
		P=1;
		Presc=0;
	}
	else if(RFfreq<=1300)
	{
		P=1;
		Presc=1;
	}
	else if(RFfreq<=2046)
	{
		P=0;
		Presc=0;
	}
	else 
	{
		P=0;
		Presc=1;
	}
	//Bandwidth=(Symbol_Rate*13.5+5)/10000;
	Bandwidth=(((Symbol_Rate*135)/10)+5)/10000;
	if(Bandwidth/2<5) PD=0;
	else if(Bandwidth/2>36) PD=31;
	else PD=Bandwidth/2-5;	
	K=K&0xFF;
	K=Fosc-16;
	regdata[0]=(u8)(((K&0x1F)<<3)|0x07);
	temp=GX_I2cReadWrite(GX_WRITE,deviceAddr, 0,&regdata[0],1);
	
	if(P==0) R_DIV=Fosc/2;
	else if(P==1) R_DIV=Fosc/4;
	if(R_DIV==16)
		R=0x03;
	else if(R_DIV==8)
		R=0x02;
	else if(R_DIV==4)
		R=0x01;
	else R=0x00;
	freq_tmp= RFfreq;
	A= (u8) (freq_tmp&0xFF);
	N= (u8)((freq_tmp>>8)&0x0F); 	
	regdata[2]=(u8)(A&0xFF);
	regdata[3]=(u8)(((R&0x03)<<6)|((Presc&0x01)<<5)|((P&0x01)<<4)|(N&0x0F));
	temp=GX_I2cReadWrite(GX_WRITE,deviceAddr, 3,&regdata[3],1);
	temp=GX_I2cReadWrite(GX_WRITE,deviceAddr, 2,&regdata[2],1);
	GX_I2cReadWrite(GX_READ, deviceAddr, 5, &regdata[5], 1);
	regdata[5]=(u8)((regdata[5]&0xFB)|0x04);
	temp=GX_I2cReadWrite(GX_WRITE,deviceAddr, 5,&regdata[5],1);
	GX_Delay_N_ms(20);
	GX_I2cReadWrite(GX_READ, deviceAddr, 4, &regdata[4], 1);
	regdata[4]=(u8)(PD&0x1F);
	temp=GX_I2cReadWrite(GX_WRITE,deviceAddr, 4,&regdata[4],1);
	GX_I2cReadWrite(GX_READ, deviceAddr, 5, &regdata[5], 1);
	regdata[5]=(u8)((regdata[5]&0xFD)|0x02);
	temp=GX_I2cReadWrite(GX_WRITE,deviceAddr, 5,&regdata[5],1);
	
	GX_Delay_N_ms(10);
	GX_I2cReadWrite(GX_READ, deviceAddr, 4, &regdata[4], 1);
	regdata[4]=(u8)((regdata[4]&0xBF)|0x40);
	temp=GX_I2cReadWrite(GX_WRITE,deviceAddr, 4,&regdata[4],1);
	
	regdata[1]=(u8)(((CO&0x03)<<6)|((REF&0x01)<<4)|(BB_Gain&0x0F)|0x20);
	temp=GX_I2cReadWrite(GX_WRITE,deviceAddr, 1,&regdata[1],1);


/*	for(i=0;i<8;i++)
	{
		state|=GX_I2cReadWrite(GX_WRITE,deviceAddr,regaddr[i],&regdata[i],1);
		print("1REGDATA[%d]=0x%x\n",i,regdata[i]);
	}
	*/
	return SUCCESS;
}


/*
********************************************************************************
Function:	STV6110AInit
Input:
Output:
	SUCCESS or FAILURE
********************************************************************************
*/
GX1132_STATE STV6110AInit(void)
{
/*
	u8 i = 0;
    GX1132_STATE state = SUCCESS;

	u8 regaddr[]={
		0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};
	u8 regdata[]={
		0xdf,0x33,0xf2,0xc6,0x17,0x01,0x04,0x1e};
		//0x07,0x33,0x66,0xa8,0x17,0x01,0xa2,0x1e};

	for(i=0;i<8;i++)
	{
		state|=GX_I2cReadWrite(GX_WRITE,deviceAddr,regaddr[i],&regdata[i],1);
		print("REGDATA[%d]=0x%x\n",i,regdata[i]);
	}
*/
	GX_Delay_N_ms(10);


	return SUCCESS;
}

#endif
//#endif
