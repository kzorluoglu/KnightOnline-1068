//////////////////////////////////////////////////////////////////////
//
// Jpeg.h: interface for the CJpeg class.
//
// �ֿ䵿�� : JPEG ������ ���ڵ��Ͽ� �о���̰ų� ��ũ�� ����.
//
// ������ : ��
// ������ : ������
// E-Mail : evaasuka1004@hotmail.com
//////////////////////////////////////////////////////////////////////

#ifndef _JPEG_MODULE_BODY_
#define _JPEG_MODULE_BODY_

struct SET
{
	BYTE C[3]; // Y, Cb, Cr ����
};

struct DQT
{
	BYTE Q[64]; // Qantization Table ��
};

struct DHT
{
	BOOL Flag; // ���Ǿ����� ���θ� ��Ÿ���� �÷���
	int Num; // ������ �ڵ��� ��
	WORD *HUFFCODE; // ������ �ڵ�
	BYTE *HUFFSIZE; // ������ �ڵ��� ����
	BYTE *HUFFVAL;  // ������ �ڵ尡 ��Ÿ���� ��
	WORD MAXCODE[17]; // �ش� ���̿��� ���� ū �ڵ�
	WORD MINCODE[17]; // �ش� ���̿��� ���� ���� �ڵ�
	int VALPTR[17]; // �ش� ������ �ڵ尡 ���۵Ǵ� �ε���
	int *PT; // VALUE�� INDEX�� ������ ã�� ���� ������
};

struct FRAMEHEADER
{
	WORD Y; // �̹����� ����
	WORD X; // �̹����� ����
	BYTE Nf; // ������Ʈ ��
	BYTE C[3]; // ������Ʈ ���̵�
	BYTE H[3]; // ������Ʈ�� Horizontal Sampling Factor
	BYTE V[3]; // ������Ʈ�� Vertical Sampling Factor
	BYTE Tq[3]; // �ش� ������Ʈ�� ���Ǵ� ����ȭ���̺� ��ȣ
};

struct SCANHEADER
{
	BYTE Ns; // ������Ʈ ��
	BYTE Cs[3]; // ������Ʈ ���̵�
	BYTE Td[3]; // ������Ʈ�� DC Huffman Table ��ȣ
	BYTE Ta[3]; // ������Ʈ�� AC Huffman Table ��ȣ
	BYTE Ss;
	BYTE Se;
	BYTE Ah;
	BYTE Al;
};


class CJpeg  
{
public:

	// JPEG File�� Load�ϱ� ���� �Լ��� //
	void LoadJPG(LPCSTR FileName); // JPEG File�� Load�ϴ� �Լ�	
	void FindSOI(); // Start of Image ��Ŀ�� ã�� �Լ�
	void FindDQT(); // Quantization Table�� ã�� ����ü�� �����ϴ� �Լ�
	void FindDHT();	// Huffman Table�� ã�� ����ü�� �����ϴ� �Լ�
	void FindSOF(); // Frame Header�� ã�� ����ü�� �����ϴ� �Լ�
	void FindSOS(); // Scan Header�� ã�� ����ü�� �����ϴ� �Լ�
	void FindETC(); // DRI(Define Restart Interval) �ε� 
	void Decode();  // ���ڵ带 ���� ������ �����ϰ� ���ڵ带 ����
	void DecodeMCU(int mx, int my); // MCU������ ���ڵ��ϴ� �Լ�
	void DecodeDU(int N); // 8x8 Data Unit�� ���ڵ��ϴ� �Լ�
	void IDCT(); // Inverse DCT�� �ϴ� �Լ�
	void Zigzag(); // Zigzag������ �Ǿ��ִ� DU�� ���󺹱ͽ�Ű�� �Լ�
	void DecodeAC(int Th); // DU��, AC������ ���ڵ��ϴ� �Լ�
	void DecodeDC(int Th); // DU��, DC������ ���ڵ��ϴ� �Լ�
	short Extend(WORD V, BYTE T); // V�� ī�װ��� T�� �µ��� Ȯ��
	WORD Receive(BYTE SSSS); // ���ۿ��� SSSS��Ʈ��ŭ �о���� �Լ�
	BYTE hDecode(int Th); // ������ ��ȣ�� ���ڵ��ϴ� �κ�
	BYTE NextByte(); // ���ۿ��� ���� 1 ����Ʈ�� �о���� �Լ�
	WORD NextBit(); // ���ۿ��� ���� 1 ��Ʈ�� �о���� �Լ�
	void ConvertYUV2RGB(); // ���ڵ�� �����͸� �÷����� �ٲް� ���ÿ�
						   // ��Ʈ�ʿ� ȣȯ�ǵ��� ��ȯ�ϴ� �Լ�
	
	
	// JPEG File�� Save�ϱ� ���� �Լ��� //	
	void SaveJPG(LPCSTR FileName, int Width, int Height, BYTE *pp); // JPEG ������ �����ϴ� �Լ�

	void PutSOI(HANDLE hFile); // Start of Image ��Ŀ�� ����
	void PutDQT(HANDLE hFile); // Quantizatino Table�� ����
	void PutDHT(HANDLE hFile); // Huffmann Table�� ����
	void PutSOF(HANDLE hFile, int Width, int Height); // FrameHeader�� ����
	void PutSOS(HANDLE hFile); // ScanHeader�� ����
	void PutEOI(HANDLE hFile); // End of Image ��Ŀ�� ����

	void ShotBit(BYTE Bit); // 1��Ʈ�� ���ۿ� �����ϴ� �Լ�
	void ChargeCode(WORD Code, int Size ); // Size��ŭ�� ����(�Ʒ��ڸ��κ���)�� Code�� ���ۿ� �����ϴ� �Լ�
	void EncodeDU(short *pos, BOOL Flag, int bWidth); // 8x8 Block�� ���ڵ��ϴ� �Լ�
	void hEncode(int bWidth, int bHeight); // ������ ���ڵ� �ϴ� �κ�
	BYTE GetCategory(short V); // �־��� ���� ī�װ����� ���ϴ� �Լ�	
	void Zigzag2(); // Zigzag������ ����� �Լ�
	void DCT(short *pos, int bWidth, BOOL Flag); // DCT�� �� �� Zigzag �� Quantization �ϴ� �Լ�

	int GetHeight(); // �̹����� ���̸� ��ȯ�ϴ� �Լ�
	int GetWidth();	// �̹����� ���̸� ��ȯ�ϴ� �Լ�
	
	WORD Ri; // Restart Interval
	int m_rWidth; // �̹����� �������� ����
	int m_rHeight; // �̹����� �������� ����
	BYTE * pByte; // NextByte()�Լ����� ����
	
	int cnt; // ��Ʈ������ ������ �� ���̴� ī����
	
	short *Y;  // Save�� �� ���̴� Y Buffer
	short *Cb; // Save�� �� ���̴� Cb Buffer
	short *Cr; // Save�� �� ���̴� Cr Buffer

	SET *MCU; // MCU ������ ����
	
	BYTE Hmax; // Maximum Horizontal Sampling Factor
	BYTE Vmax; // Maximum Vertical Sampling Factor
	
	BYTE * m_pData;	// �̹��� ����
	BYTE * m_pBuf; // ����
	int m_Index; // ������ ��ġ�� ��Ÿ���� �ε���
	DQT TbQ[20]; // Quantization Table
	DHT TbH[20]; // Huffman Table
	short ZZ[64]; // 8x8 Block ������ ��� �迭
	FRAMEHEADER FrameHeader; // FrameHeader ����ä
	SCANHEADER ScanHeader; // ScanHeader ����ü
	short PrevDC[3]; // DC ������ Predictor

	
	CJpeg();
	virtual ~CJpeg();

};

#endif //_JPEG_MODULE_BODY_
