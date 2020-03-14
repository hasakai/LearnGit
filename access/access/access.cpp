// access.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//


// CppAccessDb.cpp : 定义控制台应用程序的入口点。
//

#include "pch.h"
//#include "CppAccessDb.h"
#include "windows.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

//每次都是新建文件
void WriteTableFile(CString strFilePath, CString strContent)
{

	CFile hFile;
	hFile.Open(strFilePath, CFile::modeCreate | CFile::modeReadWrite);
	hFile.Write(strContent, strContent.GetLength());
	hFile.Flush();
	hFile.Close();
}


//每次是以追加的方式写入
void WriteLogMsg(CString strFilePath, CString strContent)
{

	CFile hFile;
	//hFile.Open(CString("\\2010-9-20.txt"),CFile::modeCreate | CFile::modeReadWrite | CFile::modeNoTruncate);

	hFile.Open(strFilePath, CFile::modeCreate | CFile::modeReadWrite | CFile::modeNoTruncate);


	//CString buff("很好，测试一下，Hello Worlds");//如果用CString的话，会出现中文乱码,需要在项目选项中进行字符集合设置


	hFile.SeekToEnd();//将写入点定位到文件尾部
	hFile.Write(strContent, strContent.GetLength());
	hFile.Flush();
	hFile.Close();

}




//需要在头文件中引用adox.dll才具有创建的功能,ado15.dll不具有此功能
void CreateMdbFile(CString strDBFileName)
{

	//CString str;
	//str=CString("d:\\")+strDBFileName;
	//str+=".mdb";

	////检查该数据库是否已经存在，如果该数据库已经存在，弹出消息框，返回
	////使用API函数PathFileExists()检查路径文件是否存在
	////请注意：为了使用API函数PathFileExists()，需要加入
	////#include "Shlwapi.h"
	////#pragma comment(lib,"shlwapi.lib")
	//if(PathFileExists(str))
	//{
	//  CString strTemp;
	//  strTemp.Format(_T("%s已存在!"),str);
	//  AfxMessageBox(strTemp);
	//  return ;
	//}

	////定义ADOX对象指针并初始化为NULL
	////用ADOX创建access数据库方法很简单，
	////只需要新建一个Catalog对象，然后调用它的Create方法就可以了。
	////Catalog是 ADOX 的一个对象，它包含描述数据源模式目录的集合。
	////在这里，您只需知道创建数据库时使用这个对象就可以了。
	////注意用try...catch组合捕捉错误
	//_CatalogPtr m_pCatalog = NULL;
	//CString DBName=CString("Provider=Microsoft.JET.OLEDB.4.0;Data source=");
	//DBName=DBName+str;
	//try
	//{
	//  m_pCatalog.CreateInstance(__uuidof(Catalog));
	//  m_pCatalog->Create(_bstr_t((LPCTSTR)DBName));        
	//}
	//catch(_com_error &e)
	//{
	//  AfxMessageBox(e.ErrorMessage());
	//  return ;

	//} 
}





void SelectCmd(CString strCmd)
{
	CString strTableContent;

	_ConnectionPtr m_pConnection;
	_variant_t RecordsAffected;
	_RecordsetPtr m_pRecordset;

	try
	{
		m_pConnection.CreateInstance(__uuidof(Connection));
		m_pConnection->Open("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=Northwind.mdb", "", "", adModeUnknown);
	}
	catch (_com_error e)
	{
		CString errormessage;
		errormessage.Format(CString("连接数据库失败!\r错误信息:%s"), e.ErrorMessage());
		AfxMessageBox(errormessage);
		return;
	}

	try
	{
		m_pRecordset.CreateInstance("ADODB.Recordset"); //为Recordset对象创建实例
		_bstr_t strCmd = "SELECT EmployeeID,FirstName,LastName,HireDate,City FROM Employees WHERE City='London'";
		m_pRecordset = m_pConnection->Execute(strCmd, &RecordsAffected, adCmdText);

	}
	catch (_com_error &e)
	{
		AfxMessageBox(e.Description());
	}

	_variant_t vEmployeeID, vFirstName, vLastName, vHireDate, vCity;

	try
	{
		while (!m_pRecordset->adoEOF)
		{
			vEmployeeID = m_pRecordset->GetCollect(_variant_t((long)0));
			//取得第1列的值，从0开始计数，你也可以直接列出列的名称，如下一行

			vFirstName = m_pRecordset->GetCollect("FirstName");
			vLastName = m_pRecordset->GetCollect("LastName");
			vHireDate = m_pRecordset->GetCollect("HireDate");
			vCity = m_pRecordset->GetCollect("City");

			CString strtemp;
			if (vEmployeeID.vt != VT_NULL)
			{
				strtemp.Format(CString("%d"), vEmployeeID.lVal);
			}

			if (vFirstName.vt != VT_NULL)
			{
				strtemp += "      ";
				strtemp += (LPCTSTR)(_bstr_t)vFirstName;
			}

			if (vLastName.vt != VT_NULL)
			{
				strtemp += "      ";
				strtemp += (LPCTSTR)(_bstr_t)vLastName;
			}

			if (vHireDate.vt != VT_NULL)
			{
				strtemp += "      ";
				strtemp += (LPCTSTR)(_bstr_t)vHireDate;
			}

			if (vCity.vt != VT_NULL)
			{
				strtemp += "      ";
				strtemp += (LPCTSTR)(_bstr_t)vCity;
			}


			strTableContent += strtemp;
			strTableContent += CString("\r\n");



			m_pRecordset->MoveNext();
		}

	}
	catch (_com_error &e)
	{
		AfxMessageBox(e.Description());
	}

	WriteTableFile(CString("\\AccessDB.txt"), strTableContent);//将查询到的内容写到文件中

	m_pRecordset->Close();
	m_pRecordset = NULL;
	m_pConnection->Close();
	m_pConnection = NULL;

}








void InsertIntoDB()
{
	_ConnectionPtr m_pConnection;
	_variant_t RecordsAffected;

	try
	{
		m_pConnection.CreateInstance(__uuidof(Connection));
		m_pConnection->Open("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=Northwind.mdb", "", "", adModeUnknown);
	}
	catch (_com_error e)
	{
		CString errormessage;
		errormessage.Format(CString("连接数据库失败!\r错误信息:%s"), e.ErrorMessage());
		AfxMessageBox(errormessage);
		return;
	}
	try
	{
		_bstr_t strCmd = "INSERT INTO Employees(EmployeeID,FirstName,LastName,HireDate,City,Country) VALUES(10,'Mary','Simons','15/4/1993 12:00:00','New York','USA')";
		m_pConnection->Execute(strCmd, &RecordsAffected, adCmdText);
	}
	catch (_com_error &e)
	{
		AfxMessageBox(e.Description());
	}
	if (m_pConnection->State)
		m_pConnection->Close();
}






int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	::CoInitialize(NULL);//使用ADO访问数据库时注意一定要初始化OLE/COM库环境－－有了这个语句就不会创建数据库时出现指针错误

	//AfxOleInit(); //初始化COM库

	// 初始化 MFC 并在失败时显示错误
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: MFC 初始化失败\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: 在此处为应用程序的行为编写代码。
		//进行Access数据库程序设计
		//CreateMdbFile(_T("HelloMdb"));

		SelectCmd(CString("Hellos"));
		InsertIntoDB();

		WriteLogMsg(CString("\\AccessDB.txt"), CString("Hello World你好世界"));

	}


	//::CoUninitialize();//释放占用的COM资源

	return nRetCode;
}