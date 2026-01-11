/**********************************************************************
 *<
	FILE: CN3DExp.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 1997, All Rights Reserved.
 **********************************************************************/

#include "../N3Base/StdAfxBase.h"
#include "../N3Base/N3PMeshCreate.h"
#include "N3DExp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

HINSTANCE		g_hInst;
Interface*		g_pIntf;
ExpInterface*	g_pExpIntf;
CN3EngTool		g_Eng;

static CN3DExpClassDesc CN3DExpDesc;
ClassDesc* GetCN3DExpDesc() {return &CN3DExpDesc;}

//--- CN3DExp -------------------------------------------------------
__EXPORT_OPTION CN3DExp::m_Option;

CN3DExp::CN3DExp()
{
	this->Init();

	// allocation CN3Eng, CN3Scene | Init Engine
//	if(NULL == g_Eng.s_lpD3DDev)
//	{
//		g_Eng.Init(TRUE, NULL, 64, 64, 0, FALSE);
//	}
	m_pScene = NULL;
}

CN3DExp::~CN3DExp() 
{
}

void CN3DExp::Init()
{
	memset(&m_Option, 0, sizeof(m_Option));
	m_Option.bExportCamera		= TRUE;
	m_Option.bExportLight		= TRUE;
	m_Option.bExportGeometry	= TRUE;
	m_Option.bExportDummy		= FALSE;
	m_Option.bExportCharacter	= TRUE;

	m_Option.bAnimationKey	= TRUE;
	
	m_Option.bGenerateSmoothNormal = TRUE;
//	m_Option.bGenerateProgressiveMesh = FALSE;
	m_Option.bGenerateHalfSizeTexture = FALSE;
	m_Option.bGenerateCompressedTexture = TRUE;

	m_Option.fSamplingRate = 30.0f;

	m_nNodeCur = 0;
	m_nNodeCount = 0;

	lstrcpy(m_szPath, "");		// Path Name
	lstrcpy(m_szFileName, "");	// File Name
}

int CN3DExp::ExtCount()
{
	//TODO: Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *CN3DExp::Ext(int n)
{		
	//TODO: Return the 'i-th' file name extension (i.e. "3DS").
	return _T("n3scene");
}

const TCHAR *CN3DExp::LongDesc()
{
	//TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("NoahSystem N3Scene File");
}
	
const TCHAR *CN3DExp::ShortDesc() 
{			
	//TODO: Return short ASCII description (i.e. "Targa")
	return _T("Noah N3Scene");
}

const TCHAR *CN3DExp::AuthorName()
{			
	//TODO: Return ASCII Author name
	return _T("NoahSystem");
}

const TCHAR *CN3DExp::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("NoahSystem Exporter");
}

const TCHAR *CN3DExp::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *CN3DExp::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int CN3DExp::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return 200;
}

void CN3DExp::ShowAbout(HWND hWnd)
{			
	// Optional
}

int CN3DExp::DoExport(const TCHAR *szFileName, ExpInterface* pExpIntf, Interface *pIntf, BOOL suppressPrompts, DWORD dwOptions)
{
	// Interface Object Pointer
	g_pIntf	= pIntf; 
	g_pExpIntf = pExpIntf;
	if(NULL == g_pIntf || NULL == pExpIntf) return 0;

	// allocation CN3Eng, CN3Scene | Init Engine
	if(NULL == g_Eng.s_lpD3DDev) g_Eng.Init(TRUE, pIntf->GetMAXHWnd(), 64, 64, 0, FALSE);

	if(g_Eng.s_lpD3DDev == NULL)
	{
		// case failure
		MessageBox(::GetActiveWindow(), "Insufficient videoMemory or Not installed DirectX8", "Data Export Error", MB_OK);
		return 0;
	}
	
	this->Init();	// Init all variable

	delete m_pScene; 
	m_pScene =	new CN3Scene();
	m_pScene->ReleaseResrc();
	lstrcpy(m_szFileName, szFileName);	// Set File Name

	// Option Dialog 
	int rval = DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_EXPORT_OPTION), pIntf->GetMAXHWnd(), DlgProcExportOption, 0); // ��Ʈ�� �г� ��ȭ����..
	if(rval != 1) return TRUE;

	m_bCancelExport = FALSE; // Export possible

	m_pScene->Release(); // ��� �����ϰ�..

	m_pScene->m_fFrmCur		= m_Option.nFrmStart; //�𸣰ڴ�. �׳� ù���������� �Ѵ�.
	m_pScene->m_fFrmStart	= m_Option.nFrmStart;
	m_pScene->m_fFrmEnd		= m_Option.nFrmEnd;

	INode* pNodeRoot = g_pIntf->GetRootNode(); // Root Node Pointer
	// ProgressBar �ʱ�ȭ..
	m_nNodeCount = 0;
	this->CountAllNodes(pNodeRoot, m_nNodeCount);

	g_pIntf->ProgressStart(GetString(IDS_PROGRESS_MSG), TRUE, DlgProcProgress, NULL); // ���α׷��� �� ����
	this->ProcessRecursive(pNodeRoot); // ���ϵ� ��带 ��ȯ�ϸ鼭 Export.....
	g_pIntf->ProgressEnd();

	/////////////////////////////////
	// Scene Save

	// sub directory�� �ִٸ� sub directory �����
	char szDir[_MAX_DIR]="";
	::CreateDirectory(m_Option.szSubDir, NULL);
	wsprintf(szDir, "%sData", m_Option.szSubDir);	::CreateDirectory(szDir, NULL);
	wsprintf(szDir, "%sChr", m_Option.szSubDir);	::CreateDirectory(szDir, NULL);
	wsprintf(szDir, "%sObject", m_Option.szSubDir);	::CreateDirectory(szDir, NULL);
	wsprintf(szDir, "%sItem", m_Option.szSubDir);	::CreateDirectory(szDir, NULL);

	// ���� ī�޶� �ϳ��� ���ٸ�..
	if(m_pScene->CameraCount() <= 0) m_pScene->DefaultCameraAdd();
	if(m_pScene->LightCount() <= 0) m_pScene->DefaultLightAdd();
	m_pScene->SaveDataAndResourcesToFile(szFileName); // Scene ���� ���� �� ���ҽ� �� ��� ����..
	m_pScene->Release();

	m_pScene->ReleaseResrc();
	delete m_pScene;
	m_pScene = NULL;

	return TRUE;
}
	
bool CN3DExp::ProcessRecursive(INode* pNode)
{
	if(m_bCancelExport == TRUE) return false;
	if(NULL == pNode) return false;
	const char* szName = pNode->GetName(); // �̸�..

	// ���μ��� ������Ʈ (!?)
//	g_pIntf->ProgressUpdate((m_nNodeCur)*100/m_nNodeCount, TRUE, pNode->GetName()); 
	m_nNodeCur++;

	// � ��ü���� �о�´�. (camera? light? geom object?..)
	Object* pObj = pNode->EvalWorldState(m_Option.nFrmStart * 160).obj; 

	if(pObj)
	{
		DWORD dwSCID = pObj->SuperClassID();
		Class_ID cID = pObj->ClassID();

		if(	GEOMOBJECT_CLASS_ID == dwSCID && TRUE == m_Option.bExportGeometry) // Geometry Object
		{
			// ���� Export Routine
			bool bRootBiped = false;
			if(	m_Option.bExportCharacter && IsBone(pNode) ) // ĳ���� ��� �ɼ��� ���õǾ� �ְ�.. ���̸�...
			{
				Control* pCtrl = pNode->GetTMController();
				if(pCtrl) // ���� ��Ʈ �����е����� �˻��ϰ�...
				{
					if(pCtrl->ClassID() == BIPBODY_CONTROL_CLASS_ID)
					{
						bRootBiped = true;
					}
//					else if(pCtrl->ClassID() == BIPSLAVE_CONTROL_CLASS_ID) // �������� �����Ѵ�. 
//					else if(pCtrl->ClassID() == FOOTPRINT_CLASS_ID) 
				}
				if(false==bRootBiped)
				{
					INode* pParentNode = pNode->GetParentNode();
					if (pParentNode && FALSE == IsBone(pParentNode)) bRootBiped = true;	// ���߿��� �� ���� ���̸�
				}
			}

			if(bRootBiped) // ��Ʈ�ѷ��� ������� ��ü�̸�..
			{
				return this->ProcessChr(pNode); // ���� ó��...
			}
			else
			{				
				return this->ProcessShape(pNode); // �׳� ������Ʈ�� ���..
			}
		}
		else if(CAMERA_CLASS_ID == dwSCID && TRUE == m_Option.bExportCamera)
			return this->ProcessCamera(pNode); 
		else if(LIGHT_CLASS_ID == dwSCID && TRUE == m_Option.bExportLight)
			return this->ProcessLight(pNode);
//		else if(HELPER_CLASS_ID == dwSCID && TRUE == m_Option.bExportDummy)
//			return this->ProcessShape(pNode);
		else
		{
			char szDebug[512]; 
			wsprintf(szDebug, "Node : %s, �����մϴ�.\n", pNode->GetName());
			OutputDebugString(szDebug);
		}
	}
	else
	{
		char szDebug[512]; 
		wsprintf(szDebug, "Node : %s, Type ��ȯ�� �Ұ����� Node �Դϴ�. �����մϴ�.\n", pNode->GetName());
		OutputDebugString(szDebug);
	}

	int nChildCount = pNode->NumberOfChildren();
	INode* pNodeChild = NULL;
	for(int i = 0; i < nChildCount; i++)
	{
		pNodeChild = pNode->GetChildNode(i);
		if(pNodeChild==NULL) continue;

		this->ProcessRecursive(pNodeChild);
	}

	return false;
}

bool CN3DExp::ProcessCamera(INode* pNode)
{
	// Object�� pipeline ������ �о�´�. ObjectState Ŭ������ �����Ѵ�. 
	ObjectState os = pNode->EvalWorldState(m_Option.nFrmStart * 160);
	if(CAMERA_CLASS_ID != os.obj->SuperClassID()) return false;

	CN3Camera* pCamera = new CN3Camera;
	m_pScene->CameraAdd(pCamera);
	this->ProcessTransform(pNode, pCamera, false); // ��ġ ���ϸ��̼� Ű..
	pCamera->m_KeyRot.Release(); // At Vector Key
	pCamera->AtPosSet(0,0,0);
	pCamera->m_KeyScale.Release(); // Up Vector Key
	pCamera->UpVectorSet(0,1,0);

	INode* pTN = pNode->GetTarget(); // At Vector Key �� ó��..
	if(pTN)
	{
		CN3Transform TTrans;
		this->ProcessTransform(pTN, &TTrans, false);

		pCamera->AtPosSet(TTrans.Pos());
		pCamera->m_KeyRot.Add(TTrans.m_KeyPos, 0, TTrans.m_KeyPos.Count()); // Animation Key ó��..
	}

	
	// Camera Object �� �о�� ObjectState�� ������Ʈ�� ����(?)
	CameraObject* pCmObj = (CameraObject*)os.obj;

	CameraState CS;
	Interval valid = FOREVER;
	pCmObj->EvalCameraState(m_Option.nFrmStart * 160, valid, &CS);

	if(CS.manualClip)
	{
		pCamera->m_Data.fNP = CS.hither;
		pCamera->m_Data.fFP = CS.yon;
	}
	else
	{
		pCamera->m_Data.fNP = 0.3f;
		pCamera->m_Data.fFP = 256.0f;
	}
	pCamera->m_Data.fFOV = CS.fov;

	return TRUE;
}

bool CN3DExp::ProcessLight(INode* pNode)
{
	if(NULL == pNode) return false;

	CN3Light* pLight = new CN3Light;
	m_pScene->LightAdd(pLight);

	pLight->m_szName = "";
	this->ProcessName(pNode, pLight);
	this->ProcessTransform(pNode, pLight, false);

	ObjectState os = pNode->EvalWorldState(m_Option.nFrmStart * 160);

	// ����Ʈ ������Ʈ�� �����´�.
	GenLight* pLightObj = (GenLight*)os.obj;
	struct LightState ls;
	Interval valid = FOREVER;
//	Interval animRange = ip->GetAnimRange();
	pLightObj->EvalLightState(m_Option.nFrmStart * 160, valid, &ls);

	// This is part os the lightState, but it doesn't
	// make sense to output as an animated setting so
	// we dump it outside of ExportLightSettings()

//	Point3 ptLight = pNode->GetNodeTM(m_Option.nFrmStart * 160).GetTrans();
	Point3 ptLight = pNode->GetObjTMAfterWSM(m_Option.nFrmStart * 160).GetTrans();
	D3DCOLORVALUE __dcv;
	__dcv.r = ls.color.r;
	__dcv.g = ls.color.g;
	__dcv.b = ls.color.b;
	__dcv.a = 0;

	// ���� ���ϱ�
	INode* pTN = pNode->GetTarget();
	__Vector3 vDir(0,-1,0);
	if(pTN)
	{	// Ÿ���� ������� Ÿ���� ��ǥ�� light�� ��ǥ ���� ������ ������ ���Ѵ�.
		CN3Transform TTrans;
		this->ProcessTransform(pTN, &TTrans, false);
		vDir = TTrans.Pos() - D3DXVECTOR3(ptLight.x, ptLight.z, ptLight.y);
	}
	else if (OMNI_LGT != ls.type)
	{	// Ÿ���� ���� ��� light�� rotation���� ������ ������ ���Ѵ�.
		Matrix3 mtxLight = pNode->GetObjTMAfterWSM(m_Option.nFrmStart * 160);	// world��ǥ���� transform matrix�� ���Ѵ�.
		mtxLight.NoTrans(); mtxLight.NoScale();	// ��ġ�� ������ ��ȭ�� ���ش�.
		Point3 ptAt = mtxLight.PointTransform(Point3(0,0,-1));	// �⺻ ���͸� matrix�� ���ϸ� light�� ���� ���Ͱ� ���´�.
		vDir.Set(ptAt.x, ptAt.z, ptAt.y);	// z�� y�� �ٲپ��ش�.
	}
	vDir.Normalize();

	// ����Ʈ ����
	memset(&pLight->m_Data, 0, sizeof(pLight->m_Data));
	switch(ls.type)
	{
		//case OMNI_LIGHT:
		case OMNI_LGT:
			if (ls.useAtten) pLight->m_Data.InitPoint( m_pScene->LightCount() - 1, D3DXVECTOR3(ptLight.x, ptLight.z, ptLight.y), __dcv, ls.attenEnd);
			else pLight->m_Data.InitPoint( m_pScene->LightCount() - 1, D3DXVECTOR3(ptLight.x, ptLight.z, ptLight.y), __dcv);
			break;
		//case FSPOT_LIGHT:
		//case TSPOT_LIGHT:
		case SPOT_LGT:
			if (ls.useAtten) 
				pLight->m_Data.InitSpot( m_pScene->LightCount() - 1, D3DXVECTOR3(ptLight.x, ptLight.z, ptLight.y), vDir, __dcv, 
									D3DXToRadian(ls.hotsize), D3DXToRadian(ls.fallsize), ls.attenEnd);
			else
				pLight->m_Data.InitSpot( m_pScene->LightCount() - 1, D3DXVECTOR3(ptLight.x, ptLight.z, ptLight.y), vDir, __dcv, 
									D3DXToRadian(ls.hotsize), D3DXToRadian(ls.fallsize));
			break;
		//case DIR_LIGHT:
		case DIRECT_LGT:
			{
				pLight->m_Data.InitDirection(m_pScene->LightCount() - 1, vDir, __dcv);
			}
			break;
		default:
			pLight->m_Data.Type = D3DLIGHT_FORCE_DWORD;
			break;
	}

	// ���ֳ� �����ֳ� �����ش�.
	pLight->m_Data.bOn = ls.on;
	
	return true;
}

bool CN3DExp::ProcessShape(INode* pNode)
{
	if(NULL == pNode) return false;

	CN3IMesh N3IMesh;
	if(this->ProcessIMesh(pNode, &N3IMesh) == false) return false;
	__Material mtl;
	mtl.Init();
	CN3Texture* pTex = NULL;
	this->ProcessMaterial(pNode, &mtl, &pTex, "Object\\");
	if(pTex)
	{
//		char szDrv[_MAX_DIR], szDir[_MAX_DIR], szFName[_MAX_FNAME];
//		_splitpath(pTex->FileName().c_str(), szDrv, szDir, szFName, NULL);
//		char szFN[256]; wsprintf(szFN, "Object\\%s.dxt", szFName);
		pTex->FileNameSet(pTex->FileName());
	}


	CN3Shape* pShape = NULL;
	INode* pPN = pNode->GetParentNode();
	Object* pPObj = NULL;
	Class_ID cPID = Class_ID(0, 0);
	if(pPN) pPObj = pPN->EvalWorldState(m_Option.nFrmStart * 160).obj;
	if(pPObj) cPID = pPObj->ClassID();

	if(pPN && cPID == Class_ID(DUMMY_CLASS_ID,0)) // Parent Node �� ������..
	{
		CN3Shape ShpTmp;
		ProcessName(pPN, &ShpTmp);

		int nSC = m_pScene->ShapeCount();
		for(int i = 0; i < nSC; i++)
		{
			CN3Shape* pShpTmp = m_pScene->ShapeGet(i);
			if(ShpTmp.m_szName == pShpTmp->m_szName) // �׷����� �̸��� ���� ���� ������..
			{
				pShape = pShpTmp; // �ٷ� �� Shape �̴�..
				break;
			}
		}
	}

	if(NULL == pShape) // Scene �� �� ����� �׷쿡 �ش�Ǵ� Shape �� ���ų� Ȥ�� �ܵ����� �ִ� �޽��̸�..
	{
		pShape = new CN3Shape();
		Point3 ptPivot;
		if(pPN && cPID == Class_ID(DUMMY_CLASS_ID,0))
		{
			this->ProcessName(pPN, pShape);
			ptPivot = pPN->GetNodeTM(m_Option.nFrmStart * 160).GetTrans();
		}
		else
		{
			this->ProcessName(pNode, pShape);
			ptPivot = pNode->GetNodeTM(m_Option.nFrmStart * 160).GetTrans();
		}
//		pShape->PosSet(ptPivot.x * 0.0254f, ptPivot.z * 0.0254f, ptPivot.y * 0.0254f);  // Pivot ����.. y �� z �� �ݴ��.
		pShape->PosSet(ptPivot.x, ptPivot.z, ptPivot.y);  // Pivot ����.. y �� z �� �ݴ��.

		int nSI = m_pScene->ShapeAdd(pShape);
	}


	bool bCollision = false;
	::CharLower(N3IMesh.m_szName.begin()); // �ҹ��ڷ� �����..
	if(N3IMesh.m_szName.find("coll") != -1) // "collision" �̶�� ���ڿ� Ȯ�� ..  �׷��� ��Ÿ�� ����ؼ� "coll" ������ Ȯ��
		bCollision = true;

	if(true == bCollision)
	{
		// Pivot, Offset ����
		__Vector3 vOffset = pShape->Pos();
		N3IMesh.ApplyOffset(vOffset * -1.0f); // Offset ��ŭ ���ش�..
		
		CN3VMesh* pVMesh = new CN3VMesh();
		pVMesh->Import(&N3IMesh); // �޽� �����.. Indexed �޽÷κ��� Import..

		std::string szVMeshFN = "Object\\" + pShape->m_szName + ".n3vmesh";  // �̸�����..
		if (lstrlen(m_Option.szSubDir)>0) szVMeshFN = std::string(m_Option.szSubDir) + szVMeshFN;	// sub directory ������ �߰�
		pVMesh->FileNameSet(szVMeshFN); // �̸�����..
		CN3Base::s_MngVMesh.Add(pVMesh); // �Ŵ����� �ְ�..

		pShape->CollisionMeshSet(szVMeshFN); // �浹�޽� ����..
	}
	else // �浹 üũ �޽ð� �ƴϸ� ��Ʈ �߰�..
	{
		// Part �߰�.. Part Data ����..
		CN3SPart* pPD = pShape->PartAdd();
		this->ProcessName(pNode, pPD); // ��Ʈ �̸�..

		// Pivot, Offset ����
		Point3 ptPivot = pNode->GetNodeTM(m_Option.nFrmStart * 160).GetTrans();
	//	__Vector3 vOffset(ptPivot.x * 0.0254f, ptPivot.z * 0.0254f, ptPivot.y * 0.0254f);
		__Vector3 vOffset(ptPivot.x, ptPivot.z, ptPivot.y);
		N3IMesh.ApplyOffset(vOffset * -1.0f); // Offset ��ŭ ���ش�..
		
		CN3Mesh MeshTmp;
		MeshTmp.Import(&N3IMesh); // �ӽ÷� �޽� �����.. Indexed �޽÷κ��� Import..
		if(m_Option.bGenerateSmoothNormal)
		{
			MeshTmp.ReGenerateSmoothNormal(); // Normal �� ����..
		}
		MeshTmp.MakeIndexed(); // Indexed Mesh �� �����..

		// PMesh ����..
		CN3PMeshCreate PMCreate;
		PMCreate.ConvertFromN3Mesh(&MeshTmp);
		CN3PMesh* pPMesh = PMCreate.CreateRendererMesh();
		this->ProcessName(pNode, pPMesh);
		pShape->s_MngPMesh.Add(pPMesh);

		pPD->MeshSet(pPMesh->FileName());
		if(pTex)
		{
			pPD->TexAlloc(1); // Texture �Ҵ�..
			pPD->TexSet(0, pTex->FileName());
		}
		pPD->m_Mtl = mtl;
		pPD->m_vPivot = vOffset - pShape->Pos(); // Pivot point �� ���..
		pPD->ReCalcMatrix(pShape->m_Matrix); // ��� �ٽ� ���..
	}

	return true;
}

BOOL CALLBACK CN3DExp::DlgProcExportOption(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			CenterWindow(hWndDlg, GetParent(hWndDlg));

			// Registry �� �ɼǰ��� �־�ξ���..
			HKEY hKey = g_Eng.RegistryOpen("N3Export Option");
			if(hKey) 
			{
				g_Eng.RegistryValueGet(hKey, "Export Camera", &m_Option.bExportCamera, 4);
				g_Eng.RegistryValueGet(hKey, "Export Light", &m_Option.bExportLight, 4);
				g_Eng.RegistryValueGet(hKey, "Export Geometry", &m_Option.bExportGeometry, 4);
				g_Eng.RegistryValueGet(hKey, "Export Dummy", &m_Option.bExportDummy, 4);
				g_Eng.RegistryValueGet(hKey, "Export Character", &m_Option.bExportCharacter, 4);

				g_Eng.RegistryValueGet(hKey, "Animation Key", &m_Option.bAnimationKey, 4);
				g_Eng.RegistryValueGet(hKey, "Generate Smooth Normal", &m_Option.bGenerateSmoothNormal, 4);
//				g_Eng.RegistryValueGet(hKey, "Generate Progressive Mesh", &m_Option.bGenerateProgressiveMesh, 4);
				g_Eng.RegistryValueGet(hKey, "Generate Half Size Texture", &m_Option.bGenerateHalfSizeTexture, 4);
				g_Eng.RegistryValueGet(hKey, "Generate Compressed Texture", &m_Option.bGenerateCompressedTexture, 4);
				
				g_Eng.RegistryClose(hKey);
			}

			CheckDlgButton(hWndDlg, IDC_C_OBJ_CAMERA, m_Option.bExportCamera);
			CheckDlgButton(hWndDlg, IDC_C_OBJ_LIGHT, m_Option.bExportLight);
			CheckDlgButton(hWndDlg, IDC_C_OBJ_GEOMETRY, m_Option.bExportGeometry);
			CheckDlgButton(hWndDlg, IDC_C_OBJ_DUMMY, m_Option.bExportDummy);
			CheckDlgButton(hWndDlg, IDC_C_OBJ_CHARACTER, m_Option.bExportCharacter);

			CheckDlgButton(hWndDlg, IDC_C_ANIMATION_KEY, m_Option.bAnimationKey);

			CheckDlgButton(hWndDlg, IDC_C_GENERATE_SMOOTH_NORMAL, m_Option.bGenerateSmoothNormal);
//			CheckDlgButton(hWndDlg, IDC_C_GENERATE_PROGRESSIVE_MESH, m_Option.bGenerateProgressiveMesh);
			CheckDlgButton(hWndDlg, IDC_C_GENERATE_HALF_SIZE_TEXTURE, m_Option.bGenerateHalfSizeTexture);
			CheckDlgButton(hWndDlg, IDC_C_GENERATE_COMPRESSED_TEXTURE, m_Option.bGenerateCompressedTexture);

			Interval ii = g_pIntf->GetAnimRange();
			m_Option.nFrmStart = ii.Start() / 160; // ���� ������.
			m_Option.nFrmEnd = ii.End() / 160; // �� ������

			SetDlgItemInt(hWndDlg, IDC_E_FRAME_START, m_Option.nFrmStart, FALSE);
			SetDlgItemInt(hWndDlg, IDC_E_FRAME_END, m_Option.nFrmEnd, FALSE);
			SetDlgItemInt(hWndDlg, IDC_E_SAMPLING_RATE, (int)(m_Option.fSamplingRate), FALSE);

			SetDlgItemText(hWndDlg, IDC_E_SUBDIR, "");
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			{
				memset(&m_Option, 0, sizeof(m_Option));

				m_Option.bExportCamera = IsDlgButtonChecked(hWndDlg, IDC_C_OBJ_CAMERA);
				m_Option.bExportLight = IsDlgButtonChecked(hWndDlg, IDC_C_OBJ_LIGHT);
				m_Option.bExportGeometry = IsDlgButtonChecked(hWndDlg, IDC_C_OBJ_GEOMETRY);
				m_Option.bExportDummy = IsDlgButtonChecked(hWndDlg, IDC_C_OBJ_DUMMY);
				m_Option.bExportCharacter = IsDlgButtonChecked(hWndDlg, IDC_C_OBJ_CHARACTER);

				m_Option.bAnimationKey = IsDlgButtonChecked(hWndDlg, IDC_C_ANIMATION_KEY);
				m_Option.nFrmStart = GetDlgItemInt(hWndDlg, IDC_E_FRAME_START, NULL, FALSE); // ���� ������
				m_Option.nFrmEnd = GetDlgItemInt(hWndDlg, IDC_E_FRAME_END, NULL, FALSE); // �� ������
				m_Option.fSamplingRate = GetDlgItemInt(hWndDlg, IDC_E_SAMPLING_RATE, NULL, FALSE); // Sampling Rate

				m_Option.bGenerateSmoothNormal = IsDlgButtonChecked(hWndDlg, IDC_C_GENERATE_SMOOTH_NORMAL);
//				m_Option.bGenerateProgressiveMesh = IsDlgButtonChecked(hWndDlg, IDC_C_GENERATE_PROGRESSIVE_MESH);
				m_Option.bGenerateHalfSizeTexture = IsDlgButtonChecked(hWndDlg, IDC_C_GENERATE_HALF_SIZE_TEXTURE);
				m_Option.bGenerateCompressedTexture = IsDlgButtonChecked(hWndDlg, IDC_C_GENERATE_COMPRESSED_TEXTURE);

				GetDlgItemText(hWndDlg, IDC_E_SUBDIR, m_Option.szSubDir, _MAX_DIR);
				int iStrLen = lstrlen(m_Option.szSubDir);
				if (iStrLen>0)
				{
					if ('\\' != m_Option.szSubDir[iStrLen-1]) lstrcat(m_Option.szSubDir,"\\");
				}

				EndDialog(hWndDlg, 1);

				// Registry �� �ɼǰ��� �־�ξ���..
				HKEY hKey = g_Eng.RegistryOpen("N3Export Option");
				if(NULL == hKey) RegCreateKey(HKEY_CURRENT_USER, "N3Export Option", &hKey);
				if(hKey) 
				{
					g_Eng.RegistryValueSet(hKey, "Export Camera", &m_Option.bExportCamera, 4);
					g_Eng.RegistryValueSet(hKey, "Export Light", &m_Option.bExportLight, 4);
					g_Eng.RegistryValueSet(hKey, "Export Geometry", &m_Option.bExportGeometry, 4);
					g_Eng.RegistryValueSet(hKey, "Export Dummy", &m_Option.bExportDummy, 4);
					g_Eng.RegistryValueSet(hKey, "Export Character", &m_Option.bExportCharacter, 4);

					g_Eng.RegistryValueSet(hKey, "Animation Key", &m_Option.bAnimationKey, 4);
					g_Eng.RegistryValueSet(hKey, "Generate Smooth Normal", &m_Option.bGenerateSmoothNormal, 4);
//					g_Eng.RegistryValueSet(hKey, "Generate Progressive Mesh", &m_Option.bGenerateProgressiveMesh, 4);
					g_Eng.RegistryValueSet(hKey, "Generate Half Size Texture", &m_Option.bGenerateHalfSizeTexture, 4);
					g_Eng.RegistryValueSet(hKey, "Generate Compressed Texture", &m_Option.bGenerateCompressedTexture, 4);
					
					g_Eng.RegistryClose(hKey);
				}
			}
			return TRUE;
		case IDCANCEL:
			{
				EndDialog(hWndDlg, 0);
				return TRUE;
			}
		}
		break;
	case WM_CLOSE:
		EndDialog(hWndDlg, 0);
		return TRUE;
	default:
		break;
	}
	
	return FALSE;
}

void CN3DExp::DecodeTransformMatrix(Matrix3& matSrc, __Matrix44& matDest)
{
	// Decompose the matrix and dump the contents
	// Dump the whole Matrix
	AffineParts ap;
	decomp_affine(matSrc, &ap);

	float fAngle;
	Point3 vRot;

	// Quaternions are dumped as angle axis.
	AngAxisFromQ(ap.q, &fAngle, vRot);

	// ���...
	__Matrix44 m, tm;
	m.Identity();
	tm.Scale(ap.k.x, ap.k.z, ap.k.y); m *= tm; // ������
//	::D3DXMatrixRotationYawPitchRoll(&tm, vRot.z * fAngle, vRot.x * fAngle, vRot.y * fAngle); m *= tm; // ȸ��
	tm.RotationZ(fAngle * vRot.y); m *= tm; // ȸ��
	tm.RotationY(fAngle * vRot.z); m *= tm; // ȸ��
	tm.RotationX(fAngle * vRot.x); m *= tm; // ȸ��
//	m.PosSet(ap.t.x * 0.0254f, ap.t.z * 0.0254f, ap.t.y * 0.0254f); // �̵� // 1 Unit == 1 Inch -> Meter �� �ٲ۴�.. // Y, Z �� �ݴ��..
	m.PosSet(ap.t.x, ap.t.z, ap.t.y); // �̵� // 1 Unit == 1 Inch -> Meter �� �ٲ۴�.. // Y, Z �� �ݴ��..

	matDest = m; // ��.. ��ȯ ��� ���� �ϼ�..
}

void CN3DExp::CancelExport()
{
	m_bCancelExport = TRUE;
}

DWORD WINAPI CN3DExp::DlgProcProgress(LPVOID lpArg)
{
	return TRUE;
}

bool CN3DExp::ProcessName(INode* pNode, CN3BaseFileAccess* pBase)
{
	if(NULL == pBase) return false;
	DWORD dwType = pBase->Type();

	if(pNode && pNode->GetParentNode())
	{
		INode* pPN = pNode->GetParentNode(); // ���� �θ�
		INode* pPPN = NULL;	// ���� �θ��� �θ�
		if(pPN) pPPN = pPN->GetParentNode();

		// ĳ���� ��Ʈ�� ĳ���� ��Ʈ ��Ų�� ��� ���� �� ����̸� ���� �̸��� ���� �ʴ´�.
		if ((dwType & (OBJ_CHARACTER_PART|OBJ_CHARACTER_PART_SKINS)) && IsBone(pNode))
		{
		}
		else
		{
			if(!(pBase->m_szName.empty()))
			{
				std::string szTmp = pNode->GetName(); szTmp += '_'; szTmp += pBase->m_szName;
				pBase->m_szName =  szTmp;
			}
			else pBase->m_szName = pNode->GetName();
		}

		if(NULL != pPPN) // Scene Root �� �ƴ� ��쿡��..
		{
			this->ProcessName(pNode->GetParentNode(), pBase); // ��� ȣ��
			return true;
		}
	}

	char szDir[32]	= "";
	char szExt[32]	= "";

	if(dwType & OBJ_SHAPE) { lstrcpy(szDir, "Object\\"); lstrcpy(szExt, ".N3Shape"); }
	else if(dwType & OBJ_SHAPE_PART) { lstrcpy(szDir, "Object\\"); lstrcpy(szExt, ".N3SPart"); }
	else if(dwType & OBJ_MESH) { lstrcpy(szDir, "Object\\"); lstrcpy(szExt, ".N3Mesh"); }
	else if(dwType & OBJ_MESH_PROGRESSIVE) { lstrcpy(szDir, "Object\\"); lstrcpy(szExt, ".N3PMesh"); }
	else if(dwType & OBJ_MESH_VECTOR3) { lstrcpy(szDir, "Object\\"); lstrcpy(szExt, ".N3VMesh"); }
	
	else if(dwType & OBJ_CHARACTER) { lstrcpy(szDir, "Chr\\"); lstrcpy(szExt, ".N3Chr"); }
	else if(dwType & OBJ_JOINT) { lstrcpy(szDir, "Chr\\"); lstrcpy(szExt, ".N3Joint"); }
	else if(dwType & OBJ_CHARACTER_PART) { lstrcpy(szDir, "Item\\"); lstrcpy(szExt, ".N3CPart"); }
	else if(dwType & OBJ_CHARACTER_PART_SKINS) { lstrcpy(szDir, "Item\\"); lstrcpy(szExt, ".N3CSkins"); }
	else if(dwType & OBJ_MESH_INDEXED) { lstrcpy(szDir, "Item\\"); lstrcpy(szExt, ".N3IMesh"); }
	else if(dwType & OBJ_SKIN) { lstrcpy(szDir, "Item\\"); lstrcpy(szExt, ".N3Skin"); }
	else if(dwType & OBJ_CHARACTER_PLUG) { lstrcpy(szDir, "Item\\"); lstrcpy(szExt, ".N3CPlug"); }
	
	else if(dwType & OBJ_TEXTURE) { lstrcpy(szDir, "Texture\\"); lstrcpy(szExt, ".DXT"); }
	else if(dwType & OBJ_SCENE) { lstrcpy(szDir, ""); lstrcpy(szExt, ".N3Scene"); }
	
	else
	{
		lstrcpy(szDir, "Data\\");
		if(dwType & OBJ_CAMERA) lstrcpy(szExt, ".N3Camera");
		else if(dwType & OBJ_LIGHT) lstrcpy(szExt, ".N3Light");
		else if(dwType & OBJ_TRANSFORM) lstrcpy(szExt, ".N3Transform");
		else if(dwType & OBJ_BASE) lstrcpy(szExt, ".N3Base");
		else lstrcpy(szExt, ".Unknown");
	}

	// sub directory �� ������ �߰��Ѵ�.
	std::string szFN;
	if (lstrlen(m_Option.szSubDir) > 0) szFN = std::string(m_Option.szSubDir) + szDir + pBase->m_szName + szExt;
	else szFN = szDir + pBase->m_szName + szExt;

	pBase->FileNameSet(szFN);
	
	return true;
}

bool CN3DExp::ProcessChr(INode *pNode)
{
	int i, nCC;
	INode* pNodeRootJoint = NULL; // ��Ʈ ����Ʈ�� ã�ƾ� �Ѵ�..

	Control* pCtrl = pNode->GetTMController();
	if(pCtrl->ClassID() != BIPBODY_CONTROL_CLASS_ID)	// pNode�� �����е尡 �ƴϸ� pNode�� ��Ʈ����Ʈ��.
	{
		pNodeRootJoint = pNode;
	}//return false;
	else
	{	// pNode�� �����е�� �ڽ��߿� ��Ʈ����Ʈ�� ã�´�.

		nCC = pNode->NumberOfChildren();
		for(int i = 0; i < nCC; i++) 
		{
			INode* pNodeTmp = pNode->GetChildNode(i);
	//		Control* pCtrlTmp = pNodeTmp->GetTMController();
	//		if(NULL == pCtrlTmp) continue;
	//		const char* szJR = pNodeTmp->GetName();
	//		Class_ID cID = pCtrlTmp->ClassID();
	//		if(BIPSLAVE_CONTROL_CLASS_ID == cID) 
			if (IsBone(pNodeTmp))
			{
				pNodeRootJoint = pNodeTmp;
				break;
			}
		}
	}
	
	if(NULL == pNodeRootJoint) return false;

//	IBipedExport* pBE = (IBipedExport*)pCtrl->GetInterface(I_BIPINTERFACE); // biped export interface
//	pBE->RemoveNonUniformScale(1); // remove the non uniform scale
//	pBE->BeginFigureMode(1); // Figure Mode .. Binding pose �� ���
		
	CN3Joint* pJoint = new CN3Joint();
	if(false == this->ProcessJoint(pNodeRootJoint, pJoint)) // Joint ó��..
	{
		delete pJoint; pJoint = NULL;
//		pBE->EndFigureMode(1);
//		pCtrl->ReleaseInterface(I_BIPINTERFACE, pBE); // release biped export interface
		return false;
	}
	
	// ĳ���� ó��..
	CN3Chr* pChr = new CN3Chr();
	this->ProcessName(pNode, pChr);

	pChr->s_MngJoint.Add(pJoint);
	pChr->JointSet(pJoint->FileName());
	m_pScene->ChrAdd(pChr);

	pChr->m_szName = "Temp";
	pChr->FileNameSet(std::string("Chr\\Temp.n3Chr"));
	pChr->PartAlloc(64); // ����ϰ� Part Data �Ҵ�.. save�Ҷ� ���ʿ��� �����ʹ� ���ŵȴ�.
	for(i = 0; i < 64; i++)
	{
		CN3CPart* pPDAdd = pChr->Part(i);
		CN3CPartSkins* pSkinAdd = new CN3CPartSkins();
		char szNameTmp[256];
		wsprintf(szNameTmp, "chr\\temp_%d.N3CSkins", i);
		pSkinAdd->FileNameSet(szNameTmp);
		CN3Base::s_MngSkins.Add(pSkinAdd);
		pPDAdd->SkinsSet(szNameTmp);
		CN3Base::s_MngSkins.Delete(&pSkinAdd); // �̷��� ���־�� ���� ī��Ʈ�� �ϳ� �پ���..
	}

	// Biped ���� Editable Mesh �� ã��..
	std::list<INode*> MeshList;
	this->FindNodeRecursive(pNode, Class_ID(EDITTRIOBJ_CLASS_ID, 0x00), MeshList);

	std::list<INode*>::iterator it;
	for(it = MeshList.begin(); it != MeshList.end(); it++) // Mesh ����ŭ ó��..
	{
		INode* pNodeTmp = *it;

		// �浹 üũ������ ���� �޽����� ���캻��..
		bool bCollisionMesh = false;
		std::string szFNM = pNodeTmp->GetName();
		if(szFNM.size() > 0) CharLower(&(szFNM[0]));
		if(szFNM.find("coll") != -1) bCollisionMesh = true;

		if(true == bCollisionMesh) // �浹 üũ �޽ø�.... 
		{
		}
		else // if(false == bCollisionMesh) // �浹 üũ �޽� �ƴϸ�....���������� ����..
		{
			int nLOD = 0;
			int nPart = 0;
			INode* pMG = pNodeTmp->GetParentNode();	// Mesh Group node
			INode* pCG = NULL;						// Chr Group node
			if (pMG) pCG = pMG->GetParentNode();

			if (pCG == NULL || 
				(pMG && ( pMG == pNode  || IsBone(pMG)) ) )
				// ĳ���� ����̰ų� pMG�� �� ����� ���, �̶��� LOD�� ��� ��ü�������� �׷����� ���� �������̴�.
			{
				std::string strM1(pNodeTmp->GetName());
				std::string strM2;
				nCC = pMG->NumberOfChildren();
				for(i = 0; i < nCC; i++)
				{
					INode* pCN = pMG->GetChildNode(i);
					__ASSERT(pCN, "null pointer : no child");
					if (!CheckObjectClassID(pCN, Class_ID(EDITTRIOBJ_CLASS_ID, 0x00))) continue;	// �޽��� �ƴϸ� �Ѿ��.
					strM2 = pCN->GetName();
					if(strM1 == strM2) break; // �̸��� ���� ���..
					++nPart; // ���� ������ Part ����..
				}
			}
			else
			{	// LOD�� �־ ��ü �������� �׷� �Ǿ� ������� (pMG�� �׷��̴�)
				// part
				std::string strM1(pMG->GetName());
				std::string strM2;
				nCC = pCG->NumberOfChildren();
				for(i = 0; i < nCC; i++)
				{
					INode* pCN = pCG->GetChildNode(i);
					__ASSERT(pCN, "null pointer : no child");
					if (!CheckObjectClassID(pCN, Class_ID(DUMMY_CLASS_ID, 0x00))) continue;	// �׷��� �ƴϸ� �Ѿ��.
					strM2 = pCN->GetName();
					if(strM1 == strM2) break; // �̸��� ���� ���..
					++nPart; // ���� ������ Part ����..
				}

				// lod
				strM1 = pNodeTmp->GetName();
				nCC = pMG->NumberOfChildren();
				for(i = 0; i < nCC; i++)
				{
					INode* pCN = pMG->GetChildNode(i);
					__ASSERT(pCN, "null pointer : no child");
					if (!CheckObjectClassID(pCN, Class_ID(EDITTRIOBJ_CLASS_ID, 0x00))) continue;	// �޽��� �ƴϸ� �Ѿ��.
					strM2 = pCN->GetName();
					if(strM1 == strM2) break; // �̸��� ���� ���..
					++nLOD;	// ���� ������ LOD����
				}
			}

			__ASSERT(nPart >= 0 && nPart < 64, "Part count is bigger than 64");
			__ASSERT(nLOD >= 0 && nLOD < MAX_CHR_LOD, "LOD Count is bigger than MAX_CHR_LOD");
			
			CN3CPart* pPart = pChr->Part(nPart);
			CN3CPartSkins* pSkins = pPart->Skins();
			
			CN3Skin* pSkin = pPart->Skin(nLOD);
			if(false == this->ProcessPhysique(pNodeTmp, pNodeRootJoint, pSkin)) // Skin ó��..
			{
				MessageBox(::GetActiveWindow(), pNodeTmp->GetName(), "Skin processing failed", MB_OK);
			}

			CN3Texture* pTex = NULL;
			this->ProcessMaterial(pNodeTmp, &(pPart->m_Mtl), &pTex, "Item\\");
			if(pTex)
			{
//				char szDrv[_MAX_DIR], szDir[_MAX_DIR], szFName[_MAX_FNAME];
//				_splitpath(pTex->FileName().c_str(), szDrv, szDir, szFName, NULL);
//				char szFN[256]; wsprintf(szFN, "Item\\%s.dxt", szFName);
//				pTex->FileNameSet(szFN);
				pPart->TexSet(pTex->FileName());
			}
			ProcessName(pNodeTmp, pPart);	// �̸� ����
			ProcessName(pNodeTmp, pSkins);	// �̸� ����
		}
	}

	pChr->Init(); // Initializing Skin, IMesh, .... Inverse Matrix....
	
//	pBE->EndFigureMode(1);
//	pCtrl->ReleaseInterface(I_BIPINTERFACE, pBE); // release biped export interface
//	pBE = NULL;
	g_pIntf->RedrawViews(0); // ����� ����.. Redraw if you want to see the result

//	IBipedExport* pBE = (IBipedExport*)pCtrl->GetInterface(I_BIPINTERFACE); // biped export interface
//	pBE->RemoveNonUniformScale(1); // remove the non uniform scale
//	pBE->BeginFigureMode(1); // Figure Mode .. Binding pose �� ���
//
//	
//	pBE->EndFigureMode(1);
//	g_pIntf->RedrawViews(0); // ����� ����.. Redraw if you want to see the result
//	pCtrl->ReleaseInterface(I_BIPINTERFACE, pBE); // release biped export interface

	// biped chr
/*	Control *c = pNode->GetTMController();
	if( (c->ClassID()==BIPSLAVE_CONTROL_CLASS_ID)||
		(c->ClassID()==BIPBODY_CONTROL_CLASS_ID)||
		(c->ClassID()==FOOTPRINT_CLASS_ID))
	{			
		// get the biped export interface from the controller
		IBipedExport* BipIface=(IBipedExport*)c->GetInterface(I_BIPINTERFACE);

		
		BipIface->RemoveNonUniformScale(1); // remove the non uniform scale
		BipIface->BeginFigureMode(1); // Figure Mode .. Binding pose �� ���

		// to do ----------------------------------------------------------------------------
		// these are subanim numbers for the center of mass controller
        Animatable* pVer = c->SubAnim(VERTICAL_SUBANIM);		// vertical
		Animatable* pHor = c->SubAnim(HORIZONTAL_SUBANIM);	// horizontal
		Animatable* pRot = c->SubAnim(ROTATION_SUBANIM);		// rotation
		
		if(NULL!=pVer)
		{
			OutputDebugString("pVer is not NULL\n");
			char temp[512]=_T("");
			sprintf(temp,"[%s][%s][%s]\n",pVer->SubAnimName(0),pVer->SubAnimName(1),pVer->SubAnimName(2));
			OutputDebugString(temp);
		}
		
		if(NULL!=pHor)
		{
			OutputDebugString("pHor is not NULL\n");
			char temp[512]=_T("");
			sprintf(temp,"[%s][%s][%s]\n",pHor->SubAnimName(0),pHor->SubAnimName(1),pHor->SubAnimName(2));
			OutputDebugString(temp);
		}

		if(NULL!=pRot)
		{
			OutputDebugString("pRot is not NULL\n");
			char temp[512]=_T("");
			sprintf(temp,"[%s][%s][%s]\n",pRot->SubAnimName(0),pRot->SubAnimName(1),pRot->SubAnimName(2));
			OutputDebugString(temp);
		}
	
		char temp[512]=_T("");
		sprintf(temp,"[%s][%s][%s]\n",c->SubAnimName(0),c->SubAnimName(1),c->SubAnimName(2));
		OutputDebugString(temp);

		//-----------------------------------------------------------------------------------
		// temp///////////////////////////////////////////////////////////////////////////////
//		Control* pPosCtrl = c->GetPositionController();
//		Control* pRotCtrl = c->GetRotationController();
//	 	Control* pSclCtrl = c->GetScaleController();
//
//		this->ExportAnimationPosition(pPosCtrl); // ��ġ
//		this->ExportAnimationRotation(pRotCtrl); // ȸ��
//		this->ExportAnimationScale(pSclCtrl);	 // ������
//
//		Control* pTPosCtrl = NULL;
//		INode* pTarget = pNode->GetTarget();
//		if(pTarget != NULL) pTPosCtrl = pTarget->GetTMController()->GetPositionController();
//		this->ExportAnimationPosition(pTPosCtrl); // Ÿ�� ��ġ ���ϸ��̼�..
		///////////////////////////////////////////////////////////////////////////////////////

		BipIface->EndFigureMode(1);
		
		// Redraw if you want to see the result
		g_pIntf->RedrawViews(0);

		// release the interface when you are done with it
		c->ReleaseInterface(I_BIPINTERFACE,BipIface);
	}
*/
	return true;
}

bool CN3DExp::ProcessTransform(INode* pNode, CN3Transform* pTransform, bool bLocalCoordinate) // ��ġ
{
	if(NULL == pTransform || NULL == pNode) return false;

	pTransform->m_szName = "";
	this->ProcessName(pNode, pTransform);

	TimeValue mTime;
	Matrix3 mMtx, mMtxIP; // Matrix, Inverse Parent Matrix
	AffineParts mAP;

	mTime = m_Option.nFrmStart * 160;
	mMtx = pNode->GetNodeTM(mTime); // 0 Frame °�� Ű���� �����´�..
	if(bLocalCoordinate) mMtx *= Inverse(pNode->GetParentTM(mTime)); // ���� ��ǥ�� �ٲٱ�..
	decomp_affine(mMtx, &mAP);

//	pTransform->PosSet(mAP.t.x * 0.0254f, mAP.t.z * 0.0254f, mAP.t.y * 0.0254f); // 1 Unit == 1 Inch -> Meter �� �ٲ۴�.. // ��ġ - y, z �� �ٲپ� �ش�..
	pTransform->PosSet(mAP.t.x, mAP.t.z, mAP.t.y); // 1 Unit == 1 Inch -> Meter �� �ٲ۴�.. // ��ġ - y, z �� �ٲپ� �ش�..
	__Quaternion qt; // ȸ��.
	qt.x = mAP.q.x; qt.y = mAP.q.y; qt.z = mAP.q.z; qt.w = mAP.q.w;
	float fD = 0.0f;
	__Vector3 vAxis(0,1,0);
	qt.AxisAngle(vAxis, fD);
	float fTmp = vAxis.y; vAxis.y = vAxis.z; vAxis.z = fTmp; // y��� z���� �ٲ۴���..
	qt.RotationAxis(vAxis, fD); // ���ʹϾ� ��� �� ����.
	pTransform->RotSet(qt);
	pTransform->ScaleSet(mAP.k.x * mAP.f, mAP.k.y * mAP.f, mAP.k.z * mAP.f); // Scale

	if(FALSE == m_Option.bAnimationKey) return true; // Animation Key ó�� �ɼ�..

	int nKC = 0;
	if(pTransform->Type() & OBJ_JOINT) // Joint �϶��� Sampling...
	{
		nKC = (m_Option.nFrmEnd - m_Option.nFrmStart) * (m_Option.fSamplingRate / 30.0f);
	}
	else
	{
		Control* pmCtrl = pNode->GetTMController();
		if(NULL == pmCtrl) return true;

		IKeyControl* pmKey = GetKeyControlInterface(pmCtrl);
		if(NULL == pmKey) return true;

		nKC = pmKey->GetNumKeys();
		if(nKC <= 0) return true; // Ű���� ������ ���ø� ���� �ʴ´�..

		IKey mKTmp;
		pmKey->GetKey(nKC - 1, &mKTmp); // ������Ű�� ��������..
		nKC = (mKTmp.time / 160.0f) * (m_Option.fSamplingRate / 30.0f); // �̼�ġ�� ���� ��� ���������� �ȴ�.. ���� ������ �������� �ȴ�.
		if(nKC <= 0) return true;
	}
	
	pTransform->m_KeyPos.Alloc(nKC, m_Option.fSamplingRate, KEY_VECTOR3); // Ű �Ҵ�..
	if(pTransform->Type() & OBJ_JOINT) pTransform->m_KeyRot.Alloc(nKC, m_Option.fSamplingRate, KEY_QUATERNION); // ȸ�� Ű �Ҵ�.. �̰� ���ʹϾ� �Ҵ��̴�...
	else pTransform->m_KeyRot.Alloc(nKC, m_Option.fSamplingRate, KEY_VECTOR3); // ȸ�� Ű �Ҵ�.. �̰� ���ʹϾ� �Ҵ��̴�...
	pTransform->m_KeyScale.Alloc(nKC, m_Option.fSamplingRate, KEY_VECTOR3); // Ű �Ҵ�..

	__Vector3* pVKey = NULL;
	
	for(int i = 0; i < nKC; i++)
	{
		mTime = m_Option.nFrmStart * 160 +
				(int)(i*30.0f/m_Option.fSamplingRate)*160;
		mMtx = pNode->GetNodeTM(mTime); // Key �� �ش��ϴ� Transform Sampling
		if(bLocalCoordinate) mMtx *= Inverse(pNode->GetParentTM(mTime)); // ���� ��ǥ�� �ٲٱ�.. // Parent Matrix �����
		decomp_affine(mMtx, &mAP);

		__Vector3* pVP = (__Vector3*)(pTransform->m_KeyPos.DataGet(i));
		__Vector3* pVS = (__Vector3*)(pTransform->m_KeyScale.DataGet(i));
		__Quaternion* pQR = (__Quaternion*)(pTransform->m_KeyRot.DataGet(i));
		pVP->Set(mAP.t.x, mAP.t.z, mAP.t.y); // ��ġ // 1 Unit == 1 Inch -> Meter �� �ٲ۴�..
		if(pQR) // ȸ��.
		{
			__Quaternion qt;
			qt.x = mAP.q.x; qt.y = mAP.q.y; qt.z = mAP.q.z; qt.w = mAP.q.w;
			fD = 0.0f;
			__Vector3 vAxis(0,1,0);
			qt.AxisAngle(vAxis, fD);
			float fTmp = vAxis.y; vAxis.y = vAxis.z; vAxis.z = fTmp; // y��� z���� �ٲ۴���..
//			while (fD>D3DX_PI) fD -= (D3DX_PI*2.0f);
//			while (fD<= (-D3DX_PI)) fD += (D3DX_PI*2.0f);
			pQR->RotationAxis(vAxis, fD); // ���ʹϾ� ��� �� ����.
		}
		pVS->Set(mAP.k.x * mAP.f, mAP.k.y * mAP.f, mAP.k.z * mAP.f); // Scale
	}

	// Ű�� ����..
	CN3AnimKey* pKey[3] = { &(pTransform->m_KeyPos), &(pTransform->m_KeyRot), &(pTransform->m_KeyScale) };
	for(i = 0; i < 3; i++)
	{
		nKC = pKey[i]->Count();
		
		bool bSameKeys = true;
		float fCriticlaMass = 0.00001f;
		if(pKey[i]->Type() == KEY_VECTOR3)
		{
			__Vector3* pV0 = (__Vector3*)pKey[i]->DataGet(0);
			__Vector3* pV = NULL;
			for(int j = 0; j < nKC; j++)
			{
				pV = (__Vector3*)(pKey[i]->DataGet(j));
				if(	fabs(pV0->x - pV->x) > fCriticlaMass ||
					fabs(pV0->y - pV->y) > fCriticlaMass || 
					fabs(pV0->z - pV->z) > fCriticlaMass )
				{
					bSameKeys = false;
					break;
				}
			}
		}
		else if(pKey[i]->Type() == KEY_QUATERNION)
		{
			__Quaternion* pV0 = (__Quaternion*)pKey[i]->DataGet(0);
			__Quaternion* pV = NULL;
			for(int j = 0; j < nKC; j++)
			{
				pV = (__Quaternion*)(pKey[i]->DataGet(j));
				if(	fabs(pV0->x - pV->x) > fCriticlaMass ||
					fabs(pV0->y - pV->y) > fCriticlaMass || 
					fabs(pV0->z - pV->z) > fCriticlaMass || 
					fabs(pV0->w - pV->w) > fCriticlaMass )
				{
					bSameKeys = false;
					break;
				}
			}
		}
		if(bSameKeys)
			pKey[i]->Release(); // Ű���� ��� �Ȱ��ٸ�.. �����.
	}

	return true;
}

bool CN3DExp::ProcessJoint(INode *pNode, CN3Joint *pJoint)
{
	if(NULL == pJoint || NULL == pNode) return false;
	if (false == IsBone(pNode)) return false;	// ���� �ƴϸ� ����

	bool bLocalCoordinate = true;
	if(pJoint->Parent() == NULL) bLocalCoordinate = false;
	this->ProcessTransform(pNode, pJoint, bLocalCoordinate); // Transformation ó��.. ��Ʈ�϶��� ���� ��..
	
	// �̸��� �ʹ� ��ϱ�.. ������ �Ѵ�.
	pJoint->m_szName = pNode->GetName(); 
	char szJFN[256];
	wsprintf(szJFN, "%sChr\\%s.N3Joint", m_Option.szSubDir, pNode->GetName());
	pJoint->FileNameSet(szJFN);

	///////////////////////////////////////
	// �ڽ� ��ü ó��..		
	int nCC = pNode->NumberOfChildren();
	for(int i = 0; i < nCC; i++)
	{
		INode* pNodeChild = pNode->GetChildNode(i);
		if(false == IsBone(pNodeChild)) continue;

		CN3Joint* pChild = new CN3Joint();
		pJoint->ChildAdd(pChild);

		if(true != this->ProcessJoint(pNodeChild, pChild))
		{
			pJoint->ChildDelete(pChild); // �����ϸ� ���ϵ忡�� ����..
			delete pChild; pChild = NULL;
		}
	}
	// �ڽ� ��ü ó��..
	///////////////////////////////////////

	return true;
}

bool CN3DExp::IsBone(INode *pNode)
{
	if(pNode == NULL)return false;
//	const char* szName = pNode->GetName();
	ObjectState os = pNode->EvalWorldState(m_Option.nFrmStart * 160);
	if (!os.obj) return false;

	Class_ID cID = os.obj->ClassID();
	if(Class_ID(BONE_CLASS_ID,0) == cID ||
		BONE_OBJ_CLASSID == cID)	// New procedural bone object for Magma (subclass of GeomObject)
		return true;

	if(cID == Class_ID(DUMMY_CLASS_ID, 0))	// ���������� TMController�� ������Ʈ ��Ʈ���ϰ�찡 �����Ƿ�
		return false;
	
	Control *cont = pNode->GetTMController();
	//other Biped parts
	if(	cont->ClassID() == BIPSLAVE_CONTROL_CLASS_ID ||
		//Biped root "Bip01"
		cont->ClassID() == BIPBODY_CONTROL_CLASS_ID
	) return true;
	return false;
}

bool CN3DExp::FindNodeRecursive(INode *pNode, Class_ID cID, std::list<INode*> &list)
{
	if(NULL == pNode) return false;

	Object* pObj = pNode->EvalWorldState(m_Option.nFrmStart * 160).obj;
	const char* szName = pNode->GetName();
	
	if(pObj)
	{
		TSTR szClass;
		pObj->GetClassName(szClass);
		Class_ID cID2 = pObj->ClassID();
		DWORD dwSCID = pObj->SuperClassID();

		if(cID2 == cID)
			list.push_back(pNode);
	}

	///////////////////////////////////////
	// �ڽ� ��ü ó��..		
	int nCC = pNode->NumberOfChildren();
	for(int i = 0; i < nCC; i++)
	{
		INode* pNodeChild = pNode->GetChildNode(i);

		this->FindNodeRecursive(pNodeChild, cID, list);
	}

	return true;
}

bool CN3DExp::CheckObjectClassID(INode* pNode, const Class_ID& cID) const	// �־��� ��尡 �־��� Ŭ���� ���̵����� �˻�
{
	if (NULL == pNode) return false;
	Object* pObj = pNode->EvalWorldState(m_Option.nFrmStart * 160).obj;

	if(pObj)
	{
//		TSTR szClass;
//		pObj->GetClassName(szClass);
		Class_ID cID2(pObj->ClassID());
//		DWORD dwSCID = pObj->SuperClassID();

		if(cID2 == cID) return true;
	}
	return false;
}




// Bone �� Skin ó�� ��ƾ.. �Ϻκ������� ���߿� ������ �ִ�..
////////////////////////////////////
/*
				ISkin* pmSkin = NULL;
				ModContext *pMC = NULL;
				Object* pObj = pNode->GetObjectRef();
				if (NULL == pObj) return NULL;
				
				TSTR szClass;
				pObj->GetClassName(szClass);
				Class_ID cID = pObj->ClassID();
				SClass_ID scID = pObj->SuperClassID();

				while (scID == GEN_DERIVOB_CLASS_ID && pMC == NULL && pmSkin == NULL)
				{
					IDerivedObject* pDerObj = (IDerivedObject *)(pObj);

					pDerObj->GetClassName(szClass);

					int nMC = nMC = pDerObj->NumModifiers();
					for(int i = 0; i < nMC; i++)
					{
						Modifier* pMod = pDerObj->GetModifier(i); // Get the modifier. 
						
						pMod->GetClassName(szClass);
						cID = pMod->ClassID();
						scID = pMod->SuperClassID();


						if (pMod->ClassID() == SKIN_CLASSID) // is this the correct Physique Modifier based on index?
						{
							pmSkin = (ISkin*)pMod;
							pMC = pDerObj->GetModContext(i);
							break;
						}
					}
					pObj = pDerObj->GetObjRef();
					scID = pObj->SuperClassID();
				}

				if(pmSkin)
				{
					int nBC = pmSkin->GetNumBones();
					for(int i = 0 ; i < nBC; i++)
					{
						const char* szBone = pmSkin->GetBoneName(i);
						INode* pBN = pmSkin->GetBone(i);
					}
				}

//				if(pMC && pMC->localData)
//				{
//					BoneModData *bmd = (BoneModData *) mc->localData;
//				}
				
*/

bool CN3DExp::ProcessIMesh(INode *pNode, CN3IMesh* pIMesh)
{
	if(NULL == pNode) return false;
	if(NULL == pIMesh) return false;
	if(pNode->ClassID() == Class_ID(DUMMY_CLASS_ID,0)) return false; // �����̸�.. �Ѿ��..
	const char* szNamTmp = pNode->GetName();

	TriObject* pTriObj = NULL;

	BOOL bNeedDelete = FALSE;
	Object *pObj = pNode->EvalWorldState(m_Option.nFrmStart * 160).obj;
	if (pObj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
	{ 
		pTriObj = (TriObject *)pObj->ConvertToType(m_Option.nFrmStart * 160, Class_ID(TRIOBJ_CLASS_ID, 0));
		if (pObj != pTriObj) bNeedDelete = TRUE;
	}

	if (pTriObj == NULL || pNode->IsGroupHead()) // ������Ʈ���� �ƴϰų� �׷� ����..
	{
		char szDebug[512];
		wsprintf(szDebug, "%s -> May be Camera Target or Light Symbol\n", pNode->GetName());
		OutputDebugString(szDebug);
		return false;
	}

	Mesh* pmMesh = &pTriObj->GetMesh(); // TriObject���� Mesh�� ���´�.
	pmMesh->buildNormals(); // Normal �� �����..

	int nFC = pmMesh->getNumFaces(); // Mesh���� Face�� �о�´�.
	int nVC = pmMesh->getNumVerts(); // // Mesh���� �� ���� �о�´�.
	int nUVC = pmMesh->getNumTVerts(); // �ؽ�ó ���� ��...

	if(nFC <= 0) // ���� ���ؽ��� �ϳ��� ������ ���ư���..
	{
		OutputDebugString("Mesh Export - ���� ���̽��� �ϳ��� �����ϴ�..\n");
		if(bNeedDelete) delete pTriObj;
		return false;
	}

//	Matrix3 mMtx = pNode->GetNodeTM(m_Option.nFrmStart * 160); // ���� ��İ����´�.
	Matrix3 mMtx = pNode->GetObjTMAfterWSM(m_Option.nFrmStart * 160); // ���� ��İ����´�.
//	Matrix3 mMtx = pNode->GetObjectTM(m_Option.nFrmStart * 160); // ���� ��İ����´�.

	__VertexXyzNormal*	pvDest1 = NULL;
	__Vector3*			pvDest2 = NULL;
	CN3VMesh* pVMesh = NULL;
	if(nFC >= 8192) // �� ū�Ÿ� .. ���������� ���� VMesh ��� �����ϰ�... 
	{
		pVMesh = new CN3VMesh();
		pVMesh->CreateVertices(nFC * 3);
		pvDest2 = pVMesh->Vertices();
	}
	else
	{
		Point3 ptTmp;
		pIMesh->Create(nFC, nVC, nUVC);
		pvDest1 = pIMesh->Vertices(); // �� �����͸� �������� 
		this->ProcessName(pNode, pIMesh);

		for(int i = 0; i < nVC; i++)
		{
			ptTmp = mMtx * pmMesh->getVert(i); // Vertex ����ŭ ������ ������.
			pvDest1[i].x = ptTmp.x; // * 0.0254f; // 1 Unit == 1 Inch -> Meter �� �ٲ۴�..
			pvDest1[i].y = ptTmp.z; // * 0.0254f;
			pvDest1[i].z = ptTmp.y; // * 0.0254f; // y �� z �� �ٲ۴�..
		}
		for(i = 0; i < nUVC; i++)
		{
			pIMesh->UVSet(i, pmMesh->tVerts[i].x, 1.0f - pmMesh->tVerts[i].y); // �ϴ� �ؽ�ó ������ ����� �д�..
		}
	}

	//////////////////////////////////////
	// �������� �˻��Ѵ�.
	int vx1 = 0, vx2 = 2, vx3 = 1;  // Triangle ���� ȸ�� ���ϱ�.
	AffineParts ap; 
	decomp_affine(mMtx, &ap);
	if(ap.k.x * ap.f <= 0 || ap.k.y * ap.f <= 0 || ap.k.z * ap.f <= 0)
	{
//		MessageBox(::GetActiveWindow(), pNode->GetName(), 
//			"��� : ������ ���� ���� �Դϴ� -> Face�� ������ ���� �� �ֽ��ϴ�.", MB_OK);
		vx1 = 0; vx2 = 1; vx3 = 2;
	}

	Point3 ptTmp, ptNTmp;
	int nVI[3], nUVI[3];
	for (int i = 0; i < nFC; i++) // Face Count ��ŭ ���鼭..
	{
		Face* pFace = &(pmMesh->faces[i]);
		nVI[0] = pFace->v[vx1];
		nVI[1] = pFace->v[vx2];
		nVI[2] = pFace->v[vx3];
		
		if(pvDest2) // ���������� ���� �Ÿ�...
		{
			for(int j = 0; j < 3; j++)
			{
				ptTmp = mMtx * pmMesh->getVert(nVI[j]); // Vertex ����ŭ ������ ������.
				pvDest2[i*3+j].x = ptTmp.x; // * 0.0254f; // 1 Unit == 1 Inch -> Meter �� �ٲ۴�..
				pvDest2[i*3+j].y = ptTmp.z; // * 0.0254f; // 1 Unit == 1 Inch -> Meter �� �ٲ۴�..
				pvDest2[i*3+j].z = ptTmp.y; // * 0.0254f; // 1 Unit == 1 Inch -> Meter �� �ٲ۴�..
			}
		}
		else
		{
			pIMesh->VertexIndexSet(i*3+0, nVI[0]); // Vertex Indices
			pIMesh->VertexIndexSet(i*3+1, nVI[1]);
			pIMesh->VertexIndexSet(i*3+2, nVI[2]);

			ptNTmp = mMtx * pmMesh->getFaceNormal(i); // Face Normal
			ptNTmp = ptNTmp.Normalize(); // Normalize

			pvDest1[nVI[0]].n.x = ptNTmp.x;
			pvDest1[nVI[0]].n.y = ptNTmp.z; // y 
			pvDest1[nVI[0]].n.z = ptNTmp.y;

			pvDest1[nVI[1]].n.x = ptNTmp.x;
			pvDest1[nVI[1]].n.y = ptNTmp.z; // y 
			pvDest1[nVI[1]].n.z = ptNTmp.y;

			pvDest1[nVI[2]].n.x = ptNTmp.x;
			pvDest1[nVI[2]].n.y = ptNTmp.z; // y 
			pvDest1[nVI[2]].n.z = ptNTmp.y;

			if(nUVC > 0)
			{
				nUVI[0] = pmMesh->tvFace[i].t[vx1]; // UV Indices
				nUVI[1] = pmMesh->tvFace[i].t[vx2];
				nUVI[2] = pmMesh->tvFace[i].t[vx3];

				pIMesh->UVIndexSet(i*3+0, nUVI[0]);
				pIMesh->UVIndexSet(i*3+1, nUVI[1]);
				pIMesh->UVIndexSet(i*3+2, nUVI[2]);
			}
		}
	}

	if(m_Option.bGenerateSmoothNormal == TRUE && pvDest1)
	{
		pIMesh->ReGenerateSmoothNormal();	// ���� ���� �ε巴�� ����..
	}

	if (bNeedDelete)
	{
		delete pTriObj;
		pTriObj = NULL;
	}

	if(pVMesh) // ���������� ���� �޽ø�..
	{
		pVMesh->m_szName = pNode->GetName();
		pVMesh->SaveToFile(pVMesh->m_szName + ".n3vmesh"); // ����..
		
		delete pVMesh; pVMesh = NULL;
		return false; // �̷��� �����ؾ� IMesh �� ���� �ʴ´�..
	}

	return true;
}

bool CN3DExp::ProcessMaterial(INode* pNode, __Material* pMtl, CN3Texture** ppTex, LPCTSTR pszDir)
{
	if(NULL == pNode || NULL == pMtl || NULL == ppTex) return false;

	const char* szName = pNode->GetName();

	pMtl->Init();
	*ppTex = NULL;
	
	Mtl* pmMtl = pNode->GetMtl(); // ��忡�� Material�� ���´�.
	if(NULL == pmMtl) return false; // Material �� ������ ������ �̾ƿ´�.

	// Methods to access sub-materials of meta-materials 
//	Mtl* pmMtlTmp = NULL;
	int nSM = pmMtl->NumSubMtls();
//	if(nSM > 0)
//	{
//		for(int i = 0; i < nSM; i++)
//		{
//			if(pmMtl->GetSubMtl(i) != NULL) 
//			{
//				pmMtlTmp = pmMtl->GetSubMtl(i);
//				break;
//			}
//		}
//	}
//	else
//	{
//		pmMtlTmp = pmMtl;
//	}
	
	if(pmMtl && pmMtl->GetSubTexmap(1) != NULL) // �ؽ�ó�� ���� �ִ� �����̸�.
	{
		Texmap* pmTex = pmMtl->GetSubTexmap(1); // Diffuse Texture Map
		BitmapTex* pBMT = NULL;
		
		if(pmTex && pmTex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00))
		{ 
			pBMT = (BitmapTex*)pmTex; // ĳ��Ʈ �����ڷ� �ٲ��� �� �ִ�.
		}

		if(pBMT && lstrlen(pBMT->GetMapName()) > 0)
		{
			std::string szBMPFN, szBMPFN2;
			char szDrv[32], szDir[128], szName[128], szExt[128];
			szBMPFN = pBMT->GetMapName();
			_splitpath(szBMPFN.c_str(), szDrv, szDir, szName, szExt);

			// sub directory �����Ǿ� ������ ���̱�
			if (lstrlen(m_Option.szSubDir)>0) szBMPFN2 = std::string(m_Option.szSubDir);
			else szBMPFN2 = "";

			// �ؽ��� ���� ��� ���̱�
			if (pszDir) szBMPFN2 += pszDir;
			else szBMPFN2 += "Texture\\";
			szBMPFN2 += szName; szBMPFN2 += ".DXT"; // �̸��� Ȯ���ڸ� �ٲپ� �ش�..
			
			CharLower(szBMPFN.begin());
			CharLower(szBMPFN2.begin());

			int nTCPrev = m_pScene->s_MngTex.Count(); // �ؽ�ó �ߺ� üũ..
			bool bOverlapped = false;
			for(int i = 0; i < nTCPrev; i++)
			{
				if(m_pScene->s_MngTex.Get(i)->FileName() == szBMPFN2)
				{
					bOverlapped = true;
				}
			}

			if(bOverlapped)
			{
				(*ppTex) = m_pScene->s_MngTex.Get(szBMPFN2); // ��Ʈ���� �а�..
			}
			else
			{
				(*ppTex) = new CN3Texture();
				
				if(false == (*ppTex)->LoadFromFile(szBMPFN)) // ��Ʈ�� �бⰡ �����ϸ�..
				{
					delete (*ppTex); (*ppTex) = NULL;
					return false;
				}
				else // ��Ʈ���� �о�����..
				{
					(*ppTex)->m_szName = pBMT->GetName();
					(*ppTex)->FileNameSet(szBMPFN2);
					m_pScene->s_MngTex.Add(*ppTex); // Texture Manager �� �־��ش�.

					CN3Texture* pTex = *ppTex;
					if(m_Option.bGenerateHalfSizeTexture) // Texture Size �� �������� �ٿ� ���..
					{
						D3DFORMAT fmt = pTex->PixelFormat();
						int nW2 = pTex->Width() / 2;
						int nH2 = pTex->Height() / 2;
						pTex->Convert(fmt, nW2, nH2);
					}

					if(m_Option.bGenerateCompressedTexture) // Texture ���� ���
					{
						D3DFORMAT fmt = pTex->PixelFormat(), NewFormat = D3DFMT_UNKNOWN;
						if(D3DFMT_X8R8G8B8 == fmt) NewFormat = D3DFMT_DXT1;
						else if(D3DFMT_A8R8G8B8 == fmt) NewFormat = D3DFMT_DXT3;

						if(D3DFMT_UNKNOWN != NewFormat)
						{
							pTex->Convert(NewFormat);
						}
					}
				}
			}

			if(*ppTex && pMtl)
			{
				D3DFORMAT fmt = (*ppTex)->PixelFormat();
				if(D3DFMT_A8R8G8B8 == fmt || D3DFMT_DXT3 == fmt) // ����ä�� �ؽ�ó�̸�..
				{
					pMtl->nRenderFlags |= RF_ALPHABLENDING;
					pMtl->dwSrcBlend = D3DBLEND_SRCALPHA;
					pMtl->dwSrcBlend = D3DBLEND_INVSRCALPHA;
				}
			}
		}
		pMtl->Diffuse.a = 1.0f;
		pMtl->Diffuse.r = pMtl->Diffuse.g = pMtl->Diffuse.b = 150.0f/255.0f;

		pMtl->Ambient.a = 1.0f;
		pMtl->Ambient.r = pMtl->Ambient.g = pMtl->Ambient.b = 150.0f/255.0f;
	}
	else
	{	// �̷��� �ؽ��İ� ������� �ϴ� ������ �ƽ������� diffuse��ſ� �ؽ��ĸ� �ִ� �����̱� ������ 
		// �ؽ��İ� �ִ°�쿡�� �׳� 1�� �ְ� �ƴѰ�쿡�� ���͸��� ������ ���� �ִ´�.
		pMtl->Diffuse.a = 1.0f;
		pMtl->Diffuse.r = pmMtl->GetDiffuse().r;
		pMtl->Diffuse.g = pmMtl->GetDiffuse().g;
		pMtl->Diffuse.b = pmMtl->GetDiffuse().b;

		pMtl->Ambient.a = 1.0f;
		pMtl->Ambient.r = pmMtl->GetAmbient().r;
		pMtl->Ambient.g = pmMtl->GetAmbient().g;
		pMtl->Ambient.b = pmMtl->GetAmbient().b;
	}



	pMtl->Specular.a = 1.0f;
	pMtl->Specular.r = pmMtl->GetSpecular().r;
	pMtl->Specular.g = pmMtl->GetSpecular().g;
	pMtl->Specular.b = pmMtl->GetSpecular().b;

	if (pmMtl->GetSelfIllumColorOn())
	{
		pMtl->Emissive.a = 1.0f;
		pMtl->Emissive.r = pmMtl->GetSelfIllumColor().r;
		pMtl->Emissive.g = pmMtl->GetSelfIllumColor().g;
		pMtl->Emissive.b = pmMtl->GetSelfIllumColor().b;
	}
	else
	{
		pMtl->Emissive.a = 1.0f;
		pMtl->Emissive.r = pMtl->Emissive.g = pMtl->Emissive.b = pmMtl->GetSelfIllum();
	}

	pMtl->Power = pmMtl->GetShinStr();
	if(!pNode->GetBackCull()) pMtl->nRenderFlags |= RF_DOUBLESIDED;	

	return true;
}

void CN3DExp::CountAllNodes(INode *pNode, int &nCount)
{
	nCount++;

	int nChildCount = pNode->NumberOfChildren();
	INode* pNodeChild = NULL;
	for(int i = 0; i < nChildCount; i++)
	{
		pNodeChild = pNode->GetChildNode(i);
		if(pNodeChild==NULL) continue;

		this->CountAllNodes(pNodeChild, nCount);
	}
}

Modifier* CN3DExp::FindPhysiqueModifier(INode* pNode)
{
	// Get object from node. Abort if no object.
	for(Object* pObj = pNode->GetObjectRef(); pObj && pObj->SuperClassID() == GEN_DERIVOB_CLASS_ID; ) // Is derived object ?
	{
		IDerivedObject *pDerObj = (IDerivedObject*)(pObj); // Yes -> Cast.
		// Iterate over all entries of the modifier stack.
		int nModStackCount = pDerObj->NumModifiers();
		for(int i = 0; i < nModStackCount; i++)
		{
			Modifier* pMod = pDerObj->GetModifier(i); // Get current modifier.
			if (pMod->ClassID() == Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B)) // Is this Physique ?
			{
				return pMod; // Yes -> Exit.
			}
		}
		pObj = pDerObj->GetObjRef();
	}
	
	return NULL; // Not found.
}

bool CN3DExp::ProcessPhysique(INode* pNode, INode* pNodeRootJoint, CN3Skin* pN3Skin)
{
    if(NULL == pNode) return false;
	if(NULL == pNodeRootJoint) return false;
	if(NULL == pN3Skin) return false;

	Modifier *pPhyMod = FindPhysiqueModifier(pNode);
    if (NULL == pPhyMod) return false;

    IPhysiqueExport *pPE = (IPhysiqueExport*)pPhyMod->GetInterface(I_PHYINTERFACE);
    if (NULL == pPE) return false;

	// create a ModContext Export Interface for the specific node of the Physique Modifier
	IPhyContextExport *pPCE = (IPhyContextExport*)pPE->GetContextInterface(pNode);
	if(NULL == pPCE)
	{
		pPhyMod->ReleaseInterface(I_PHYINTERFACE, pPE);
		return false;
	}

	Object* pObj = pNode->EvalWorldState(m_Option.nFrmStart * 160).obj; 
	int nPtCount = pObj->NumPoints(); // compute the transformed Point3 at time t
	if(nPtCount <= 0)
	{
		pPE->ReleaseContextInterface(pPCE);
		pPhyMod->ReleaseInterface(I_PHYINTERFACE, pPE);
		return false;
	}
	
	if(false == this->ProcessIMesh(pNode, pN3Skin)) return false;
	if(nPtCount != 	pN3Skin->VertexCount())
	{
		MessageBox(::GetActiveWindow(), pNode->GetName(), "Can't process physique. Because it's vertex count is different from mesh's vertex count", MB_OK);
		return false;
	}

	__VertexSkinned* pVDest = pN3Skin->SkinVertices();
	memset(pVDest, 0, sizeof(__VertexSkinned) * nPtCount);

//	pPCE->ConvertToRigid(TRUE); // we convert all vertices to Rigid in this example

	Matrix3 mMtxOrg = pNode->GetObjTMAfterWSM(m_Option.nFrmStart * 160); // ���� �޽��� ���� ���
	for (int i = 0; i < nPtCount;  i++)
	{
		IPhyVertexExport *pVE = pPCE->GetVertexInterface(i);
		if(NULL == pVE) continue;

		Point3 ptOrg = pObj->GetPoint(i);
		ptOrg = ptOrg * mMtxOrg;
//		pVDest[i].vOrigin.Set(ptOrg.x * 0.0254f, ptOrg.z * 0.0254f, ptOrg.y * 0.0254f);
		pVDest[i].vOrigin.Set(ptOrg.x, ptOrg.z, ptOrg.y);
//		pObj->SetPoint(i, ptBlendP); // Ȯ���ϱ� ���ؼ� �־��..

		int nVType = pVE->GetVertexType();
		if(nVType & BLENDED_TYPE) // Blend Type
		{
			IPhyBlendedRigidVertex *pVtxBlend = (IPhyBlendedRigidVertex*)pVE;
			
			int nWC = pVtxBlend->GetNumberNodes();
			if(nWC > 0)
			{
				pVDest[i].nAffect = nWC;
				pVDest[i].pfWeights = new float[nWC];
				pVDest[i].pnJoints = new int[nWC];

				for (int j = 0; j < nWC; j++)
				{
					int nJI = 0;
					INode *pBone = pVtxBlend->GetNode(j);
					this->FindBoneIndex(pNodeRootJoint, pBone, nJI); // �����͸� ������ �ε����� ã�´�..
					float fWeight = pVtxBlend->GetWeight(j);

					const char* szBone = pBone->GetName();

					pVDest[i].pnJoints[j] = nJI;
					pVDest[i].pfWeights[j] = fWeight;
				}
			}
		}
		else 
		{
			pVDest[i].nAffect = 1;
			pVDest[i].pnJoints = new int[1];

			IPhyRigidVertex *pVtxNoBlend = (IPhyRigidVertex*)pVE;
			INode *pBone = pVtxNoBlend->GetNode();
			int nJI = 0;
			this->FindBoneIndex(pNodeRootJoint, pBone, nJI); // �����͸� ������ �ε����� ã�´�..

			const char* szBone = pBone->GetName();

			pVDest[i].pnJoints[0] = nJI;
		}

		pPCE->ReleaseVertexInterface(pVE);
		pVE = NULL;	
	}
	
	pPE->ReleaseContextInterface(pPCE);
	pPhyMod->ReleaseInterface(I_PHYINTERFACE, pPE);

	pN3Skin->RecalcWeight();	// weight ���� 1.0�� �ǵ��� �ѹ��� �ٽ� ������ش�.

	return TRUE;
}
/*
bool CN3DExp::FindNodeIndex(INode* pNodeCompare, INode* pNodeSrc, Class_ID& cID, int& nNodeIndex)
{
	if(NULL == pNodeCompare || NULL == pNodeSrc) return false;
	Control* pCtrl = pNodeCompare->GetTMController();
	if(NULL == pCtrl) return false;
	if(pCtrl->ClassID() != cID) return false; // ������尡 �ƴϸ� �����Ѵ�..

	DWORD dwID = pNodeSrc->SuperClassID();
	DWORD dwID2 = pNodeCompare->SuperClassID();

	const char* sz1 = pNodeSrc->GetName();
	const char* sz2 = pNodeCompare->GetName();

	if(pNodeSrc == pNodeCompare)
		return true;
	else
		nNodeIndex++;

	int nCC = pNodeCompare->NumberOfChildren();
	for(int i = 0; i < nCC; i++)
	{
		INode* pNodeChild = pNodeCompare->GetChildNode(i);
		if(this->FindNodeIndex(pNodeChild, pNodeSrc, cID, nNodeIndex) == true) return true;
	}

	return false;
}
*/
bool CN3DExp::FindBoneIndex(INode* pNodeCompare, INode* pNodeSrc, int& nNodeIndex)
{
	if(NULL == pNodeCompare || NULL == pNodeSrc) return false;
	if(false == IsBone(pNodeCompare)) return false; // ���� �ƴϸ� �����Ѵ�..

	DWORD dwID = pNodeSrc->SuperClassID();
	DWORD dwID2 = pNodeCompare->SuperClassID();

	const char* sz1 = pNodeSrc->GetName();
	const char* sz2 = pNodeCompare->GetName();

	if(pNodeSrc == pNodeCompare)
		return true;
	else
		nNodeIndex++;

	int nCC = pNodeCompare->NumberOfChildren();
	for(int i = 0; i < nCC; i++)
	{
		INode* pNodeChild = pNodeCompare->GetChildNode(i);
		if(this->FindBoneIndex(pNodeChild, pNodeSrc, nNodeIndex) == true) return true;
	}

	return false;
}
