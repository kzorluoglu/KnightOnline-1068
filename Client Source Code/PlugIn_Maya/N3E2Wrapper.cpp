// Copyright (C) 1998-2000 Alias | Wavefront, a division of Silicon Graphics Limited.//
// The information in this file is provided for the exclusive use of the
// licensees of Alias | Wavefront.  Such users have the right to use, modify,
// and incorporate this code into other products for purposes authorized
// by the Alias | Wavefront license agreement, without fee.
//
// ALIAS | WAVEFRONT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
// INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
// EVENT SHALL ALIAS | WAVEFRONT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
// CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
// DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

// $Revision: /main/24 $
// $Date: 2000/04/19 16:12:24 $
//
//
// Description: 
//            
// N3E2 translator for Maya    
// 
//

#ifdef WIN32
#include <process.h>
#endif

#ifdef WIN32
#define MAXPATHLEN 1024
#endif

#include <crtdbg.h>
#include "N3E2Wrapper.h"
#include "IFFReader.h"

#include "../N3Base/N3Eng.h"
#include "../N3Base/N3Scene.h"
#include "../N3Base/N3PMeshCreate.h"

extern const char * objectName( MObject object );


#include "CommCtrl.h"
#include "Resource.h"

BOOL CN3E2Wrapper::m_bCancelExport = FALSE;
__EXPORT_OPTION CN3E2Wrapper::m_Option;

CN3EngTool*		g_pEng;

// CN3E2Wrapper class methods:
CN3E2Wrapper::CN3E2Wrapper()
{
	// ���� ��Ȳ ��ȭ����
	HWND hWnd = M3dView::applicationShell();
	HINSTANCE hInst = MhInstPlugin;
	m_hDlgProgress = CreateDialog(hInst, MAKEINTRESOURCE(IDD_PROGRESS), hWnd, CN3E2Wrapper::DlgProcProgress);
	__ASSERT(m_hDlgProgress, "Failed to create progress dialog");
	m_hWndPB = GetDlgItem(m_hDlgProgress, IDC_PROGRESS);
	m_hWndLB = GetDlgItem(m_hDlgProgress, IDC_LIST_PROGRESS);

	m_pScene = NULL;
	g_pEng = NULL;

	this->Release();
}

CN3E2Wrapper::~CN3E2Wrapper()
{
	::DestroyWindow(m_hDlgProgress); // ���� ���� ���ֱ�..
	this->Release();
}

void CN3E2Wrapper::Release()
{
	memset(&m_Option, 0, sizeof(m_Option));

	m_Option.bExportCamera		= TRUE;
	m_Option.bExportLight		= TRUE;
	m_Option.bExportGeometry	= TRUE;
	m_Option.bExportDummy		= FALSE;
	m_Option.bExportCharacter	= TRUE;

	m_Option.bAnimationKey	= TRUE;
	
	m_Option.bGenerateFileName = FALSE;
	m_Option.bGenerateSmoothNormal = TRUE;
//	m_Option.bGenerateProgressiveMesh = FALSE;
	m_Option.bGenerateHalfSizeTexture = FALSE;
	m_Option.bGenerateCompressedTexture = TRUE;
	m_Option.fSamplingRate = 30.0f;

	lstrcpy(m_szPath, "");		// ��� �̸�
	lstrcpy(m_szFileName, "");	// ���� �̸�

	delete m_pScene; m_pScene = NULL;
	delete g_pEng; g_pEng = NULL;
}

void CN3E2Wrapper::SetFileName(const char *szFileName)
{
	lstrcpy(m_szFileName, szFileName);	// ���� �̸�
}

void CN3E2Wrapper::SetPath(const char *szPath)
{
	lstrcpy(m_szPath, szPath);	// ���� �̸�
}

// ����Ʈ ������ ����.
CN3Light* CN3E2Wrapper::ProcessLight(MFnLight &mLight)
{
	CN3Light* pLight = new CN3Light();
	MFnTransform mT = MFnTransform(mLight.parent(0));
	this->ProcessTransform(mT, pLight); // Transform Node

	pLight->m_szName = mT.name().asChar();
	pLight->FileNameSet("Data\\" + pLight->m_szName + ".N3Light"); // ���� �̸� ����..

	// ����Ʈ ����
	D3DCOLORVALUE dcv = {1,1,1,1};
	pLight->m_Data.InitPoint(m_pScene->LightCount(), __Vector3(0,0,0), dcv);
	MFn::Type Type = mLight.type();
	if(Type == MFn::kDirectionalLight)
	{
		pLight->m_Data.Type = D3DLIGHT_DIRECTIONAL; // a directional light
	}
	else if(Type == MFn::kSpotLight) // a sopt light
	{
		pLight->m_Data.Type = D3DLIGHT_SPOT;
		
		MFnSpotLight Spot(mLight.object());
		pLight->m_Data.Theta = (float)Spot.coneAngle(); // ���� ���԰�
		pLight->m_Data.Phi = (float)Spot.penumbraAngle(); // �ܺ� ���԰�
	}
	else // �⺻������ point light
	{
		pLight->m_Data.Type = D3DLIGHT_POINT;
	}

	pLight->m_Data.Position = pLight->Pos();

	MFloatVector dir = mLight.lightDirection(); // ����
	pLight->m_Data.Direction.x = dir.x;
	pLight->m_Data.Direction.y = dir.y;
	pLight->m_Data.Direction.z = -dir.z;

	MColor color = mLight.color(); // ����Ʈ �÷�.

	pLight->m_Data.Ambient.a = 1.0f;
	pLight->m_Data.Ambient.r = color.r * 0.3f;
	pLight->m_Data.Ambient.g = color.g * 0.3f;
	pLight->m_Data.Ambient.b = color.b * 0.3f;

	pLight->m_Data.Diffuse.a = 1.0f;
	pLight->m_Data.Diffuse.r = color.r;
	pLight->m_Data.Diffuse.g = color.g;
	pLight->m_Data.Diffuse.b = color.b;

/*	MFnDependencyNode lightDG(mLight);
    MObject lightAttr = lightDG.attribute(MString("intensity"));
	MPlug plug(mLight, lightAttr); // ����
	double intensity;
	plug.getValue(intensity);
	
	lightAttr = lightDG.attribute(MString("specular"));
	plug.setAttribute(lightAttr);
	plug.getValue(
*/

	// ������ �⺻��.
	pLight->m_Data.nNumber = m_pScene->LightCount();
	pLight->m_Data.bOn = TRUE;

	return pLight;
}

CN3Camera* CN3E2Wrapper::ProcessCamera(MFnCamera &mCamera)
{
	CN3Camera* pCamera = new CN3Camera();
	MFnTransform mT(mCamera.parent(0));
	this->ProcessTransform(mT, pCamera); // Transform ó��..
	std::string szFN = "Data\\" + pCamera->m_szName + ".N3Camera";
	pCamera->FileNameSet(szFN);

//	double dHFOV, dVFOV;
//	mCamera.getPortFieldOfView(800, 600, dHFOV, dVFOV);
//	pCamera->s_CameraData.fNP = (float)(mCamera.nearClippingPlane() * 0.01); // meter ���� �̱⶧���� 100 ���� ������ �ش�.
//	pCamera->s_CameraData.fFP = (float)(mCamera.farClippingPlane() * 0.01);
//	pCamera->s_CameraData.fFOV = (float)dHFOV; // 90 �� ����� ���� ����..

	return pCamera;
}

MObject CN3E2Wrapper::MeshGetShader(MFnMesh& mMesh)
{
	MObjectArray ShaderArray; MIntArray IndicesArray;
	mMesh.getConnectedShaders(0, ShaderArray, IndicesArray);
//	int nInstance = PathCur.instanceNumber();
//	Mesh.getConnectedShaders(nInstance, ShaderArray, IndicesArray);

	int nSC = ShaderArray.length();
	int nIC = IndicesArray.length();
	if(nSC > 0)
	{
		MFnDependencyNode NodeShader(ShaderArray[0]);
		MObject ssAttr = NodeShader.attribute(MString("surfaceShader"));
		MPlug Plug(ShaderArray[0], ssAttr);
		MPlugArray srcPlugArray;
		Plug.connectedTo( srcPlugArray, true, false); // as destination, as source

		if(srcPlugArray.length() > 0) 
			return srcPlugArray[0].node();
	}

	MObject nullObj;
	return nullObj;
}

void CN3E2Wrapper::SceneExport()
{
	// Option Dialog 
	HINSTANCE hInst = MhInstPlugin;
	HWND hWnd = M3dView::applicationShell();
	int rval = DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_EXPORT_OPTION), hWnd, DlgProcPane, NULL); // ��Ʈ�� �г� ��ȭ����..
	if(rval != 1) return;
	
	g_pEng = new CN3EngTool();
//	g_pEng->InitEnv();
	g_pEng->Init(TRUE, hWnd, 64, 64, 0, FALSE);
	
	char szPath[256];
	::GetCurrentDirectory(256, szPath);
	g_pEng->PathSet(szPath); // ��� ����...

	m_pScene = new CN3Scene();

	if(NULL == g_pEng->s_lpD3DDev)
	{
		delete m_pScene; m_pScene = NULL;
		delete g_pEng; g_pEng = NULL;

		MessageBox(::GetActiveWindow(), "VideoMemory �� �����մϴ�. VideoMemory Ȯ�� �� �ٽ� �õ��ϼ���.", "Data Export Error", MB_OK);
		return;
	}

	MTime mOldTime = MAnimControl::currentTime(); // ���� �������� �� ������°���� ����..
	MTime ZeroFrm; 
	ZeroFrm.setValue(0); // ZeroFrm.setUnit(MTime::kNTSCFrame);
	MAnimControl::setCurrentTime(ZeroFrm);
	MGlobal::viewFrame(ZeroFrm);
	
	int nLI = 0;
	DWORD dwRWC = 0;
	char szInfo[1024] = "";

	m_pScene->Release(); // ��� �����ϰ�..

//	m_pScene->m_fFrmCur = (float)MAnimControl::currentTime().value();
	m_pScene->m_fFrmStart = (float)m_Option.nFrmStart; // (float)MAnimControl::minTime().value();
	m_pScene->m_fFrmEnd = (float)m_Option.nFrmEnd; // (float)MAnimControl::maxTime().value();

	MObjectArray mObjects;

	int i = 0;

	MItDependencyNodes IterNodes1(MFn::kCamera); // scene �� ������ üũ..
	MItDependencyNodes IterNodes2(MFn::kLight); // scene �� ������ üũ..
	MItDependencyNodes IterNodes3(MFn::kMesh); // scene �� ������ üũ..
	MItDependencyNodes IterNodes4(MFn::kSkinClusterFilter); // scene �� ������ üũ..

	for(i = 0; !IterNodes1.isDone(); IterNodes1.next(), i++) mObjects.append(IterNodes1.item());
	for(i = 0; !IterNodes2.isDone(); IterNodes2.next(), i++) mObjects.append(IterNodes2.item());
	for(i = 0; !IterNodes3.isDone(); IterNodes3.next(), i++) mObjects.append(IterNodes3.item());
	for(i = 0; !IterNodes4.isDone(); IterNodes4.next(), i++) mObjects.append(IterNodes4.item());

	MSelectionList mSelList;
	if(m_Option.bExportSelectedOnly)
	{
		MGlobal::getActiveSelectionList(mSelList);
	}

	// Object Count... 
	int nObjectCount = mObjects.length();

	// ���� ��Ȳ ��ȭ���� ��Ʈ�� �ڵ�
	::SendMessage(m_hWndPB, PBM_SETRANGE, 0, MAKELPARAM(0, nObjectCount));
	::SendMessage(m_hWndLB, LB_RESETCONTENT, 0, 0);
	::ShowWindow(m_hDlgProgress, SW_SHOW); // ���� ��ȭ���ڸ� ���̰�..
	::ShowWindow(m_hDlgProgress, SW_RESTORE);

	m_bCancelExport = FALSE;

	for (i = 0; i < nObjectCount && FALSE == m_bCancelExport; i++)
	{
		// Dialog Message ó��...
		MSG msg;
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }

		if(TRUE == m_bCancelExport) break; // Export Cancel ��ư�� ������ ���..

		MObject mObj = mObjects[i];
		MFn::Type mType = mObj.apiType();

		if(m_Option.bExportSelectedOnly && IsSelected(mSelList, mObj) == false) continue; // ���õ� �͸� Export �ؾ� �ϸ�..

		bool bExport = true;
		if(MFn::kCamera == mType && TRUE == m_Option.bExportCamera) // ī�޶�..
		{
			MFnCamera mCamera(mObj);
			const char* szCamera = mCamera.name().asChar();
			if(	strstr(szCamera, "front") == NULL && 
				strstr(szCamera, "side") == NULL && 
				strstr(szCamera, "top") == NULL)
			{
				CN3Camera* pCamera = this->ProcessCamera(mCamera);
				m_pScene->CameraAdd(pCamera);
			}
		}
		else if(	(MFn::kPointLight == mType ||
					MFn::kDirectionalLight == mType ||
					MFn::kAmbientLight == mType ||
					MFn::kSpotLight == mType) && 
					TRUE == m_Option.bExportLight )// ����Ʈ
		{
			CN3Light* pLight = this->ProcessLight(MFnLight(mObj));
			pLight->m_Data.nNumber = m_pScene->LightCount(); // ����Ʈ ��ȣ ���̱�..
			m_pScene->LightAdd(pLight); // Scene �� ����Ʈ �߰�..
		}
		else if(mType == MFn::kMesh && TRUE == m_Option.bExportGeometry)
		{
			MFnMesh mMesh(mObj);

			const char* szMeshName = mMesh.name().asChar();

			bool bHaveJoint = false; // ���� ���븦 ó���ؾ� �ϸ�..
			if(TRUE == m_Option.bExportCharacter) bHaveJoint = this->HaveJoint(mMesh);
			if(true == bHaveJoint) // ������ ����� �޽ø� ��������..
			{
//				wsprintf(szInfo, "Skinning �� �� �޽�(%s)�� �����մϴ�.", mMesh.name().asChar());
//				nLI = ::SendMessage(m_hWndLB, LB_ADDSTRING, 0, (LPARAM)szInfo); // Progress dialog
//				::SendMessage(m_hWndLB, LB_SETCURSEL, (WPARAM)nLI,0); // Progress dialog
				continue; 
			}
			else
			{
				bool bProcessShape = false;
				if(mMesh.parentCount() > 0 && mMesh.parent(0).apiType() == MFn::kTransform)
				{
					MFnTransform mTM(mMesh.parent(0));
					if(mTM.childCount() == 1)  bProcessShape = true; // ù��° �ڽ��� ��츸 ó��..
					else if(mTM.childCount() > 1 && mTM.child(0) == mMesh.object()) bProcessShape = true; // ù��° �ڽ��� ��츸 ó��..
				}
				if(bProcessShape) ProcessShape(mMesh);
			}
		}
		else if(mType == MFn::kSkinClusterFilter && m_Option.bExportCharacter) // ��Ű�� ������ �� ĳ����....
		{
			MFnSkinCluster mSkin(mObj);
			this->ProcessChr(mSkin);
		}
		else
		{
			wsprintf(szInfo, "���(%s) �����մϴ�.", mObj.apiTypeStr());
			nLI = ::SendMessage(m_hWndLB, LB_ADDSTRING, 0, (LPARAM)szInfo); // Progress dialog
			::SendMessage(m_hWndLB, LB_SETCURSEL, (WPARAM)nLI,0); // Progress dialog

			bExport = false;
		}

		if(bExport == true)
		{
			if(mObj.hasFn(MFn::kDependencyNode) == true)
			{
				MFnDependencyNode node(mObj);
				wsprintf(szInfo, "%d/%d - Type : %s Name : %s", i, nObjectCount, mObj.apiTypeStr(), node.name().asChar());
			}
			else
			{
				wsprintf(szInfo, "%d/%d - Type : %s NoName", i, nObjectCount, mObj.apiTypeStr());
			}
			nLI = ::SendMessage(m_hWndLB, LB_ADDSTRING, 0, (LPARAM)szInfo); // Progress dialog
			::SendMessage(m_hWndLB, LB_SETCURSEL, (WPARAM)nLI,0); // Progress dialog
			SendMessage(m_hWndPB, PBM_SETPOS, i+1, 0); // Progress dialog
		}
	}

	// Scene ����.
	bool bSaveOK = true;
	if(m_bCancelExport == TRUE)
	{
		bSaveOK = false;
		int nYesNo = MessageBox(hWnd, "File ����", "Export�� ����ϼ̽��ϴ�. �����Ͻðڽ��ϱ�?", MB_YESNO);
		if(nYesNo == IDYES) bSaveOK = true;
	}
	
	if(bSaveOK)
	{
		m_pScene->Tick(0);
		if(m_pScene->CameraCount() <= 0) m_pScene->DefaultCameraAdd(); // �⺻ ī�޶�
		if(m_pScene->LightCount() <= 0) m_pScene->DefaultLightAdd(); // �⺻ ����Ʈ �߰�..

		m_pScene->SaveDataAndResourcesToFile(m_szFileName); // Scene , Resource ����..
		m_pScene->Release();
	}

	MAnimControl::setCurrentTime(mOldTime); // ���� ���..
	MGlobal::viewFrame(mOldTime);

	delete m_pScene; m_pScene = NULL;
	delete g_pEng; g_pEng = NULL;
}

bool CN3E2Wrapper::ProcessIMesh(MFnMesh &mMesh, CN3IMesh* pIMesh)
{
	if(NULL == pIMesh) return false;
	pIMesh->Release();

	// �̸� ����..
	MFnTransform mTM(mMesh.parent(0));
	pIMesh->m_szName = mTM.name().asChar();

	// Polygon �� ��� �ﰢ�� �޽÷� ����� ������ �����ش�..
	int nPC = mMesh.numPolygons();
	if(nPC <= 0) return false;

	int nFC = 0, nVI = 0, nUVI = 0;

	MIntArray mVIArray, mVITmp, mUVIArray;
	MFloatArray mFAU, mFAV;
	MPointArray mVArray;
	mMesh.getUVs(mFAU, mFAV);
	int nUVC = mFAU.length(); // UV ��ǥ Count
	mMesh.getPoints(mVArray);
	int nVC = mVArray.length(); // Vertex Count
	
	std::vector<MVector> Normals(nVC, MVector());
	std::vector<int> iNormalCounts(nVC, 0);

	for(int i = 0; i < nPC; i++)
	{
		MVector vNTmp;
		mMesh.getPolygonNormal(i, vNTmp); // normal
		mMesh.getPolygonVertices(i, mVITmp); // polygon �� �ִ� �� Index

		mMesh.getPolygonUVid(i, 0, nUVI);
		nVI = mVITmp[0];
		Normals[nVI] += vNTmp; iNormalCounts[nVI]++; // ���� Normal �� �߰�.
		mVIArray.append(nVI); // Vertex Index
		mUVIArray.append(nUVI); // VertexIndex, UV Index ���..

		mMesh.getPolygonUVid(i, 1, nUVI);
		nVI = mVITmp[1];
		Normals[nVI] += vNTmp; iNormalCounts[nVI]++; // ���� Normal �� �߰�.
		mVIArray.append(nVI); // Vertex Index
		mUVIArray.append(nUVI); // VertexIndex, UV Index ���..

		mMesh.getPolygonUVid(i, 2, nUVI);
		nVI = mVITmp[2];
		Normals[nVI] += vNTmp; iNormalCounts[nVI]++; // ���� Normal �� �߰�.
		mVIArray.append(nVI); // Vertex Index
		mUVIArray.append(nUVI); // VertexIndex, UV Index ���..

		nFC++;

		int nVITmp = mVITmp.length();
		for(int j = 3; j < nVITmp; j++) // �Ѹ鿡 ���� �װ� �̻� �ִ� �Ÿ�..
		{
			mMesh.getPolygonUVid(i, 0, nUVI);
			nVI = mVITmp[0];
			Normals[nVI] += vNTmp; iNormalCounts[nVI]++; // ���� Normal �� �߰�.
			mVIArray.append(nVI); // Vertex Index
			mUVIArray.append(nUVI); // VertexIndex, UV Index ���..

			mMesh.getPolygonUVid(i, nVITmp - j + 1, nUVI);
			nVI = mVITmp[nVITmp - j + 1];
			Normals[nVI] += vNTmp; iNormalCounts[nVI]++; // ���� Normal �� �߰�.
			mVIArray.append(nVI); // Vertex Index
			mUVIArray.append(nUVI); // VertexIndex, UV Index ���..

			mMesh.getPolygonUVid(i, nVITmp - j + 2, nUVI);
			nVI = mVITmp[nVITmp - j + 2];
			Normals[nVI] += vNTmp; iNormalCounts[nVI]++; // ���� Normal �� �߰�.
			mVIArray.append(nVI); // Vertex Index
			mUVIArray.append(nUVI); // VertexIndex, UV Index ���..

			nFC++;
		}
	}

	pIMesh->Create(nFC, nVC, nUVC); // �޽� ����..

	MFnTransform mTransform(mMesh.parent(0));
	MMatrix mMtxWorld; mMtxWorld.setToIdentity();
	this->GetWorldTransform(mTransform, mMtxWorld);
	MMatrix mMtxWorldRot = mMtxWorld; // ȸ����ĸ� ����..
	mMtxWorldRot.matrix[3][0] = mMtxWorldRot.matrix[3][1] = mMtxWorldRot.matrix[3][2] = 0;
	mMtxWorldRot.matrix[3][3] = 1.0;
	
	__VertexXyzNormal* pVDest = pIMesh->Vertices();
	for(i = 0; i < nVC; i++) 
	{
		MPoint mVTmp = mVArray[i]; // World Matrix ���ϰ�..
		mVTmp *= mMtxWorld;
		pVDest[i].x = (float)(mVTmp.x * 0.01); // Vertex Data ���� - meter ���� �̱� ������ 100���� ������ �ش�.
		pVDest[i].y = (float)(mVTmp.y * 0.01);
		pVDest[i].z = -(float)(mVTmp.z * 0.01); // Z ���� �ݴ�� �Ѵ�.

		// Normal �� ó��...
//		if(iNormalCounts[i] > 0)
//		{
//			mNTmp.x = mNTmp.x / iNormalCounts[i];
//			mNTmp.y = mNTmp.y / iNormalCounts[i];
//			mNTmp.z = mNTmp.z / iNormalCounts[i];
//		}
		
		MVector mNTmp = Normals[i] * mMtxWorldRot;
		mNTmp.normalize();
		pVDest[i].n.x = (float)mNTmp.x;
		pVDest[i].n.y = (float)mNTmp.y;
		pVDest[i].n.z = -(float)mNTmp.z; // Z ���� �ݴ�� �Ѵ�.

	}

	for(i = 0; i < nUVC; i++) pIMesh->UVSet(i, mFAU[i], 1.0f - mFAV[i]); // UV ������ ����..
//	for(i = 0; i < nUVC; i++) pIMesh->UVSet(i, mFAU[i], mFAV[i]); // UV ������ ����..
	for(i = 0; i < nFC; i++) // 
	{
		pIMesh->VertexIndexSet(i*3+0, mVIArray[i*3+0]); // Vertex Index ����
		pIMesh->VertexIndexSet(i*3+1, mVIArray[i*3+2]);
		pIMesh->VertexIndexSet(i*3+2, mVIArray[i*3+1]);

		if(nUVC > 0) // UV Index �� �־�� �Ѵ�..
		{
			pIMesh->UVIndexSet(i*3+0, mUVIArray[i*3+0]); // UV Index ����..
			pIMesh->UVIndexSet(i*3+1, mUVIArray[i*3+2]);
			pIMesh->UVIndexSet(i*3+2, mUVIArray[i*3+1]);
		}
	}

	if(m_Option.bGenerateSmoothNormal)
	{
		pIMesh->ReGenerateSmoothNormal();
	}

	return true;
}

bool CN3E2Wrapper::ProcessVMesh(MFnMesh &mMesh, CN3VMesh* pVMesh)
{
	if(NULL == pVMesh) return false;
	pVMesh->Release();

	// Polygon �� ��� �ﰢ�� �޽÷� ����� ������ �����ش�..
	int nPC = mMesh.numPolygons();
	if(nPC <= 0) return false;

	pVMesh->CreateVertices(nPC*3);
	__Vector3* pvDests = pVMesh->Vertices();

	MIntArray mVIs;
	MPointArray mVs; mMesh.getPoints(mVs); // ��ġ��..
	MPoint mPosTmp;

	MFnTransform mTransform(mMesh.parent(0)); // ���� ���
	MMatrix mMtxWorld; mMtxWorld.setToIdentity();
	this->GetWorldTransform(mTransform, mMtxWorld);
	
	for(int i = 0; i < nPC; i++)
	{
		mMesh.getPolygonVertices(i, mVIs); // polygon �� �ִ� �� Index
		if(mVIs.length() != 3)
		{
			char szErr[256];
			wsprintf(szErr, "%s �� �ﰢ �������� �ƴմϴ�.", mMesh.name().asChar());
			break;
		}

		for(int j = 0; j < 3; j++)
		{
			mPosTmp = mVs[mVIs[j]];
			mPosTmp *= mMtxWorld;

			pvDests[i*3+j].Set(	(float)(mPosTmp.x*0.01),
								(float)(mPosTmp.y*0.01),
								-(float)(mPosTmp.z*0.01) );
		}
	}

	// �̸� ó��..
	pVMesh->m_szName = "";
	this->ProcessName(mTransform.object(), pVMesh->m_szName);
	std::string szFN = pVMesh->m_szName + ".N3VMesh";
	pVMesh->FileNameSet(szFN);

	return true;
}

bool CN3E2Wrapper::ProcessSkin(MFnSkinCluster &mSkin, CN3Skin* pSkin)
{
	if(NULL == pSkin) return false;
	pSkin->Release();

	int nLI = 0;
	DWORD dwRWC = 0;
	char szInfo[1024] = "";

	MObjectArray mMeshArray;
	mSkin.getOutputGeometry(mMeshArray);
	__ASSERT(mMeshArray.length() >= 1, "Input geometry count is not 1");
	MFnMesh mMeshOutput(mMeshArray[0]); // Original Mesh

	MStatus stat;
	MDagPath mSkinPath;
	int nGI = mSkin.indexForOutputConnection(0, &stat); // Geometry Index
	mSkin.getPathAtIndex(nGI, mSkinPath); // Geometry Path
	MItGeometry mGIt(mSkinPath); // iterate through the components of this geometry

	int nGVC = mGIt.count(); // Geometry Data
	MPointArray mVArray;
	mMeshOutput.getPoints(mVArray);
	int nVC = mVArray.length();
	if(nGVC != nVC) 
	{
		char szWarning[256];
		MFnDagNode nodeTmp(mMeshOutput.parent(0));
		wsprintf(szWarning, "Mesh - %s, Skin - %s", nodeTmp.name().asChar(), mSkin.name().asChar());
		MessageBox(::GetActiveWindow(), szWarning, "Warning - Skin vertex count is different to mesh vertex count", MB_OK);
	}











	if(false == this->ProcessIMesh(mMeshOutput, pSkin)) return false; // Indexed Mesh ó��.

	// Joint �� �׷� �Ǿ� �ִٸ� Parent Transform ó��..
/*	if(mJointRoot.parentCount() > 0 && mJointRoot.parent(0).hasFn(MFn::kTransform))
	{
		MFnTransform mTJP(mJointRoot.parent(0));

		// Normal �� �ٽ� ����..
		MMatrix mMtx; mMtx.setToIdentity();
		this->GetWorldTransform(mTJP, mMtx);
		MTransformationMatrix mTMtx(mMtx);

		double dRots[3];
		MTransformationMatrix::RotationOrder rotOrder = MTransformationMatrix::kXYZ;
		mTMtx.getRotation(dRots, rotOrder);
		__Matrix44 mtxRot;
		mtxRot.Rotation((float)dRots[0], (float)dRots[1], -(float)dRots[2]);

		int nVC = pIMesh->VertexCount();
		__VertexXyzNormal* pVs = pIMesh->Vertices();
		__Vector3 vNTmp;
		
		for(int i = 0; i < nVC; i++)
		{
			vNTmp = pVs[i].n;
			vNTmp *= mtxRot;
			pVs[i].n = vNTmp;
		}
	}
*/


	__VertexSkinned* pVBone = pSkin->SkinVertices();
	MFnTransform mTM(mMeshOutput.parent(0)); // Mesh, Joint �� ����� Transform
	MMatrix mMtxM; mMtxM.setToIdentity();
	this->GetWorldTransform(mTM, mMtxM);

	int iFind = pSkin->m_szName.find('_');
	if(iFind > 0) pSkin->m_szName = pSkin->m_szName.substr(iFind + 1); // ����ٰ� ������ �߶��ش�..
	pSkin->FileNameSet("Item\\" + pSkin->m_szName + ".N3Skin"); // ���� �̸� ����..

	for(int i = 0; !mGIt.isDone(); mGIt.next(), i++) // ������ ���鼭 ������Ʈ(Geometry �� ����...)�� ó���Ѵ�.
	{
		MObject mComp = mGIt.component();
		MFn::Type mType = mComp.apiType();

		// Weight ���� ���Ѵ�.
		MFloatArray mWTs;
		unsigned int nWTs;
		mSkin.getWeights(mSkinPath, mComp, mWTs, nWTs);
		if (nWTs == 0)
		{
			lstrcpy(szInfo, "Skin ó�� ���� : Weight ���� �����ϴ�.");
			nLI = ::SendMessage(m_hWndLB, LB_ADDSTRING, 0, (LPARAM)szInfo); // Progress dialog
			::SendMessage(m_hWndLB, LB_SETCURSEL, (WPARAM)nLI,0); // Progress dialog

			continue;
		}

		MPoint mPt;
		mPt = mGIt.position();
//		mPt = mVArray[j]; // ���� �̰� ����������.. ���ε� ����� ���ư��� �Ѵ�..

		mPt *= mMtxM; // ����� �����ְ�..
		mPt.x *= 0.01; mPt.y *= 0.01; mPt.z *= -0.01; // Z ���� ������..
		
		pVBone[i].vOrigin.Set((float)mPt.x, (float)mPt.y, (float)mPt.z);

		
		// Weight ���� 0�̻��� �͸� ������..
		int nAffect = 0;
		static int s_nJoints[128];
		static float s_fWeights[128];
		for (int j = 0; j < (int)nWTs ; j++ )
		{
			if(mWTs[j] > 0) 
			{
				s_nJoints[nAffect] = j;
				s_fWeights[nAffect] = mWTs[j];
				nAffect++;
			}
		}

		if(nAffect > 0)
		{
			pVBone[i].nAffect = nAffect;
			pVBone[i].pnJoints = new int[nAffect];
			if(nAffect > 1) pVBone[i].pfWeights = new float[nAffect];

			for (j = 0; j < nAffect; j++ )
			{
				pVBone[i].pnJoints[j] = s_nJoints[j];
				if(nAffect > 1) pVBone[i].pfWeights[j] = s_fWeights[j];
			}
		}
	}

	return true;
}

bool CN3E2Wrapper::ProcessShape(MFnMesh& mMesh)
{
	if(mMesh.numPolygons() > 10000) // ���� ������ ���� �Ѿ��... CN3IMesh �� ó�� �Ұ���...
	{
		CN3VMesh VMesh;
		this->ProcessVMesh(mMesh, &VMesh);
		VMesh.SaveToFile();
		return false;
	}

	int nLI = 0;
	DWORD dwRWC = 0;
	char szInfo[1024] = "";

	MFnDagNode mNodeTmp(mMesh.object());
	MFnTransform mTG;
	bool bValid = false;
	while(1)
	{
		if(mNodeTmp.parent(0).apiType() == MFn::kWorld)
		{
			mTG.setObject(mNodeTmp.object());
			bValid = true;
			break;
		}

		mNodeTmp.setObject(mNodeTmp.parent(0));
	}
	if(false == bValid) return false;

	MFnTransform mTM(mMesh.parent(0));

	std::string szNodeFullName;
	this->ProcessName(mTG.object(), szNodeFullName);
	int nSC = m_pScene->ShapeCount();
	CN3Shape* pShape = NULL;
	for(int i = 0; i < nSC; i++)
	{
		if(szNodeFullName == m_pScene->ShapeGet(i)->m_szName)
		{
			pShape = m_pScene->ShapeGet(i);
			break;
		}
	}

	if(NULL == pShape)
	{
		pShape = new CN3Shape();
		this->ProcessTransform(mTG, pShape);
		pShape->m_szName = ""; this->ProcessName(mTG.object(), pShape->m_szName); // �̸��� �ٽ����ϱ�.
		pShape->FileNameSet("Object\\" + pShape->m_szName + ".N3Shape"); // ���� �̸� ���ϱ�..
		
		m_pScene->ShapeAdd(pShape);

		MMatrix mMtxJWorld; mMtxJWorld.setToIdentity();
		this->GetWorldTransform(mTG, mMtxJWorld);

		// �ǹ��� ���
		MPoint mvPivot;
		mvPivot = mTG.rotatePivot(MSpace::kTransform);
		pShape->PosSet((float)(mvPivot.x*0.01), (float)(mvPivot.y*0.01), -(float)(mvPivot.z*0.01));
	}

	CN3IMesh IMesh;
	this->ProcessIMesh(mMesh, &IMesh);
	if(IMesh.FaceCount() <= 0) return false;

	// �̸��� "collision" �̶�� ���ڿ��� ����.. �浹 �޽ô�..
	std::string szTmp = IMesh.m_szName;
	if(szTmp.size()) CharLower(&(szTmp[0]));
	if(szTmp.find("coll") != -1 || szTmp.find("climb") != -1)
	{
		// �޽��� ����ġ�� �ǹ����� ���� �ٽ� ���.. Shape �� ���� ��ǥ�� ���߾� �ش�..
		IMesh.ApplyOffset(pShape->Pos() * -1.0f);
		int nFC = IMesh.FaceCount();

		CN3VMesh* pVMesh = new CN3VMesh();
		szNodeFullName = ""; this->ProcessName(mTM.object(), szNodeFullName);
		pVMesh->m_szName = mTM.name().asChar(); // �̸� �ٽ� ���ϰ�..
		pVMesh->FileNameSet("Object\\" + szNodeFullName + ".N3VMesh"); // ���� �̸� ����..

		pVMesh->CreateVertices(nFC*3);

		__VertexT1* pVSrc = IMesh.BuildVertexList();
		__Vector3* pVDest = pVMesh->Vertices();
		for(i = 0; i < nFC * 3; i++)
		{
			pVDest[i].x = pVSrc[i].x; // ��ġ�� ������ �ش�...
			pVDest[i].y = pVSrc[i].y; // ��ġ�� ������ �ش�...
			pVDest[i].z = pVSrc[i].z; // ��ġ�� ������ �ش�...
		}
		pShape->s_MngVMesh.Add(pVMesh);
		if(szTmp.find("coll") != -1) pShape->CollisionMeshSet(pVMesh->FileName());
		else if(szTmp.find("climb") != -1) pShape->ClimbMeshSet(pVMesh->FileName());
	}
	else // �浹 �޽ð� �ƴϸ�..
	{
		CN3SPart* pPD = pShape->PartAdd(); // Part �߰� ���ְ�..
		szNodeFullName = ""; this->ProcessName(mTM.object(), szNodeFullName);
		pPD->m_szName = mTM.name().asChar(); // Part �̸� ����..
		pPD->FileNameSet("Object\\" + szNodeFullName + "N3CPart");

		// �ǹ��� ���
		MPoint mvPivot = mTM.rotatePivot(MSpace::kTransform);
		pPD->m_vPivot.Set((float)(mvPivot.x*0.01), (float)(mvPivot.y*0.01), -(float)(mvPivot.z*0.01));
		pPD->m_vPivot -= pShape->Pos();

		// �޽��� ����ġ�� Shape ��Ʈ�� �ǹ����� ���� �ٽ� ���.. Shape�� part ���� ��ǥ�� ���߾� �ش�..
		__Vector3 vOffset((float)(mvPivot.x * 0.01), (float)(mvPivot.y * 0.01), -(float)(mvPivot.z * 0.01));
		IMesh.ApplyOffset(vOffset * -1.0f);

		// CN3Mesh �� Convert
		CN3Mesh N3Mesh;
		N3Mesh.Import(&IMesh);
		N3Mesh.MakeIndexed();
		if(m_Option.bGenerateSmoothNormal)
		{
			N3Mesh.ReGenerateSmoothNormal();
		}

		// Progressive Mesh ����..
		CN3PMeshCreate PMeshCreate;
		if (PMeshCreate.ConvertFromN3Mesh(&N3Mesh)==false)
		{
			MessageBox(::GetActiveWindow(), "��ȯ�� �� ���ų� ������ ���� N3Mesh�Դϴ�.", "PMesh Convert error", MB_OK);
			//return;
		}

		// �ڵ� PMesh ���� �ɼ�
		PMeshCreate.m_PMCOption.bUseSumOfLoss = true;	// Loss�� ���� ���
		PMeshCreate.m_PMCOption.bTriangleWeight = true;	// ������� �ﰢ�� ����ġ ���
		PMeshCreate.m_PMCOption.bArea = true;	// false : ������ ������ ������ �Ѵ�.
		PMeshCreate.m_PMCOption.fWeight = 1.0f;	// ������� �ﰢ�� ����ġ (�߿䵵)

		CN3PMesh *pPMesh = PMeshCreate.CreateRendererMesh(); // Progressive Mesh ����
		pPMesh->m_szName = mTM.name().asChar(); // �� �̸�..
		std::string szFN; this->ProcessName(mTM.object(), szFN);

		int iLen = szFN.size();
		if(m_Option.bGenerateFileName && iLen >= 11) // Item Code ::: 0_2345_78_0
			szFN = szFN.substr(iLen - 11);
		pPMesh->FileNameSet("Object\\" + szFN + ".N3PMesh"); // ���� �̸� ����..

		pShape->s_MngPMesh.Add(pPMesh); // Progressive Mesh Manager �� �߰�..
		pPD->MeshSet(pPMesh->FileName()); // Mesh ����..
		
		////////////////////////////////////////////////
		// ���� �� �ؽ�ó ó��..
		pPD->m_Mtl.dwColorArg1 = D3DTA_DIFFUSE;
		pPD->m_Mtl.dwColorArg2 = D3DTA_TEXTURE;

		CN3Texture* pTex = this->ProcessTexture(mMesh);
		if(pTex) // �ؽ�ó�� ���� �����̸� ������ �⺻���� ���.. �÷� ���۷��̼��� Modulate
		{
			std::string szTFN = pTex->FileName(); // ���� �̸��� �˻��ؼ�..
			if(szTFN.size() > 0) CharLower(&(szTFN[0]));
			if(-1 == szTFN.find("object\\")) // "Item\" �̶�� ���ڿ��� ������
			{
				szTFN = "Object\\" + pTex->FileName(); // Item �̶�� ��θ� ���δ�..
				pTex->FileNameSet(szTFN);
			}

			
			pPD->m_Mtl.dwColorOp = D3DTOP_MODULATE;
			pPD->TexAlloc(1);
			pPD->TexSet(0, pTex); // Texture Setting ... 
			
//			D3DFORMAT fmtTex = pTex->PixelFormat();
//			if(	fmtTex == D3DFMT_DXT2 ||
//				fmtTex == D3DFMT_DXT3 || 
//				fmtTex == D3DFMT_DXT4 || 
//				fmtTex == D3DFMT_DXT5 ) pPD->m_Mtl.nRenderFlags = RF_ALPHABLENDING;
		}
		else
		{
//			this->ProcessMaterial(this->MeshGetShader(mMesh), &(pPD->m_Mtl));
			pPD->m_Mtl.dwColorOp = D3DTOP_SELECTARG1;
		}
		// ���� �� �ؽ�ó ó��..
		////////////////////////////////////////////////
	}

	return true;
}

CN3Joint* CN3E2Wrapper::ProcessJoint(MFnIkJoint &mJoint)
{
	CN3Joint* pJoint = new CN3Joint();
	//////////////////////////////////////////////////////////////////////////////////////////
	// Joint �� Transform�� ó��..
	this->ProcessTransform(mJoint, pJoint);
	
	// Rotation Order
	MTransformationMatrix::RotationOrder rotOrder = mJoint.rotationOrder(); // MTransformationMatrix::kXYZ;
//	if(rotOrder == MTransformationMatrix::kXYZ) pJoint->m_RotSeq = CN3Joint::ROT_SEQ_XYZ;
//	else if(rotOrder == MTransformationMatrix::kYXZ) pJoint->m_RotSeq = CN3Joint::ROT_SEQ_YXZ;
//	else
//	{
//		MessageBox(::GetActiveWindow(), "NotSupported rotation order. Must kXYZ or kYXZ", "Joint export Warning", MB_OK);
//	}

	char szName[512];
	if(mJoint.parent(0).apiType() == MFn::kTransform)
	{
		MFnTransform mTJ(mJoint.parent(0));
		wsprintf(szName, "Chr\\%s.N3Joint", mTJ.name().asChar()); // ���� �̸� ���ϱ�..
	}
	else
	{
		wsprintf(szName, "Chr\\%s.N3Joint", mJoint.name().asChar()); // ���� �̸� ���ϱ�..
	}
	
	pJoint->m_szName = mJoint.name().asChar(); // �̸�����.. FullName ���� ������ �ʴ´�..

	// ȸ�� �� ���� ���Ѵ�..
	if(pJoint->m_KeyOrient.Count() <= 0) // Joint Orient Key ���� ������.. Rotation Key ���� Orient ��ŭ ��ư��..
	{
		double dAxis[3];
		MStatus stat = MStatus::kSuccess;
		stat = mJoint.getOrientation(dAxis, rotOrder); // local axis...

		// Origin Rotation ���� ������..
		if(dAxis[0] || dAxis[1] || dAxis[2]) // Origin Rotation �� ������..
		{
			__Matrix44 mtxRot;
			mtxRot.Rotation((float)-dAxis[0], (float)-dAxis[1], (float)dAxis[2]);
			__Quaternion qRot = mtxRot;
			this->ProcessJointTransform(pJoint, NULL, &qRot, NULL, false);
		}
	}
	
	// ���ϵ� ����Ʈ ó��..
	int nCC = mJoint.childCount();
	for(int i = 0; i < nCC; i++)
	{
		MObject mObj = mJoint.child(i);
		if(mObj.hasFn(MFn::kJoint)) // Joint �� ���..
		{
			MFnIkJoint mJointChild(mObj);
			CN3Joint* pJointChild = this->ProcessJoint(mJointChild);
			pJoint->ChildAdd(pJointChild);
		}
		else if(mObj.hasFn(MFn::kTransform)) // ���� Transform �� ��� ���ϵ尡 �޽� ���� ����..
		{
			int ttt = 0;
		}
	}

	return pJoint;
}

void CN3E2Wrapper::ProcessJointTransform(CN3Joint *pJoint, __Vector3* pvTrans, __Quaternion* pqRot, __Vector3* pvScale, bool bProcessChild)
{
	if(NULL == pJoint) return;

	bool bRoot = true;
	if(pJoint->Parent()) bRoot = false;

	if(pvScale)
	{
		pJoint->PosSet(pJoint->Pos() * (*pvScale)); // trans Ű�� �����Ͽ� ���� ���߰�..
		int nK = pJoint->m_KeyPos.Count(); // ���ϸ��̼� Ű�� �������ش�..
		for(int i = 0; i < nK; i++)
		{
			__Vector3* pvKey = (__Vector3*)(pJoint->m_KeyPos.DataGet(i));
			(*pvKey) *= (*pvScale);
		}
	}

	if(pvTrans) // �̵��� ����
	{
		pJoint->PosSet(pJoint->Pos() + (*pvTrans));
		int nK = pJoint->m_KeyPos.Count(); // ���ϸ��̼� Ű�� �������ش�..
		for(int i = 0; i < nK; i++)
		{
			__Vector3* pvKey = (__Vector3*)(pJoint->m_KeyPos.DataGet(i));
			(*pvKey) += (*pvTrans);
		}
	}
	
	if(pqRot) // ȸ������ �������ش�..
	{
		__Quaternion qRot2 = pJoint->Rot() * (*pqRot);
		pJoint->RotSet(qRot2);

		int nK = pJoint->m_KeyRot.Count(); // ȸ���� Ű�� �������ش�..
		for(int i = 0; i < nK; i++)
		{
			__Quaternion* pqRot2 = (__Quaternion*)pJoint->m_KeyRot.DataGet(i);
			(*pqRot2) *= (*pqRot);
		}
	}
	
	if(bProcessChild && pvScale) // ���� ��� ó�� ?? 
	{
		int nCC = pJoint->ChildCount();
		for(int i = 0; i < nCC; i++) 
			this->ProcessJointTransform((CN3Joint*)pJoint->Child(i), NULL, NULL, pvScale, true); // ���� ������ �����ϸ� ó���� �ش�..
	}
}

void CN3E2Wrapper::ProcessMatrix(MFnTransform &mTransform, __Matrix44& mtx, __Vector3& vPos, __Quaternion& qtRot, __Vector3& vScale)
{
	// ��� ��ȯ..
	static double dRs[4], dSs[3];
//	MTransformationMatrix::RotationOrder RotOrder = MTransformationMatrix::kXYZ;
//	mTransform.getRotation(dRs, RotOrder, MSpace::kTransform); // ȸ��
	MVector mVecPos;
	mVecPos = mTransform.translation(MSpace::kTransform);
	mVecPos *= 0.01; // 100 ���� 1��.. ���� ��ǥ�� �����..
	mVecPos.z = -mVecPos.z;

	mTransform.getScale(dSs); // Ȯ��

	vPos.Set((float)mVecPos.x, (float)mVecPos.y, (float)mVecPos.z);
//	vRot.Set((float)-dRs[0], (float)-dRs[1], (float)dRs[2]);
	vScale.Set((float)dSs[0], (float)dSs[1], (float)dSs[2]);

	mTransform.getRotationQuaternion(dRs[0], dRs[1], dRs[2], dRs[3]); // ȸ�� ���ʹϾ�
	qtRot.x = (float)dRs[0];
	qtRot.y = (float)dRs[1];
	qtRot.z = (float)dRs[2];
	qtRot.w = (float)dRs[3];

	__Vector3 vAxis; float fAngle;
	qtRot.AxisAngle(vAxis, fAngle);
	vAxis.x *= -1; // x �� ȸ���� �ݴ�..
	vAxis.y *= -1; // y �� ȸ���� �ݴ�..
	vAxis.Normalize();
	qtRot.RotationAxis(vAxis, fAngle);

	// ���� ��� ���..
	mtx.Identity();
	static __Matrix44 mtxTmp;
//	mtxTmp.Rotation((float)-dRs[0], (float)-dRs[1], (float)dRs[2]);	mtx *= mtxTmp;
	mtxTmp = qtRot; mtx *= mtxTmp;
	mtxTmp.Scale((float)dSs[0], (float)dSs[1], (float)dSs[2]);		mtx *= mtxTmp;
	mtx.PosSet((float)mVecPos.x, (float)mVecPos.y, (float)mVecPos.z); // ��ġ..
}

bool CN3E2Wrapper::FindAnimCurve(MObject &mObj, MObjectArray &mAnimCurveArray)
{
	MItDependencyGraph::Direction Direction = MItDependencyGraph::kUpstream;
//	MItDependencyGraph::Traversal TraversalType = MItDependencyGraph::kBreadthFirst;
//	MItDependencyGraph::Level Level = MItDependencyGraph::kNodeLevel;
	MItDependencyGraph::Traversal TraversalType = MItDependencyGraph::kDepthFirst;
	MItDependencyGraph::Level Level = MItDependencyGraph::kPlugLevel;
	MFn::Type Filter = MFn::kAnimCurve;
	
	MStatus stat = MS::kSuccess;
	MItDependencyGraph dgIter(mObj, Filter, Direction, TraversalType, Level, &stat);

	bool bFind = false;
	for (int i = 0; !dgIter.isDone(); dgIter.next(), i++)
	{
		bFind = true;
		mAnimCurveArray.append(dgIter.thisNode(&stat));
	}

	return bFind;
}

bool CN3E2Wrapper::HaveJoint(MFnMesh &mMesh)
{
	MFnTransform mTransform(mMesh.parent(0));
	int nCC = mTransform.childCount();
	int nMC = 0;
	for(int i = 0; i < nCC; i++)
	{
		if(mTransform.child(i).apiType() == MFn::kMesh) nMC++;
	}

	if(nMC >= 2) // return true; // �ΰ����� ũ��.. 
	{
		MItDependencyGraph::Direction Direction = MItDependencyGraph::kUpstream;
		MItDependencyGraph::Traversal TraversalType = MItDependencyGraph::kBreadthFirst;
		MItDependencyGraph::Level Level = MItDependencyGraph::kNodeLevel;
		MFn::Type Filter = MFn::kJoint;

		MItDependencyGraph dgIter(mMesh.object(), Filter, Direction, TraversalType, Level);

		for (int i = 0; !dgIter.isDone(); dgIter.next(), i++)
		{
			MObject mObjTmp = dgIter.thisNode();
			if(mObjTmp.apiType() == MFn::kJoint)
				return true; // ����Ʈ�� ������..
		}
	}
	return false;
}

// �ؽ�ó �ε��� ���..
CN3Texture* CN3E2Wrapper::ProcessTexture(MFnMesh &mMesh)
{
	int nLI = 0;
	DWORD dwRWC = 0;
	char szInfo[1024] = "";

	MStringArray szResult;
	MFnLambertShader mShader(this->MeshGetShader(mMesh));
	MString szCmd = MString("listConnections ") + mShader.name() + MString(".color;");
	MGlobal::executeCommand( szCmd, szResult );

	MString szTexture = szResult[0];
	MString szSurface = mMesh.name();

	if(szTexture.length() <= 0) return NULL;

	// ����� �����´�..
	MString szCommand = MString( "getAttr " ) + szTexture + MString( ".outSize" );
	MIntArray nWH;
	if(MGlobal::executeCommand(szCommand, nWH) != MS::kSuccess)
	{
		wsprintf(szInfo, "�ؽ�ó ���� ó�� ���� : Surface - %s, Texture - %s", szSurface.asChar(), szTexture.asChar());
		nLI = ::SendMessage(m_hWndLB, LB_ADDSTRING, 0, (LPARAM)szInfo); // Progress dialog
		::SendMessage(m_hWndLB, LB_SETCURSEL, (WPARAM)nLI,0); // Progress dialog

		return NULL;
	}

//	int nW = nWH[1], nH = nWH[0]; // ����� �˾Ƴ���..
//	if(nW < 4 || nH < 4)
//	{
//		wsprintf(szInfo, "�ؽ�ó ���� ó�� ���� : �ʺ�, ���̰� �ʹ� �۽��ϴ�. Surface - %s, Texture - %s", szSurface.asChar(), szTexture.asChar());
//		nLI = ::SendMessage(m_hWndLB, LB_ADDSTRING, 0, (LPARAM)szInfo); // Progress dialog
//		::SendMessage(m_hWndLB, LB_SETCURSEL, (WPARAM)nLI,0); // Progress dialog
//
//		return NULL;
//	}

	// �ؽ�ó�� ��ȯ�Ѵ�..
	MString szFile;
	MStringArray szResult2;
	szCommand = MString("getAttr ") + szTexture + MString(".fileTextureName");
	MGlobal::executeCommand(szCommand, szResult2);
	szFile = szResult2[0];

	////////////////////////////////////////////
	// Texture List �� ��ϵǾ� �ִ��� ����..
	char szFNSrc[1024]; // ���� �̸��� ���ϰ�..
	lstrcpy(szFNSrc, szFile.asChar());
	int nFN = lstrlen(szFNSrc);
	for(int i = 0; i < nFN; i++) 
		if(szFNSrc[i] == '/') szFNSrc[i] = '\\';

	// ���ڿ���  ":" �� ������ ���� �α��ڰ� '\\' Ȥ�� '//' �� �ƴϸ� Ǯ������ ������ �Ѵ�.
	if(!strstr(szFNSrc, ":") && !(szFNSrc[0] == '\\' && szFNSrc[1] == '\\'))
	{
		MString szWorkSpace;
		MGlobal::executeCommand(MString("workspace -fullName"), szWorkSpace);
		lstrcpy(szFNSrc, szWorkSpace.asChar());
		lstrcat(szFNSrc, szFile.asChar());
	}
	// WorkSpace �̸��� ��������..

	char szFNDest[1024]; // ������ �̸�
	lstrcpy(szFNDest, szFNSrc);
	nFN = lstrlen(szFNDest);
	for(i = nFN - 1; i >= 0; i--) // ������ �̸��� �����..
	{
		if(szFNDest[i] == '.') { szFNDest[i+1] = 'D'; szFNDest[i+2] = 'X'; szFNDest[i+3] = 'T'; szFNDest[i+4] = NULL; }
		if(szFNDest[i] == '\\' || szFNDest[i] == '/') 
		{
			lstrcpy(szFNDest, &szFNDest[i+1]);
			break;
		}
	}
	
	CN3Texture* pTex = NULL;

	static char szFNs[1024][256]; // ���� �̸��� �ߺ��Ǵ��� üũ...
	if(m_pScene->s_MngTex.Count() <= 0) memset(szFNs, 0, sizeof(szFNs));
	for(i = 0; i < 1024; i++)
	{
		if(NULL == szFNs[i][0]) break;
		if(lstrcmpi(szFNDest, szFNs[i]) == 0)
		{
			pTex = m_pScene->s_MngTex.Get(szFNDest);
			return pTex;
		}
	}

	lstrcpy(szFNs[i], szFNDest);
	pTex = new CN3Texture();
	if(pTex->LoadFromFile(szFNSrc)) // ������ �а�...
	{
		pTex->m_szName = szFNDest; // �̸� ���ϱ�.. - ���� �̸����� ���Ѵ�.
		pTex->FileNameSet(szFNDest); // ���� �̸� ���ϱ�.
		CN3Base::s_MngTex.Add(pTex); // Manager �� ���
	}
	else
	{
		wsprintf(szInfo, "�ؽ�ó ���� ó�� ���� : ������ ������ �����ϴ�. - %s", szFNSrc);
		nLI = ::SendMessage(m_hWndLB, LB_ADDSTRING, 0, (LPARAM)szInfo); // Progress dialog
		::SendMessage(m_hWndLB, LB_SETCURSEL, (WPARAM)nLI,0); // Progress dialog

		delete pTex;
		return NULL;
	}

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

	return pTex;
/*
	// �̺κ��� Maya Image �� ���� �д� �κ��̴�..
	IFFimageReader Reader;
	if(Reader.open(szFile) == MS::kSuccess)
	{
		int nW2, nH2;
		Reader.getSize(nW2, nH2);
		if(nW != nW2 || nH != nH2)
		{
			wsprintf(szInfo, "�ؽ�ó ���� ó�� ���� : ������ ������ �����ϴ�. - %s", szFile.asChar());
			nLI = ::SendMessage(m_hWndLB, LB_ADDSTRING, 0, (LPARAM)szInfo); // Progress dialog
			::SendMessage(m_hWndLB, LB_SETCURSEL, (WPARAM)nLI,0); // Progress dialog

			delete pTex;
			return NULL;
		}

	//	if(Reader.isRGB()) nBPP += 3;
	//	else
	//	{
	//		Reader.close();
	//		DeleteFile(szFile.asChar()); // ���� �����..
	//		wsprintf(szInfo, "�ؽ�ó ���� ó�� ���� : GrayScale Texture �� ���Ŀ� ���� �˴ϴ�. Surface - %s, Texture - %s", szSurface.asChar(), szTexture.asChar());
	//		::SendMessage(m_hWndLB, LB_ADDSTRING, 0, (LPARAM)szInfo); // Progress dialog
	//		::SendMessage(m_hWndLB, LB_SETCURSEL, (WPARAM)nLI,0); // Progress dialog
	//		return false;
	//	}
		
		// Surface ���� �� ä���.
		LPDIRECT3DSURFACE9 lpSurf;
		g_pEng->s_lpD3DDev->CreateImageSurface(nW, nH, D3DFMT_A8R8G8B8, &lpSurf);

		if(NULL == lpSurf)
		{
			Reader.close();
			wsprintf(szInfo, "�ؽ�ó ���� ó�� ���� : Direct3D Texture ���� ����(%d, %d)", nW, nH);
			nLI = ::SendMessage(m_hWndLB, LB_ADDSTRING, 0, (LPARAM)szInfo); // Progress dialog
			::SendMessage(m_hWndLB, LB_SETCURSEL, (WPARAM)nLI,0); // Progress dialog

			return NULL;
		}

		Reader.readImage(); // �̹��� �б�..
		int nBPP = 0;
		nBPP = Reader.getBytesPerChannel() * 4;
		BOOL bAlpha = Reader.hasAlpha();
		const unsigned char * pBuffer = Reader.getPixelMap();

		D3DLOCKED_RECT LR;
		lpSurf->LockRect(&LR, NULL, 0);
		unsigned char *pDest = NULL, *pSrc = NULL;
		for(int y = 0; y < nH; y++)
		{
			pSrc = (unsigned char*)pBuffer + (nH - y - 1) * nW * nBPP; // �Ųٷ� ����Ǿ� �ִ�..
			pDest = (unsigned char*)LR.pBits + y * LR.Pitch;
			for(int x = 0; x < nW; x++, pDest += 4, pSrc += nBPP)
			{
				*(pDest+3) = *(pSrc+3); // Alpha
				*(pDest+2) = *(pSrc+2); // Red
				*(pDest+1) = *(pSrc+1); // Green
				*(pDest+0) = *(pSrc+0); // Blue
			}
		}
		lpSurf->UnlockRect();
		Reader.close();

		if(bAlpha) pTex->CreateFromSurface(lpSurf, D3DFMT_DXT3, TRUE);
//		if(bAlpha) pTex->CreateFromSurface(lpSurf, D3DFMT_DXT4, TRUE);
		else pTex->CreateFromSurface(lpSurf, D3DFMT_DXT1, TRUE);
		
		lpSurf->Release(); lpSurf = NULL;

		if(pTex->Get() == NULL)
		{
			wsprintf(szInfo, "### !!! Texture Export ����(%.3d) : w,h,w2,h2(%.4d, %.4d, %.4d, %.4d) FileName : \"%s\" TextureName \"%s\" MeshName - \"%s\"", 
				m_pScene->s_MngTex.Count(), pTex->Width(), pTex->Height(), nW, nH, szFile.asChar(), pTex->m_szName.c_str(), mMesh.name().asChar());
			nLI = ::SendMessage(m_hWndLB, LB_ADDSTRING, 0, (LPARAM)szInfo); // Progress dialog
			::SendMessage(m_hWndLB, LB_SETCURSEL, (WPARAM)nLI,0); // Progress dialog

			delete pTex;
			return NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Output Window �� �ؽ�ó ���..
	wsprintf(szInfo, "### Texture Export (%.3d) : w,h,w2,h2(%.4d, %.4d, %.4d, %.4d) FileName : \"%s\" TextureName \"%s\" MeshName - \"%s\"", 
		m_pScene->s_MngTex.Count(), pTex->Width(), pTex->Height(), nW, nH, szFile.asChar(), pTex->m_szName.c_str(), mMesh.name().asChar());
	nLI = ::SendMessage(m_hWndLB, LB_ADDSTRING, 0, (LPARAM)szInfo); // Progress dialog
	::SendMessage(m_hWndLB, LB_SETCURSEL, (WPARAM)nLI,0); // Progress dialog
*/
}

int CN3E2Wrapper::ProcessMaterial(MObject mShaderObj, __Material *pMtl)
{
	if(NULL == pMtl) return -1;

	pMtl->Init();

	MColor DC, AC, SC, TC, EC(0.0, 0.0, 0.0, 0.0);
	AC = SC = TC = EC;
	DC.r = 1.0; DC.g = 1.0; DC.b = 1.0; DC.a = 1.0;

	if(mShaderObj.apiType() == MFn::kLambert)
	{
		MFnLambertShader mShader(mShaderObj);
		DC = mShader.color();
		AC = mShader.ambientColor();
//		SC = mShader.specularColor();
		TC = mShader.transparency();
		EC = mShader.incandescence();
	}
	else if(mShaderObj.apiType() == MFn::kReflect)
	{
		MFnReflectShader mShader(mShaderObj);
		DC = mShader.color();
		AC = mShader.ambientColor();
		SC = mShader.specularColor();
		TC = mShader.transparency();
		EC = mShader.incandescence();
	}
	else if(mShaderObj.apiType() == MFn::kBlinn)
	{
		MFnBlinnShader mShader(mShaderObj);
		DC = mShader.color();
		AC = mShader.ambientColor();
		SC = mShader.specularColor();
		TC = mShader.transparency();
		EC = mShader.incandescence();
	}
	else if(mShaderObj.apiType() == MFn::kPhong)
	{
		MFnPhongShader mShader(mShaderObj);
		DC = mShader.color();
		AC = mShader.ambientColor();
		SC = mShader.specularColor();
		TC = mShader.transparency();
		EC = mShader.incandescence();
	}
	else
	{
	}

	pMtl->Diffuse.r = DC.r;
	pMtl->Diffuse.g = DC.g;
	pMtl->Diffuse.b = DC.b;
	pMtl->Diffuse.a = ((1.0f - TC.r) + (1.0f - TC.g) + (1.0f - TC.b))/3.0f; // ������..

	pMtl->Ambient.r = DC.r * 0.7f;
	pMtl->Ambient.g = DC.g * 0.7f;
	pMtl->Ambient.b = DC.b * 0.7f;
	pMtl->Ambient.a = pMtl->Diffuse.a;

//	pMtl->Specular.r = SC.r;
//	pMtl->Specular.g = SC.g;
//	pMtl->Specular.b = SC.b;
//	pMtl->Specular.a = 1.0f;

//	pMtl->Emissive.r = EC.r;
//	pMtl->Emissive.g = EC.g;
//	pMtl->Emissive.b = EC.b;
//	pMtl->Emissive.a = 1.0f;

//	pMtl->Power = 0.0f;

	return 0;
}

int CN3E2Wrapper::ProcessTransform(MFnTransform &mTransform, CN3Transform *pTransform)
{
	int nLI = 0;
	char szInfo[1024] = "";

	if(NULL == pTransform) return -1;
	pTransform->m_szName = mTransform.name().asChar();  // �̸� ����....

	__Vector3 vPos, vScale;
	__Quaternion qtRot;
	this->ProcessMatrix(mTransform, pTransform->m_Matrix, vPos, qtRot, vScale); // ��� ó��..

	if(pTransform->Type() & OBJ_CAMERA) // ī�޶� �ϰ��
	{
		CN3Camera* pCamera = (CN3Camera*)pTransform;
		
		pCamera->EyePosSet(vPos);
		__Matrix44 mtxR = qtRot;
		__Vector3 vAt(0,0,50); // ������ �Ÿ���ŭ...
		vAt *= mtxR; 
		pCamera->AtPosSet(vAt);
		pCamera->UpVectorSet(0,1,0);
	}
	else
	{
		pTransform->PosSet(vPos);
		pTransform->RotSet(qtRot); // ȸ���� ���ʹϾ����� ����..
		pTransform->ScaleSet(vScale);
	}

	if(FALSE == m_Option.bAnimationKey) return -1; // Animation Key ó�� �ɼ��� ���� ��츸 �Ѵ�..

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ���ϸ��̼�..
	MObjectArray mAKs;
	this->FindAnimCurve(mTransform.object(), mAKs);
//	MStatus stat2 = mTransform.setRotationOrder(MTransformationMatrix::kXYZ, true);

	MFnAnimCurve mACs[3][3]; // Position, Rotation, Scale...
	MFnAnimCurve mACJointOrients[3]; // Joint Orient

	int nTranslateCount = 0;

	int nAK = mAKs.length();
	int nKType = -1, nT = 0, nR = 0, nS = 0;
	char szTmp[512];
	for(int i = 0; i < nAK; i++)
	{
		MFnAnimCurve mAC(mAKs[i]);

		lstrcpy(szTmp, mAC.name().asChar()); // �ϴ� �̸����� ���غ���.....
		CharLower(szTmp);

		if(NULL != strstr(szTmp, "translatex")) // ī�޶��� ��쿡�� translate ���� �ΰ��� ���´�..
		{
			 nTranslateCount++; 
		}

		nKType = -1;
		if(NULL != strstr(szTmp, "translatex")) { if(nTranslateCount == 2) nKType = 3; else nKType = 0; }
		else if(NULL != strstr(szTmp, "translatey")) { if(nTranslateCount == 2) nKType = 4; else nKType = 1; }
		else if(NULL != strstr(szTmp, "translatez")) { if(nTranslateCount == 2) nKType = 5; else nKType = 2; }
		else if(NULL != strstr(szTmp, "rotatex")) nKType = 3;
		else if(NULL != strstr(szTmp, "rotatey")) nKType = 4;
		else if(NULL != strstr(szTmp, "rotatez")) nKType = 5;
		else if(NULL != strstr(szTmp, "scalex")) nKType = 6;
		else if(NULL != strstr(szTmp, "scaley")) nKType = 7;
		else if(NULL != strstr(szTmp, "scalez")) nKType = 8;
		else if(NULL != strstr(szTmp, "jointorientx")) nKType = 9;
		else if(NULL != strstr(szTmp, "jointorienty")) nKType = 10;
		else if(NULL != strstr(szTmp, "jointorientz")) nKType = 11;
		else if(NULL != strstr(szTmp, "visibility")) nKType = -1;
		else // �̸����� �Ǵ��Ҽ� ���ٸ�.. ID �� �Ǵ��Ѵ�...??
		{
//			MTypeId id = mAC.typeId();
//			DWORD dwID = id.id();
//			if(dwID == 0x5043544c) { nKType = 0 + nT; nT++; } // Translate
//			else if(dwID == 0x50435441) { nKType = 3 + nR; nR++; } // Rotate
//			else if(dwID == 0x50435455) { nKType = 6 + nS; nS++; } // Scale
//			else nKType = -1;
		
			lstrcpy(szTmp, mAC.typeName().asChar());
			if(NULL != strstr(szTmp, "animCurveTL")) { nKType = 0 + nT; nT++; } // Translate
			else if(NULL != strstr(szTmp, "animCurveTA")) { nKType = 3 + nR; nR++; } // Rotate
			else if(NULL != strstr(szTmp, "animCurveTU")) { nKType = 6 + nS; nS++; } // Scale
			else nKType = -1;
		}

		CN3AnimKey* pKey = NULL;
		if(nKType == 0) { mACs[0][0].setObject(mAC.object()); }
		else if(nKType == 1) { mACs[0][1].setObject(mAC.object()); }
		else if(nKType == 2) { mACs[0][2].setObject(mAC.object()); }
		else if(nKType == 3) { mACs[1][0].setObject(mAC.object()); }
		else if(nKType == 4) { mACs[1][1].setObject(mAC.object()); }
		else if(nKType == 5) { mACs[1][2].setObject(mAC.object()); }
		else if(nKType == 6) { mACs[2][0].setObject(mAC.object()); }
		else if(nKType == 7) { mACs[2][1].setObject(mAC.object()); }
		else if(nKType == 8) { mACs[2][2].setObject(mAC.object()); }

		// Joint Orient Key ��..
		else if(nKType == 9) { mACJointOrients[0].setObject(mAC.object()); }
		else if(nKType == 10) { mACJointOrients[1].setObject(mAC.object()); }
		else if(nKType == 11) { mACJointOrients[2].setObject(mAC.object()); }

		else continue;
	}

	for(i = 0; i < 3; i++)
	{
		int nKs[3];
		nKs[0] = mACs[i][0].numKeys();
		nKs[1] = mACs[i][1].numKeys();
		nKs[2] = mACs[i][2].numKeys();
		if(nKs[0] > 0 || nKs[1] > 0 || nKs[2] > 0)
		{
			if(nKs[0] <= 0 || nKs[1] <= 0 || nKs[2] <= 0)
			{
				wsprintf(szInfo, "Transform %s : All Animation Key must have at least over 1 Key value.", mTransform.name().asChar());
				MessageBox(::GetActiveWindow(), szInfo, "Invalid Animation Key", MB_OK);
			}
			else
			{
//				if(i == 0) this->ProcessAnimKey(mACs[i], &pTransform->m_KeyPos, true, 0.01f, false); // Translation Animation Key �� ó���Ѵ�..
				if(i == 0) this->ProcessAnimKey(mACs[i], &pTransform->m_KeyPos, true, 0.01f, false); // Translation Animation Key �� ó���Ѵ�..
				else if(i == 1)
				{
					if(nTranslateCount == 2) // View Translate ... ��� ī�޶��ϴ� Ÿ��ó�� ó���Ѵ�.
					{
						this->ProcessAnimKey(mACs[i], &pTransform->m_KeyRot, true, 0.01f, false); // ViewTarget Position Key �� ó���Ѵ�..
					}
					else
					{
						MTransformationMatrix::RotationOrder rotOrder = mTransform.rotationOrder(); // ȸ�� ������ ���� ó���� �޶����� �Ѵ�..
						this->ProcessAnimKey(mACs[i], &pTransform->m_KeyRot, true, 1.0f, true, rotOrder); // Rotation Animation Key �� ���ʹϾ����� ó���Ѵ�.. 

//						if(pTransform->Type() & OBJ_JOINT) // ����Ʈ�� ���...
//						{
						// -180 ������ 180 �� ���̿� �ְ� �����..
//						int nKC = pTransform->m_KeyRot.Count();
//						for(int i = 0; i < nKC; i++)
//						{
//							__Vector3* pV = pTransform->m_KeyRot.DataGet(i);
//							if(pV->x > D3DX_PI) { while(pV->x > D3DX_PI) pV->x -= 3.1415192654f * 2.0f; }
//							if(pV->y > D3DX_PI) { while(pV->y > D3DX_PI) pV->y -= 3.1415192654f * 2.0f; }
//							if(pV->z > D3DX_PI) { while(pV->z > D3DX_PI) pV->z -= 3.1415192654f * 2.0f; }
//
//							if(pV->x < -D3DX_PI) { while(pV->x < -D3DX_PI) pV->x += 3.1415192654f * 2.0f; }
//							if(pV->y < -D3DX_PI) { while(pV->y < -D3DX_PI) pV->y += 3.1415192654f * 2.0f; }
//							if(pV->z < -D3DX_PI) { while(pV->z < -D3DX_PI) pV->z += 3.1415192654f * 2.0f; }
//						}
//						}
					} // end of else
				}
				else if(i == 2)
				{
					this->ProcessAnimKey(mACs[i], &pTransform->m_KeyScale, false, 1.0f, false, MTransformationMatrix::kXYZ); // Scale Animation Key �� ó���Ѵ�..
				} // end of if(i == 0)
			} // end of if(nKs[0] <= 0 || nKs[1] <= 0 || nKs[2] <= 0)
		} // end of if(nKs[0] > 0 || nKs[1] > 0 || nKs[2] > 0)
	} // end of for(i = 0; i < 3; i++)

	if(pTransform->Type() & OBJ_JOINT) // Joint �̸�... Orient Ű ���� ����..
	{
		CN3Joint* pJoint = (CN3Joint*)pTransform;
		this->ProcessAnimKey(mACJointOrients, &(pJoint->m_KeyOrient), true, 1.0f, true, MTransformationMatrix::kXYZ);
	}
	// ���ϸ��̼�..
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	return 0;
}

void CN3E2Wrapper::ProcessAnimKey(	MFnAnimCurve* pmACs,
									CN3AnimKey *pKey,
									bool bReverseZ,
									float fScale,
									bool bQuaternion,
									MTransformationMatrix::RotationOrder mRotOrder)
{
	// ���� ����� ã�´�..
	double dfFrmMax = 0.0f, dfFrmTmp = 0.0f;
	int nKC = 0;
	for(int i = 0; i < 3; i++)
	{
		nKC = pmACs[i].numKeys();
		dfFrmTmp = pmACs[i].time(nKC - 1).value();
		if(dfFrmTmp > dfFrmMax) dfFrmMax = dfFrmTmp;
	}

	if(dfFrmMax <= 1) return;
	dfFrmMax = m_pScene->m_fFrmEnd - m_pScene->m_fFrmStart; // ��ü Scene �� ������� �Ѵ�.

	int nFrmMax = (int)(dfFrmMax * m_Option.fSamplingRate / 30.0f); // 30 Frame Per Sec �� Sampling
	
	if(bQuaternion) pKey->Alloc(nFrmMax, m_Option.fSamplingRate, KEY_QUATERNION); // �����̼��̸� ���ʹϾ����� �Ҵ�.
	else pKey->Alloc(nFrmMax, m_Option.fSamplingRate, KEY_VECTOR3); // ���ͷ� �Ҵ�..

	MTime mTime; mTime.setUnit(MTime::kNTSCFrame);
	if(bQuaternion)
	{
		double dRs[4];
		__Quaternion* pQt;
//		MMatrix mMtx;
//		MTransformationMatrix mTMtx;
		__Matrix44 mtxRotFinal, mtxRots[3];
		int nRotSeqs[3] = {1,0,2}; // �⺻ YXZ ȸ��..

		// ȸ������
		if(MTransformationMatrix::kXYZ == mRotOrder) { nRotSeqs[0] = 0; nRotSeqs[1] = 1; nRotSeqs[2] = 2; }
		else if(MTransformationMatrix::kYZX == mRotOrder) { nRotSeqs[0] = 1; nRotSeqs[1] = 2; nRotSeqs[2] = 0; }
		else if(MTransformationMatrix::kZXY == mRotOrder) { nRotSeqs[0] = 2; nRotSeqs[1] = 0; nRotSeqs[2] = 1; }
		else if(MTransformationMatrix::kXZY == mRotOrder) { nRotSeqs[0] = 0; nRotSeqs[1] = 2; nRotSeqs[2] = 1; }
		else if(MTransformationMatrix::kYXZ == mRotOrder) { nRotSeqs[0] = 1; nRotSeqs[1] = 0; nRotSeqs[2] = 2; }
		else if(MTransformationMatrix::kZYX == mRotOrder) { nRotSeqs[0] = 2; nRotSeqs[1] = 1; nRotSeqs[2] = 0; }
		else MessageBox(::GetActiveWindow(), "Not supported rotation order", "Warning - animation key", MB_OK);

		for(i = 0; i < nFrmMax; i++)
		{
			pQt = (__Quaternion*)(pKey->DataGet(i));

			double dfFrm = i * 30.0f / m_Option.fSamplingRate; // 60 Frame Per Sec �� Sampling
			mTime.setValue(dfFrm);

			memset(dRs, 0, sizeof(double) * 4);

			pmACs[0].evaluate(mTime, dRs[0]);
			pmACs[1].evaluate(mTime, dRs[1]);
			pmACs[2].evaluate(mTime, dRs[2]);

			// -180 ~ 180 �� ���̷� �����..
			if(dRs[0] < -D3DX_PI)
				dRs[0] -= __PI2 * (int)(dRs[0]/__PI2 - 1);
			if(dRs[0] > D3DX_PI)
				dRs[0] -= __PI2 * (int)(dRs[0]/__PI2 + 1);
			
			if(dRs[1] < -D3DX_PI)
				dRs[1] -= __PI2 * (int)(dRs[1]/__PI2 - 1);
			if(dRs[1] > D3DX_PI)
				dRs[1] -= __PI2 * (int)(dRs[1]/__PI2 + 1);
			
			if(dRs[2] < -D3DX_PI)
				dRs[2] -= __PI2 * (int)(dRs[2]/__PI2 - 1);
			if(dRs[2] > D3DX_PI)
				dRs[2] -= __PI2 * (int)(dRs[2]/__PI2 + 1);

			mtxRots[0].RotationX(-dRs[0]);
			mtxRots[1].RotationY(-dRs[1]);
			mtxRots[2].RotationZ(dRs[2]);

//			mMtx.setToIdentity();
//			mTMtx = mMtx;
//			mTMtx.setRotation(dRs, MTransformationMatrix::kYXZ, MSpace::kTransform);
//			mTMtx.getRotationQuaternion(dRs[0], dRs[1], dRs[2], dRs[3], MSpace::kTransform);
//			pQt->x = (float)dRs[0];
//			pQt->y = (float)dRs[1];
//			pQt->z = (float)dRs[2];
//			pQt->w = -(float)dRs[3];

			mtxRotFinal.Identity();
			mtxRotFinal *= mtxRots[nRotSeqs[0]];
			mtxRotFinal *= mtxRots[nRotSeqs[1]];
			mtxRotFinal *= mtxRots[nRotSeqs[2]];

			*pQt = mtxRotFinal;
		}
	}
	else
	{
		double dfValue;
		__Vector3* pV = NULL;
		for(i = 0; i < nFrmMax; i++)
		{
			pV = (__Vector3*)(pKey->DataGet(i));

			double dfFrm = i * 30.0f / m_Option.fSamplingRate;
			mTime.setValue(dfFrm);

			pmACs[0].evaluate(mTime, dfValue); pV->x = (float)dfValue;
			pmACs[1].evaluate(mTime, dfValue); pV->y = (float)dfValue;
			pmACs[2].evaluate(mTime, dfValue); pV->z = (float)dfValue;

			if(1.0f != fScale) *pV *= fScale; // ������ meter �� �׷���..
			if(TRUE == bReverseZ) pV->z = -(pV->z);// Z ���� ������ �Ѵ�..
		}
	}
}

bool CN3E2Wrapper::ProcessChr(MFnSkinCluster &mSkin)
{
	/////////////////////////////////////////////////////////////////////////////////////
	// ������ �ִ� ������ ã�Ƽ� ó���Ѵ�..
	MStatus stat;
	MDagPathArray PathArray;
	mSkin.influenceObjects(PathArray, &stat);
	int nJC = PathArray.length(); // ���� ������ ����� �д�.
	MFnIkJoint mJointRoot;
	for(int i = 0; i < nJC; i++)
	{
		mJointRoot.setObject(PathArray[i].node());
		if(mJointRoot.parent(0).apiType() != MFn::kJoint) // ��Ʈ ������ ��쿡 ó��..
			break;
	}
	if(i == nJC) return false; // Root Joint �� ���� ���..

	// �ߺ��Ǵ��� ã�ƺ���..
	std::string szJointFN;
	if(mJointRoot.parentCount() > 0 && mJointRoot.parent(0).hasFn(MFn::kTransform))
	{
		MFnTransform mP1(mJointRoot.parent(0));
		szJointFN = ""; this->ProcessName(mP1.parent(0), szJointFN); // ���� �̸��� �˾ƺ���..
		szJointFN = "Chr\\" + szJointFN + ".N3Joint";
	}
	else
		szJointFN = mJointRoot.name().asChar(); // ���� �̸��� �˾ƺ���..

	static std::string szJointFNs[512]; // �ߺ��Ǵ��� üũ...
	if(m_pScene->s_MngJoint.Count() <= 0) 
	{
		for(i = 0; i < 512; i++) szJointFNs[i] = "";
	}

	bool bOverlapped = false;
	for(i = 0; i < 256; i++)
	{
		if(szJointFNs[i].size() <= 0) break;
		if(szJointFN == szJointFNs[i])
		{
			bOverlapped = true;
			break;
		}
	}
	
	CN3Chr* pChr = NULL;
	CN3Joint* pJoint = NULL;

	if(false == bOverlapped) // �ߺ����� �ʴ´ٸ�..���� �����..
	{
		szJointFNs[i] = szJointFN; // ����Ʈ �̸��� ����ϰ�..

		pChr = new CN3Chr();
		m_pScene->ChrAdd(pChr); // Scene �� ĳ���� �߰�..
		pChr->PartAlloc(64); // ����ϰ� Part Data �Ҵ�..

		if(mJointRoot.parentCount() > 0)
		{
			if(mJointRoot.parent(0).hasFn(MFn::kTransform))
			{
				MFnTransform mP1(mJointRoot.parent(0));
				if(mP1.parentCount() > 0) this->ProcessName(mP1.parent(0), pChr->m_szName); // �̸� ����....
				else this->ProcessName(mP1.parent(0), pChr->m_szName); // �̸� ����....
				pChr->FileNameSet("Chr\\" + pChr->m_szName + ".N3Chr"); // ���� �̸� ����..
			}
			else
			{
				this->ProcessName(mJointRoot.parent(0), pChr->m_szName); // �̸� ����....
				pChr->FileNameSet("Chr\\" + pChr->m_szName + ".N3Chr"); // ���� �̸� ����..);
			}
		}
		else 
		{
			pChr->m_szName = mJointRoot.name().asChar();
			std::string szFN = "Chr\\"; szFN += mJointRoot.name().asChar(); szFN += ".N3Chr"; // ���� �̸� ����..
			pChr->FileNameSet(szFN);
		}

		pJoint = this->ProcessJoint(mJointRoot); // Joint ó��
		if(NULL == pJoint) 
		{
			delete pChr;
			return NULL;
		}
		pJoint->FileNameSet(szJointFN); // ���� �̸� ����..
		
		pChr->s_MngJoint.Add(pJoint);
		pChr->JointSet(pJoint->FileName()); // Joint Setting;
		
		///////////////////////////////////////////////////////////////////////
		// Joint �� �׷� �Ǿ� �ִٸ� Parent Transform ó��..
		if(mJointRoot.parentCount() > 0 && mJointRoot.parent(0).hasFn(MFn::kTransform))
		{
			MFnTransform mTJ(mJointRoot.parent(0)); // Joint Transform Node
			
			MTransformationMatrix mTMJ = mTJ.transformation();
//			MMatrix mMtxJT; mMtxJT.setToIdentity();
//			this->GetWorldTransform(mTJ, mMtxJT);
//			MTransformationMatrix mTMJ(mMtxJT);

			MVector mVPos;
			double dfScale[3], dfRot[3];
			MTransformationMatrix::RotationOrder RotOrder = MTransformationMatrix::kXYZ;

			mVPos = mTMJ.translation(MSpace::kTransform) * 0.01;
			
			MMatrix mMtxJWorld; mMtxJWorld.setToIdentity();
			this->GetWorldTransform(mTJ, mMtxJWorld);
			mVPos.x = mMtxJWorld.matrix[3][0] * 0.01;
			mVPos.y = mMtxJWorld.matrix[3][1] * 0.01;
			mVPos.z = mMtxJWorld.matrix[3][2] * 0.01;
			
			MTransformationMatrix mTMtxJWorld(mMtxJWorld);
			mTMtxJWorld.getScale(dfScale, MSpace::kTransform);
			mTMtxJWorld.getRotation(dfRot, RotOrder, MSpace::kTransform);

			__Vector3 vTrans((float)mVPos.x, (float)mVPos.y, -(float)mVPos.z);
			__Vector3 vScale((float)dfScale[0], (float)dfScale[1], (float)dfScale[2]);
			__Vector3 vRot((float)-dfRot[0], (float)-dfRot[1], (float)dfRot[2]);
			
			__Matrix44 mtxRotG; mtxRotG.Rotation(vRot);
			__Quaternion qRot = mtxRotG;
			vTrans *= mtxRotG;
			this->ProcessJointTransform(pJoint, &vTrans, &qRot, &vScale, true);

			pJoint->PosSet(pJoint->Pos() * mtxRotG); // ��ġ ���� ����� �����Ͽ� �°� 
			int nK = pJoint->m_KeyPos.Count(); // ���ϸ��̼� Ű�� �������ش�..
			for(i = 0; i < nK; i++)
			{
				__Vector3* pvKey = (__Vector3*)(pJoint->m_KeyPos.DataGet(i));
				(*pvKey) *= mtxRotG;
			}
		}
	}
	else
	{
		pJoint = m_pScene->s_MngJoint.Get(szJointFN);

		int nChrCount = m_pScene->ChrCount(); // ���� ����Ʈ �����͸� ���� ĳ���� �����͸� ã�´�..
		for(int i = 0; i < nChrCount; i++)
		{
			CN3Chr* pChrTmp = m_pScene->ChrGet(i);
			if(pChrTmp->Joint() == pJoint)
			{
				pChr = pChrTmp;
				break;
			}
		}
	}

	if(NULL == pChr) return false;

	/////////////////////////////////////////////////// ............ // Mesh
	// ���� Mesh
	MObjectArray mMeshArray;
	mSkin.getInputGeometry(mMeshArray);
	__ASSERT(1 == mMeshArray.length(), "binding mesh count is not 1");
	MFnMesh mMeshOrg(mMeshArray[0]); // Original Mesh

	mSkin.getOutputGeometry(mMeshArray); 
	__ASSERT(1 == mMeshArray.length(), "Output mesh count is not 1");
	MFnMesh mMeshOutput(mMeshArray[0]); // Output Mesh

	bool bCollisionMesh = false;

	MFnTransform mMT(mMeshOrg.parent(0)); // Mesh Transform
	MFnTransform mGT;// Mesh GroupTransform
	if(mMT.parentCount() > 0 && mMT.parent(0).hasFn(MFn::kTransform))
		mGT.setObject(mMT.parent(0));
	else
		mGT.setObject(mMT.object());

	// �浹 üũ������ ���� �޽����� ���캻��..
	std::string szFNM = mMT.name().asChar();
	if(szFNM.size() > 0) CharLower(&(szFNM[0]));
	if(szFNM.find("coll") != -1) bCollisionMesh = true;

	if(true == bCollisionMesh) // �浹 üũ �޽ø�.... 
	{
/*		CN3IMesh* pN3Mesh = this->ProcessIMesh(mMeshOrg); // Indexed Mesh ó��.
		int nVC = pN3Mesh->VertexCount();
		int nFC = pN3Mesh->FaceCount();
		__VertexXyzNormal* pVSrc = pN3Mesh->Vertices();
		WORD* pwISrc = pN3Mesh->VertexInices();

		
		CN3VMesh* pVMesh = new CN3VMesh();
		pVMesh->CreateVertices(nVC);
		pVMesh->CreateIndex(nFC * 3);
		__Vector3* pVDest = pVMesh->Vertices();
		WORD* pwIDest = pVMesh->Indices();

		memcpy(pwIDest, pwISrc, nFC * 3 * 2);
		for(int i = 0; i < nVC; i++) pVDest[i] = pVSrc[i].v;

		delete pN3Mesh;

		pVMesh->m_szName = "";
		this->ProcessName(mMeshOrg.object(), pVMesh);
		std::string szFN = "Chr\\" + pVMesh->m_szName + ".N3VMesh"; // ���� �̸� ����..
		pVMesh->FileNameSet(szFN);

		pChr->s_MngVMesh.Add(pVMesh);
		pChr->CollisionMeshSet(pVMesh->m_szName);

		this->ProcessSkin(mSkin, pChr->CollisionSkin()); // Skin ó��.
*/
	}
	else if(false == bCollisionMesh) // �浹 üũ �޽� �ƴϸ�....���������� ����..
	{
		int nLOD = 0;
		int nPart = 0;

		if(mGT.parentCount() > 0 && mGT.parent(0).hasFn(MFn::kTransform)) // �׷� Ʈ�������� ���� ��尡 ���� ���..
		{
			MFnTransform mCT(mGT.parent(0)); // Character Transform
			int nC = mCT.childCount();
			for(i = 0; i < nC; i++)
			{
				if(!mCT.child(i).hasFn(MFn::kTransform)) continue;

				MFnTransform mTTmp(mCT.child(i));
				if(mTTmp.object() == mGT.object()) break; // ĳ���� Ʈ������ ���� ���ϵ��� �̸��� �׷��� �̸��� ���� ���..
				nPart++; // ���� ������ Part ����..
			}

			int nGTC = mGT.childCount();
			for(i = 0; i < nGTC; i++)
			{
				if(!mGT.child(i).hasFn(MFn::kTransform)) continue; 

				MFnTransform mTTmp(mGT.child(i));
				if(mTTmp.name() == mMT.name()) break; // Mesh Transform �� Mesh �׷��� ���ϵ� �޽ð� �̸��� ������..
				nLOD++;
			}
		}
		else // pair of if(mGT.parentCount() > 0 && mGT.parent(0).apiType() == MFn::kTransform) // �׷� Ʈ�������� ���� ��尡 ���� ���..
		{
			int nC = mGT.childCount();
			for(i = 0; i < nC; i++)
			{
				if(mGT.child(i).apiType() != MFn::kTransform) continue;

				MFnTransform mTTmp(mGT.child(i));
				if(mTTmp.name() == mMT.name()) break; // ĳ���� Ʈ������ ���� ���ϵ��� �̸��� �׷��� �̸��� ���� ���..
				nPart++; // ���� ������ Part ����..
			}
		}
		
		__ASSERT(nPart >= 0 && nPart < 64, "Part count is bigger than 64");
		__ASSERT(nLOD >= 0 && nLOD < MAX_CHR_LOD, "LOD Count is bigger than MAX_CHR_LOD");
		
		CN3CPart* pPart = pChr->Part(nPart);
		CN3Skin* pSkin = pPart->Skin(nLOD);

		if(pPart->m_szName.empty()) // ��Ʈ�� �̸��� ������..
		{
			pPart->m_szName = mGT.name().asChar();
			std::string szFN; this->ProcessName(mGT.object(), szFN);

			int iLen = szFN.size();
			int iLen2 = pPart->m_szName.size();
			if(m_Option.bGenerateFileName && iLen >= 11 && iLen2 >= 11) // Item Code ::: 0_2345_78_0
				szFN = szFN.substr(iLen - 11);
			pPart->FileNameSet("item\\" + szFN + ".N3CPart");
		}
		
		if(NULL == pSkin) // Skin �� ������ �ִ´�..
		{
			CN3CPartSkins* pSkinsAdd = new CN3CPartSkins();
			pSkinsAdd->m_szName = mGT.name().asChar(); // �̸��� ���� �̸��� ���ϰ�..
			std::string szFN; this->ProcessName(mGT.object(), szFN);

			int iLen = szFN.size();
			int iLen2 = pSkinsAdd->m_szName.size();
			if(m_Option.bGenerateFileName && iLen >= 11 && iLen2 >= 11) // Item Code ::: 0_2345_78_0
				szFN = szFN.substr(iLen - 11);
			pSkinsAdd->FileNameSet("item\\" + szFN + ".N3CSkins");

			CN3Base::s_MngSkins.Add(pSkinsAdd);
			pPart->SkinsSet(pSkinsAdd->FileName());
			pSkin = pPart->Skin(nLOD); // �ٽ� ������ ���ϱ�.
		} 

		if(false == this->ProcessSkin(mSkin, pSkin)) // Skin ó��..
		{
			MessageBox(::GetActiveWindow(), mSkin.name().asChar(), "Skin processing failed", MB_OK);
			return false;
		}

		// �ؽ�ó ó��..
		pPart->m_Mtl.Init();
		pPart->m_Mtl.dwColorArg1 = D3DTA_DIFFUSE;
		pPart->m_Mtl.dwColorArg2 = D3DTA_TEXTURE;

		CN3Texture* pTex = ProcessTexture(mMeshOutput);
		if(pTex)
		{
			pTex->m_szName = mGT.name().asChar(); // �̸��� ���� �̸��� ���ϰ�..
			std::string szFN; this->ProcessName(mGT.object(), szFN);
			int iLen = szFN.size();
			int iLen2 = pTex->m_szName.size();
			if(m_Option.bGenerateFileName && iLen >= 11 && iLen2 >= 11) // Item Code ::: 0_2345_78_0
				szFN = szFN.substr(iLen - 11);
			pTex->FileNameSet("Item\\" + szFN + ".DXT"); // Part �̸��� Texture �̸��� ���� �Ѵ�.
			
			pPart->m_Mtl.dwColorOp = D3DTOP_MODULATE;
			pPart->TexSet(pTex);

//			D3DFORMAT fmtTex = pTex->PixelFormat();
//			if(	fmtTex == D3DFMT_DXT2 ||
//				fmtTex == D3DFMT_DXT3 || 
//				fmtTex == D3DFMT_DXT4 || 
//				fmtTex == D3DFMT_DXT5 ) pPart->m_Mtl.nRenderFlags = RF_ALPHABLENDING;
		}
		else // �ؽ�ó�� ���� ��쿡�� ����
		{
//			this->ProcessMaterial(this->MeshGetShader(mMeshOutput), &(pPart->m_Mtl)); 
			pPart->m_Mtl.dwColorOp = D3DTOP_SELECTARG1;
		}
	} // if(false == bCollisionMesh) // �浹 üũ �޽� �ƴϸ�....���������� ����..

	return true;
}

void CN3E2Wrapper::ProcessName(MObject mObj, std::string& szName)
{
	if(mObj.hasFn(MFn::kWorld) == FALSE && mObj.hasFn(MFn::kDependencyNode)) // �ֻ��� ��尡 �ƴϸ�..
	{
		MFnDependencyNode mNode(mObj);

		std::string szNameBack = szName;
		szName = mNode.name().asChar();
		if(szNameBack.size() > 0)
		{
			szName += '_';
			szName += szNameBack;
		}

		if(mObj.hasFn(MFn::kDagNode))
		{
			MFnDagNode mDAG(mObj);
			if(mDAG.parentCount() > 0) this->ProcessName(mDAG.parent(0), szName);
		}
	}
}
/*
const DWORD OBJ_UNKNOWN				= 0;
const DWORD OBJ_BASE				= 0x1;
const DWORD OBJ_BASE_HIERARCHY 		= 0x2;
const DWORD OBJ_TRANSFORM 			= 0x4;
const DWORD OBJ_SCENE				= 0x8;
const DWORD OBJ_TEXTURE				= 0x10;

const DWORD OBJ_CAMERA				= 0x20;
const DWORD OBJ_LIGHT				= 0x40;
const DWORD OBJ_SHAPE				= 0x80;
const DWORD OBJ_CHARACTER			= 0x100;

const DWORD OBJ_MESH				= 0x200;
const DWORD OBJ_MESH_PROGRESSIVE	= 0x400;
const DWORD OBJ_MESH_INDEXED		= 0x800;
const DWORD OBJ_MESH_VECTOR3		= 0x1000;
const DWORD OBJ_JOINT				= 0x2000;
const DWORD OBJ_SKIN				= 0x4000;

const DWORD OBJ_DUMMY				= 0x8000;
const DWORD OBJ_EFFECT				= 0x10000;

const DWORD OBJ_ANIM_CONTROL		= 0x20000;
const DWORD OBJ_SUBSCENE			= 0x40000;
*/


void CN3E2Wrapper::GetWorldTransform(MFnTransform &mTransform, MMatrix &mMtx)
{
	int nParent = mTransform.parentCount();
	mMtx *= mTransform.transformationMatrix();
	if(1 == nParent && mTransform.parent(0).hasFn(MFn::kTransform))
	{
		MFnTransform mPT(mTransform.parent(0));
		this->GetWorldTransform(mPT, mMtx);
	}
}

bool CN3E2Wrapper::IsSelected(MSelectionList& mSelList, MObject mObj)
{
	if(mObj.hasFn(MFn::kDependencyNode))
	{
		MFnDependencyNode mDag(mObj);
		const char* szName = mDag.name().asChar();
		int ttttt = 0;
	}

	bool bSel = false;
	int nSelCount = mSelList.length();
	MObject mObjTmp;
	for(int i = 0; i < nSelCount; i++)
	{
		mSelList.getDependNode(i, mObjTmp);
		if(mObjTmp == mObj)
		{
			return true;
		}
	}

	if(mObj.hasFn(MFn::kDagNode))
	{
		MFnDagNode mDNode(mObj);
		if(mDNode.parentCount() > 0)
		{
			return IsSelected(mSelList, mDNode.parent(0));
		}
	}
	else if(mObj.hasFn(MFn::kSkinClusterFilter))
	{
		MFnSkinCluster mSkin(mObj);
		MObjectArray mMeshArray;
		mSkin.getInputGeometry(mMeshArray);
		__ASSERT(1 == mMeshArray.length(), "binding mesh count is not 1");
		return IsSelected(mSelList, mMeshArray[0]); // Original Mesh
	}

	return false;
}


INT_PTR CALLBACK CN3E2Wrapper::DlgProcProgress( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg ) 
    {
        case WM_INITDIALOG:
        {
			break;
        }
        case WM_COMMAND:
        {
			switch( LOWORD(wParam) )
			{
			case IDC_B_CANCEL:
				m_bCancelExport = TRUE;
				break;
			}
			break;
		}
		case WM_CLOSE:
		{
			ShowWindow(hDlg, SW_HIDE);
			break;
		}
    }
    return FALSE; // Didn't handle message
}

BOOL CALLBACK CN3E2Wrapper::DlgProcPane(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			// Registry �� �ɼǰ��� �־�ξ���..
			HKEY hKey = g_pEng->RegistryOpen("N3Export Option");
			if(hKey) 
			{
				g_pEng->RegistryValueGet(hKey, "Export Camera", &m_Option.bExportCamera, 4);
				g_pEng->RegistryValueGet(hKey, "Export Light", &m_Option.bExportLight, 4);
				g_pEng->RegistryValueGet(hKey, "Export Geometry", &m_Option.bExportGeometry, 4);
				g_pEng->RegistryValueGet(hKey, "Export Dummy", &m_Option.bExportDummy, 4);
				g_pEng->RegistryValueGet(hKey, "Export Character", &m_Option.bExportCharacter, 4);

				g_pEng->RegistryValueGet(hKey, "Animation Key", &m_Option.bAnimationKey, 4);
				g_pEng->RegistryValueGet(hKey, "Generate File Name", &m_Option.bGenerateFileName, 4);
				g_pEng->RegistryValueGet(hKey, "Generate Smooth Normal", &m_Option.bGenerateSmoothNormal, 4);
//				g_pEng->RegistryValueGet(hKey, "Generate Progressive Mesh", &m_Option.bGenerateProgressiveMesh, 4);
				g_pEng->RegistryValueGet(hKey, "Generate Half Size Texture", &m_Option.bGenerateHalfSizeTexture, 4);
				g_pEng->RegistryValueGet(hKey, "Generate Compressed Texture", &m_Option.bGenerateCompressedTexture, 4);
				
				g_pEng->RegistryClose(hKey);
			}

			CheckDlgButton(hDlg, IDC_C_OBJ_CAMERA, m_Option.bExportCamera);
			CheckDlgButton(hDlg, IDC_C_OBJ_LIGHT, m_Option.bExportLight);
			CheckDlgButton(hDlg, IDC_C_OBJ_GEOMETRY, m_Option.bExportGeometry);
			CheckDlgButton(hDlg, IDC_C_OBJ_DUMMY, m_Option.bExportDummy);
			CheckDlgButton(hDlg, IDC_C_OBJ_CHARACTER, m_Option.bExportCharacter);

			CheckDlgButton(hDlg, IDC_C_ANIMATION_KEY, m_Option.bAnimationKey);

			CheckDlgButton(hDlg, IDC_C_GENERATE_FILE_NAME, m_Option.bGenerateFileName);
			CheckDlgButton(hDlg, IDC_C_GENERATE_SMOOTH_NORMAL, m_Option.bGenerateSmoothNormal);
//			CheckDlgButton(hDlg, IDC_C_GENERATE_PROGRESSIVE_MESH, m_Option.bGenerateProgressiveMesh);
			CheckDlgButton(hDlg, IDC_C_GENERATE_HALF_SIZE_TEXTURE, m_Option.bGenerateHalfSizeTexture);
			CheckDlgButton(hDlg, IDC_C_GENERATE_COMPRESSED_TEXTURE, m_Option.bGenerateCompressedTexture);

			m_Option.nFrmStart = (int)(MAnimControl::minTime().value()); // ���� ������.
			m_Option.nFrmEnd = (int)(MAnimControl::maxTime().value() +0.5); // �� ������

			SetDlgItemInt(hDlg, IDC_E_FRAME_START, m_Option.nFrmStart, FALSE);
			SetDlgItemInt(hDlg, IDC_E_FRAME_END, m_Option.nFrmEnd, FALSE);
			SetDlgItemInt(hDlg, IDC_E_SAMPLING_RATE, (int)(m_Option.fSamplingRate), FALSE);
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			{
				memset(&m_Option, 0, sizeof(m_Option));

				m_Option.bExportCamera = IsDlgButtonChecked(hDlg, IDC_C_OBJ_CAMERA);
				m_Option.bExportLight = IsDlgButtonChecked(hDlg, IDC_C_OBJ_LIGHT);
				m_Option.bExportGeometry = IsDlgButtonChecked(hDlg, IDC_C_OBJ_GEOMETRY);
				m_Option.bExportDummy = IsDlgButtonChecked(hDlg, IDC_C_OBJ_DUMMY);
				m_Option.bExportCharacter = IsDlgButtonChecked(hDlg, IDC_C_OBJ_CHARACTER);

				m_Option.bAnimationKey = IsDlgButtonChecked(hDlg, IDC_C_ANIMATION_KEY);
				m_Option.nFrmStart = GetDlgItemInt(hDlg, IDC_E_FRAME_START, NULL, FALSE); // ���� ������
				m_Option.nFrmEnd = GetDlgItemInt(hDlg, IDC_E_FRAME_END, NULL, FALSE); // �� ������
				m_Option.fSamplingRate = (float)GetDlgItemInt(hDlg, IDC_E_SAMPLING_RATE, NULL, FALSE); // Sampling Rate

				m_Option.bGenerateFileName = IsDlgButtonChecked(hDlg, IDC_C_GENERATE_FILE_NAME);
				m_Option.bGenerateSmoothNormal = IsDlgButtonChecked(hDlg, IDC_C_GENERATE_SMOOTH_NORMAL);
//				m_Option.bGenerateProgressiveMesh = IsDlgButtonChecked(hDlg, IDC_C_GENERATE_PROGRESSIVE_MESH);
				m_Option.bGenerateHalfSizeTexture = IsDlgButtonChecked(hDlg, IDC_C_GENERATE_HALF_SIZE_TEXTURE);
				m_Option.bGenerateCompressedTexture = IsDlgButtonChecked(hDlg, IDC_C_GENERATE_COMPRESSED_TEXTURE);

				EndDialog(hDlg, 1);

				// Registry �� �ɼǰ��� �־�ξ���..
				HKEY hKey = g_pEng->RegistryOpen("N3Export Option");
				if(NULL == hKey) RegCreateKey(HKEY_CURRENT_USER, "N3Export Option", &hKey);
				if(hKey) 
				{
					g_pEng->RegistryValueSet(hKey, "Export Camera", &m_Option.bExportCamera, 4);
					g_pEng->RegistryValueSet(hKey, "Export Light", &m_Option.bExportLight, 4);
					g_pEng->RegistryValueSet(hKey, "Export Geometry", &m_Option.bExportGeometry, 4);
					g_pEng->RegistryValueSet(hKey, "Export Dummy", &m_Option.bExportDummy, 4);
					g_pEng->RegistryValueSet(hKey, "Export Character", &m_Option.bExportCharacter, 4);

					g_pEng->RegistryValueSet(hKey, "Animation Key", &m_Option.bAnimationKey, 4);
					g_pEng->RegistryValueSet(hKey, "Generate File Name", &m_Option.bGenerateFileName, 4);
					g_pEng->RegistryValueSet(hKey, "Generate Smooth Normal", &m_Option.bGenerateSmoothNormal, 4);
//					g_pEng->RegistryValueSet(hKey, "Generate Progressive Mesh", &m_Option.bGenerateProgressiveMesh, 4);
					g_pEng->RegistryValueSet(hKey, "Generate Half Size Texture", &m_Option.bGenerateHalfSizeTexture, 4);
					g_pEng->RegistryValueSet(hKey, "Generate Compressed Texture", &m_Option.bGenerateCompressedTexture, 4);
					
					g_pEng->RegistryClose(hKey);
				}
			}
			return TRUE;
		case IDCANCEL:
			{
				EndDialog(hDlg, 0);
				return TRUE;
			}
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	default:
		break;
	}
	
	return FALSE;
}
