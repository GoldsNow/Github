#include "stdafx.h"
#include "Main.h"

LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	HELP(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	AddStuItem(HWND, UINT, WPARAM, LPARAM);

DWORD WINAPI RandThread(LPVOID pM);		//ʵ�ֹ���������߳�
int AddStudent(HWND hwnd, int IDC_CLASS, int IDC_ID, int IDC_NUM);
void InitTheFile();				//��ʼ�����ݰ�

TCHAR CurrentClassName[100];	//Ŀǰѡ�����������
int CurrentClassID;				//Ŀǰѡ�������ѧ�ŵ�һλ
int CurrentClassNum;			//Ŀǰѡ�����������
TCHAR ClassName[100];
int id, ClassFirstId, ClassNum;
int THEID = 1;

int NumArr[1000];				
int NumMax = 0;				//Ĭ��������Ϊ30��
int RandNum = 0;				//�����
int TheNum;
int Control = 0;

static HWND FunBtnHwnd;
static HWND ComboboxHwnd;		//�����˵�
HINSTANCE hInst;
HANDLE RandHandle;

static int TheClassNumber;

static TCHAR RandText[100];

void Reset(){
	char path[100];
	FILE *DATA;
	sprintf(path, ".\\res\\%d", CurrentClassID);

	if (PathFileExists(path)){
		if (IDYES == MessageBox(FunBtnHwnd, TEXT("��⵽�д浵���Ƿ��ȡԭ�д浵��"), TEXT("��ܰ��ʾ"), MB_YESNO)){
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
	StringCchPrintf(CurrectInfo, 100, TEXT("�Ѿ�ѡ�� %d ���ˣ���ʣ�� %d ����    "), CurrentClassNum - NumMax, NumMax);
	StringCchLength(CurrectInfo, 100, &iTarget);
	TextOut(hdc, 30, 315, CurrectInfo, iTarget);

	ReleaseDC(hwd, hdc);
}


///////////////////////////////////////////////////////////////////
/*-----------------			  ������		  -------------------*/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	int Value;							//������ģ��
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
		//PostQuitMessage(0);				//�Լ����Ե�ʱ�� ���������ע��
	}

	static TCHAR szAppName[] = TEXT("�����");
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
		MessageBox(NULL, TEXT("�޷�ע�ᴰ����!"), TEXT("����"), MB_OK | MB_ICONERROR);
		return 0;
	}

	int scrWidth, scrHeight;			// ��ȡ��Ļ�Ŀ�Ⱥ͸߶�
	scrWidth = GetSystemMetrics(SM_CXSCREEN);
	scrHeight = GetSystemMetrics(SM_CYSCREEN);

	hInst = hInstance;

	hwnd = CreateWindow(szAppName, TEXT("�����"), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		scrWidth / 2 - 300, scrHeight / 2 - 215, 600, 420, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	
	//��ʼ���ļ���
	InitTheFile();

	//��Ϣѭ��
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}


///////////////////////////////////////////////////////////////////
/*-----------------           �ص�����        -------------------*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	_asm jz label		//����̬����ģ��
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

	static TCHAR Print1[] = { "ѡ������:" };
	static TCHAR Print2[] = { "�н�ͯЬ��" };
	static TCHAR ZhiChi[] = { "By��15��һ  " };
	static TCHAR NumInfo[100];
	static TCHAR Result[100];		//���
	TCHAR TheClass[100];

	/////////////////////////////////////////////////
	//�����ܴ�СΪ 600 X 400
	//������������Ĵ�С
	static POINT Poly1[5] = { { 10, 10 }, { 575, 10 }, { 575, 230 }, { 10, 230 }, { 10, 10 } };
	static POINT Poly2[5] = { { 30, 240 }, { 330, 240 }, { 330, 300 }, { 30, 300 }, { 30, 240 } };

	//����
	static HFONT hFont;
	int count;
	char path[100];

	switch (message)
	{
	case WM_PAINT:
		hdc = GetDC(hwnd);

		//������
		Polyline(hdc, Poly1, 5);
		Polyline(hdc, Poly2, 5);

		//��ӡѡ�����ӵ�����
		StringCchLength(Print1, 100, &iTarget);
		hFont = CreateFont(27, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "ѡ������");
		RE_GDI = SelectObject(hdc, hFont);
		TextOut(hdc, 40, 255, Print1, iTarget);

		//����֧��
		hFont = CreateFont(13, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "����֧������");
		SelectObject(hdc, hFont);
		SetTextColor(hdc, 0xD4D4D4);				//����������ɫΪ��ɫ
		RE_TA = SetTextAlign(hdc, TA_RIGHT);		//�����ı����뷽ʽ
		StringCchLength(ZhiChi, 30, &iTarget);		//�ַ�������
		TextOut(hdc, 595, 345, ZhiChi, iTarget);	//����ַ���
		SetTextAlign(hdc, RE_TA);					//�ָ�Ĭ��

		//ѡ��������
		PrintCurrectInfo(hwnd);

		//���ѡ�����
		hFont = CreateFont(30, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "�������");
		SetTextColor(hdc, RGB(0, 0, 255));				//����������ɫΪ��ɫ
		SelectObject(hdc, hFont);						//��������
		StringCchLength(Print2, 30, &iTarget);
		TextOut(hdc, 20, 30, Print2, iTarget);

		//������ˢ�£���Ҫ���»�ͼ
		if (CurrentClassNum > NumMax ){				
			hFont = CreateFont(150, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "��ʾ���������");
			SelectObject(hdc, hFont);
			SetTextAlign(hdc, TA_CENTER);
			SetTextColor(hdc, RGB(255, 0, 0));
			StringCchPrintf(RandText, 100, "%7d", TheNum);	//����ѧ��Ĭ�ϳ���Ϊ7λ�����������Ϊ%7d
			StringCchLength(RandText, 100, &iTarget);
			TextOut(hdc, 280, 75, RandText, iTarget);
		}

		SetTextAlign(hdc, RE_TA);			//�ָ�Ĭ���ı����뷽ʽ
		SelectObject(hdc, RE_GDI);			//�ָ�Ĭ��
		SetTextColor(hdc, RGB(0, 0, 0));	//�ָ���ɫ

		ReleaseDC(hwnd, hdc);				//�ͷ�DC
		UpdateWindow(hwnd);					//���´���

		break;


	case WM_CREATE:			//�����ڴ�����ʱ��

		
		///////////////////////////////////////////////////
		////ComboBox
		//�����������Ϊ30,240 ע������һ�����봰��
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
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "�ı�������");
		SendMessage(ComboboxHwnd, WM_SETFONT, (WPARAM)hFont, 1);		//�����ı����ڵ�����

		//////�齱��ť /////
		FunBtnHwnd = CreateWindow(TEXT("button"), TEXT("GO"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			380, 245, 130, 70, hwnd, (HMENU)2, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
		hFont = CreateFont(50, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "�齱��ť����");
		SendMessage(FunBtnHwnd, WM_SETFONT, (WPARAM)hFont, 1);

		return 0;


	case WM_COMMAND:

		switch (LOWORD(wParam))        //�ж��Ӵ���ID, �����Ӵ���ID������ͬ��Ӧ
		{
		///////////////////////////////////////////////
		///
		//��Ӧ������仯����Ϣ
		//
		case IDC_COMBOBOX:
			switch (HIWORD(wParam)){
			case CBN_SELCHANGE:
				GetDlgItemText(hwnd, IDC_COMBOBOX, ClassName, 100);
				//��������б仯
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
		//���ڲ˵�
		//
		case IDM_HELP:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_HELPBOX), hwnd, About);
			return 0;
		///////////////////////////////////////////////
		//���ڲ˵�
		//
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);			
			return 0;
		////////////////////////////////////////////////
		//�رհ�ť
		//
		case IDM_EXIT:
			PostQuitMessage(0);
			return 0;
		////////////////////////////////////////////////
		//�������Ӱ�ť
		//
		case IDM_ADD:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ADDBOX), hwnd, AddStuItem);
			return 0;
		////////////////////////////////////////////////
		//ɾ�����Ӱ�ť
		//
		case IDM_DELETE:
			count = 1;
			GetDlgItemText(hwnd, IDC_COMBOBOX, TheClass, 100);
			sprintf(path, TEXT("�Ƿ�ɾ�� %s ���ӣ�"),TheClass);
			if (MessageBox(hwnd, path, TEXT("��ܰ��ʾ"), MB_YESNO) == IDYES){
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
						//���������ַ������ڵ�λ��Ȼ����ɾ��
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
		//�������ð�ť
		//
		case IDM_RESETCLASS:
			wsprintf(path, ".\\res\\%d", CurrentClassID);
			DeleteFile(path);
			Reset();
			PrintCurrectInfo(hwnd);
			return 0;
		////////////////////////////////////////////////
		//ѡ�����Ӱ�ť
		//
		//case IDM_CHOOSE:
		//	GetDlgItemText(hwnd, IDC_COMBOBOX, ClassName, 100);
		//	if (strlen(ClassName) == 0){
		//		MessageBox(hwnd, TEXT("�������½������˵�ѡ������"), TEXT("��ܰ��ʾ"), MB_OK);
		//	}
		//	else
		//	{
		//		sprintf(Result,"��ѡ��������ǣ� %s",ClassName);
		//		MessageBox(hwnd, Result, TEXT("��ܰ��ʾ"), MB_OK);
		//	}

		//	return 0;

		case 2:			//����齱��ť����Ϣ

			GetDlgItemText(hwnd, IDC_COMBOBOX, Result, 100);
			StringCchLength(Result, 100, &iTarget);
			//MessageBox(hwnd, Result, "1", MB_OK);
			if (iTarget == 0){
				MessageBox(hwnd, TEXT("����ѡ������"), TEXT("��ܰ��ʾ"), MB_OK);
				return 0;
			}


			switch (HIWORD(wParam))    //ͨ��HIWORD(wParam)��һ���ж���Ϣ����
			{
			case BN_CLICKED:
				if (Control == 0){
					if (NumMax == 0){
						sprintf(path, ".\\res\\%d",CurrentClassID);
						if (PathFileExists(path)){
							DeleteFile(path);
						}
						if (MessageBox(hwnd, TEXT("�Ѿ�����һ��,�Ƿ�ʼ�µ�һ�֣�"), TEXT("��ܰ��ʾ"), MB_YESNO) == IDYES){

							Reset();
							PrintCurrectInfo(hwnd);

							hdc = GetDC(hwnd);
							//����ԭ�е�����
							hFont = CreateFont(150, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
								DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "��ʾ���������");
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
					//����ԭ������
					//hFont = CreateFont(150, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					//	DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "��ʾ���������");
					//SelectObject(hdc, hFont);
					//SetTextAlign(hdc, TA_CENTER);
					//StringCchPrintf(RandText, 100, "%d", TheNum);
					//StringCchLength(RandText, 100, &iTarget);
					//RE_SB = SetTextColor(hdc, RGB(255, 255, 255));
					//TextOut(hdc, 280, 75, RandText, iTarget);
					//SetTextColor(hdc, RE_SB);

					Control = 1;
					//����������Ĵ���  �Ե�ǰʱ��Ϊ����
					if (NumMax >= 0){
						RandHandle = CreateThread(NULL, 0, RandThread, (LPVOID)hwnd, 0, 0);
					}

					SetWindowText(FunBtnHwnd, TEXT("STOP"));

				}
				else if (Control == 1){
					hdc = GetDC(hwnd);
					Control = 0;
					//�̶߳Ͽ�
					if (CloseHandle(RandHandle) == TRUE){
						//MessageBox(hwnd, "111", "1", 0);
					}
					//��������
					hFont = CreateFont(150, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
						DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "��ʾ���������");
					SelectObject(hdc, hFont);
					SetTextAlign(hdc, TA_CENTER);
					TheNum = NumArr[RandNum];
					StringCchPrintf(RandText, 100, "%7d", NumArr[RandNum]);
					RE_SB = SetTextColor(hdc, RGB(255, 0, 0));
					StringCchLength(RandText, 100, &iTarget);
					TextOut(hdc, 280, 75, RandText, iTarget);
					SetTextColor(hdc, RE_SB);

					//�������м�ȥ�鵽����һλ
					NumMax--;
					for (int i = RandNum; i < NumMax; i++){
						NumArr[i] = NumArr[i + 1];
					}

					/////////////////////////////////////////
					///
					/// �浵
					///
					sprintf(path, ".\\res\\%d", CurrentClassID);
					DATA = fopen(path, "w");
					for (int i = 0; i < NumMax; i++){
						fprintf(DATA, "%d\n", NumArr[i]);
					}
					fclose(DATA);


					//����Ϣ��������Ļ���ܱ�֤��ƽ����
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
/*-----------------    ����ѧ����ť�Ļص�����   -------------------*/

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
/* -----------         ���ݵĳ�ʼ������            ----------*/
void InitTheFile(){
	FILE * DATA;
	FILE *INFO;


	//////////////////////
	//�ڵ�ǰĿ¼�´��� res ���ļ���
	//
	if (!PathFileExists("res")){
		if (!CreateDirectory(TEXT("res"), NULL)){
			MessageBox(0, TEXT("�ļ��д���ʧ��"), TEXT("Wrong"), MB_OK | MB_ICONERROR);
			PostQuitMessage(1);
		}
	}
	//////////////////////
	//�� res ���ļ��� �´��� info.ini
	//
	if (!PathFileExists(".\\res\\info.ini")){
		INFO = fopen( ".\\res\\info.ini", "w");
		fprintf_s(INFO, "0 \n");
		fclose(INFO);

	}
	else{
		//��ȡ���е���������
		INFO = fopen(".\\res\\info.ini", "r");
		fscanf(INFO, "%d", &TheClassNumber);
		fclose(INFO);
	}
	//////////////////////
	//�� res ���ļ��� �´��� data.dat
	//
	if (!PathFileExists(".\\res\\data.dat")){
		DATA = fopen(".\\res\\data.dat", "w");
		fclose(DATA);
	}

	if (PathFileExists(".\\res\\cache")){

		MessageBox(0, TEXT("�浵��"), TEXT("Sorry"), MB_OK);
		DeleteFile(".\\res\\cache");
		DeleteFile(".\\res\\data.dat");
		DeleteFile(".\\res\\info.ini");
		InitTheFile();
	}
}
///////////////////////////////////////////////////////////////
/* -----------      �����ݴ浽dat�ĵ�             ----------*/

int AddStudent(HWND hwnd, int IDC_CLASS, int IDC_ID, int IDC_NUM){

	TCHAR Name[100];
	TCHAR ClassID[100];
	TCHAR Number[100];

	FILE *DATA;
	char info[100];


	//��ȡ��������
	if (GetDlgItemText(hwnd, IDC_CLASS, Name, 100) == 0){
		MessageBox(hwnd, TEXT("������Ϣ����Ϊ��"), TEXT("����"), MB_ICONINFORMATION);
		return 1;
	}
	else{
		DATA = fopen(".\\res\\data.dat", "r");
		for (int i = 0; i < TheClassNumber; i++)
		{
			fscanf(DATA, "%d%s%d%d", &id, ClassName, &ClassFirstId, &ClassNum);
			if (!strcmp(ClassName, Name)){
				fclose(DATA);
				MessageBox(hwnd, TEXT("�浵���Ѿ��и�����"), TEXT("��ܰ��ʾ"), MB_OK | MB_ICONINFORMATION);
				return 3;
			}
		}
		if (fclose(DATA) == EOF){
			MessageBox(hwnd, TEXT("ERROR:0 δ�������浵"), TEXT("����"), MB_ICONERROR);
		}
	}


	//��ȡѧ����Ϣ
	if (GetDlgItemText(hwnd, IDC_ID, ClassID, 100) == 0){
		MessageBox(hwnd, TEXT("ѧ����Ϣ����Ϊ��"), TEXT("����"), MB_ICONINFORMATION);
		return 1;
	}

	int ClassNumber = atoi(ClassID);

	if (ClassNumber <= 0 || ClassNumber > 10000000){
		MessageBox(hwnd, TEXT("ѧ�Ÿ�ʽ����"), TEXT("����"), MB_ICONINFORMATION);
		return 2;
	}
	else{
		DATA = fopen(".\\res\\data.dat", "r");
		for (int i = 0; i < TheClassNumber; i++)
		{
			fscanf(DATA, "%d%s%d%d", &id, ClassName, &ClassFirstId, &ClassNum);
			if (ClassFirstId == ClassNumber){
				fclose(DATA);
				MessageBox(hwnd, TEXT("�ó�ʼѧ���ظ�������"), TEXT("��ܰ��ʾ"), MB_OK | MB_ICONINFORMATION);
				return 3;
			}

		}
		fclose(DATA);
	}


	//��ȡ������Ϣ
	if (GetDlgItemText(hwnd, IDC_NUM, Number, 100) == 0){
		MessageBox(hwnd, TEXT("������Ϣ����Ϊ��"), TEXT("����"), MB_ICONINFORMATION);
		return 1;
	}
	int AllNum = atoi(Number);
	if (AllNum > 500){
		MessageBox(hwnd, TEXT("��������500��"), TEXT("����"), MB_ICONINFORMATION);
		return 2;
	}
	else if (AllNum <= 0){
		MessageBox(hwnd, TEXT("������һ����������"), TEXT("����"), MB_ICONINFORMATION);
		return 2;
	}

	sprintf(info, "�����������Ϊ %s  ���ӳ�ʼѧ��Ϊ %d  ����������Ϊ%d\n�Ƿ񱣴棿", Name, ClassNumber, AllNum);

	if (MessageBox(hwnd, info, TEXT("��ܰ��ʾ"), MB_YESNO )== IDYES){

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
//������ѧ���߳�
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
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "��ʾ���������");
	SelectObject(hdc, hFont);
	SetTextAlign(hdc, TA_CENTER);

	srand(time(0));
	while (1){
		Sleep(10);	//ѭ��ʱ�����ʱ ����ԽС����Խ��
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