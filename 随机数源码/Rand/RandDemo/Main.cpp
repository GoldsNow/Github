#include "stdafx.h"
#include "Main.h"

LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	HELP(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	AddStuItem(HWND, UINT, WPARAM, LPARAM);

DWORD WINAPI RandThread(LPVOID pM);		//实现滚动随机的线程
int AddStudent(HWND hwnd, int IDC_CLASS, int IDC_ID, int IDC_NUM);
void InitTheFile();				//初始化数据包

TCHAR CurrentClassName[100];	//目前选择的区队名称
int CurrentClassID;				//目前选择的区队学号第一位
int CurrentClassNum;			//目前选择的区队总数
TCHAR ClassName[100];
int id, ClassFirstId, ClassNum;
int THEID = 1;

int NumArr[1000];				
int NumMax = 0;				//默认总人数为30人
int RandNum = 0;				//随机数
int TheNum;
int Control = 0;

static HWND FunBtnHwnd;
static HWND ComboboxHwnd;		//下拉菜单
HINSTANCE hInst;
HANDLE RandHandle;

static int TheClassNumber;

static TCHAR RandText[100];

void Reset(){
	char path[100];
	FILE *DATA;
	sprintf(path, ".\\res\\%d", CurrentClassID);

	if (PathFileExists(path)){
		if (IDYES == MessageBox(FunBtnHwnd, TEXT("检测到有存档，是否读取原有存档？"), TEXT("温馨提示"), MB_YESNO)){
			memset(NumArr, 0, 1000);
			DATA = fopen(path, "r");
			NumMax = 0;
			fscanf(DATA, "%d", &NumArr[NumMax]);
			while (NumArr[NumMax] != 0){
				NumMax++;
				fscanf(DATA, "%d", &NumArr[NumMax]);
			}
			fclose(DATA);
			return;
		}
		goto NewLabel;
	}
	else{
NewLabel:
		NumMax = CurrentClassNum;
		memset(NumArr, 0, 1000);
		for (int i = 0; i < CurrentClassNum; i++){
			NumArr[i] = CurrentClassID + i;
		}
		
		DATA = fopen(path, "w");
		for (int i = 0; i < NumMax; i++){
			fprintf(DATA, "%d\n", NumArr[i]);
		}
		fclose(DATA);
	}

}

void PrintCurrectInfo(HWND hwd){
	HDC hdc;
	size_t iTarget;
	HFONT hFont;
	TCHAR CurrectInfo[100];

	hdc = GetDC(hwd);

	hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "info");
	SelectObject(hdc, hFont);
	SetTextAlign(hdc, TA_LEFT);
	SetTextColor(hdc, 0);
	StringCchPrintf(CurrectInfo, 100, TEXT("已经选过 %d 个人，还剩下 %d 个！    "), CurrentClassNum - NumMax, NumMax);
	StringCchLength(CurrectInfo, 100, &iTarget);
	TextOut(hdc, 30, 315, CurrectInfo, iTarget);

	ReleaseDC(hwd, hdc);
}


///////////////////////////////////////////////////////////////////
/*-----------------			  主函数		  -------------------*/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	int Value;							//反调试模块
	_asm jz label1
	_asm jnz label1
	_asm __emit 0e8h
label1:
	_asm
	{
		push eax;
		mov eax, fs:[0x30];
		movzx eax, byte ptr[eax + 2];
		mov dword ptr[Value], eax;
		pop eax;
	}
	if (Value)
	{
		//PostQuitMessage(0);				//自己调试的时候 在这里加上注释
	}

	static TCHAR szAppName[] = TEXT("随机数");
	HWND        hwnd;
	MSG            msg;
	WNDCLASS    wndclass;

	wndclass.lpfnWndProc = WndProc;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.hInstance = hInstance;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = LoadIcon(wndclass.hInstance, MAKEINTRESOURCE(IDI_MYBIGICON));
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = MAKEINTRESOURCE(IDC_MENU);
	wndclass.hIcon = LoadIcon(wndclass.hInstance, MAKEINTRESOURCE(IDI_MYSMALLICON));


	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("无法注册窗口类!"), TEXT("错误"), MB_OK | MB_ICONERROR);
		return 0;
	}

	int scrWidth, scrHeight;			// 读取屏幕的宽度和高度
	scrWidth = GetSystemMetrics(SM_CXSCREEN);
	scrHeight = GetSystemMetrics(SM_CYSCREEN);

	hInst = hInstance;

	hwnd = CreateWindow(szAppName, TEXT("随机数"), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		scrWidth / 2 - 300, scrHeight / 2 - 215, 600, 420, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	
	//初始化文件夹
	InitTheFile();

	//消息循环
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}


///////////////////////////////////////////////////////////////////
/*-----------------           回调函数        -------------------*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	_asm jz label		//反静态编译模块
	_asm jnz label
	_asm __emit 0e8h

label:


	HDC hdc;
	size_t iTarget;
	HGDIOBJ RE_GDI;
	UINT RE_TA;
	COLORREF RE_SB;

	FILE *DATA;
	FILE *CACHE;

	static TCHAR Print1[] = { "选择区队:" };
	static TCHAR Print2[] = { "中奖童鞋：" };
	static TCHAR ZhiChi[] = { "By：15网一  " };
	static TCHAR NumInfo[100];
	static TCHAR Result[100];		//结果
	TCHAR TheClass[100];

	/////////////////////////////////////////////////
	//窗口总大小为 600 X 400
	//定义两个方框的大小
	static POINT Poly1[5] = { { 10, 10 }, { 575, 10 }, { 575, 230 }, { 10, 230 }, { 10, 10 } };
	static POINT Poly2[5] = { { 30, 240 }, { 330, 240 }, { 330, 300 }, { 30, 300 }, { 30, 240 } };

	//字体
	static HFONT hFont;
	int count;
	char path[100];

	switch (message)
	{
	case WM_PAINT:
		hdc = GetDC(hwnd);

		//画方框
		Polyline(hdc, Poly1, 5);
		Polyline(hdc, Poly2, 5);

		//打印选择区队的文字
		StringCchLength(Print1, 100, &iTarget);
		hFont = CreateFont(27, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "选择区队");
		RE_GDI = SelectObject(hdc, hFont);
		TextOut(hdc, 40, 255, Print1, iTarget);

		//技术支持
		hFont = CreateFont(13, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "技术支持字体");
		SelectObject(hdc, hFont);
		SetTextColor(hdc, 0xD4D4D4);				//设置文字颜色为蓝色
		RE_TA = SetTextAlign(hdc, TA_RIGHT);		//设置文本对齐方式
		StringCchLength(ZhiChi, 30, &iTarget);		//字符串长度
		TextOut(hdc, 595, 345, ZhiChi, iTarget);	//输出字符串
		SetTextAlign(hdc, RE_TA);					//恢复默认

		//选过多少人
		PrintCurrectInfo(hwnd);

		//输出选择的人
		hFont = CreateFont(30, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "大号字体");
		SetTextColor(hdc, RGB(0, 0, 255));				//设置文字颜色为蓝色
		SelectObject(hdc, hFont);						//设置字体
		StringCchLength(Print2, 30, &iTarget);
		TextOut(hdc, 20, 30, Print2, iTarget);

		//若窗口刷新，需要重新绘图
		if (CurrentClassNum > NumMax ){				
			hFont = CreateFont(150, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "显示随机数字体");
			SelectObject(hdc, hFont);
			SetTextAlign(hdc, TA_CENTER);
			SetTextColor(hdc, RGB(255, 0, 0));
			StringCchPrintf(RandText, 100, "%7d", TheNum);	//由于学号默认长度为7位，故输出设置为%7d
			StringCchLength(RandText, 100, &iTarget);
			TextOut(hdc, 280, 75, RandText, iTarget);
		}

		SetTextAlign(hdc, RE_TA);			//恢复默认文本对齐方式
		SelectObject(hdc, RE_GDI);			//恢复默认
		SetTextColor(hdc, RGB(0, 0, 0));	//恢复黑色

		ReleaseDC(hwnd, hdc);				//释放DC
		UpdateWindow(hwnd);					//更新窗口

		break;


	case WM_CREATE:			//当窗口创建的时候

		
		///////////////////////////////////////////////////
		////ComboBox
		//框的左上坐标为30,240 注册生成一个输入窗口
		//
		ComboboxHwnd = CreateWindow("COMBOBOX", NULL, WS_CHILD | WS_VSCROLL | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_SORT,
			170, 250, 150, 150, hwnd, (HMENU)IDC_COMBOBOX, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
		ShowWindow(ComboboxHwnd, SW_SHOW);
		InitTheFile();
		DATA = fopen(".\\res\\data.dat", "r");
		for (int i = 0; i < TheClassNumber; i++){
			
			fscanf(DATA, "%d%s%d%d", &id, ClassName, &ClassFirstId, &ClassNum);
			SendMessage(ComboboxHwnd, CB_ADDSTRING, (WPARAM)i, (LPARAM)(LPCTSTR)ClassName);

		}
		fclose(DATA);

		hFont = CreateFont(35, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "文本框字体");
		SendMessage(ComboboxHwnd, WM_SETFONT, (WPARAM)hFont, 1);		//设置文本框内的字体

		//////抽奖按钮 /////
		FunBtnHwnd = CreateWindow(TEXT("button"), TEXT("GO"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			380, 245, 130, 70, hwnd, (HMENU)2, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
		hFont = CreateFont(50, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "抽奖按钮字体");
		SendMessage(FunBtnHwnd, WM_SETFONT, (WPARAM)hFont, 1);

		return 0;


	case WM_COMMAND:

		switch (LOWORD(wParam))        //判断子窗口ID, 根据子窗口ID做出不同响应
		{
		///////////////////////////////////////////////
		///
		//响应下拉框变化的消息
		//
		case IDC_COMBOBOX:
			switch (HIWORD(wParam)){
			case CBN_SELCHANGE:
				GetDlgItemText(hwnd, IDC_COMBOBOX, ClassName, 100);
				//如果区队有变化
				if (strcmp(ClassName, CurrentClassName))
				{
					StringCchCopy(CurrentClassName, 100, ClassName);
					DATA = fopen(".\\res\\data.dat", "r");
					for (int i = 0; i < TheClassNumber; i++){
						fscanf(DATA, "%d%s%d%d", &id, ClassName, &ClassFirstId, &ClassNum);
						if (!strcmp(ClassName, CurrentClassName))
						{
							CurrentClassID = ClassFirstId;
							CurrentClassNum = ClassNum;
							Reset();
							PrintCurrectInfo(hwnd);
							break;
						}
					}
					fclose(DATA);
				}

				break;
			}
			
			return 0;
		///////////////////////////////////////////////
		//关于菜单
		//
		case IDM_HELP:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_HELPBOX), hwnd, About);
			return 0;
		///////////////////////////////////////////////
		//关于菜单
		//
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);			
			return 0;
		////////////////////////////////////////////////
		//关闭按钮
		//
		case IDM_EXIT:
			PostQuitMessage(0);
			return 0;
		////////////////////////////////////////////////
		//增加区队按钮
		//
		case IDM_ADD:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ADDBOX), hwnd, AddStuItem);
			return 0;
		////////////////////////////////////////////////
		//删除区队按钮
		//
		case IDM_DELETE:
			count = 1;
			GetDlgItemText(hwnd, IDC_COMBOBOX, TheClass, 100);
			sprintf(path, TEXT("是否删除 %s 区队？"),TheClass);
			if (MessageBox(hwnd, path, TEXT("温馨提示"), MB_YESNO) == IDYES){
				DATA = fopen(".\\res\\data.dat", "r");
				CACHE = fopen(".\\res\\cache", "w");
				for (int i = 0; i < TheClassNumber; i++){
					fscanf(DATA, "%d%s%d%d", &id, ClassName, &ClassFirstId, &ClassNum);
					if (!strcmp(ClassName, TheClass)){
						
						sprintf(path, ".\\res\\%d", ClassFirstId);
					//	MessageBox(0, path, ClassName, MB_OK);
						DeleteFile(path);
					//	sprintf(path, "%d", id);
					//	MessageBox(0, TheClass,path, MB_OK);
					//	SendMessage(ComboboxHwnd, CB_RESETCONTENT, 0, 0);
					//	test = SendMessage(ComboboxHwnd, CB_FINDSTRINGEXACT, 0, (WPARAM)(LPCTSTR)TheClass);
					//	sprintf(testchar, "%d",test);
					//	MessageBox(hwnd, testchar, "1", MB_OK);
						//先搜索到字符串所在的位置然后再删除
						SendMessage(ComboboxHwnd, CB_DELETESTRING, SendMessage(ComboboxHwnd, CB_FINDSTRINGEXACT, 0, (WPARAM)(LPCTSTR)TheClass), (WPARAM)(LPCTSTR)TheClass);

					//						SendMessage(ComboboxHwnd, CB_DELETESTRING, id, 0);
						continue;
					}
					fprintf(CACHE, "%d %s %d %d\n", count, ClassName, ClassFirstId, ClassNum);

					count++;
				}

				fclose(CACHE);
				fclose(DATA);

				TheClassNumber--;
				DATA = fopen(".\\res\\info.ini", "w");
				fprintf(DATA, "%d \n", TheClassNumber);
				fclose(DATA);

				Sleep(10);
				DeleteFile(".\\res\\data.dat");
				MoveFile(".\\res\\cache", ".\\res\\data.dat");
			}
			return 0;
		////////////////////////////////////////////////
		//区队重置按钮
		//
		case IDM_RESETCLASS:
			wsprintf(path, ".\\res\\%d", CurrentClassID);
			DeleteFile(path);
			Reset();
			PrintCurrectInfo(hwnd);
			return 0;
		////////////////////////////////////////////////
		//选择区队按钮
		//
		//case IDM_CHOOSE:
		//	GetDlgItemText(hwnd, IDC_COMBOBOX, ClassName, 100);
		//	if (strlen(ClassName) == 0){
		//		MessageBox(hwnd, TEXT("请在左下角下拉菜单选择区队"), TEXT("温馨提示"), MB_OK);
		//	}
		//	else
		//	{
		//		sprintf(Result,"您选择的区队是： %s",ClassName);
		//		MessageBox(hwnd, Result, TEXT("温馨提示"), MB_OK);
		//	}

		//	return 0;

		case 2:			//处理抽奖按钮的消息

			GetDlgItemText(hwnd, IDC_COMBOBOX, Result, 100);
			StringCchLength(Result, 100, &iTarget);
			//MessageBox(hwnd, Result, "1", MB_OK);
			if (iTarget == 0){
				MessageBox(hwnd, TEXT("请先选择区队"), TEXT("温馨提示"), MB_OK);
				return 0;
			}


			switch (HIWORD(wParam))    //通过HIWORD(wParam)进一步判断消息类型
			{
			case BN_CLICKED:
				if (Control == 0){
					if (NumMax == 0){
						sprintf(path, ".\\res\\%d",CurrentClassID);
						if (PathFileExists(path)){
							DeleteFile(path);
						}
						if (MessageBox(hwnd, TEXT("已经抽完一轮,是否开始新的一轮？"), TEXT("温馨提示"), MB_YESNO) == IDYES){

							Reset();
							PrintCurrectInfo(hwnd);

							hdc = GetDC(hwnd);
							//覆盖原有的文字
							hFont = CreateFont(150, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
								DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "显示随机数字体");
							SelectObject(hdc, hFont);
							SetTextColor(hdc, RGB(255, 255, 255));
							RE_TA = SetTextAlign(hdc, TA_CENTER);
							StringCchPrintf(RandText, 100, "       ");
							StringCchLength(RandText, 100, &iTarget);
							TextOut(hdc, 280, 75, RandText, iTarget);
							SetTextAlign(hdc, RE_TA);

							ReleaseDC(hwnd, hdc);
							UpdateWindow(hwnd);

						}
						break;
					}
					//hdc = GetDC(hwnd);
					//覆盖原有数据
					//hFont = CreateFont(150, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					//	DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "显示随机数字体");
					//SelectObject(hdc, hFont);
					//SetTextAlign(hdc, TA_CENTER);
					//StringCchPrintf(RandText, 100, "%d", TheNum);
					//StringCchLength(RandText, 100, &iTarget);
					//RE_SB = SetTextColor(hdc, RGB(255, 255, 255));
					//TextOut(hdc, 280, 75, RandText, iTarget);
					//SetTextColor(hdc, RE_SB);

					Control = 1;
					//随机数产生的代码  以当前时间为种子
					if (NumMax >= 0){
						RandHandle = CreateThread(NULL, 0, RandThread, (LPVOID)hwnd, 0, 0);
					}

					SetWindowText(FunBtnHwnd, TEXT("STOP"));

				}
				else if (Control == 1){
					hdc = GetDC(hwnd);
					Control = 0;
					//线程断开
					if (CloseHandle(RandHandle) == TRUE){
						//MessageBox(hwnd, "111", "1", 0);
					}
					//输出随机数
					hFont = CreateFont(150, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
						DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "显示随机数字体");
					SelectObject(hdc, hFont);
					SetTextAlign(hdc, TA_CENTER);
					TheNum = NumArr[RandNum];
					StringCchPrintf(RandText, 100, "%7d", NumArr[RandNum]);
					RE_SB = SetTextColor(hdc, RGB(255, 0, 0));
					StringCchLength(RandText, 100, &iTarget);
					TextOut(hdc, 280, 75, RandText, iTarget);
					SetTextColor(hdc, RE_SB);

					//在数组中减去抽到的那一位
					NumMax--;
					for (int i = RandNum; i < NumMax; i++){
						NumArr[i] = NumArr[i + 1];
					}

					/////////////////////////////////////////
					///
					/// 存档
					///
					sprintf(path, ".\\res\\%d", CurrentClassID);
					DATA = fopen(path, "w");
					for (int i = 0; i < NumMax; i++){
						fprintf(DATA, "%d\n", NumArr[i]);
					}
					fclose(DATA);


					//将信息公开到屏幕，能保证公平公正
					PrintCurrectInfo(hwnd);

					ReleaseDC(hwnd, hdc);
					
					SetWindowText(FunBtnHwnd, TEXT("GO"));
				}
				
			}

			break;
		}
		return 0;


	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}


///////////////////////////////////////////////////////////////////
/*-----------------    增加学生按钮的回调函数   -------------------*/

INT_PTR CALLBACK AddStuItem(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int result;
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:

		switch (LOWORD(wParam)){
		
		case ID_OK:

			result = AddStudent(hDlg, IDC_EDT1, IDC_EDT2, IDC_EDT3);

			if (result != 0){
				return (INT_PTR)FALSE;
			}
			//StringCchPrintf(info,100,)
			//MessageBox(hDlg, info1, TEXT("OK"), MB_OK);
			EndDialog(hDlg, LOWORD(wParam));
			
			return (INT_PTR)TRUE;
		case ID_NO:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

	}
	return (INT_PTR)FALSE;
}


INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;

	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK HELP(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

///////////////////////////////////////////////////////////////
/* -----------         数据的初始化操作            ----------*/
void InitTheFile(){
	FILE * DATA;
	FILE *INFO;


	//////////////////////
	//在当前目录下创建 res 子文件夹
	//
	if (!PathFileExists("res")){
		if (!CreateDirectory(TEXT("res"), NULL)){
			MessageBox(0, TEXT("文件夹创建失败"), TEXT("Wrong"), MB_OK | MB_ICONERROR);
			PostQuitMessage(1);
		}
	}
	//////////////////////
	//在 res 子文件夹 下创建 info.ini
	//
	if (!PathFileExists(".\\res\\info.ini")){
		INFO = fopen( ".\\res\\info.ini", "w");
		fprintf_s(INFO, "0 \n");
		fclose(INFO);

	}
	else{
		//读取含有的区队数量
		INFO = fopen(".\\res\\info.ini", "r");
		fscanf(INFO, "%d", &TheClassNumber);
		fclose(INFO);
	}
	//////////////////////
	//在 res 子文件夹 下创建 data.dat
	//
	if (!PathFileExists(".\\res\\data.dat")){
		DATA = fopen(".\\res\\data.dat", "w");
		fclose(DATA);
	}

	if (PathFileExists(".\\res\\cache")){

		MessageBox(0, TEXT("存档损坏"), TEXT("Sorry"), MB_OK);
		DeleteFile(".\\res\\cache");
		DeleteFile(".\\res\\data.dat");
		DeleteFile(".\\res\\info.ini");
		InitTheFile();
	}
}
///////////////////////////////////////////////////////////////
/* -----------      将数据存到dat文档             ----------*/

int AddStudent(HWND hwnd, int IDC_CLASS, int IDC_ID, int IDC_NUM){

	TCHAR Name[100];
	TCHAR ClassID[100];
	TCHAR Number[100];

	FILE *DATA;
	char info[100];


	//读取区队名字
	if (GetDlgItemText(hwnd, IDC_CLASS, Name, 100) == 0){
		MessageBox(hwnd, TEXT("区队信息不能为空"), TEXT("错误"), MB_ICONINFORMATION);
		return 1;
	}
	else{
		DATA = fopen(".\\res\\data.dat", "r");
		for (int i = 0; i < TheClassNumber; i++)
		{
			fscanf(DATA, "%d%s%d%d", &id, ClassName, &ClassFirstId, &ClassNum);
			if (!strcmp(ClassName, Name)){
				fclose(DATA);
				MessageBox(hwnd, TEXT("存档中已经有该区队"), TEXT("温馨提示"), MB_OK | MB_ICONINFORMATION);
				return 3;
			}
		}
		if (fclose(DATA) == EOF){
			MessageBox(hwnd, TEXT("ERROR:0 未能正常存档"), TEXT("错误"), MB_ICONERROR);
		}
	}


	//读取学号信息
	if (GetDlgItemText(hwnd, IDC_ID, ClassID, 100) == 0){
		MessageBox(hwnd, TEXT("学号信息不能为空"), TEXT("错误"), MB_ICONINFORMATION);
		return 1;
	}

	int ClassNumber = atoi(ClassID);

	if (ClassNumber <= 0 || ClassNumber > 10000000){
		MessageBox(hwnd, TEXT("学号格式错误"), TEXT("错误"), MB_ICONINFORMATION);
		return 2;
	}
	else{
		DATA = fopen(".\\res\\data.dat", "r");
		for (int i = 0; i < TheClassNumber; i++)
		{
			fscanf(DATA, "%d%s%d%d", &id, ClassName, &ClassFirstId, &ClassNum);
			if (ClassFirstId == ClassNumber){
				fclose(DATA);
				MessageBox(hwnd, TEXT("该初始学号重复，请检查"), TEXT("温馨提示"), MB_OK | MB_ICONINFORMATION);
				return 3;
			}

		}
		fclose(DATA);
	}


	//读取人数信息
	if (GetDlgItemText(hwnd, IDC_NUM, Number, 100) == 0){
		MessageBox(hwnd, TEXT("人数信息不能为空"), TEXT("错误"), MB_ICONINFORMATION);
		return 1;
	}
	int AllNum = atoi(Number);
	if (AllNum > 500){
		MessageBox(hwnd, TEXT("人数超出500！"), TEXT("错误"), MB_ICONINFORMATION);
		return 2;
	}
	else if (AllNum <= 0){
		MessageBox(hwnd, TEXT("请输入一个正整数！"), TEXT("错误"), MB_ICONINFORMATION);
		return 2;
	}

	sprintf(info, "您输入的区队为 %s  区队初始学号为 %d  区队总人数为%d\n是否保存？", Name, ClassNumber, AllNum);

	if (MessageBox(hwnd, info, TEXT("温馨提示"), MB_YESNO )== IDYES){

		TheClassNumber++;

		SendMessage(ComboboxHwnd, CB_ADDSTRING, (WPARAM)-1, (LPARAM)(LPCTSTR)Name);

		DATA = fopen(".\\res\\data.dat", "ab+");
		fprintf(DATA, "%d %s %d %d\n", TheClassNumber, Name, ClassNumber, AllNum);

		fclose(DATA);
		DATA = fopen(".\\res\\info.ini", "w");
		fprintf(DATA, "%d \n", TheClassNumber);
		fclose(DATA);
		return 0;

	}
	return 4;


}


///////////////////////////////////////////////////////////////
//
//滚动抽学号线程
DWORD WINAPI RandThread(LPVOID pM){
	size_t iTarget;
	HDC hdc;
	COLORREF RE_SB;
	HFONT hFont;

	char RandChar[100];
	hdc = GetDC((HWND)pM);
	StringCchPrintf(RandText, 100, "%d", NumArr[NumMax]);
	StringCchLength(RandText, 100, &iTarget);
	RE_SB = SetTextColor(hdc, RGB(255, 0, 0));
	hFont = CreateFont(150, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "显示随机数字体");
	SelectObject(hdc, hFont);
	SetTextAlign(hdc, TA_CENTER);

	srand(time(0));
	while (1){
		Sleep(10);	//循环时候的延时 数字越小滚动越快
		if (Control == 0){
			break;
		}
		RandNum = rand() % NumMax;
		TheNum = NumArr[RandNum];
		sprintf(RandChar, "%7d", TheNum);
		TextOut(hdc, 280, 75, RandChar, strlen(RandChar));
	}
	SetTextColor(hdc, RE_SB);
	ReleaseDC((HWND)pM, hdc);
	return 0;
}