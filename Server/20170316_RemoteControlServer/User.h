/**************************************************************************************

	�������ƣ�Զ�̿��Ʒ�����
	����汾��REV 0.2
	�������ڣ�2017.03.17
	����޸ģ�2017.03.31
	��Ʊ�д��rainhenry

	�汾�޶���
		0.1�汾			��������
		0.2�汾			�޸�pScreenshort���ٽ����ݷ��ʴ���BUG
						�Ż���ťЧ��
						����FPS��ָʾ��ȷ�̶�

	����ԭ������:
		ͨ��������Э�飬ʵ�ֲ鿴Զ�̿ͻ��ˣ�����ػ���������Ļ���ݣ���ʾ���ֲ���OpenCV�ķ�ʽ

**************************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////
//	�ض��屣��
#ifndef __USER_H__
#define __USER_H__

///////////////////////////////////////////////////////////////////////////////////////
//	����ͷ�ļ�
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/tracking.hpp"
#include "cv.h"

#include "iconv.h"
#include "UDPTCP\UDPTCP.h"
#include "CThread\CThread.h"
#include "CMyTime\CMyTime.h"
#include "CMyCriticalSection\CMyCriticalSection.hpp"
#include "CMonitorProtocol\CMonitorProtocol.h"
#include "CScreenshot\CScreenshot.h"

#include "20170316_RemoteControlServer.h"
#include "20170316_RemoteControlServerDlg.h"

///////////////////////////////////////////////////////////////////////////////////////
//	�������
//	����TCP��������Դ����
extern CTCPServerResources myTCPServerRes;

//	����TCP�������������
extern CMonitorProtocol::CintegratedTCPServerManage myTCPManage;

//	���嵱ǰѡ�е����Ӷ�����Ϣ
extern CMonitorProtocol::CintegratedTCPServerManage::SReConInfo CurrentSelectConnInfo;

//	�����û�������ָ��
extern CMy20170316_RemoteControlServerDlg* pUserMainWindow;

//	������ӿͻ��˲���,����true��ʾ��ӳɹ�������false��ʾʧ��
bool AddClient(CMonitorProtocol::CintegratedTCPServerManage::SReConInfo in_info);

//	�����Ƴ��ͻ��˲������ɹ�����true��ʧ�ܷ���false
bool RemoveClient(SOCKET in_socket);

//	��õ�ǰѡ�еĿͻ�����Ϣ
CMonitorProtocol::CintegratedTCPServerManage::SReConInfo GetCurrentSelectInfo(void);

//	�Ƴ����пͻ���
void RemoveAllClient(void);

//	��õ�ǰѡ��Ŀͻ�����Ϣ�������ѡ��ģ��򷵻�true�����򷵻�false
bool GetCurrentSelectClientInfo(CMonitorProtocol::CintegratedTCPServerManage::SReConInfo* out_info);

//	�����û����������б��CS
extern CMyCriticalSection UserInterface_ClientList_CS;

//	�����û����浱ǰ�����ı����CS
extern CMyCriticalSection UserInterface_CurrentClientEdit_CS;

//	��ǰ�Ѿ�ѡ��Ŀͻ�����Ϣ
extern CMonitorProtocol::CintegratedTCPServerManage::SReConInfo CurrentClientInfo;

//	���״̬��true��ʾ����Ѿ���ʼ��false��ʾ���û�п�ʼ
extern bool MonitorStatus;

//	TCP���ӽ������Ӧ����
void OnTCPJoin(
	SOCKET in_socket,								//	�ɹ��������ӵ�SOCKET���
	SOCKADDR_IN ClientAddr,							//	�ͻ�����Ϣ
	CMyTime	begin_time								//	�ɹ��������ӵ�ʱ���
	);

//	TCP���ӶϿ�����Ӧ����
void OnTCPBreak(
	SOCKET in_socket,								//	�Ͽ�����֮ǰ��SOCKET���
	SOCKADDR_IN ClientAddr,							//	�Ͽ�����֮ǰ�Ŀͻ�����Ϣ
	CMyTime	end_time								//	�Ͽ����ӵ�ʱ���
	);

//	��ǰ�������Ĺ���״̬��trueΪ�Ѿ������ˣ�falseΪû�й���
extern bool TCPServerStatus;

//	���Э��ķ��������մ�����
void OnRecvUserData(
	SOCKET in_socket,														//	�����SOCKET����ʾ����һ��������������
	std::vector<CMonitorProtocol::SDataArea> in_vec							//	��ڲ��������Կͻ��˵Ļ�ִ������
	);

//	����OpenCVͼ��ָ��
extern IplImage* pScreenshort;
extern CMyCriticalSection pScreenshort_cs;

//	����OpenCV�������ڵ�����
#define REMOTE_DESKTOP_WINDOW_NAME		"Remote Desktop Window"

//	�յ��ͻ������ݣ�����ɻ�ͼ��־��true��ʾ�Ѿ��յ�������ɴ���false��ʾû���յ�
extern bool RecvDataFlag;

//	���巢�ͽ����߳���
extern CThread SendCtrlThread;

//	�����̺߳���
DWORD SendCtrlThreadFunc(LPVOID lpParam);

//	������ʼ�ذ�ť�ͼ��״̬��ص�CS
extern CMyCriticalSection MonitorAboutVar_CS;

//	�õ�FPS
double GetFPS(void);

//	����FPS
void SetFPS(double in);

///////////////////////////////////////////////////////////////////////////////////////
#endif	//	__USER_H__

