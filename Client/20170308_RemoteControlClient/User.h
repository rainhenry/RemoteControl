
/*****************************************************************************************

	�������ƣ�Զ�̿��ƿͻ��� 
	����汾��REV 0.2
	�������ڣ�2017.03.08
	����޸ģ�2017.03.17
	��Ʊ�д��rainhenry

	�汾�޶���
		�汾0.1			��������
		�汾0.2			���ӹ��ܣ�������һ�ε��������ã�IP��ַ�Ͷ˿�
						�Ż���ť����

	����ԭ��������
		�������в鿴Զ�̼������Ļ�Ĺ���
		����Զ�̼��Э��
		�ͻ����յ��ĵ�һ�����������ݱ�����"screenshort"�ַ�������������Ƚ��ַ��������Ե�һ���������ĳ��ȿ������⣬������0x00��伴��
		Ȼ��ͻ��˻�ִ�н����������ڵ�һ����������������Ļ����Ϣ�������ֱ��ʺͱ����ÿ���ֽ���Ŀ
		�ڵڶ�����������ȫ��ͼ������ؾ���

*****************************************************************************************/

#ifndef __USER_H__
#define __USER_H__

//--------------------------------------------------------------------
//	�����Զ���ͷ�ļ�
#include "CMyTime\CMyTime.h"
#include "UDPTCP\UDPTCP.h"
#include "CBrowseDlg\CBrowseDlg.h"
#include "iconv.h"
#include "CScreenshot\CScreenshot.h"
#include "CKeyRecv\CKeyRecv.h"
#include "CMonitorProtocol\CMonitorProtocol.h"
#include "20170308_RemoteControlClient.h"
#include "20170308_RemoteControlClientDlg.h"
#include "CFileSave\CFileSave.h"

//--------------------------------------------------------------------
//	���徲̬�������ڼ�¼��ǰ�Ĵ��ڵ���ʾ�����Ĭ��Ϊtrue����ʾ��false�ǲ���ʾ
extern bool window_show_status;

//	���尴����Ӧ����
void UserOnKey(
	CKeyRecv_base::EOnKeyType type,						//	�����Ĵ�������
	LPVOID lpparam										//	���ݽ����Ĳ���
	);

extern CKeyRecv k1;

//---------------------------------------------------------------------------
//	�����������
//	TCP�ͻ���ʵ��
extern CMonitorProtocol::CintegratedTCPClient myClient;

//	���Э��������ݴ�����
std::vector<CMonitorProtocol::SDataArea> OnRecvData(
	std::vector<CMonitorProtocol::SDataArea> in_vec							//	��ڲ���������ʱ��������
	);

//	�������쳣�Ͽ���Ӧ����
void OnServerBreak(
	SOCKET in_socket,								//	����ʱ��socket
	SOCKADDR_IN ServerAddr,							//	�������ĵ�ַ��Ϣ
	int fail_count									//	�Ѿ�ʧ�ܵĴ���
	);

//	�û�����ָ��
extern CMy20170308_RemoteControlClientDlg* pUserWin;

//---------------------------------------------------------------------------
//	�����ļ������������
//	����������Ϣ�ṹ��
extern SOCKADDR_IN AppConfig;
extern SOCKADDR_IN defaultAppConfig;

//	�����ʼ������
void DedaultUserDataInit(void);

//	�����ļ��洢��
extern CFileSave myfilesave;

//---------------------------------------------------------------------------
//	�����û���ť��CS
extern CMyCriticalSection mConnectButton_cs;

//	��ǰ������״̬��trueΪ�Ѿ����ӣ�falseΪû������,�����Ӱ�ť����һ��CS
extern bool currect_connect_status;

#endif		//	__USER_H__

