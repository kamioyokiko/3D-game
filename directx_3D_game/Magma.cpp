//=============================================================================
//
// �t�B�[���h���� [MeshField.cpp]
// Author : ���V�r�a
//
//============================================================================
#include "Magma.h"
#include "Input.h"
#include "Manager.h"
#include "main.h"
#include "Renderer.h"

#define NUM_VTX_INDEX	(14)
#define TEX_SIZE_X		(90)			// �e�N�X�`���̃T�C�Y�̓|���S����
#define TEX_SIZE_Z		(90)			// �e�N�X�`���̃T�C�Y�̓|���S���c
#define NUM_BLOCK_X		(50)			//�t�B�[���h�̉��̐�
#define NUM_BLOCK_Z		(50)			//�t�B�[���h�̏c�̐�


static D3DXVECTOR3 NORMAL_MAP[100*100];



//=============================================================================
// �R���X�g���N�^
//=============================================================================
CMagma::CMagma(OBJTYPE nObjType,int nPriority):CScene3D(nObjType,nPriority)
{
	n_pVtx=NULL;
	n_pD3DDevice=NULL;
	n_pD3DVtexBuff=NULL;
	n_pD3DIndexBuff=NULL;
	n_pD3DTex=NULL;
	m_Pos=D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_Rot=D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_Scl=D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_Rotate=0.00004f;

}

//=============================================================================
// �f�X�g���N�^
//=============================================================================
CMagma::~CMagma()
{

}

//=============================================================================
// ����������
//=============================================================================
HRESULT CMagma::Init(LPDIRECT3DDEVICE9 device)
{
	m_Rot = D3DXVECTOR3(0,0,0);
	m_Scl = D3DXVECTOR3(1.0f,1.0f,1.0f);

	float n=0;
	m_fWaterTime = 0.f;
	m_Color = 255;
	m_ColorAdd = 1;

	for(int i=0;i<50*50;i++)
	{
		n=(float)(rand()%50);
		n /= 3;
	}

	//�p�x���K��
	if(CMagma::m_Rot.y < -D3DX_PI)
	{
		CMagma::m_Rot.y = D3DX_PI;
	}
	else if(CMagma::m_Rot.y > D3DX_PI)
	{
		CMagma::m_Rot.y = -D3DX_PI;
	}

	n_nNumVertexIndex = NUM_VTX_INDEX;
	n_pD3DDevice = device;

	//m_nNumBlockX * m_nNumBlockZ�̃��b�V���쐬
	m_nNumBlockX = NUM_BLOCK_X;
	m_nNumBlockZ = NUM_BLOCK_Z;
	//�|���S���ꖇ�̃T�C�Y
	m_fSizeBlockX = TEX_SIZE_X;
	m_fSizeBlockZ = TEX_SIZE_Z;

	// �����_��
	m_nNumVertex = (m_nNumBlockX+1)*(m_nNumBlockZ+1);
	// ���|���S����
	m_nNumPolygon = m_nNumBlockX*m_nNumBlockZ*2+(m_nNumBlockZ-1)*4;

	// ���_�̑��C���f�b�N�X��
	m_nNumVertexIndex = (m_nNumBlockX+1)*(m_nNumBlockZ+1)+(m_nNumBlockZ-1)*(3+m_nNumBlockX);

	// ���_���̃Z�b�g
	if(FAILED(InitMeshFieldVerTex(m_nNumBlockX, m_nNumBlockZ, m_fSizeBlockX, m_fSizeBlockZ)))
	{
		MessageBox(NULL, "InitVerTex():���_���̏������Ɏ��s���܂���", "�x��" ,MB_OK);
		return E_FAIL;
	}

	//�|���S���̐ݒ�
	D3DXCreateTextureFromFile(device,"data/TEXTURE/magmas2.jpg",&n_pD3DTex);

	return S_OK;
}

//=============================================================================
// ���_���̏���������
//=============================================================================
HRESULT CMagma::InitMeshFieldVerTex(int nNumBlockX, int nNumBlockZ, float fSizeBlockX, float fSizeBlockZ)
{
	// �z��ԍ�
	int nNum = 0;
	int nNum2 = 0;
	// ���_���v�Z�p
	float fPosX, fPosZ;
	double dTexU, dTexV;
	// i�st��
	int t = 0;
	int i = 0;

	// �C���f�b�N�X���̃Z�b�g
	WORD *pIndex;

	D3DXVECTOR3 line1;
	D3DXVECTOR3 line2;
	D3DXVECTOR3 Vec1;
	D3DXVECTOR3 Vec2;


	// ���_�o�b�t�@�������m�ہI�i������Y�ꂸ�Ɂj
	if(FAILED(n_pD3DDevice -> CreateVertexBuffer(sizeof(VERTEX_3D)*m_nNumVertex,
											D3DUSAGE_WRITEONLY,
											FVF_VERTEX_3D,
											D3DPOOL_MANAGED,
											&n_pD3DVtexBuff,
											NULL)))
	{
		return E_FAIL;
	}

	// index�o�b�t�@�������m�ہI�i������Y�ꂸ�Ɂj
	if(FAILED(n_pD3DDevice -> CreateIndexBuffer(sizeof(WORD)*m_nNumVertexIndex,
											D3DUSAGE_WRITEONLY,
											D3DFMT_INDEX16,
											D3DPOOL_MANAGED,
											&n_pD3DIndexBuff,
											NULL)))
	{
		return E_FAIL;
	}

	//���b�N
	n_pD3DVtexBuff -> Lock(0, 0, (void**)&pVtx, 0);

	// i�st��ɒ��_�ݒ�
	for(i = 0; i <= nNumBlockZ; i++)
	{
		dTexU = 0;
		dTexV = (double)i/TEX_SIZE_Z;
		for(t = 0; t <= nNumBlockX; t++)
		{
			nNum = i * (nNumBlockX+1) + t;

			fPosX = m_Pos.x + (fSizeBlockX * t);
			fPosZ = m_Pos.z + (-fSizeBlockZ * i);
			//fPosX = m_fSizeBlockX * (m_nNumBlockX/2) - (fSizeBlockX * t);
			//fPosZ = m_fSizeBlockZ * (m_nNumBlockZ/2) - (fSizeBlockZ * i);
			dTexU = (double)-t/TEX_SIZE_X;

			pVtx[nNum].vtx = D3DXVECTOR3(fPosX, 0.f, fPosZ);		// �������ɂ��邱�ƂŁA�e�Ȃǂ����ꂢ�ɏo��
			//pVtx[nNum].vtx = D3DXVECTOR3(fPosX,0.0, fPosZ);					// �������ɂ��邱�ƂŁA�e�Ȃǂ����ꂢ�ɏo��
			//pVtx[nNum].nor = D3DXVECTOR3(0,1,0);
			pVtx[nNum].diffuse = D3DCOLOR_RGBA(m_Color, m_Color, m_Color, 255);
			pVtx[nNum].tex = D3DXVECTOR2(t*1.0f,i*1.0f);
		}
	}
	//	// �x�N�g���������NorMap�Ɋi�[
	//for(i = 0; i <= nNumBlockZ; i++)
	//{
	//	for(t = 0; t <= nNumBlockX; t++)
	//	{
	//		if(t == nNumBlockX)
	//		{
	//			continue;
	//		}
	//		nNum = i * (nNumBlockX) + t;
	//		nNum2 = i * (nNumBlockX + 1) + t;
	//		line1 = pVtx[nNum2 + 1].vtx - pVtx[nNum2].vtx; 
	//		line2 = pVtx[nNum2 + nNumBlockZ + 1].vtx - pVtx[nNum2].vtx; 

	//		D3DXVec3Cross(&Vec1,&line1,&line2);

	//		line1 = pVtx[nNum2 + nNumBlockZ + 1].vtx - pVtx[nNum2 + nNumBlockZ + 2].vtx; 
	//		line2 = pVtx[nNum2 + 1].vtx - pVtx[nNum2 + nNumBlockZ + 2].vtx; 

	//		D3DXVec3Cross(&Vec2,&line1,&line2);

	//		Vec1 = (Vec1 + Vec2) / 2;

	//		D3DXVec3Normalize(&Vec1,&Vec1);

	//		NORMAL_MAP[nNum] = Vec1;
	//	}
	//}


	// �@���x�N�g���Z�o
	for(i = 0; i <= nNumBlockZ; i++)
	{
		for(t = 0; t <= nNumBlockX; t++)
		{
			nNum = i * (nNumBlockX) + t;
			nNum2 = i * (nNumBlockX+1) + t;

			//����
			if( i == 0 && t == 0)
			{
				Vec1 = NORMAL_MAP[nNum];
			}else
			//����
			if( i == nNumBlockZ && t == 0)
			{
				Vec1 = NORMAL_MAP[nNum - nNumBlockX + nNumBlockZ];
			}else
			//�E��
			if( i == 0 && t == nNumBlockX)
			{
				Vec1 = NORMAL_MAP[nNum - 1];
			}else
			//�E��
			if( i == nNumBlockZ && t == nNumBlockX)
			{
				Vec1 = NORMAL_MAP[nNum2 - i - 1];
			}else
			//���[
			if( i < nNumBlockZ && i > 0 && t == 0)
			{
				Vec1 = (NORMAL_MAP[nNum - i] + NORMAL_MAP[nNum - nNumBlockX - i]) / 2;
			}else
			//�E�[
			if( i < nNumBlockZ && i > 0 && t == nNumBlockX)
			{
				Vec1 = (NORMAL_MAP[nNum - i - 1] + NORMAL_MAP[nNum - nNumBlockX - i - 1]) / 2;
			}else
			//��[
			if( i == 0 && t < nNumBlockX && t > 0)
			{
				Vec1 = (NORMAL_MAP[0] + NORMAL_MAP[1]) / 2;
			}else
			//���[
			if( i == nNumBlockZ && t < nNumBlockX && t > 0)
			{
				Vec1 = (NORMAL_MAP[0] + NORMAL_MAP[1]) / 2;
			}else
			//�^��
			if( i < nNumBlockZ && i > 0 && t < nNumBlockX && t > 0)
			{
				Vec1 = NORMAL_MAP[nNum -  i] + NORMAL_MAP[nNum - i - 1] + NORMAL_MAP[nNum - i - nNumBlockX] + NORMAL_MAP[nNum - i - nNumBlockX - 1] / 4;
			}

			D3DXVec3Normalize(&Vec1,&Vec1);
			pVtx[nNum].nor = Vec1;
		}
	}
	// �A�����b�N
	n_pD3DVtexBuff -> Unlock();

	// ���b�N
	n_pD3DIndexBuff -> Lock(0, 0, (void**)&pIndex, 0);

	// i�st��
	i=0;
	t=0;

	for(int j = 0; j < m_nNumVertexIndex; ++j)
	{
		// �k�ރ|���S��
		if(j == 2*(nNumBlockX+1)+t*(2*(nNumBlockX+1)+2))
		{
			pIndex[j] =  (i -1) + t*(nNumBlockX+1);
			pIndex[j+1] = i + (t+1)*(nNumBlockX+1);
			// �i�������
			j += 2;
			t++;
			i = 0;
		}
		// ��
		if(j % 2 == 0)
		{
			pIndex[j] = i + (t+1)*(nNumBlockX+1);
		}
		// ��
		else
		{
			pIndex[j] = i + t*(nNumBlockX+1);
			++i;
		}
	}

	// �A�����b�N
	n_pD3DIndexBuff -> Unlock();

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void CMagma::Uninit()
{
	// �e�N�X�`���̊J��
	if(n_pD3DTex != NULL)
	{
		n_pD3DTex -> Release();
		n_pD3DTex = NULL;
	}

	// ���_�o�b�t�@���
	if(n_pD3DVtexBuff != NULL)
	{
		n_pD3DVtexBuff -> Release();
		n_pD3DVtexBuff = NULL;
	}

	// ���_�C���f�b�N�X�o�b�t�@���
	if(n_pD3DIndexBuff != NULL)
	{
		n_pD3DIndexBuff -> Release();
		n_pD3DIndexBuff = NULL;
	}

	CScene::Release();
}

//=============================================================================
// �X�V����
//=============================================================================
void CMagma::Update()
{
	// �o�ߎ��ԑ��x
	const float dt = 0.01f;

	// �U��
	const float amplitube = 4.f;

	// �F�ύX
	if (m_Color > 254 || m_Color < 140)
	{
		m_ColorAdd *= -1;
	}

	// �F���Z
	m_Color+=m_ColorAdd;

	//���b�N
	n_pD3DVtexBuff->Lock(0, 0, (void**)&pVtx, 0);

	// i�st��ɒ��_�ݒ�
	for (int i = 0; i <= m_nNumBlockZ; i++)
	{
		for (int t = 0; t <= m_nNumBlockX; t++)
		{
			// 
			int nNum = i * (m_nNumBlockX + 1) + t;

			// ���S�̃C���f�b�N�X
			int centerIndex = (m_nNumBlockZ / 2) * (m_nNumBlockX + 1) + (m_nNumBlockX / 2);

			// �g�䔭���_����̋���
			float length = sqrtf(
				(pVtx[nNum].vtx.x - pVtx[centerIndex].vtx.x) * (pVtx[nNum].vtx.x - pVtx[centerIndex].vtx.x) + 
				(pVtx[nNum].vtx.z - pVtx[centerIndex].vtx.z) * (pVtx[nNum].vtx.z - pVtx[centerIndex].vtx.z)	);

			// �g�̍���
			float fPosY = sinf(-m_fWaterTime * 3.f + length * 0.02f) *
				amplitube * (1000 - length) * 0.0015f ;

			pVtx[nNum].vtx.y = fPosY;		// �������ɂ��邱�ƂŁA�e�Ȃǂ����ꂢ�ɏo��

			// �F���㉺�����邱�ƂŃ}�O�}�������o
			pVtx[nNum].diffuse = D3DCOLOR_RGBA(m_Color, m_Color, m_Color, 255);
		}
	}

	// �A�����b�N
	n_pD3DVtexBuff->Unlock();

	// �o�ߎ���
	m_fWaterTime += dt;

	m_Rot.y+=m_Rotate;


	if(m_Rot.y > 0.38f)
	{
		m_Rotate=-m_Rotate;
	}
	if(m_Rot.y < 0)
	{
		m_Rotate=-m_Rotate;
	}


}

//=============================================================================
// �`�揈��
//=============================================================================
void CMagma::Draw()
{
	D3DXMATRIX mtxWorld,mtxScl,mtxRot,mtxTranslate;

	// ���C�g����
	n_pD3DDevice->SetRenderState(D3DRS_LIGHTING , FALSE);

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&mtxWorld);
	//�X�P�[���𔽉f
	D3DXMatrixScaling(&mtxScl,m_Scl.x,m_Scl.y,m_Scl.z);
	D3DXMatrixMultiply(&mtxWorld,&mtxWorld,&mtxScl);

	//��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot,m_Rot.y,m_Rot.x,m_Rot.z);
	D3DXMatrixMultiply(&mtxWorld,&mtxWorld,&mtxRot);

	//�ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTranslate,-1000,m_Pos.y,1000);
	D3DXMatrixMultiply(&mtxWorld,&mtxWorld,&mtxTranslate);

	//���[���h�}�g���b�N�X�̐ݒ�
	n_pD3DDevice->SetTransform(D3DTS_WORLD,&mtxWorld);
	n_pD3DDevice->SetIndices(n_pD3DIndexBuff);

	//���_�t�H�[�}�b�g�̐ݒ�
	n_pD3DDevice->SetFVF(FVF_VERTEX_3D);
	//�e�N�X�`���̐ݒ�
	n_pD3DDevice->SetTexture(0,n_pD3DTex);
	n_pD3DDevice->SetStreamSource(0,n_pD3DVtexBuff,0,sizeof(VERTEX_3D));

	//�|���S��
	n_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,0,0,m_nNumVertex,0,m_nNumPolygon);

	// ���C�g�߂�
	n_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

}

//=============================================================================
// �I�u�W�F�N�g����
//=============================================================================
CMagma* CMagma::Create(LPDIRECT3DDEVICE9 device,float fPosX,float fPosY,float fPosZ)
{
	CMagma* pMagma;
	pMagma = new CMagma(OBJTYPE_MESHMAGMA);
	
	pMagma ->m_Pos=D3DXVECTOR3(fPosX,fPosY,fPosZ);
	pMagma->SetPos(fPosX,fPosY,fPosZ);
	pMagma ->Init(device);
	return pMagma;
}


float CMagma::GetHeight(D3DXVECTOR3 Pos,D3DXVECTOR3* pNormal)
{
	D3DXVECTOR3 Vec0,Vec1;
	int nNum=0;
	// �@���x�N�g��������
	for(int i = 0; i <= m_nNumBlockZ; i++)
	{
		for(int t = 0; t <= m_nNumBlockX; t++)
		{
			nNum = i * (m_nNumBlockX) + t;
			Vec0=pVtx[nNum].vtx-pVtx[nNum+m_nNumBlockX+1].vtx;
			Vec1=Pos-pVtx[nNum+m_nNumBlockX+1].vtx;

			//2D�̊O��
			if(Vec0.z * Vec1.x - Vec0.x * Vec1.z >=0)
			{
				Vec0=pVtx[nNum+m_nNumBlockX+2].vtx-pVtx[nNum].vtx;
				Vec1=Pos-pVtx[nNum].vtx;

				if(Vec0.z*Vec1.x-Vec0.x*Vec1.z >=0)
				{
					Vec0=pVtx[nNum+m_nNumBlockX+1].vtx-pVtx[nNum+m_nNumBlockX+2].vtx;
					Vec1=Pos-pVtx[nNum+m_nNumBlockX+2].vtx;

					if(Vec0.z*Vec1.x-Vec0.x*Vec1.z >=0)
					{
						//����Ă���
						return GetHeightPolygon(pVtx[nNum+m_nNumBlockX+1].vtx,
										pVtx[nNum+m_nNumBlockX+2].vtx,
										pVtx[nNum].vtx,
										Pos,
										pNormal);
					}
				}
			}


			Vec0=pVtx[nNum+1].vtx-pVtx[nNum].vtx;
			Vec1=Pos-pVtx[nNum].vtx;

			//2D�̊O��
			if(Vec0.z * Vec1.x - Vec0.x * Vec1.z >=0)
			{
				Vec0=pVtx[nNum+m_nNumBlockX+2].vtx-pVtx[nNum+1].vtx;
				Vec1=Pos-pVtx[nNum+1].vtx;

				if(Vec0.z * Vec1.x - Vec0.x * Vec1.z >=0)
				{
					Vec0=pVtx[nNum].vtx-pVtx[nNum+m_nNumBlockX+2].vtx;
					Vec1=Pos-pVtx[nNum+m_nNumBlockX+2].vtx;

					if(Vec0.z * Vec1.x - Vec0.x * Vec1.z >=0)
					{
						//����Ă���
						return GetHeightPolygon(pVtx[nNum+1].vtx,
										pVtx[nNum+m_nNumBlockX].vtx,
										pVtx[nNum+m_nNumBlockX+2].vtx,
										Pos,
										pNormal);
					}
				}
			}
		}
	}
	return 0.0f;
}


float CMagma::GetHeightPolygon(const D3DXVECTOR3& P0,const D3DXVECTOR3& P1,const D3DXVECTOR3& P2,const D3DXVECTOR3& Pos,D3DXVECTOR3* pNormal)
{
	D3DXVECTOR3 Normal,Vec0,Vec1;
	float Height;

	Vec0=P1-P0;
	Vec1=P2-P0;
	D3DXVec3Cross(&Normal,&Vec0,&Vec1);
	D3DXVec3Normalize(&Normal,&Normal);
	pNormal=&Normal;

	if(Normal.y==0.0)
	{
		return 0.0f;
	}

	//����
	Height=P0.y-(Normal.x*(Pos.x-P0.x)+Normal.z*(Pos.z-P0.z))/Normal.y;

	return Height;
}