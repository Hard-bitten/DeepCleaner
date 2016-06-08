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
		MyCrypto::HandleError("����2ʧ�ܣ�");
}

MyCrypto::MyCrypto(INT select){
	m_Select = select;
	if (m_Select != 1 && m_Select != 2)
		MyCrypto::HandleError("����1ʧ�ܣ�");
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
		HandleError("�ӽ���ʧ�ܣ�");
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
	//   ��������

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
	// �򿪼����ļ�
	if (!(hSource = fopen(szSource, "rb")))
	{
		HandleError("��Դ�ļ�����!");
	}

	//--------------------------------------------------------------------
	// ������ļ�
	if (!(hDestination = fopen(szDestination, "wb")))
	{
		HandleError("��Ŀ���ļ�����!!");
	}

	//���»��һ��CSP���
	if (!(CryptAcquireContext(
		&hCryptProv,
		NULL,				//NULL��ʾʹ��Ĭ����Կ������Ĭ����Կ������Ϊ�û���½��
		NULL,
		PROV_RSA_FULL,
		0)))
	{
		if (CryptAcquireContext(
			&hCryptProv,
			NULL,
			NULL,
			PROV_RSA_FULL,
			CRYPT_NEWKEYSET))//������Կ����
		{
			//������Կ�����ɹ������õ�CSP���
			printf("������Կ�����ɹ�.\n");
		}
		else
		{
			HandleError("������Կ����ʧ��.\n");
		}

	}

	//--------------------------------------------------------------------
	// ����һ���Ự��Կ��session key��
	// �Ự��ԿҲ�жԳ���Կ�����ڶԳƼ����㷨��
	// ��ע: һ��Session��ָ�ӵ��ú���CryptAcquireContext�����ú���
	//   CryptReleaseContext �ڼ�Ľ׶Ρ���

	if (!(CryptCreateHash(
		hCryptProv,
		CALG_MD5,
		0,
		0,
		&hHash)))
	{
		HandleError("�����Ự��Կʧ��!\n");
	}

	//--------------------------------------------------------------------
	// ��������������һ��ɢ��
	if (!(CryptHashData(
		hHash,
		(BYTE *)szPassword,
		strlen(szPassword),
		0)))
	{
		HandleError("����ɢ�в���ʧ��. \n");
	}

	//--------------------------------------------------------------------
	// ͨ��ɢ�����ɻỰ��Կ(session key)
	if (!(CryptDeriveKey(
		hCryptProv,
		ENCRYPT_ALGORITHM,
		hHash,
		KEYLENGTH,
		&hKey)))
	{
		HandleError("���ɻỰ��Կʧ��!\n");
	}
	//--------------------------------------------------------------------
	// ����hash����

	CryptDestroyHash(hHash);
	hHash = NULL;


	//--------------------------------------------------------------------
	// ��Ϊ�����㷨�ǰ�ENCRYPT_BLOCK_SIZE ��С�Ŀ���ܵģ����Ա����ܵ�
	// ���ݳ��ȱ�����ENCRYPT_BLOCK_SIZE �����������������һ�μ��ܵ�
	// ���ݳ��ȡ�

	dwBlockLen = 1000 - 1000 % ENCRYPT_BLOCK_SIZE;

	//--------------------------------------------------------------------
	// ���������Ĳ���

	if (ENCRYPT_BLOCK_SIZE > 1)
		dwBufferLen = dwBlockLen + ENCRYPT_BLOCK_SIZE;
	else
		dwBufferLen = dwBlockLen;

	//--------------------------------------------------------------------
	// �����ڴ�
	if (!(pbBuffer = (BYTE *)malloc(dwBufferLen)))
	{
		HandleError("�ڴ����. \n");
	}
	//--------------------------------------------------------------------
	// ���ܹ��̿�ʼ

	do
	{

		//--------------------------------------------------------------------
		// Read up to dwBlockLen bytes from the source file. 
		dwCount = fread(pbBuffer, 1, dwBlockLen, hSource);
		if (ferror(hSource))
		{
			HandleError("��ȡ�ļ�����!\n");
		}

		//--------------------------------------------------------------------
		// ��������
		if (!CryptEncrypt(
			hKey,			//��Կ
			0,				//�������ͬʱ����ɢ�кͼ��ܣ����ﴫ��һ��ɢ�ж���
			feof(hSource),	//��������һ�������ܵĿ飬����TRUE.�����������FALSE.
			//����ͨ���ж��Ƿ��ļ�β�������Ƿ�Ϊ���һ�顣
			0,				//����
			pbBuffer,		//���뱻�������ݣ�������ܺ������
			&dwCount,		//���뱻��������ʵ�ʳ��ȣ�������ܺ����ݳ���
			dwBufferLen))	//pbBuffer�Ĵ�С��
		{
			HandleError("���ܴ���. \n");
		}

		//--------------------------------------------------------------------
		// �ļ�д��

		fwrite(pbBuffer, 1, dwCount, hDestination);
		if (ferror(hDestination))
		{
			HandleError("�ļ�д�����.");
		}

	} while (!feof(hSource));


	if (hSource)
		fclose(hSource);
	if (hDestination)
		fclose(hDestination);

	//--------------------------------------------------------------------
	// �ͷ��ڴ����Կ

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
	//   ��������

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
	// �򿪼����ļ�. 
	if (!(hSource = fopen(szSource, "rb")))
	{
		HandleError("���ļ�����!");
	}
	//--------------------------------------------------------------------
	// ������ļ�. 

	if (!(hDestination = fopen(szDestination, "wb")))
	{
		HandleError("���ļ�����!");
	}
	//--------------------------------------------------------------------
	// ��þ��. 
	if (!CryptAcquireContext(
		&hCryptProv,
		NULL,
		NULL,
		PROV_RSA_FULL,
		0))
	{
		HandleError("��ȡ���ʧ��!");
	}

	//--------------------------------------------------------------------
	// ����һ��hashɢ��

	if (!CryptCreateHash(
		hCryptProv,
		CALG_MD5,
		0,
		0,
		&hHash))
	{
		HandleError("hash��ȡʧ��!");
	}
	//--------------------------------------------------------------------
	// �����ݽ���ɢ��. 

	if (!CryptHashData(
		hHash,
		(BYTE *)szPassword,
		strlen(szPassword),
		0))
	{
		HandleError("����ɢ�в���ʧ��!");
	}
	//--------------------------------------------------------------------
	// ���ɻỰ��Կ. 

	if (!CryptDeriveKey(
		hCryptProv,
		ENCRYPT_ALGORITHM,
		hHash,
		KEYLENGTH,
		&hKey))
	{
		HandleError("���ɻỰ��Կʧ��!");
	}
	//--------------------------------------------------------------------
	// ����hash����. 

	CryptDestroyHash(hHash);
	hHash = 0;

	//--------------------------------------------------------------------
	// ��Ϊ�����㷨�ǰ�ENCRYPT_BLOCK_SIZE ��С�Ŀ���ܵģ����Ա����ܵ�
	// ���ݳ��ȱ�����ENCRYPT_BLOCK_SIZE �����������������һ�μ��ܵ�
	// ���ݳ��ȡ�

	dwBlockLen = 1000 - 1000 % ENCRYPT_BLOCK_SIZE;
	dwBufferLen = dwBlockLen;

	//--------------------------------------------------------------------
	// Allocate memory. 

	if (!(pbBuffer = (BYTE *)malloc(dwBufferLen)))
	{
		HandleError("�ڴ����!\n");
	}
	//--------------------------------------------------------------------
	// �����ļ���д��. 

	do {
		//--------------------------------------------------------------------
		// ��������. 

		dwCount = fread(
			pbBuffer,
			1,
			dwBlockLen,
			hSource);
		if (ferror(hSource))
		{
			HandleError("��ȡ�ļ�ʧ��!");
		}
		//--------------------------------------------------------------------
		// ��������. 
		if (!CryptDecrypt(
			hKey,
			0,
			feof(hSource),
			0,
			pbBuffer,
			&dwCount))
		{
			HandleError("��������ʧ��!");
		}
		//--------------------------------------------------------------------
		// д������ļ�. 

		fwrite(
			pbBuffer,
			1,
			dwCount,
			hDestination);
		if (ferror(hDestination))
		{
			HandleError("д��ʧ��!");
		}
	} while (!feof(hSource));
	status = TRUE;

	//--------------------------------------------------------------------
	// �ر��ļ�. 
	if (hSource)
		fclose(hSource);
	if (hDestination)
		fclose(hDestination);

	//--------------------------------------------------------------------
	// �ͷ��ڴ��������Կ. 

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
	//   ��������

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
	// ��þ��. 
	if (!CryptAcquireContext(
		&hCryptProv,
		NULL,
		NULL,
		PROV_RSA_FULL,
		0))
	{
	}

	//--------------------------------------------------------------------
	// ����һ��hashɢ��

	if (!CryptCreateHash(
		hCryptProv,
		CALG_MD5,
		0,
		0,
		&hHash))
	{
	}
	//--------------------------------------------------------------------
	// �����ݽ���ɢ��. 

	if (!CryptHashData(
		hHash,
		(BYTE *)szPassword,
		strlen(szPassword),
		0))
	{
	}
	//--------------------------------------------------------------------
	// ���ɻỰ��Կ. 

	if (!CryptDeriveKey(
		hCryptProv,
		ENCRYPT_ALGORITHM,
		hHash,
		KEYLENGTH,
		&hKey))
	{
	}
	//--------------------------------------------------------------------
	// ����hash����. 

	CryptDestroyHash(hHash);
	hHash = 0;

	//--------------------------------------------------------------------
	// ��Ϊ�����㷨�ǰ�ENCRYPT_BLOCK_SIZE ��С�Ŀ���ܵģ����Ա����ܵ�
	// ���ݳ��ȱ�����ENCRYPT_BLOCK_SIZE �����������������һ�μ��ܵ�
	// ���ݳ��ȡ�

	dwBlockLen = 1000 - 1000 % ENCRYPT_BLOCK_SIZE;
	dwBufferLen = dwBlockLen;

	//--------------------------------------------------------------------
	// Allocate memory. 

	if (!(pbBuffer = (BYTE *)malloc(dwBufferLen)))
	{
	}
	//--------------------------------------------------------------------
	// �����ļ���д��. 
	dwCount = sizeof(text);
	
	do {
		//--------------------------------------------------------------------
		// ��������. 
		memcpy(pbBuffer, szText, ENCRYPT_BLOCK_SIZE);
		
		//--------------------------------------------------------------------
		// ��������. 
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
		// д������ļ�. 

		result += (CString)pbBuffer;

	} while (!(dwCount / ENCRYPT_BLOCK_SIZE));


	//--------------------------------------------------------------------
	// �ͷ��ڴ��������Կ. 

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
	//   ��������

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

	//���»��һ��CSP���
	if (!(CryptAcquireContext(
		&hCryptProv,
		NULL,				//NULL��ʾʹ��Ĭ����Կ������Ĭ����Կ������Ϊ�û���½��
		NULL,
		PROV_RSA_FULL,
		0)))
	{
		if (CryptAcquireContext(
			&hCryptProv,
			NULL,
			NULL,
			PROV_RSA_FULL,
			CRYPT_NEWKEYSET))//������Կ����
		{
			//������Կ�����ɹ������õ�CSP���
			printf("������Կ�����ɹ�.\n");
		}
		else
		{
		}

	}

	//--------------------------------------------------------------------
	// ����һ���Ự��Կ��session key��
	// �Ự��ԿҲ�жԳ���Կ�����ڶԳƼ����㷨��
	// ��ע: һ��Session��ָ�ӵ��ú���CryptAcquireContext�����ú���
	//   CryptReleaseContext �ڼ�Ľ׶Ρ���

	if (!(CryptCreateHash(
		hCryptProv,
		CALG_MD5,
		0,
		0,
		&hHash)))
	{
	}

	//--------------------------------------------------------------------
	// ��������������һ��ɢ��
	if (!(CryptHashData(
		hHash,
		(BYTE *)szPassword,
		strlen(szPassword),
		0)))
	{

	}

	//--------------------------------------------------------------------
	// ͨ��ɢ�����ɻỰ��Կ(session key)
	if (!(CryptDeriveKey(
		hCryptProv,
		ENCRYPT_ALGORITHM,
		hHash,
		KEYLENGTH,
		&hKey)))
	{
	}
	//--------------------------------------------------------------------
	// ����hash����

	CryptDestroyHash(hHash);
	hHash = NULL;


	//--------------------------------------------------------------------
	// ��Ϊ�����㷨�ǰ�ENCRYPT_BLOCK_SIZE ��С�Ŀ���ܵģ����Ա����ܵ�
	// ���ݳ��ȱ�����ENCRYPT_BLOCK_SIZE �����������������һ�μ��ܵ�
	// ���ݳ��ȡ�

	dwBlockLen = 1000 - 1000 % ENCRYPT_BLOCK_SIZE;

	//--------------------------------------------------------------------
	// ���������Ĳ���

	if (ENCRYPT_BLOCK_SIZE > 1)
		dwBufferLen = dwBlockLen + ENCRYPT_BLOCK_SIZE;
	else
		dwBufferLen = dwBlockLen;

	//--------------------------------------------------------------------
	// �����ڴ�
	if (!(pbBuffer = (BYTE *)malloc(dwBufferLen)))
	{
	}
	//--------------------------------------------------------------------
	// ���ܹ��̿�ʼ
	dwCount = text.GetLength() * sizeof(text);
	do
	{
		memcpy(pbBuffer, szText, ENCRYPT_BLOCK_SIZE);
		
		//--------------------------------------------------------------------
		// Read up to dwBlockLen bytes from the source file. 
		
		
		//--------------------------------------------------------------------
		// ��������
		if (!CryptEncrypt(
			hKey,			//��Կ
			0,				//�������ͬʱ����ɢ�кͼ��ܣ����ﴫ��һ��ɢ�ж���
			!(dwCount /ENCRYPT_BLOCK_SIZE),	//��������һ�������ܵĿ飬����TRUE.�����������FALSE.
			//����ͨ���ж��Ƿ��ļ�β�������Ƿ�Ϊ���һ�顣
			0,				//����
			pbBuffer,		//���뱻�������ݣ�������ܺ������
			&dwCount,		//���뱻��������ʵ�ʳ��ȣ�������ܺ����ݳ���
			dwBufferLen))	//pbBuffer�Ĵ�С��
		{
			dwCount -= 7;
			szText += ENCRYPT_BLOCK_SIZE;
		}

		//--------------------------------------------------------------------
		// �ļ�д��

		result += (CString)pbBuffer;
	} while ((dwCount / ENCRYPT_BLOCK_SIZE));


	//--------------------------------------------------------------------
	// �ͷ��ڴ����Կ

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

//�쳣����
void MyCrypto::HandleError(char *s)
{
	fprintf(stderr, "�������. \n");
	fprintf(stderr, "%s\n", s);
	fprintf(stderr, "������� %x.\n", GetLastError());
	fprintf(stderr, "��������ֹ. \n");
	exit(1);
} // End of HandleError