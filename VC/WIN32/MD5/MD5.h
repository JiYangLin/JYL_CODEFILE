#pragma once
#include <io.h>
#include <string>
using namespace std;

class MD5  
{
	//四个寄存器具变量
	unsigned int A,B,C,D;

	//常量

	int S11, S21, S31, S41;
	int S12, S22, S32, S42;
	int S13, S23, S33, S43;
	int S14, S24, S34, S44;

	//对原始信息需要追加的字节数
	int m_AppendByte;

	//原始信息长度,用一个64位整数记录
	unsigned char m_MsgLen[8];

private:

	//位移函数
	unsigned int ROTATE_LEFT(unsigned int x,unsigned int n){return (((x) << (n)) | ((x) >> (32-(n))));}
	unsigned int F(unsigned int x,unsigned int y,unsigned int z){return ((x & y) | ((~x) & z));}
	unsigned int G(unsigned int x,unsigned int y,unsigned int z){return ((x & z) | (y & (~z)));}
	unsigned int H(unsigned int x,unsigned int y,unsigned int z){return x ^ y ^ z;}
	unsigned int I(unsigned int x,unsigned int y,unsigned int z){return (y ^ (x | (~z)));}

	//四轮变换函数
	void FF(unsigned int& a,unsigned int b,unsigned int c,unsigned int d,unsigned int x,int s,unsigned int ac)
	{
		(a) += F ((b), (c), (d)) + (x) + (ac);
		(a) = ROTATE_LEFT ((a), (s));
		(a) += (b);
	}
	void GG(unsigned int& a,unsigned int b,unsigned int c,unsigned int d,unsigned int x,int s,unsigned int ac)
	{
		(a) += G ((b), (c), (d)) + (x) + (ac);
		(a) = ROTATE_LEFT ((a), (s));
		(a) += (b);
	}
	void HH(unsigned int& a,unsigned int b,unsigned int c,unsigned int d,unsigned int x,int s,unsigned int ac)
	{
		(a) += H ((b), (c), (d)) + (x) + (ac);
		(a) = ROTATE_LEFT ((a), (s));
		(a) += (b);
	}
	void II(unsigned int& a,unsigned int b,unsigned int c,unsigned int d,unsigned int x,int s,unsigned int ac)
	{
		(a) += I ((b), (c), (d)) + (x) + (ac);
		(a) = ROTATE_LEFT ((a), (s));
		(a) += (b);
	}


	void Init()
	{
		S11 =  7;	S21 =  5;	S31 =  4;	S41 =  6;
		S12 = 12;	S22 =  9;	S32 = 11;	S42 = 10;
		S13 = 17;	S23 = 14;	S33 = 16;	S43 = 15;
		S14 = 22;	S24 = 20;	S34 = 23;	S44 = 21;

		A = 0x67452301;  // in memory, this is 0x01234567
		B = 0xEFCDAB89;  // in memory, this is 0x89ABCDEF
		C = 0x98BADCFE;  // in memory, this is 0xFEDCBA98
		D = 0x10325476;  // in memory, this is 0x76543210
	}

	//计算追加字节数及记录原始信息长度
	void Append(unsigned int MsgLen)
	{
		//计算要补位的字节数
	int m = MsgLen % 64;
	if(m==0)
		m_AppendByte=56;
	else if(m<56)
		m_AppendByte=56-m;
	else
		m_AppendByte=64-m+56;

	/*
	记录原始信息长度(单位:bit),以下过程将一个32位的字节数转
	换为64位的bit数,这个计算过程和rfc1321.txt中的略有不同.
	*/
	
	//截取传入长度的高十六位和低十六位
	int hWord=(MsgLen & 0xFFFF0000) >> 16;
	int lWord=MsgLen & 0x0000FFFF;

	//将低十六位和高十六位分别乘以八(1byte=8bit)
	int hDiv=hWord*8;
	int lDiv=lWord*8;

	//
	m_MsgLen[0] = lDiv & 0xFF ;
	m_MsgLen[1] = (lDiv >> 8) & 0xFF ;
	m_MsgLen[2] = ((lDiv >> 16) & 0xFF) | (hDiv & 0xFF);
	m_MsgLen[3] = (hDiv >> 8) & 0xFF ;
	m_MsgLen[4] = (hDiv >> 16) & 0xFF ;
	m_MsgLen[5] = (hDiv >> 24) & 0xFF ;
	m_MsgLen[6] = 0;
	m_MsgLen[7] = 0;
	}

	//将原始信息以64字节进行分组,进行错位打乱
	void Transform(unsigned char Block[64])
	{
		//将64字节位转换为16个字节
		unsigned long x[16];
		for (int i=0,j=0;j<64;i++,j+=4)
			x[i]=Block[j] | Block[j+1] <<8 | Block[j+2] <<16 | Block[j+3] <<24 ;

		//初始化临时寄存器变量
		unsigned int a,b,c,d;
		a=A; b=B; c=C; d=D;

		//第一轮计算
		FF (a, b, c, d, x[ 0], S11, 0xD76AA478); //  1
		FF (d, a, b, c, x[ 1], S12, 0xE8C7B756); //  2
		FF (c, d, a, b, x[ 2], S13, 0x242070DB); //  3
		FF (b, c, d, a, x[ 3], S14, 0xC1BDCEEE); //  4
		FF (a, b, c, d, x[ 4], S11, 0xF57C0FAF); //  5
		FF (d, a, b, c, x[ 5], S12, 0x4787C62A); //  6
		FF (c, d, a, b, x[ 6], S13, 0xA8304613); //  7
		FF (b, c, d, a, x[ 7], S14, 0xFD469501); //  8
		FF (a, b, c, d, x[ 8], S11, 0x698098D8); //  9
		FF (d, a, b, c, x[ 9], S12, 0x8B44F7AF); // 10 
		FF (c, d, a, b, x[10], S13, 0xFFFF5BB1); // 11 
		FF (b, c, d, a, x[11], S14, 0x895CD7BE); // 12 
		FF (a, b, c, d, x[12], S11, 0x6B901122); // 13 
		FF (d, a, b, c, x[13], S12, 0xFD987193); // 14 
		FF (c, d, a, b, x[14], S13, 0xA679438E); // 15 
		FF (b, c, d, a, x[15], S14, 0x49B40821); // 16 

		//第二轮计算
		GG (a, b, c, d, x[ 1], S21, 0xF61E2562); // 17 
		GG (d, a, b, c, x[ 6], S22, 0xC040B340); // 18 
		GG (c, d, a, b, x[11], S23, 0x265E5A51); // 19 
		GG (b, c, d, a, x[ 0], S24, 0xE9B6C7AA); // 20 
		GG (a, b, c, d, x[ 5], S21, 0xD62F105D); // 21 
		GG (d, a, b, c, x[10], S22,  0x2441453); // 22 
		GG (c, d, a, b, x[15], S23, 0xD8A1E681); // 23 
		GG (b, c, d, a, x[ 4], S24, 0xE7D3FBC8); // 24 
		GG (a, b, c, d, x[ 9], S21, 0x21E1CDE6); // 25 
		GG (d, a, b, c, x[14], S22, 0xC33707D6); // 26 
		GG (c, d, a, b, x[ 3], S23, 0xF4D50D87); // 27 
		GG (b, c, d, a, x[ 8], S24, 0x455A14ED); // 28 
		GG (a, b, c, d, x[13], S21, 0xA9E3E905); // 29 
		GG (d, a, b, c, x[ 2], S22, 0xFCEFA3F8); // 30 
		GG (c, d, a, b, x[ 7], S23, 0x676F02D9); // 31 
		GG (b, c, d, a, x[12], S24, 0x8D2A4C8A); // 32 

		//第三轮计算
		HH (a, b, c, d, x[ 5], S31, 0xFFFA3942); // 33 
		HH (d, a, b, c, x[ 8], S32, 0x8771F681); // 34 
		HH (c, d, a, b, x[11], S33, 0x6D9D6122); // 35 
		HH (b, c, d, a, x[14], S34, 0xFDE5380C); // 36 
		HH (a, b, c, d, x[ 1], S31, 0xA4BEEA44); // 37 
		HH (d, a, b, c, x[ 4], S32, 0x4BDECFA9); // 38 
		HH (c, d, a, b, x[ 7], S33, 0xF6BB4B60); // 39 
		HH (b, c, d, a, x[10], S34, 0xBEBFBC70); // 40 
		HH (a, b, c, d, x[13], S31, 0x289B7EC6); // 41 
		HH (d, a, b, c, x[ 0], S32, 0xEAA127FA); // 42 
		HH (c, d, a, b, x[ 3], S33, 0xD4EF3085); // 43 
		HH (b, c, d, a, x[ 6], S34,  0x4881D05); // 44 
		HH (a, b, c, d, x[ 9], S31, 0xD9D4D039); // 45 
		HH (d, a, b, c, x[12], S32, 0xE6DB99E5); // 46 
		HH (c, d, a, b, x[15], S33, 0x1FA27CF8); // 47 
		HH (b, c, d, a, x[ 2], S34, 0xC4AC5665); // 48 

		//第四轮计算
		II (a, b, c, d, x[ 0], S41, 0xF4292244); // 49 
		II (d, a, b, c, x[ 7], S42, 0x432AFF97); // 50 
		II (c, d, a, b, x[14], S43, 0xAB9423A7); // 51 
		II (b, c, d, a, x[ 5], S44, 0xFC93A039); // 52 
		II (a, b, c, d, x[12], S41, 0x655B59C3); // 53 
		II (d, a, b, c, x[ 3], S42, 0x8F0CCC92); // 54 
		II (c, d, a, b, x[10], S43, 0xFFEFF47D); // 55 
		II (b, c, d, a, x[ 1], S44, 0x85845DD1); // 56 
		II (a, b, c, d, x[ 8], S41, 0x6FA87E4F); // 57 
		II (d, a, b, c, x[15], S42, 0xFE2CE6E0); // 58 
		II (c, d, a, b, x[ 6], S43, 0xA3014314); // 59 
		II (b, c, d, a, x[13], S44, 0x4E0811A1); // 60 
		II (a, b, c, d, x[ 4], S41, 0xF7537E82); // 61 
		II (d, a, b, c, x[11], S42, 0xBD3AF235); // 62 
		II (c, d, a, b, x[ 2], S43, 0x2AD7D2BB); // 63 
		II (b, c, d, a, x[ 9], S44, 0xEB86D391); // 64 

		//保存当前寄存器结果
		A+=a; B+=b; C+=c; D+=d;
	}

	//将寄存器A,B,C,D中最后保存的值转换为十六进制并返回给用户
	string ToHex(bool upperCase)
	{
		string strResult;
		int ResultArray[4]={A,B,C,D};
		char Buf[33]={0};
		for(int i=0;i<4;i++)
		{
			memset(Buf,0,3);
			sprintf_s(Buf,"%02x",ResultArray[i] & 0x00FF);
			strResult+=Buf;

			memset(Buf,0,3);
			sprintf_s(Buf,"%02x",(ResultArray[i] >>  8) & 0x00FF);
			strResult+=Buf;

			memset(Buf,0,3);
			sprintf_s(Buf,"%02x",(ResultArray[i] >> 16) & 0x00FF);
			strResult+=Buf;

			memset(Buf,0,3);
			sprintf_s(Buf,"%02x",(ResultArray[i] >> 24) & 0x00FF);
			strResult+=Buf;
		}

		if(upperCase) CharUpperA((char *)strResult.c_str());
		return strResult;
	}

public:
	string GetMD5OfString(string InputMessage,bool upperCase=false)
	{
		//初始化MD5所需常量
		Init();

		//计算追加长度
		Append(InputMessage.length());
		//对原始信息进行补位
		unsigned char uc;
		for(int i=0;i<m_AppendByte;i++)
		{
			if(i==0) InputMessage+=(unsigned char)0x80;
			else 
			{
				uc=(unsigned char)0x0;
				InputMessage+=uc;
			}
		}

		//将原始信息长度附加在补位后的数据后面
		for(int i=0;i<8;i++) InputMessage+=m_MsgLen[i];

		//位块数组
		unsigned char x[64]={0};

		//循环,将原始信息以64字节为一组拆分进行处理
		for(int i=0,Index=-1;i<InputMessage.length();i++)
		{
			x[++Index]=InputMessage[i];
			if(Index==63)
			{
				Index=-1;
				Transform(x);
			}
		}
		return ToHex(upperCase);
	}
};
