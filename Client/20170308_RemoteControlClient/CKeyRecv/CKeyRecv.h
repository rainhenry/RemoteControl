/**************************************************************************************************

	�������ƣ����̼�¼��
	�������ڣ�2017.3.8
	����޸ģ�2017.3.13
	��    ����REV 0.1
	��Ʊ��룺rainhenry
	ƽ    ̨��VS2013 @ Win7 64bit SP1

	�汾�޶���
		0.1�汾			������Ŀ

	����ԭ��
		������Ӱ��������б���ӽ����İ�����ͬʱ���µ�ʱ��Żᴥ���û�������
		������һ����̬�������������ڲ���ԭ���Ǽ�ⰴ����״̬Ȼ��ִ����ص��û�����
		ÿ���������һ�������б� �� һ���û�����ָ��
		���еĶ��󣬼���ľ�̬������ӵ��һ����������������������ļ�����Ϣ������������������

		�����������ʱ�������ӵİ���Ϊ��Ҫ��������������Ҫ���������ĸı��źŽ������û������ĵ��á����ఴ�������ڰ��µ������

	ʹ�þ�����
	//	���尴����Ӧ����
	void UserOnKey(
		CKeyRecv_base::EOnKeyType type,						//	�����Ĵ�������
		LPVOID lpparam										//	���ݽ����Ĳ���
		)
	{
		//	���Ϊ̧��Ĵ����¼�
		if (type == CKeyRecv::ON_UP)
		{
			//....
		}
	}

	//	���ö��󣬿��Զ���������ÿ������Ҳ���Զ��������������Ƕ������е�ͬһ�����������������ӵİ�������Ҫ����������Ҫ��ⴥ���¼�
	CKeyRecv k1;											//	���尴����ʵ��
	k1.AddKey(VK_F4);										//	��Ӱ���
	k1.Config(UserOnKey, &window_show_status);				//	����֮��Ż���Ч���ڶ�������Ϊ��ѡ�������Ǵ��ݵ��û���Ӧ�������û��Զ������

	//	Ȼ������OnTimer���������߳��е�������������һ����10msһ�Σ�����Ҫ�����1msһ�Ρ��û��ĺ������ɸ������������õ�
	CKeyRecv::HeartbeatOnHandle();

**************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////
//	�ض��屣��
#ifndef __CKEYRECV_H__
#define __CKEYRECV_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
//	����ͷ�ļ�
#include <vector>
#include <set>
#include <algorithm>
#include <afx.h>
#include "../CMyCriticalSection/CMyCriticalSection.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
//	���ͻ��ඨ��
class CKeyRecv_base
{
public:
	//	���尴����������ö��
	typedef enum tagEOnKeyType
	{
		ON_DOWN = 1,						//	���µ�ʱ��
		ON_UP = 2							//	̧���ʱ��
	}EOnKeyType;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//	�������
//	��Ҫ�û�����İ�����Ӧ����ָ��
typedef void(*P_USER_CKEYRECV_ONKEY_FUNC)(
	CKeyRecv_base::EOnKeyType,						//	�����Ĵ�������
	LPVOID											//	���ݵĲ���
	);

//	����̧��Ͱ������µĺ�
#define CKEYRECV_KEY_UP_A			((SHORT)0)
#define CKEYRECV_KEY_UP_B			((SHORT)1)
#define CKEYRECV_KEY_DOWN_A			((SHORT)-127)
#define CKEYRECV_KEY_DOWN_B			((SHORT)-128)

///////////////////////////////////////////////////////////////////////////////////////////////////
//	�ඨ��
class CKeyRecv :public CKeyRecv_base
{
public:
	//	�����ŵ���������Ϣ�Ľṹ��
	typedef struct tagSOnKeyInfo
	{
		//	����ָ��
		CKeyRecv* pObj;

		//	���ڴ洢һ�������İ����б�
		std::vector<int> KeyVec;

		//	�����û���Ӧ�����ĺ���ָ��
		P_USER_CKEYRECV_ONKEY_FUNC pOnKeyFunc;

		//	�û����ݸ��Զ��庯���Ĳ���
		LPVOID lpParam;
	}SOnKeyInfo;

private:
	//	��ʼ�����ñ�־
	bool init_flag;

	//	���ڴ洢һ�������İ����б�
	std::vector<int> KeyVec;

	//	�����û���Ӧ�����ĺ���ָ��
	P_USER_CKEYRECV_ONKEY_FUNC pOnKeyFunc;

	//	�û����ݸ��Զ��庯���Ĳ���
	LPVOID lpParam;

	//	���ڴ洢�����������Ϣ���б�
	static std::vector<SOnKeyInfo> AllOnKeyInfo;

	//	���߳�֧�֣����ӷ��ʹ�����Ϣ��CS
	static CMyCriticalSection allonkeyinfo_cs;

protected:
	//	���һ���������Ϣ
	static void AddInfo(		
		CKeyRecv* p_obj,								//	�����ָ��
		std::vector<int> keyvec,						//	���ڴ洢һ�������İ����б�	
		P_USER_CKEYRECV_ONKEY_FUNC ponkeyfunc,			//	�����û���Ӧ�����ĺ���ָ��
		LPVOID lpparam
		);

	//	ɾ��һ���������Ϣ
	static void DeleteInfo(
		CKeyRecv* p_obj									//	�����ָ��
		);

public:
	//	�޲������캯��
	CKeyRecv();

	//	��������
	~CKeyRecv();

	//	��Ӱ���������true��ӳɹ���false���ʧ��
	bool AddKey(int key_value);

	//	���ã�����trueΪ���óɹ���falseΪʧ��
	bool Config(
		P_USER_CKEYRECV_ONKEY_FUNC ponkeyfunc,			//	�����û���Ӧ�����ĺ���ָ��
		LPVOID lpparam = NULL
		);

	//	�ͷ���Դ
	void Release(void);

	//	�ṩ�ⲿ���õ�����������
	static void HeartbeatOnHandle(void);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif	//	__CKEYRECV_H__



