#pragma once
#include "stdafx.h"
#include <windows.h>
#include <wincrypt.h>

//--------------------------------------------------------------------
//编码类型和密钥长度
#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)
#define KEYLENGTH  0x00800000

//--------------------------------------------------------------------
//  宏定义 加密算法和分块
#define ENCRYPT_ALGORITHM CALG_RC4 
#define ENCRYPT_BLOCK_SIZE 7 
class MyCrypto
{
private:
	PCHAR m_Source ;
	PCHAR m_Destination ;
	PCHAR m_Password ;
	INT m_Select = 0;
public:
	MyCrypto();
	~MyCrypto();
	MyCrypto(PCHAR Source, PCHAR Destination, PCHAR Password, INT select);
	MyCrypto(CString &Source, CString &Destination, CString &Password, INT select){
		MyCrypto((PCHAR)Source.GetBuffer(), (PCHAR)Destination.GetBuffer(), (PCHAR)Password.GetBuffer(), 1);
	}
	MyCrypto(INT select);

	BOOL set(PCHAR Source, PCHAR Destination, PCHAR Password);
	BOOL run();

	BOOL EncryptFile(PCHAR szSource,
		PCHAR szDestination,
		PCHAR szPassword);
	BOOL DecryptFile(
		PCHAR szSource,
		PCHAR szDestination,
		PCHAR szPassword);
	static CString EncryptString(CString &text, CString &Password);
	static CString DecryptString(CString &text, CString &Password);

	void HandleError(char *s);
};

