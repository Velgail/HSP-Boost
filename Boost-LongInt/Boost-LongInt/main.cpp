
//
//		HSP3.0 plugin sample
//		onion software/onitama 2004/9
//

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define NOMINMAX
#include <Windows.h>
#include <cstdio>
#include <cstdlib>
#include "../../HSP_Includes/hsp3plugin.h"

#include "Boost-LongInt.h"

#include <boost/multiprecision/cpp_int.hpp>
/*------------------------------------------------------------*/
/*
controller
*/
/*------------------------------------------------------------*/
typedef boost::multiprecision::cpp_int boost_longint;
static boost_longint ref_fval;						// 返値のための変数
static double dp1;

static void *reffunc(int *type_res, int cmd)
{
	//		関数・システム変数の実行処理 (値の参照時に呼ばれます)
	//
	//			'('で始まるかを調べる
	//
	if (*type != TYPE_MARK) puterror(HSPERR_INVALID_FUNCPARAM);
	if (*val != '(') puterror(HSPERR_INVALID_FUNCPARAM);
	code_next();


	switch (cmd) {							// サブコマンドごとの分岐

	case 0x00:								// boost_longint関数

		dp1 = code_getd();					// 整数値を取得(デフォルトなし)
		ref_fval = (boost_longint)dp1;				// 返値を設定
		break;

	default:
		puterror(HSPERR_UNSUPPORTED_FUNCTION);
	}

	//			'('で終わるかを調べる
	//
	if (*type != TYPE_MARK) puterror(HSPERR_INVALID_FUNCPARAM);
	if (*val != ')') puterror(HSPERR_INVALID_FUNCPARAM);
	code_next();

	*type_res = HspVarBoost_Longint_typeid();		// 返値のタイプを指定する
	return (void *)&ref_fval;
}


/*------------------------------------------------------------*/

static int termfunc(int option)
{
	//		終了処理 (アプリケーション終了時に呼ばれます)
	//
	return 0;
}


/*------------------------------------------------------------*/
/*
interface
*/
/*------------------------------------------------------------*/

int WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
{
	//		DLLエントリー (何もする必要はありません)
	//
	return TRUE;
}


EXPORT void WINAPI hsp3cmdinit(HSP3TYPEINFO *info)
{
	//		プラグイン初期化 (実行・終了処理を登録します)
	//
	hsp3sdk_init(info);		// SDKの初期化(最初に行なって下さい)

	info->reffunc = reffunc;		// 参照関数(reffunc)の登録
	info->termfunc = termfunc;		// 終了関数(termfunc)の登録

	registvar(-1, HspVarBoost_Longint_Init);		// 新しい型の追加
}

/*------------------------------------------------------------*/

