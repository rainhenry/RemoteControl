#include "stdafx.h"
#include "CKeyRecv.h"

//	用于存储所有类对象信息的列表
std::vector<CKeyRecv::SOnKeyInfo> CKeyRecv::AllOnKeyInfo;

//	多线程支持，增加访问公共信息的CS
CMyCriticalSection CKeyRecv::allonkeyinfo_cs;

//	添加一个对象的信息
void CKeyRecv::AddInfo(
	CKeyRecv* p_obj,								//	对象的指针
	std::vector<int> keyvec,						//	用于存储一个类对象的按键列表	
	P_USER_CKEYRECV_ONKEY_FUNC ponkeyfunc,			//	定义用户响应按键的函数指针
	LPVOID lpparam
	)
{
	//	构造添加数据
	SOnKeyInfo tmp;
	tmp.KeyVec = keyvec;
	tmp.pObj = p_obj;
	tmp.pOnKeyFunc = ponkeyfunc;
	tmp.lpParam = lpparam;

	//	添加数据
	CKeyRecv::allonkeyinfo_cs.Enter();
	CKeyRecv::AllOnKeyInfo.insert(CKeyRecv::AllOnKeyInfo.end(),tmp);
	CKeyRecv::allonkeyinfo_cs.Leave();
}

//	删除一个对象的信息
void CKeyRecv::DeleteInfo(
	CKeyRecv* p_obj									//	对象的指针
	)
{
	//	遍历查找列表，由于指针是唯一的，所以遍历一次进行删除即可
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

//	无参数构造函数
CKeyRecv::CKeyRecv()
{
	this->init_flag = false;
	this->pOnKeyFunc = NULL;
	this->lpParam = NULL;
}

//	析构函数
CKeyRecv::~CKeyRecv()
{
	//	主要目的是从静态的列表信息中删除数据
	this->Release();
}

//	添加按键，返回true添加成功，false添加失败
bool CKeyRecv::AddKey(int key_value)
{
	//	检查是否配置过
	if (this->init_flag == true)
	{
		return false;
	}

	//	添加按键,不许重复添加
	if (std::find(this->KeyVec.begin(), this->KeyVec.end(), key_value) == this->KeyVec.end())
	{
		this->KeyVec.insert(this->KeyVec.end(), key_value);
	}
	else
	{
		//	重复添加
		return false;
	}

	//	添加成功
	return true;
}

//	配置，返回true为配置成功，false为失败
bool CKeyRecv::Config(
	P_USER_CKEYRECV_ONKEY_FUNC ponkeyfunc,			//	定义用户响应按键的函数指针
	LPVOID lpparam
	)
{
	//	检查输入参数
	if ((ponkeyfunc == NULL) || (ponkeyfunc == (P_USER_CKEYRECV_ONKEY_FUNC)(-1)))
	{
		return false;
	}

	//	检查按键数目
	if (this->KeyVec.size() == 0)
	{
		return false;		//	没有被监听的按键
	}

	//	如果配置过
	if (this->init_flag == true)
	{
		//	从总体信息中删除数据
		this->DeleteInfo(this);

		//	设置标志
		this->init_flag = false;
	}

	//	设置用户函数和传递参数
	this->pOnKeyFunc = ponkeyfunc;
	this->lpParam = lpparam;

	//	配置
	this->pOnKeyFunc = ponkeyfunc;
	CKeyRecv::AddInfo(
		this,
		this->KeyVec,
		this->pOnKeyFunc,
		this->lpParam
		);

	return true;
}

//	释放资源
void CKeyRecv::Release(void)
{
	//	从总体信息中删除数据
	CKeyRecv::DeleteInfo(this);

	//	清除上一次的所有按键信息
	this->KeyVec.clear();

	//	设置标志
	this->init_flag = false;
}

//	定义保存按键信息的结构体
typedef struct tagSKeyValInfo
{
	int KeyIndex;					//	按键代码
	SHORT Value;					//	按键返回的数值
}SKeyValInfo;

//	查找按键函数,返回true表示找到，返回false表示没有找到
bool FindKey(
	std::vector<SKeyValInfo> in_vec,			//	按键信息列表
	int index									//	要找的按键代码
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

//	查找按键函数,返回true表示找到，返回false表示没有找到
bool FindKey(
	std::vector<int> in_vec,					//	按键信息列表
	int index									//	要找的按键代码
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

//	查找，并返回键值数
SHORT FindKeyGetValue(
	std::vector<SKeyValInfo> in_vec,			//	按键信息列表
	int index									//	要找的按键代码
	)
{
	for (int i = 0; (in_vec.begin() + i) != (in_vec.end()); ++i)
	{
		if ((*(in_vec.begin() + i)).KeyIndex == index)
		{
			return (*(in_vec.begin() + i)).Value;
		}
	}

	//	严重错误！运行终止！！
	AfxMessageBox(_T("CKeyRecv运行发生严重错误！由于处理后老变量新变量表格不同导致！！"));
	::abort();
}

//	提供外部调用的心跳处理函数
void CKeyRecv::HeartbeatOnHandle(void)
{
	//	定义临时的存储所有按键的变量
	std::vector<int> tmp_vec;

	//	循环遍历信息列表，插入不同的内容
	CKeyRecv::allonkeyinfo_cs.Enter();
	for (int i = 0; ((CKeyRecv::AllOnKeyInfo.begin() + i) != (CKeyRecv::AllOnKeyInfo.end())); ++i)
	{
		for (int j = 0; (((*(CKeyRecv::AllOnKeyInfo.begin() + i)).KeyVec.begin() + j) != ((*(CKeyRecv::AllOnKeyInfo.begin() + i)).KeyVec.end())); ++j)
		{
			int tmp_val = (*((*(CKeyRecv::AllOnKeyInfo.begin() + i)).KeyVec.begin() + j));
			//	当没有找到的时候才会插入
			if (std::find(tmp_vec.begin(), tmp_vec.end(), tmp_val) == tmp_vec.end())
			{
				tmp_vec.insert(tmp_vec.end(),tmp_val);
			}
		}
	}
	CKeyRecv::allonkeyinfo_cs.Leave();

	//	调试信息,遍历输出临时存储的内容
//#define DEBUG_HeartbeatOnHandle
#ifdef DEBUG_HeartbeatOnHandle
	for (int i = 0; ((tmp_vec.begin() + i) != (tmp_vec.end())); ++i)
	{
		int tmp_val = (*(tmp_vec.begin() + i));
		TRACE("\r\n-------------------%d---------------------\r\n",tmp_val);
	}
#endif

#define UNDEFINE_KEY_CODE		((SHORT)234)					//	未定义的键值号码，正常的取值为0 1 -127 -128

	//	定义老变量，保存上一次的值
	static std::vector<SKeyValInfo> old_key_status;

	//	获取所有需要的按键的值，主要目的是添加新增加的键值
	for (int i = 0; ((tmp_vec.begin() + i) != (tmp_vec.end())); ++i)
	{
		int tmp_val = (*(tmp_vec.begin() + i));

		//	老变量中这个按键值已经存在
		if (FindKey(old_key_status, tmp_val))
		{
			//	这里什么都不做
		}
		//	如果老变量中没有当前的这个键值
		else
		{
			//	增加这个值，并且初始化为未定义的
			SKeyValInfo tmp_s;
			tmp_s.KeyIndex = tmp_val;
			tmp_s.Value = UNDEFINE_KEY_CODE;
			old_key_status.insert(old_key_status.end(), tmp_s);
		}
	}

	//	遍历老的键值列表，主要目的是删除老的键值列表中多余的
	for (int i = 0; (old_key_status.begin() + i) != (old_key_status.end()); ++i)
	{
		//	如果在新的列表中没有找到老列表的当前元素，就将它删除掉
		if (!FindKey(tmp_vec, (*(old_key_status.begin() + i)).KeyIndex))
		{
			old_key_status.erase(old_key_status.begin() + i);
			--i;
		}
	}

	//	定义新的按键状态列表
	std::vector<SKeyValInfo> new_key_status = old_key_status;

	//	更新按键状态
	for (int i = 0; (new_key_status.begin() + i) != (new_key_status.end()); ++i)
	{
		(*(new_key_status.begin() + i)).Value = ::GetKeyState((*(new_key_status.begin() + i)).KeyIndex);
	}

	//	检查老按键值中是否有未定义的按键值
	for (int i = 0; (old_key_status.begin() + i) != (old_key_status.end()); ++i)
	{
		if ((*(old_key_status.begin() + i)).Value == UNDEFINE_KEY_CODE)
		{
			//	找到新的里面的键数值并赋值给老的变量
			(*(old_key_status.begin() + i)).Value = FindKeyGetValue(new_key_status, (*(old_key_status.begin() + i)).KeyIndex);
		}
	}

	//	循环遍历所有对象的数据
	CKeyRecv::allonkeyinfo_cs.Enter();
	for (int i = 0; (CKeyRecv::AllOnKeyInfo.begin() + i) != (CKeyRecv::AllOnKeyInfo.end()); ++i)
	{
		//	获取当前对象的信息
		SOnKeyInfo tmp_info = (*(CKeyRecv::AllOnKeyInfo.begin() + i));

		//	获取该对象的主要按键代码
		int main_key_code = *(tmp_info.KeyVec.end() - 1);

		//	获取该对象的次要按键代码
		std::vector<int> other_key_code(tmp_info.KeyVec.size()-1);
		std::copy(tmp_info.KeyVec.begin(), tmp_info.KeyVec.end() - 1, other_key_code.begin());

		//	检查所有的其他按键的新数值
		bool other_key_down_status = true;
		for (int i = 0; (other_key_code.begin() + i) != (other_key_code.end()); ++i)
		{
			//	如果为按下
			if ((FindKeyGetValue(new_key_status, (*(other_key_code.begin() + i))) == CKEYRECV_KEY_DOWN_A) ||
				(FindKeyGetValue(new_key_status, (*(other_key_code.begin() + i))) == CKEYRECV_KEY_DOWN_B))
			{
				//	继续遍历
			}
			//	否则为抬起
			else
			{
				other_key_down_status = false;
				break;
			}
		}

		//	如果次要按键的条件不满足
		if (other_key_down_status == false)
		{
			continue;
		}

		//	获得主要按键的老状态和新状态值
		SHORT old_main_key_status = FindKeyGetValue(old_key_status, main_key_code);
		SHORT new_main_key_status = FindKeyGetValue(new_key_status, main_key_code);

		//	检查主要按键的和次要按键的条件，如果为抬起事件
		if (((old_main_key_status == CKEYRECV_KEY_DOWN_A) || (old_main_key_status == CKEYRECV_KEY_DOWN_B)) &&
			((new_main_key_status == CKEYRECV_KEY_UP_A) || (new_main_key_status == CKEYRECV_KEY_UP_B)) &&
			(other_key_down_status == true))
		{
			//	执行用户函数
			tmp_info.pOnKeyFunc(
				CKeyRecv::ON_UP,
				tmp_info.lpParam
				);
		}

		//	如果为按下事件
		if (((old_main_key_status == CKEYRECV_KEY_UP_A) || (old_main_key_status == CKEYRECV_KEY_UP_B)) &&
			((new_main_key_status == CKEYRECV_KEY_DOWN_A) || (new_main_key_status == CKEYRECV_KEY_DOWN_B)) &&
			(other_key_down_status == true))
		{
			//	执行用户函数
			tmp_info.pOnKeyFunc(
				CKeyRecv::ON_DOWN,
				tmp_info.lpParam
				);
		}
	}
	CKeyRecv::allonkeyinfo_cs.Leave();

	//	此时所有处理都已经完成，更新老变量的列表
	old_key_status = new_key_status;
}

