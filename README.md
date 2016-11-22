# MemberFunctionHook

主要功能是SetWindowsHookEx函数中可以使用C++成员函数作为回调函数。
主要思想及实现代码来自http://blog.csdn.net/realzyc8847/article/details/6229350
我对他进行了简单的分解，将核心的成员函数转普通函数功能提取出来。
  
  转换功能例子
```C++
		//===============================================================
		//将类成员函数指针转化为一般函数指针
		LRESULT(CALLBACK * PCoreHookProcess)(int nCode, WPARAM wParam, LPARAM lParam);
		MEMBERFUNCTION_TO_NORMALFUNCTION(this, &BaseHook::CoreHookProcess, &PCoreHookProcess);
		//===============================================================
		point_HookID = SetWindowsHookEx(GetHookType(), PCoreHookProcess, hMod, GetThreadId());
```




  Hook模块需要继承BaseHook，重载HookProcess
