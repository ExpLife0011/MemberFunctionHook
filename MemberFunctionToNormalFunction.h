#pragma once

// ����ĳ�Ա����תΪȫ�ֺ���
// ʹ��������ཫ��Ա������ַ��̬д�����ݶΣ�����thisָ��ѹջ
// ��ģ����Ҫ�������ݶδ��룬��Ҫ�ر�����ִ�б��� /NXCOMPAT:NO
#include <vector>

template<typename dst_type, typename src_type>
static dst_type pointer_cast(src_type src)
{
	return *static_cast<dst_type*>(static_cast<void*>(&src));
}

class MemberFunctionToNormalFunction
{
public:
	template<typename Cls, typename Func>
	static MemberFunctionToNormalFunction * NewInstance(Cls * ClassInstance, Func MemberFunction)
	{
		MemberFunctionToNormalFunction * Instance = new MemberFunctionToNormalFunction(ClassInstance, MemberFunction);
		FunctionVector.push_back(Instance);
		return Instance;
	}

	template<typename Cls, typename Func>
	MemberFunctionToNormalFunction(Cls * ClassInstance, Func MemberFunction)
	{
		//******************************************//
		//��ʼ��ί��
		byte * DelegatePointer = DelegateThisCall;
		DelegateThisCall[0] = 0xb9; //__asm mov ecx, this
		DelegateThisCall[5] = 0xe9; //__asm jmp, MemberFunction
		__asm
		{
			push eax					//����eax
			push ebx					//����ebx
			mov eax, ClassInstance		//ȡ��this��ַ
			mov ebx, DelegatePointer	//��ȡDelegateThisCall��ַ
			mov dword ptr[ebx + 1], eax	//this ��ַ, ˫��(DWORD)
		}

		//����ָ��תvoid*����Ϊ�̳е�C++��ʹ�ñ���ָͨ�볤һ���Ĵ�С���溯����ַ�Ͷ�thisָ�������ֵ
		//�ο���http://www.oschina.net/translate/wide-pointers
		void * MemberFunctionPtr = pointer_cast<void*>(MemberFunction);
		__asm //����jmp��ַ�ο�:http://zhidao.baidu.com/question/105950930.html
		{
			mov eax, MemberFunctionPtr		//ȡ��MemberFunction��ֵַ
			mov ebx, DelegatePointer
			add ebx, 5+5					//JMP��ַ= MemberFunction �C [(DelegatePoint+5) + 5]
			sub eax, ebx					//JMP��ַ=Ŀ���ַ-(�����ַ+����ָ���ֽ���)
			mov dword ptr[ebx - 4], eax		//MemberFunction ��ַ, ˫��(DWORD)
			pop ebx							//��ԭebx
			pop eax							//��ԭeax
		}
		//******************************************//
	}

	template<typename Func>
	void ConvertTo(Func NormalFunction)
	{
		byte * DelegatePointer = DelegateThisCall;
		void * NormalFunctionPtr = pointer_cast<void*>(NormalFunction);
		__asm 
		{ 
			push eax
			push ebx
			mov eax, DelegatePointer	//��ȡDelegateThisCall��ַ
			mov ebx, NormalFunctionPtr	//��ȡNormalFunction��ַ
			mov dword ptr[ebx], eax		//�����ݴ���д�뺯����ַ
			pop ebx
			pop eax
		}
	}

	byte * GetDelegate()
	{
		return DelegateThisCall;
	}

	static void ReleaseAll()
	{
		for (auto it = FunctionVector.begin(); it != FunctionVector.end(); it++)
		{
			delete *it;
		}
		FunctionVector.clear();
	}

protected:
	byte DelegateThisCall[10];
	static std::vector<MemberFunctionToNormalFunction*> FunctionVector;
};

#define MEMBERFUNCTION_TO_NORMALFUNCTION(ClassInstance, MemberFunction, NormalFunction) \
	{ \
		MemberFunctionToNormalFunction * Instance = MemberFunctionToNormalFunction::NewInstance(ClassInstance, MemberFunction); \
		Instance->ConvertTo(NormalFunction); \
	}

#define MEMBERFUNCTION_TO_NORMALFUNCTION_RELEASEALL() \
	MemberFunctionToNormalFunction::ReleaseAll();


