// N3CEDoc.cpp : implementation of the CN3CEDoc class
//

#include "stdafx.h"
#include "N3CE.h"
#include "N3CEDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CN3CEDoc

IMPLEMENT_DYNCREATE(CN3CEDoc, CDocument)

BEGIN_MESSAGE_MAP(CN3CEDoc, CDocument)
	//{{AFX_MSG_MAP(CN3CEDoc)
	ON_COMMAND(ID_FILE_SAVE_AS_ONE_FOLDER, OnFileSaveAsOneFolder)
	ON_COMMAND(ID_TOOL_OPTIMIZE_ANIMATION_DATA, OnToolOptimizeAnimationData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CN3CEDoc construction/destruction

CN3CEDoc::CN3CEDoc()
{
	if(m_Scene.CameraCount() <= 0) m_Scene.DefaultCameraAdd();
	if(m_Scene.LightCount() <= 0) m_Scene.DefaultLightAdd();

	m_bLoadingNow = FALSE;

	// Character �ʱ�ȭ..
	CN3Chr* pChr = new CN3Chr();
	m_Scene.ChrAdd(pChr);
}

CN3CEDoc::~CN3CEDoc()
{
}

BOOL CN3CEDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	if(m_Scene.ChrGet(0))
	{
		m_Scene.ChrGet(0)->Release();
		m_Scene.ChrGet(0)->AniDefaultSet(); // �⺻ ���ϸ��̼� ����..
	}
	this->UpdateAllViews(NULL);

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CN3CEDoc serialization

void CN3CEDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CN3CEDoc diagnostics

#ifdef _DEBUG
void CN3CEDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CN3CEDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CN3CEDoc commands

BOOL CN3CEDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	// TODO: Add your specialized creation code here
	char szPath[512]; // ��θ� �� ������ �Ѵ�..
	lstrcpy(szPath, lpszPathName);
	int nFind = 0;
	for(int i = lstrlen(szPath) - 1; i >= 0; i--)
	{
		if('\\' == szPath[i] || '/' == szPath[i]) nFind++;
		if(nFind >= 2)
		{
			szPath[i] = NULL;
			break;
		}
	}
	
	m_bLoadingNow = TRUE;

	m_Scene.PathSet(szPath); // ��θ� �����ְ�..
	m_Scene.ChrGet(0)->Release();
	m_Scene.ChrGet(0)->LoadFromFile(lpszPathName);
	CN3Joint* pJoint = m_Scene.ChrGet(0)->Joint();
	if(pJoint)
	{
		m_Scene.m_fFrmEnd = pJoint->m_KeyPos.Count() * 30.0f / pJoint->m_KeyRot.SamplingRate();
		if(m_Scene.m_fFrmEnd <= 0 && pJoint->Child(0))
		{
			m_Scene.m_fFrmEnd = pJoint->Child(0)->m_KeyRot.Count() * 30.0f / pJoint->Child(0)->m_KeyRot.SamplingRate();
		}
	}

	if(m_Scene.ChrGet(0)->AniCtrl() == NULL)
	{
		m_Scene.ChrGet(0)->AniDefaultSet(); // �⺻ ���ϸ��̼� ����..
	}

	m_bLoadingNow = FALSE;

	this->UpdateAllViews(NULL);

	return TRUE;
}

BOOL CN3CEDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	CDocument::OnSaveDocument(lpszPathName);
	
	// TODO: Add your specialized creation code here
	char szPath[512]; // ��θ� �� ������ �Ѵ�..
	lstrcpy(szPath, lpszPathName);
	int nFind = 0;
	for(int i = lstrlen(szPath) - 1; i >= 0; i--)
	{
		if('\\' == szPath[i] || '/' == szPath[i]) nFind++;
		if(nFind >= 2)
		{
			szPath[i] = NULL;
			break;
		}
	}
	
	char szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFName[_MAX_FNAME];
	_splitpath(lpszPathName, szDrive, szDir, szFName, NULL);

	m_Scene.PathSet(szPath); // ��θ� �����ְ�..
	CN3Chr* pChr = m_Scene.ChrGet(0);
	pChr->m_szName = szFName; // �̸��� ���ϰ�..

//	CN3Joint* pJoint = pChr->Joint();
//	if(pJoint)
//	{
//		pJoint->SaveToFile(); // ������ �������ش�..
//	}

	CN3CPlugBase* pPlug = NULL;
	int nCPC = pChr->PlugCount();
	for(i = 0; i < nCPC; i++)
	{
		pPlug = pChr->Plug(i);
		pPlug->SaveToFile();
	}

	CN3AnimControl* pAni = pChr->AniCtrl();
	if(pAni) pAni->SaveToFile();

	m_Scene.ChrGet(0)->SaveToFile(lpszPathName);
	m_Scene.SaveResrc();

	return TRUE; // �׳� �����ϸ� �ȵȴ�.. 0 Byte �� �ȴ�.
}

void CN3CEDoc::OnFileSaveAsOneFolder() 
{
	// TODO: Add your specialized creation code here
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(FALSE, "N3Chr", NULL, dwFlags, "Character Data File(*.N3Chr)|*.N3Chr||", NULL);
	if(dlg.DoModal() == IDCANCEL) return;
	CString szFullFileName = dlg.GetPathName();

	char szPath[512]; // ��θ� �� ������ �Ѵ�..
	lstrcpy(szPath, szFullFileName);
	int nFind = 0;
	for(int i = lstrlen(szPath) - 1; i >= 0; i--)
	{
		if('\\' == szPath[i] || '/' == szPath[i]) nFind++;
		if(nFind >= 2)
		{
			szPath[i] = NULL;
			break;
		}
	}

	char szDrive[_MAX_DRIVE], szDir[_MAX_DIR];
	_splitpath(szFullFileName, szDrive, szDir, NULL, NULL);

	m_Scene.PathSet(szPath); // ��θ� �����ְ�..
	CN3Chr* pChr = m_Scene.ChrGet(0);

	CN3Joint* pJoint = pChr->Joint();
	if(pJoint)
	{
		char szFN[256];
		_makepath(szFN, szDrive, szDir, pChr->m_szName.c_str(), ".N3Joint");
		pJoint->SaveToFile(szFN); // ������ �������ش�..
	}

	CN3CPlugBase* pPlug = NULL;
	int nCPC = pChr->PlugCount();
	for(i = 0; i < nCPC; i++)
	{
		pPlug = pChr->Plug(i);
		if(NULL == pPlug) continue;

		char szFN[256];
		if(pPlug->PMesh())
		{
			_makepath(szFN, szDrive, szDir, pPlug->PMesh()->m_szName.c_str(), ".N3PMesh");
			pPlug->PMesh()->SaveToFile(szFN);
		}
		if(pPlug->Tex())
		{
			_makepath(szFN, szDrive, szDir, pPlug->Tex()->m_szName.c_str(), NULL);
			pPlug->Tex()->SaveToFile(szFN);
		}

		_makepath(szFN, szDrive, szDir, pPlug->m_szName.c_str(), ".N3CPlug");
		pPlug->SaveToFile(szFN);
	}

	CN3CPart* pPart = NULL;
	nCPC = pChr->PartCount();
	for(i = 0; i < nCPC; i++)
	{
		pPart = pChr->Part(i);
		if(NULL == pPart) continue;

		char szFN[256];
		
		if(pPart->Skins())
		{
			_makepath(szFN, szDrive, szDir, pPart->Skins()->m_szName.c_str(), ".N3CSkins");
			pPart->Skins()->SaveToFile(szFN);
		}

		if(pPart->Tex())
		{
			_makepath(szFN, szDrive, szDir, pPart->Tex()->m_szName.c_str(), ".DXT");
			pPart->Tex()->SaveToFile(szFN);
		}
		
		_makepath(szFN, szDrive, szDir, pPart->m_szName.c_str(), ".N3CPart");
		pPart->SaveToFile(szFN);
	}

	CN3AnimControl* pAni = pChr->AniCtrl();
	if(pAni && pAni->Count() > 0) // ���ϸ��̼��� ���� �̸����� �������ش�..
	{
		pAni->m_szName = pChr->m_szName;

		char szFN[256];
		_makepath(szFN, szDrive, szDir, pChr->m_szName.c_str(), ".N3Anim");
		CN3Base::s_MngAniCtrl.Add(pAni);
		pAni->SaveToFile(szFN);
	}
	
	m_Scene.ChrGet(0)->SaveToFile(std::string(szFullFileName));
	this->SetPathName(szFullFileName);
	this->UpdateAllViews(NULL); // ��� ������Ʈ..
}

void CN3CEDoc::OnCloseDocument() 
{
	if(m_Scene.ChrGet(0)) m_Scene.ChrGet(0)->Release(); // �̷��� �ؾ�.. ������ �������� ���� ����ϰ� ��������..

	CDocument::OnCloseDocument();
}

void CN3CEDoc::OnToolOptimizeAnimationData() 
{
	CN3Chr* pChr = m_Scene.ChrGet(0);
	if(NULL == pChr) return;
	CN3AnimControl* pAniCtrl = pChr->AniCtrl();
	if(NULL == pAniCtrl) return;
	CN3Joint* pJointSrc = pChr->Joint();
	if(NULL == pJointSrc) return;

	float fSamplingRate = 30.0f / pJointSrc->m_KeyRot.SamplingRate();
	if(	fSamplingRate != 6.0f &&
		fSamplingRate != 3.0f &&
		fSamplingRate != 2.0f &&
		fSamplingRate != 1.0f )
	{
		MessageBox(AfxGetMainWnd()->m_hWnd, "SamplingRate �� 5, 10, 15, 30 �� �����˴ϴ�.", "SamplingRate ����", MB_OK);
		return;
	}

	MessageBox(AfxGetMainWnd()->m_hWnd, "���� Joint �� Animation File �� �ٸ� �̸����� ������ �Ӵϴ�. �� ������ ���߿� �ٽ� �����Ҷ� �ʿ��մϴ�.", "Joint �� Animation ���� �ٸ��̸����� ����", MB_OK);

	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg1(FALSE, "N3Anim", NULL, dwFlags, "Animation Data(*.N3Anim)|*.n3Anim||", NULL);
	if(dlg1.DoModal() == IDCANCEL) return;
	FileName = dlg1.GetPathName();
	
	std::string szFN(FileName);
	std::string szFNPrev = pAniCtrl->FileName();
	pAniCtrl->SaveToFile(szFN); // ����..
	pAniCtrl->FileNameSet(szFNPrev);

	CFileDialog dlg2(FALSE, "N3Joint", NULL, dwFlags, "Joint File(*.N3Joint)|*.N3Joint||", NULL);
	if(dlg2.DoModal() == IDCANCEL) return;
	FileName = dlg2.GetPathName();

	szFNPrev = pJointSrc->FileName();
	pChr->Tick(0.0f); // �ʱ���ġ�� Tick
	szFN = FileName;
	pJointSrc->SaveToFile(szFN); // ������ �ٸ� �̸����� ����..
	pJointSrc->FileNameSet(szFNPrev);

	int nK = pAniCtrl->Count();
	CN3Joint JointDest;
	
	pChr->Tick(0.0f); // �ʱ���ġ�� Tick
	JointDest.CopyExceptAnimationKey(pJointSrc); // ���� ���� �� ���鸸 �����Ѵ���..
	JointDest.AddKey(pJointSrc, 0, 0); // ���ε� ���� Ű�� �߰��ϰ� �����Ѵ�..
	
	int iAniCount = pAniCtrl->Count();
	std::vector<float> fFrmOffsets(iAniCount, 0);
	std::vector<float> fFrmStarts(iAniCount, 0);
	for(int i = 0; i < iAniCount; i++) // Animation Key �߰�.
	{
		__AnimData* pAD = pAniCtrl->DataGet(i);
		if(pAD->fFrmEnd < pAD->fFrmStart) pAD->fFrmEnd = pAD->fFrmStart; // ���� ���������� �����..
		
		int iS = (int)(pAD->fFrmStart / fSamplingRate);
		int iE = (int)(pAD->fFrmEnd / fSamplingRate); // �ݿø��Ϸ��� 0.5f �־���..

		if(iE < iS) continue;

		JointDest.AddKey(pJointSrc, iS, iE); // Ű�� �߰��ϰ� �����Ѵ�..
		fFrmOffsets[i] = (iE - iS + 1) * fSamplingRate; // �� �������� Offset ���..
		fFrmStarts[i] = pAD->fFrmStart - (iS * fSamplingRate);
	}

	float fOffsetCur = fSamplingRate;
	for(i = 0; i < iAniCount; i++) // Animation Key Frame �ٽ� ���..
	{
		__AnimData* pAD = pAniCtrl->DataGet(i);

		pAD->Offset(fOffsetCur - pAD->fFrmStart + fFrmStarts[i]); // ��� �����͸� Offset��ŭ ���� ����..

		fOffsetCur += fFrmOffsets[i];
	}

	pAniCtrl->SaveToFile();
	JointDest.SaveToFile();

	pChr->Tick(0.0f); // �ʱ���ġ�� Tick
	pChr->JointSet("");
	pChr->JointSet(JointDest.FileName());

	UpdateAllViews(NULL); // ��� �� �ʱ�ȭ..
}
