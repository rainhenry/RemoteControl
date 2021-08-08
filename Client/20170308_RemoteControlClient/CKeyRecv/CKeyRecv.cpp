#include "stdafx.h"
#include "CKeyRecv.h"

//	���ڴ洢�����������Ϣ���б�
std::vector<CKeyRecv::SOnKeyInfo> CKeyRecv::AllOnKeyInfo;

//	���߳�֧�֣����ӷ��ʹ�����Ϣ��CS
CMyCriticalSection CKeyRecv::allonkeyinfo_cs;

//	���һ���������Ϣ
void CKeyRecv::AddInfo(
	CKeyRecv* p_obj,								//	�����ָ��
	std::vector<int> keyvec,						//	���ڴ洢һ�������İ����б�	
	P_USER_CKEYRECV_ONKEY_FUNC ponkeyfunc,			//	�����û���Ӧ�����ĺ���ָ��
	LPVOID lpparam
	)
{
	//	�����������
	SOnKeyInfo tmp;
	tmp.KeyVec = keyvec;
	tmp.pObj = p_obj;
	tmp.pOnKeyFunc = ponkeyfunc;
	tmp.lpParam = lpparam;

	//	�������
	CKeyRecv::allonkeyinfo_cs.Enter();
	CKeyRecv::AllOnKeyInfo.insert(CKeyRecv::AllOnKeyInfo.end(),tmp);
	CKeyRecv::allonkeyinfo_cs.Leave();
}

//	ɾ��һ���������Ϣ
void CKeyRecv::DeleteInfo(
	CKeyRecv* p_obj									//	�����ָ��
	)
{
	//	���������б�����ָ����Ψһ�ģ����Ա���һ�ν���ɾ������
	CKeyRecv::allonkeyinfo_cs.Enter();
	for (int i = 0; ((CKeyRecv::AllOnKeyInfo.begin() + i) != (CKeyRecv::AllOnKeyInfo.end())); ++i)
	{
		if (p_obj == ((*(CKeyRecv::AllOnKeyInfo.begin() + i)).pObj))
		{
			CKeyRecv::AllOnKeyInfo.erase(CKeyRecv::AllOnKeyInfo.begin() + i);
			break;
		}
	}
	CKeyRecv::allonkeyinfo_cs.Leave();
}

//	�޲������캯��
CKeyRecv::CKeyRecv()
{
	this->init_flag = false;
	this->pOnKeyFunc = NULL;
	this->lpParam = NULL;
}

//	��������
CKeyRecv::~CKeyRecv()
{
	//	��ҪĿ���ǴӾ�̬���б���Ϣ��ɾ������
	this->Release();
}

//	��Ӱ���������true��ӳɹ���false���ʧ��
bool CKeyRecv::AddKey(int key_value)
{
	//	����Ƿ����ù�
	if (this->init_flag == true)
	{
		return false;
	}

	//	��Ӱ���,�����ظ����
	if (std::find(this->KeyVec.begin(), this->KeyVec.end(), key_value) == this->KeyVec.end())
	{
		this->KeyVec.insert(this->KeyVec.end(), key_value);
	}
	else
	{
		//	�ظ����
		return false;
	}

	//	��ӳɹ�
	return true;
}

//	���ã�����trueΪ���óɹ���falseΪʧ��
bool CKeyRecv::Config(
	P_USER_CKEYRECV_ONKEY_FUNC ponkeyfunc,			//	�����û���Ӧ�����ĺ���ָ��
	LPVOID lpparam
	)
{
	//	����������
	if ((ponkeyfunc == NULL) || (ponkeyfunc == (P_USER_CKEYRECV_ONKEY_FUNC)(-1)))
	{
		return false;
	}

	//	��鰴����Ŀ
	if (this->KeyVec.size() == 0)
	{
		return false;		//	û�б������İ���
	}

	//	������ù�
	if (this->init_flag == true)
	{
		//	��������Ϣ��ɾ������
		this->DeleteInfo(this);

		//	���ñ�־
		this->init_flag = false;
	}

	//	�����û������ʹ��ݲ���
	this->pOnKeyFunc = ponkeyfunc;
	this->lpParam = lpparam;

	//	����
	this->pOnKeyFunc = ponkeyfunc;
	CKeyRecv::AddInfo(
		this,
		this->KeyVec,
		this->pOnKeyFunc,
		this->lpParam
		);

	return true;
}

//	�ͷ���Դ
void CKeyRecv::Release(void)
{
	//	��������Ϣ��ɾ������
	CKeyRecv::DeleteInfo(this);

	//	�����һ�ε����а�����Ϣ
	this->KeyVec.clear();

	//	���ñ�־
	this->init_flag = false;
}

//	���屣�水����Ϣ�Ľṹ��
typedef struct tagSKeyValInfo
{
	int KeyIndex;					//	��������
	SHORT Value;					//	�������ص���ֵ
}SKeyValInfo;

//	���Ұ�������,����true��ʾ�ҵ�������false��ʾû���ҵ�
bool FindKey(
	std::vector<SKeyValInfo> in_vec,			//	������Ϣ�б�
	int index									//	Ҫ�ҵİ�������
	)
{
	for (int i = 0; (in_vec.begin() + i) != (in_vec.end()); ++i)
	{
		if ((*(in_vec.begin() + i)).KeyIndex == index)
		{
			return true;
		}
	}

	return false;
}

//	���Ұ�������,����true��ʾ�ҵ�������false��ʾû���ҵ�
bool FindKey(
	std::vector<int> in_vec,					//	������Ϣ�б�
	int index									//	Ҫ�ҵİ�������
	)
{
	for (int i = 0; (in_vec.begin() + i) != (in_vec.end()); ++i)
	{
		if ((*(in_vec.begin() + i)) == index)
		{
			return true;
		}
	}

	return false;
}

//	���ң������ؼ�ֵ��
SHORT FindKeyGetValue(
	std::vector<SKeyValInfo> in_vec,			//	������Ϣ�б�
	int index									//	Ҫ�ҵİ�������
	)
{
	for (int i = 0; (in_vec.begin() + i) != (in_vec.end()); ++i)
	{
		if ((*(in_vec.begin() + i)).KeyIndex == index)
		{
			return (*(in_vec.begin() + i)).Value;
		}
	}

	//	���ش���������ֹ����
	AfxMessageBox(_T("CKeyRecv���з������ش������ڴ�����ϱ����±������ͬ���£���"));
	::abort();
}

//	�ṩ�ⲿ���õ�����������
void CKeyRecv::HeartbeatOnHandle(void)
{
	//	������ʱ�Ĵ洢���а����ı���
	std::vector<int> tmp_vec;

	//	ѭ��������Ϣ�б����벻ͬ������
	CKeyRecv::allonkeyinfo_cs.Enter();
	for (int i = 0; ((CKeyRecv::AllOnKeyInfo.begin() + i) != (CKeyRecv::AllOnKeyInfo.end())); ++i)
	{
		for (int j = 0; (((*(CKeyRecv::AllOnKeyInfo.begin() + i)).KeyVec.begin() + j) != ((*(CKeyRecv::AllOnKeyInfo.begin() + i)).KeyVec.end())); ++j)
		{
			int tmp_val = (*((*(CKeyRecv::AllOnKeyInfo.begin() + i)).KeyVec.begin() + j));
			//	��û���ҵ���ʱ��Ż����
			if (std::find(tmp_vec.begin(), tmp_vec.end(), tmp_val) == tmp_vec.end())
			{
				tmp_vec.insert(tmp_vec.end(),tmp_val);
			}
		}
	}
	CKeyRecv::allonkeyinfo_cs.Leave();

	//	������Ϣ,���������ʱ�洢������
//#define DEBUG_HeartbeatOnHandle
#ifdef DEBUG_HeartbeatOnHandle
	for (int i = 0; ((tmp_vec.begin() + i) != (tmp_vec.end())); ++i)
	{
		int tmp_val = (*(tmp_vec.begin() + i));
		TRACE("\r\n-------------------%d---------------------\r\n",tmp_val);
	}
#endif

#define UNDEFINE_KEY_CODE		((SHORT)234)					//	δ����ļ�ֵ���룬������ȡֵΪ0 1 -127 -128

	//	�����ϱ�����������һ�ε�ֵ
	static std::vector<SKeyValInfo> old_key_status;

	//	��ȡ������Ҫ�İ�����ֵ����ҪĿ������������ӵļ�ֵ
	for (int i = 0; ((tmp_vec.begin() + i) != (tmp_vec.end())); ++i)
	{
		int tmp_val = (*(tmp_vec.begin() + i));

		//	�ϱ������������ֵ�Ѿ�����
		if (FindKey(old_key_status, tmp_val))
		{
			//	����ʲô������
		}
		//	����ϱ�����û�е�ǰ�������ֵ
		else
		{
			//	�������ֵ�����ҳ�ʼ��Ϊδ�����
			SKeyValInfo tmp_s;
			tmp_s.KeyIndex = tmp_val;
			tmp_s.Value = UNDEFINE_KEY_CODE;
			old_key_status.insert(old_key_status.end(), tmp_s);
		}
	}

	//	�����ϵļ�ֵ�б���ҪĿ����ɾ���ϵļ�ֵ�б��ж����
	for (int i = 0; (old_key_status.begin() + i) != (old_key_status.end()); ++i)
	{
		//	������µ��б���û���ҵ����б�ĵ�ǰԪ�أ��ͽ���ɾ����
		if (!FindKey(tmp_vec, (*(old_key_status.begin() + i)).KeyIndex))
		{
			old_key_status.erase(old_key_status.begin() + i);
			--i;
		}
	}

	//	�����µİ���״̬�б�
	std::vector<SKeyValInfo> new_key_status = old_key_status;

	//	���°���״̬
	for (int i = 0; (new_key_status.begin() + i) != (new_key_status.end()); ++i)
	{
		(*(new_key_status.begin() + i)).Value = ::GetKeyState((*(new_key_status.begin() + i)).KeyIndex);
	}

	//	����ϰ���ֵ���Ƿ���δ����İ���ֵ
	for (int i = 0; (old_key_status.begin() + i) != (old_key_status.end()); ++i)
	{
		if ((*(old_key_status.begin() + i)).Value == UNDEFINE_KEY_CODE)
		{
			//	�ҵ��µ�����ļ���ֵ����ֵ���ϵı���
			(*(old_key_status.begin() + i)).Value = FindKeyGetValue(new_key_status, (*(old_key_status.begin() + i)).KeyIndex);
		}
	}

	//	ѭ���������ж��������
	CKeyRecv::allonkeyinfo_cs.Enter();
	for (int i = 0; (CKeyRecv::AllOnKeyInfo.begin() + i) != (CKeyRecv::AllOnKeyInfo.end()); ++i)
	{
		//	��ȡ��ǰ�������Ϣ
		SOnKeyInfo tmp_info = (*(CKeyRecv::AllOnKeyInfo.begin() + i));

		//	��ȡ�ö������Ҫ��������
		int main_key_code = *(tmp_info.KeyVec.end() - 1);

		//	��ȡ�ö���Ĵ�Ҫ��������
		std::vector<int> other_key_code(tmp_info.KeyVec.size()-1);
		std::copy(tmp_info.KeyVec.begin(), tmp_info.KeyVec.end() - 1, other_key_code.begin());

		//	������е���������������ֵ
		bool other_key_down_status = true;
		for (int i = 0; (other_key_code.begin() + i) != (other_key_code.end()); ++i)
		{
			//	���Ϊ����
			if ((FindKeyGetValue(new_key_status, (*(other_key_code.begin() + i))) == CKEYRECV_KEY_DOWN_A) ||
				(FindKeyGetValue(new_key_status, (*(other_key_code.begin() + i))) == CKEYRECV_KEY_DOWN_B))
			{
				//	��������
			}
			//	����Ϊ̧��
			else
			{
				other_key_down_status = false;
				break;
			}
		}

		//	�����Ҫ����������������
		if (other_key_down_status == false)
		{
			continue;
		}

		//	�����Ҫ��������״̬����״ֵ̬
		SHORT old_main_key_status = FindKeyGetValue(old_key_status, main_key_code);
		SHORT new_main_key_status = FindKeyGetValue(new_key_status, main_key_code);

		//	�����Ҫ�����ĺʹ�Ҫ���������������Ϊ̧���¼�
		if (((old_main_key_status == CKEYRECV_KEY_DOWN_A) || (old_main_key_status == CKEYRECV_KEY_DOWN_B)) &&
			((new_main_key_status == CKEYRECV_KEY_UP_A) || (new_main_key_status == CKEYRECV_KEY_UP_B)) &&
			(other_key_down_status == true))
		{
			//	ִ���û�����
			tmp_info.pOnKeyFunc(
				CKeyRecv::ON_UP,
				tmp_info.lpParam
				);
		}

		//	���Ϊ�����¼�
		if (((old_main_key_status == CKEYRECV_KEY_UP_A) || (old_main_key_status == CKEYRECV_KEY_UP_B)) &&
			((new_main_key_status == CKEYRECV_KEY_DOWN_A) || (new_main_key_status == CKEYRECV_KEY_DOWN_B)) &&
			(other_key_down_status == true))
		{
			//	ִ���û�����
			tmp_info.pOnKeyFunc(
				CKeyRecv::ON_DOWN,
				tmp_info.lpParam
				);
		}
	}
	CKeyRecv::allonkeyinfo_cs.Leave();

	//	��ʱ���д����Ѿ���ɣ������ϱ������б�
	old_key_status = new_key_status;
}

