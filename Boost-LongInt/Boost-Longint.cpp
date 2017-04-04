
//
//	HSPVAR core module
//	onion software/onitama 2003/4
//
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define NOMINMAX
#include <Windows.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <memory>
#include "../HSP_Includes/hsp3plugin.h"
#include "../HSP_Includes/hspvar_core.h"
#include "../HSP_Includes/hsp3debug.h"
#include <boost/multiprecision/cpp_int.hpp>
/*------------------------------------------------------------*/
/*
HSPVAR core interface (Boost_Longint)
*/
/*------------------------------------------------------------*/
typedef boost::multiprecision::cpp_int boost_longint;
#define GetPtr(pval) ((boost_longint *)pval)
#define sbAlloc hspmalloc
#define sbFree hspfree

static int mytype;
static boost_longint conv;
static short *aftertype;
static std::string sstr;
static int sint;
static double sdouble;


// Core
static PDAT *HspVarBoost_Longint_GetPtr(PVal *pval)
{
	return (PDAT *)(((boost_longint *)(pval->pt)) + pval->offset);
}

static void *HspVarBoost_Longint_Cnv(const void *buffer, int flag)
{
	//		リクエストされた型 -> 自分の型への変換を行なう
	//		(組み込み型にのみ対応でOK)
	//		(参照元のデータを破壊しないこと)
	//
	switch (flag) {
	case HSPVAR_FLAG_STR:
		conv = boost_longint(std::string((char*)buffer));
		return &conv;
	case HSPVAR_FLAG_INT:
		conv = (boost_longint)(*(int *)buffer);
		return &conv;
	case HSPVAR_FLAG_DOUBLE:
		conv = (boost_longint)(*(double *)buffer);
		break;
	default:
		throw HSPVAR_ERROR_TYPEMISS;
	}
	return (void *)buffer;
}


static void *HspVarBoost_Longint_CnvCustom(const void *buffer, int flag)
{
	//		(カスタムタイプのみ)
	//		自分の型 -> リクエストされた型 への変換を行なう
	//		(組み込み型に対応させる)
	//		(参照元のデータを破壊しないこと)
	//
	boost_longint p;
	p = *(boost_longint *)buffer;
	std::string sdata;
	switch (flag) {
	case HSPVAR_FLAG_STR:
		sstr= p.str();
		return const_cast<char *>(sstr.c_str());
		break;
	case HSPVAR_FLAG_INT:
		sint = static_cast<int>(p);
		return &sint;
		break;
	case HSPVAR_FLAG_DOUBLE:
		sdouble = static_cast<double>(p);
		return &sdouble;
		break;
	default:
		throw HSPVAR_ERROR_TYPEMISS;
	}
	return nullptr;
}


static int GetVarSize(PVal *pval)
{
	//		PVALポインタの変数が必要とするサイズを取得する
	//		(sizeフィールドに設定される)
	//
	int size;
	size = pval->len[1];
	if (pval->len[2]) size *= pval->len[2];
	if (pval->len[3]) size *= pval->len[3];
	if (pval->len[4]) size *= pval->len[4];
	size *= sizeof(boost_longint);
	return size;
}


static void HspVarBoost_Longint_Free(PVal *pval)
{
	//		PVALポインタの変数メモリを解放する
	//
	if (pval->mode == HSPVAR_MODE_MALLOC) { sbFree(pval->pt); }
	pval->pt = NULL;
	pval->mode = HSPVAR_MODE_NONE;
}


static void HspVarBoost_Longint_Alloc(PVal *pval, const PVal *pval2)
{
	//		pval変数が必要とするサイズを確保する。
	//		(pvalがすでに確保されているメモリ解放は呼び出し側が行なう)
	//		(flagの設定は呼び出し側が行なう)
	//		(pval2がNULLの場合は、新規データ)
	//		(pval2が指定されている場合は、pval2の内容を継承して再確保)
	//
	int i, size;
	char *pt;
	boost_longint *fv;
	if (pval->len[1] < 1) pval->len[1] = 1;		// 配列を最低1は確保する
	size = GetVarSize(pval);
	pval->mode = HSPVAR_MODE_MALLOC;
	pt = sbAlloc(size);
	fv = (boost_longint *)pt;
	for (i = 0; i<(int)(size / sizeof(boost_longint)); i++) { fv[i] = 0; }
	if (pval2 != NULL) {
		memcpy(pt, pval->pt, pval->size);
		sbFree(pval->pt);
	}
	pval->pt = pt;
	pval->size = size;
}

/*
static void *HspVarBoost_Longint_ArrayObject( PVal *pval, int *mptype )
{
//		配列要素の指定 (文字列/連想配列用)
//
throw HSPERR_UNSUPPORTED_FUNCTION;
return NULL;
}
*/

// Size
static int HspVarBoost_Longint_GetSize(const PDAT *pval)
{
	return sizeof(boost_longint);
}

// Set
static void HspVarBoost_Longint_Set(PVal *pval, PDAT *pdat, const void *in)
{
	*GetPtr(pdat) = *((boost_longint *)(in));
}

// Add
static void HspVarBoost_Longint_AddI(PDAT *pval, const void *val)
{
	*GetPtr(pval) += *((boost_longint *)(val));
	*aftertype = mytype;
}

// Sub
static void HspVarBoost_Longint_SubI(PDAT *pval, const void *val)
{
	*GetPtr(pval) -= *((boost_longint *)(val));
	*aftertype = mytype;
}

// Mul
static void HspVarBoost_Longint_MulI(PDAT *pval, const void *val)
{
	*GetPtr(pval) *= *((boost_longint *)(val));
	*aftertype = mytype;
}

// Div
static void HspVarBoost_Longint_DivI(PDAT *pval, const void *val)
{
	boost_longint p = *((boost_longint *)(val));
	if (p == 0) throw(HSPVAR_ERROR_DIVZERO);
	*GetPtr(pval) /= p;
	*aftertype = mytype;
}
// Mod
static void HspVarBoost_Longint_ModI(PDAT *pval, const void *val)
{
	boost_longint p = *((boost_longint *)(val));
	if (p == 0) throw(HSPVAR_ERROR_DIVZERO);
	*GetPtr(pval) %= p;
	*aftertype = mytype;
}
// And
static void HspVarBoost_Longint_AndI(PDAT *pval, const void *val)
{
	boost_longint p = *((boost_longint *)(val));
	if (p == 0) throw(HSPVAR_ERROR_DIVZERO);
	*GetPtr(pval) &= p;
	*aftertype = mytype;
}
// Or
static void HspVarBoost_Longint_OrI(PDAT *pval, const void *val)
{
	boost_longint p = *((boost_longint *)(val));
	if (p == 0) throw(HSPVAR_ERROR_DIVZERO);
	*GetPtr(pval) |= p;
	*aftertype = mytype;
}
// Xor
static void HspVarBoost_Longint_XorI(PDAT *pval, const void *val)
{
	boost_longint p = *((boost_longint *)(val));
	if (p == 0) throw(HSPVAR_ERROR_DIVZERO);
	*GetPtr(pval) ^= p;
	*aftertype = mytype;
}
static void HspVarBoost_Longint_RrI(PDAT *pval, const void *val)
{
	boost_longint p = *((boost_longint *)(val));
	if (p == 0) throw(HSPVAR_ERROR_DIVZERO);
	*GetPtr(pval) >>= static_cast<int>(p);
	*aftertype = mytype;
}
// Xor
static void HspVarBoost_Longint_LrI(PDAT *pval, const void *val)
{
	boost_longint p = *((boost_longint *)(val));
	if (p == 0) throw(HSPVAR_ERROR_DIVZERO);
	*GetPtr(pval) <<= static_cast<int>(p);
	*aftertype = mytype;
}
// Eq
static void HspVarBoost_Longint_EqI(PDAT *pval, const void *val)
{
	*((int *)pval) = (*GetPtr(pval) == *((boost_longint *)(val)));
	*aftertype = HSPVAR_FLAG_INT;
}

// Ne
static void HspVarBoost_Longint_NeI(PDAT *pval, const void *val)
{
	*((int *)pval) = (*GetPtr(pval) != *((boost_longint *)(val)));
	*aftertype = HSPVAR_FLAG_INT;
}

// Gt
static void HspVarBoost_Longint_GtI(PDAT *pval, const void *val)
{
	*((int *)pval) = (*GetPtr(pval) > *((boost_longint *)(val)));
	*aftertype = HSPVAR_FLAG_INT;
}

// Lt
static void HspVarBoost_Longint_LtI(PDAT *pval, const void *val)
{
	*((int *)pval) = (*GetPtr(pval) < *((boost_longint *)(val)));
	*aftertype = HSPVAR_FLAG_INT;
}

// GtEq
static void HspVarBoost_Longint_GtEqI(PDAT *pval, const void *val)
{
	*((int *)pval) = (*GetPtr(pval) >= *((boost_longint *)(val)));
	*aftertype = HSPVAR_FLAG_INT;
}

// LtEq
static void HspVarBoost_Longint_LtEqI(PDAT *pval, const void *val)
{
	*((int *)pval) = (*GetPtr(pval) <= *((boost_longint *)(val)));
	*aftertype = HSPVAR_FLAG_INT;
}

/*
// INVALID
static void HspVarBoost_Longint_Invalid( PDAT *pval, const void *val )
{
throw( HSPVAR_ERROR_INVALID );
}
*/

static void *GetBlockSize(PVal *pval, PDAT *pdat, int *size)
{
	*size = pval->size - (((char *)pdat) - pval->pt);
	return (pdat);
}

static void AllocBlock(PVal *pval, PDAT *pdat, int size)
{
}


/*------------------------------------------------------------*/

EXPORT int HspVarBoost_Longint_typeid(void)
{
	return mytype;
}


EXPORT void HspVarBoost_Longint_Init(HspVarProc *p)
{
	aftertype = &p->aftertype;

	p->Set = HspVarBoost_Longint_Set;
	p->Cnv = HspVarBoost_Longint_Cnv;
	p->GetPtr = HspVarBoost_Longint_GetPtr;
	p->CnvCustom = HspVarBoost_Longint_CnvCustom;
	p->GetSize = HspVarBoost_Longint_GetSize;
	p->GetBlockSize = GetBlockSize;
	p->AllocBlock = AllocBlock;

	//	p->ArrayObject = HspVarBoost_Longint_ArrayObject;
	p->Alloc = HspVarBoost_Longint_Alloc;
	p->Free = HspVarBoost_Longint_Free;

	p->AddI = HspVarBoost_Longint_AddI;
	p->SubI = HspVarBoost_Longint_SubI;
	p->MulI = HspVarBoost_Longint_MulI;
	p->DivI = HspVarBoost_Longint_DivI;
	p->ModI = HspVarBoost_Longint_ModI;

	p->AndI = HspVarBoost_Longint_AndI;
	p->OrI  = HspVarBoost_Longint_OrI;
	p->XorI = HspVarBoost_Longint_XorI;

	p->EqI = HspVarBoost_Longint_EqI;
	p->NeI = HspVarBoost_Longint_NeI;
	p->GtI = HspVarBoost_Longint_GtI;
	p->LtI = HspVarBoost_Longint_LtI;
	p->GtEqI = HspVarBoost_Longint_GtEqI;
	p->LtEqI = HspVarBoost_Longint_LtEqI;

	p->RrI = HspVarBoost_Longint_RrI;
	p->LrI = HspVarBoost_Longint_LrI;

	p->vartype_name = "boost_longint";				// タイプ名
	p->version = 0x001;					// 型タイプランタイムバージョン(0x100 = 1.0)
	p->support = HSPVAR_SUPPORT_STORAGE | HSPVAR_SUPPORT_FLEXARRAY;
	// サポート状況フラグ(HSPVAR_SUPPORT_*)
	p->basesize = sizeof(boost_longint);		// １つのデータが使用するサイズ(byte) / 可変長の時は-1
	mytype = p->flag;
}

/*------------------------------------------------------------*/

