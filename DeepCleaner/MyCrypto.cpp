#include "stdafx.h"
#include "MyCrypto.h"
#include <stdio.h>
#include <iostream>

#include <windows.h>
#include <wincrypt.h>
#include <stdlib.h>


MyCrypto::MyCrypto(PCHAR Source, PCHAR Destination, PCHAR Password, INT select){
	m_Source = Source;
	m_Destination = Destination;
	m_Password = Password;
	m_Select = select;
	if (m_Select != 1 && m_Select != 2)
		MyCrypto::HandleError("构造2失败！");
}

MyCrypto::MyCrypto(INT select){
	m_Select = select;
	if (m_Select != 1 && m_Select != 2)
		MyCrypto::HandleError("构造1失败！");
}

BOOL MyCrypto::set(PCHAR Source, PCHAR Destination, PCHAR Password){
	m_Source = Source;
	m_Destination = Destination;
	m_Password = Password;
	return true;
}

BOOL MyCrypto::run(){
	if (m_Select == 1){
		EncryptFile(m_Source, m_Destination, m_Password);
		return true;
	}
	else if (m_Select == 2){
		DecryptFile(m_Source, m_Destination, m_Password);
		return true;
	}
	else{
		HandleError("加解密失败！");
		return false;
	}
}
MyCrypto::~MyCrypto()
{
}


BOOL MyCrypto::EncryptFile(
	PCHAR szSource,
	PCHAR szDestination,
	PCHAR szPassword)
{
	//--------------------------------------------------------------------
	//   声明变量

	FILE *hSource;
	FILE *hDestination;

	HCRYPTPROV hCryptProv;
	HCRYPTKEY hKey;
	HCRYPTHASH hHash;

	PBYTE pbBuffer;
	DWORD dwBlockLen;
	DWORD dwBufferLen;
	DWORD dwCount;

	//--------------------------------------------------------------------
	// 打开加密文件
	if (!(hSource = fopen(szSource, "rb")))
	{
		HandleError("打开源文件错误!");
	}

	//--------------------------------------------------------------------
	// 打开输出文件
	if (!(hDestination = fopen(szDestination, "wb")))
	{
		HandleError("打开目标文件错误!!");
	}

	//以下获得一个CSP句柄
	if (!(CryptAcquireContext(
		&hCryptProv,
		NULL,				//NULL表示使用默认密钥容器，默认密钥容器名为用户登陆名
		NULL,
		PROV_RSA_FULL,
		0)))
	{
		if (CryptAcquireContext(
			&hCryptProv,
			NULL,
			NULL,
			PROV_RSA_FULL,
			CRYPT_NEWKEYSET))//创建密钥容器
		{
			//创建密钥容器成功，并得到CSP句柄
			printf("创建密钥容器成功.\n");
		}
		else
		{
			HandleError("创建密钥容器失败.\n");
		}

	}

	//--------------------------------------------------------------------
	// 创建一个会话密钥（session key）
	// 会话密钥也叫对称密钥，用于对称加密算法。
	// （注: 一个Session是指从调用函数CryptAcquireContext到调用函数
	//   CryptReleaseContext 期间的阶段。）

	if (!(CryptCreateHash(
		hCryptProv,
		CALG_MD5,
		0,
		0,
		&hHash)))
	{
		HandleError("创建会话密钥失败!\n");
	}

	//--------------------------------------------------------------------
	// 用输入的密码产生一个散列
	if (!(CryptHashData(
		hHash,
		(BYTE *)szPassword,
		strlen(szPassword),
		0)))
	{
		HandleError("密码散列产生失败. \n");
	}

	//--------------------------------------------------------------------
	// 通过散列生成会话密钥(session key)
	if (!(CryptDeriveKey(
		hCryptProv,
		ENCRYPT_ALGORITHM,
		hHash,
		KEYLENGTH,
		&hKey)))
	{
		HandleError("生成会话密钥失败!\n");
	}
	//--------------------------------------------------------------------
	// 销毁hash对象

	CryptDestroyHash(hHash);
	hHash = NULL;


	//--------------------------------------------------------------------
	// 因为加密算法是按ENCRYPT_BLOCK_SIZE 大小的块加密的，所以被加密的
	// 数据长度必须是ENCRYPT_BLOCK_SIZE 的整数倍。下面计算一次加密的
	// 数据长度。

	dwBlockLen = 1000 - 1000 % ENCRYPT_BLOCK_SIZE;

	//--------------------------------------------------------------------
	// 解决非整块的部分

	if (ENCRYPT_BLOCK_SIZE > 1)
		dwBufferLen = dwBlockLen + ENCRYPT_BLOCK_SIZE;
	else
		dwBufferLen = dwBlockLen;

	//--------------------------------------------------------------------
	// 分配内存
	if (!(pbBuffer = (BYTE *)malloc(dwBufferLen)))
	{
		HandleError("内存溢出. \n");
	}
	//--------------------------------------------------------------------
	// 加密过程开始

	do
	{

		//--------------------------------------------------------------------
		// Read up to dwBlockLen bytes from the source file. 
		dwCount = fread(pbBuffer, 1, dwBlockLen, hSource);
		if (ferror(hSource))
		{
			HandleError("读取文件错误!\n");
		}

		//--------------------------------------------------------------------
		// 加密数据
		if (!CryptEncrypt(
			hKey,			//密钥
			0,				//如果数据同时进行散列和加密，这里传入一个散列对象
			feof(hSource),	//如果是最后一个被加密的块，输入TRUE.如果不是输入FALSE.
			//这里通过判断是否到文件尾来决定是否为最后一块。
			0,				//保留
			pbBuffer,		//输入被加密数据，输出加密后的数据
			&dwCount,		//输入被加密数据实际长度，输出加密后数据长度
			dwBufferLen))	//pbBuffer的大小。
		{
			HandleError("加密错误. \n");
		}

		//--------------------------------------------------------------------
		// 文件写入

		fwrite(pbBuffer, 1, dwCount, hDestination);
		if (ferror(hDestination))
		{
			HandleError("文件写入错误.");
		}

	} while (!feof(hSource));


	if (hSource)
		fclose(hSource);
	if (hDestination)
		fclose(hDestination);

	//--------------------------------------------------------------------
	// 释放内存和密钥

	if (pbBuffer)
		free(pbBuffer);

	if (hKey)
		CryptDestroyKey(hKey);

	if (hHash)
		CryptDestroyHash(hHash);

	if (hCryptProv)
		CryptReleaseContext(hCryptProv, 0);
	return(TRUE);
}


BOOL MyCrypto::DecryptFile(
	PCHAR szSource,
	PCHAR szDestination,
	PCHAR szPassword)
{
	//--------------------------------------------------------------------
	//   声明变量

	FILE *hSource;
	FILE *hDestination;

	HCRYPTPROV hCryptProv;
	HCRYPTKEY hKey;
	HCRYPTHASH hHash;

	PBYTE pbBuffer;
	DWORD dwBlockLen;
	DWORD dwBufferLen;
	DWORD dwCount;

	BOOL status = FALSE;

	//--------------------------------------------------------------------
	// 打开加密文件. 
	if (!(hSource = fopen(szSource, "rb")))
	{
		HandleError("打开文件错误!");
	}
	//--------------------------------------------------------------------
	// 打开输出文件. 

	if (!(hDestination = fopen(szDestination, "wb")))
	{
		HandleError("打开文件错误!");
	}
	//--------------------------------------------------------------------
	// 获得句柄. 
	if (!CryptAcquireContext(
		&hCryptProv,
		NULL,
		NULL,
		PROV_RSA_FULL,
		0))
	{
		HandleError("获取句柄失败!");
	}

	//--------------------------------------------------------------------
	// 生成一个hash散列

	if (!CryptCreateHash(
		hCryptProv,
		CALG_MD5,
		0,
		0,
		&hHash))
	{
		HandleError("hash获取失败!");
	}
	//--------------------------------------------------------------------
	// 对数据进行散列. 

	if (!CryptHashData(
		hHash,
		(BYTE *)szPassword,
		strlen(szPassword),
		0))
	{
		HandleError("数据散列产生失败!");
	}
	//--------------------------------------------------------------------
	// 生成会话密钥. 

	if (!CryptDeriveKey(
		hCryptProv,
		ENCRYPT_ALGORITHM,
		hHash,
		KEYLENGTH,
		&hKey))
	{
		HandleError("生成会话密钥失败!");
	}
	//--------------------------------------------------------------------
	// 销毁hash对象. 

	CryptDestroyHash(hHash);
	hHash = 0;

	//--------------------------------------------------------------------
	// 因为加密算法是按ENCRYPT_BLOCK_SIZE 大小的块加密的，所以被加密的
	// 数据长度必须是ENCRYPT_BLOCK_SIZE 的整数倍。下面计算一次加密的
	// 数据长度。

	dwBlockLen = 1000 - 1000 % ENCRYPT_BLOCK_SIZE;
	dwBufferLen = dwBlockLen;

	//--------------------------------------------------------------------
	// Allocate memory. 

	if (!(pbBuffer = (BYTE *)malloc(dwBufferLen)))
	{
		HandleError("内存溢出!\n");
	}
	//--------------------------------------------------------------------
	// 解密文件并写入. 

	do {
		//--------------------------------------------------------------------
		// 读入密文. 

		dwCount = fread(
			pbBuffer,
			1,
			dwBlockLen,
			hSource);
		if (ferror(hSource))
		{
			HandleError("读取文件失败!");
		}
		//--------------------------------------------------------------------
		// 解密数据. 
		if (!CryptDecrypt(
			hKey,
			0,
			feof(hSource),
			0,
			pbBuffer,
			&dwCount))
		{
			HandleError("解密数据失败!");
		}
		//--------------------------------------------------------------------
		// 写入输出文件. 

		fwrite(
			pbBuffer,
			1,
			dwCount,
			hDestination);
		if (ferror(hDestination))
		{
			HandleError("写入失败!");
		}
	} while (!feof(hSource));
	status = TRUE;

	//--------------------------------------------------------------------
	// 关闭文件. 
	if (hSource)
		fclose(hSource);
	if (hDestination)
		fclose(hDestination);

	//--------------------------------------------------------------------
	// 释放内存和销毁密钥. 

	if (pbBuffer)
		free(pbBuffer);

	if (hKey)
		CryptDestroyKey(hKey);

	if (hHash)
		CryptDestroyHash(hHash);

	if (hCryptProv)
		CryptReleaseContext(hCryptProv, 0);

	return status;
}

CString MyCrypto::DecryptString(CString &text,CString &Password)
{
	//--------------------------------------------------------------------
	//   声明变量

	HCRYPTPROV hCryptProv;
	HCRYPTKEY hKey;
	HCRYPTHASH hHash;

	PBYTE pbBuffer;
	DWORD dwBlockLen;
	DWORD dwBufferLen;
	DWORD dwCount;


	PCHAR szPassword=Password.GetBuffer();
	PCHAR szText = text.GetBuffer();
	CString result = "";

	//--------------------------------------------------------------------
	// 获得句柄. 
	if (!CryptAcquireContext(
		&hCryptProv,
		NULL,
		NULL,
		PROV_RSA_FULL,
		0))
	{
	}

	//--------------------------------------------------------------------
	// 生成一个hash散列

	if (!CryptCreateHash(
		hCryptProv,
		CALG_MD5,
		0,
		0,
		&hHash))
	{
	}
	//--------------------------------------------------------------------
	// 对数据进行散列. 

	if (!CryptHashData(
		hHash,
		(BYTE *)szPassword,
		strlen(szPassword),
		0))
	{
	}
	//--------------------------------------------------------------------
	// 生成会话密钥. 

	if (!CryptDeriveKey(
		hCryptProv,
		ENCRYPT_ALGORITHM,
		hHash,
		KEYLENGTH,
		&hKey))
	{
	}
	//--------------------------------------------------------------------
	// 销毁hash对象. 

	CryptDestroyHash(hHash);
	hHash = 0;

	//--------------------------------------------------------------------
	// 因为加密算法是按ENCRYPT_BLOCK_SIZE 大小的块加密的，所以被加密的
	// 数据长度必须是ENCRYPT_BLOCK_SIZE 的整数倍。下面计算一次加密的
	// 数据长度。

	dwBlockLen = 1000 - 1000 % ENCRYPT_BLOCK_SIZE;
	dwBufferLen = dwBlockLen;

	//--------------------------------------------------------------------
	// Allocate memory. 

	if (!(pbBuffer = (BYTE *)malloc(dwBufferLen)))
	{
	}
	//--------------------------------------------------------------------
	// 解密文件并写入. 
	dwCount = sizeof(text);
	
	do {
		//--------------------------------------------------------------------
		// 读入密文. 
		memcpy(pbBuffer, szText, ENCRYPT_BLOCK_SIZE);
		
		//--------------------------------------------------------------------
		// 解密数据. 
		if (!CryptDecrypt(
			hKey,
			0,
			dwCount / ENCRYPT_BLOCK_SIZE,
			0,
			pbBuffer,
			&dwCount))
		{
			dwCount -= 7;
			szText += ENCRYPT_BLOCK_SIZE;
		}
		//--------------------------------------------------------------------
		// 写入输出文件. 

		result += (CString)pbBuffer;

	} while (!(dwCount / ENCRYPT_BLOCK_SIZE));


	//--------------------------------------------------------------------
	// 释放内存和销毁密钥. 

	if (pbBuffer)
		free(pbBuffer);

	if (hKey)
		CryptDestroyKey(hKey);

	if (hHash)
		CryptDestroyHash(hHash);

	if (hCryptProv)
		CryptReleaseContext(hCryptProv, 0);

	return result;
}

CString MyCrypto::EncryptString(CString &text, CString &Password)
{
	//--------------------------------------------------------------------
	//   声明变量

	HCRYPTPROV hCryptProv;
	HCRYPTKEY hKey;
	HCRYPTHASH hHash;

	PBYTE pbBuffer;
	DWORD dwBlockLen;
	DWORD dwBufferLen;
	DWORD dwCount;

	CString result;
	PCHAR szPassword = text.GetBuffer();
	PCHAR szText = text.GetBuffer();

	//以下获得一个CSP句柄
	if (!(CryptAcquireContext(
		&hCryptProv,
		NULL,				//NULL表示使用默认密钥容器，默认密钥容器名为用户登陆名
		NULL,
		PROV_RSA_FULL,
		0)))
	{
		if (CryptAcquireContext(
			&hCryptProv,
			NULL,
			NULL,
			PROV_RSA_FULL,
			CRYPT_NEWKEYSET))//创建密钥容器
		{
			//创建密钥容器成功，并得到CSP句柄
			printf("创建密钥容器成功.\n");
		}
		else
		{
		}

	}

	//--------------------------------------------------------------------
	// 创建一个会话密钥（session key）
	// 会话密钥也叫对称密钥，用于对称加密算法。
	// （注: 一个Session是指从调用函数CryptAcquireContext到调用函数
	//   CryptReleaseContext 期间的阶段。）

	if (!(CryptCreateHash(
		hCryptProv,
		CALG_MD5,
		0,
		0,
		&hHash)))
	{
	}

	//--------------------------------------------------------------------
	// 用输入的密码产生一个散列
	if (!(CryptHashData(
		hHash,
		(BYTE *)szPassword,
		strlen(szPassword),
		0)))
	{

	}

	//--------------------------------------------------------------------
	// 通过散列生成会话密钥(session key)
	if (!(CryptDeriveKey(
		hCryptProv,
		ENCRYPT_ALGORITHM,
		hHash,
		KEYLENGTH,
		&hKey)))
	{
	}
	//--------------------------------------------------------------------
	// 销毁hash对象

	CryptDestroyHash(hHash);
	hHash = NULL;


	//--------------------------------------------------------------------
	// 因为加密算法是按ENCRYPT_BLOCK_SIZE 大小的块加密的，所以被加密的
	// 数据长度必须是ENCRYPT_BLOCK_SIZE 的整数倍。下面计算一次加密的
	// 数据长度。

	dwBlockLen = 1000 - 1000 % ENCRYPT_BLOCK_SIZE;

	//--------------------------------------------------------------------
	// 解决非整块的部分

	if (ENCRYPT_BLOCK_SIZE > 1)
		dwBufferLen = dwBlockLen + ENCRYPT_BLOCK_SIZE;
	else
		dwBufferLen = dwBlockLen;

	//--------------------------------------------------------------------
	// 分配内存
	if (!(pbBuffer = (BYTE *)malloc(dwBufferLen)))
	{
	}
	//--------------------------------------------------------------------
	// 加密过程开始
	dwCount = text.GetLength() * sizeof(text);
	do
	{
		memcpy(pbBuffer, szText, ENCRYPT_BLOCK_SIZE);
		
		//--------------------------------------------------------------------
		// Read up to dwBlockLen bytes from the source file. 
		
		
		//--------------------------------------------------------------------
		// 加密数据
		if (!CryptEncrypt(
			hKey,			//密钥
			0,				//如果数据同时进行散列和加密，这里传入一个散列对象
			!(dwCount /ENCRYPT_BLOCK_SIZE),	//如果是最后一个被加密的块，输入TRUE.如果不是输入FALSE.
			//这里通过判断是否到文件尾来决定是否为最后一块。
			0,				//保留
			pbBuffer,		//输入被加密数据，输出加密后的数据
			&dwCount,		//输入被加密数据实际长度，输出加密后数据长度
			dwBufferLen))	//pbBuffer的大小。
		{
			dwCount -= 7;
			szText += ENCRYPT_BLOCK_SIZE;
		}

		//--------------------------------------------------------------------
		// 文件写入

		result += (CString)pbBuffer;
	} while ((dwCount / ENCRYPT_BLOCK_SIZE));


	//--------------------------------------------------------------------
	// 释放内存和密钥

	if (pbBuffer)
		free(pbBuffer);

	if (hKey)
		CryptDestroyKey(hKey);

	if (hHash)
		CryptDestroyHash(hHash);

	if (hCryptProv)
		CryptReleaseContext(hCryptProv, 0);

	return result;
}

//异常处理
void MyCrypto::HandleError(char *s)
{
	fprintf(stderr, "程序错误. \n");
	fprintf(stderr, "%s\n", s);
	fprintf(stderr, "错误代码 %x.\n", GetLastError());
	fprintf(stderr, "程序已终止. \n");
	exit(1);
} // End of HandleError