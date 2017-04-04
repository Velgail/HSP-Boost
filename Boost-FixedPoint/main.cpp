
//
//		HSP3.0 plugin sample
//		onion software/onitama 2004/9
//

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define NOMINMAX
#include <Windows.h>
#include <cstdio>
#include <cstdlib>
#include <unordered_map>
#include "../HSP_Includes/hsp3plugin.h"
#include "Boost-FixedPoint-Core.h"
#include "Boost-FixedPoint.h"

#include <boost/multiprecision/cpp_int.hpp>
/*------------------------------------------------------------*/
/*
controller
*/
/*------------------------------------------------------------*/
static boost_fixedpoint ref_fval;						// 返値のための変数
static double dp1;
extern int N;

static void *reffunc(int *type_res, int cmd)
{
	//		関数・システム変数の実行処理 (値の参照時に呼ばれます)
	//
	//			'('で始まるかを調べる
	//
	if (*type != TYPE_MARK) puterror(HSPERR_INVALID_FUNCPARAM);
	if (*val != '(') puterror(HSPERR_INVALID_FUNCPARAM);
	code_next();
	if (cmd >= 0) {
		N = cmd;
							// 整数値を取得(デフォルトなし)
		int cp = code_getprm();
		switch (cp) {
		case PARAM_DEFAULT:
		case PARAM_END:
		case PARAM_ENDSPLIT:
			ref_fval = boost_fixedpoint();
			break;
		default:
			switch (mpval->flag) {
			case 2:
				ref_fval = boost_fixedpoint((char*)mpval->pt);
				break;
			case 3:
				ref_fval = boost_fixedpoint(*(double*)mpval->pt);
				break;
			case 4:
				ref_fval = boost_fixedpoint(*(int*)mpval->pt);
				break;
			default:
				ref_fval = boost_fixedpoint(0);
			}
		}
						// 返値を設定
	}else{
		puterror(HSPERR_UNSUPPORTED_FUNCTION);
	}

	//			'('で終わるかを調べる
	//
	if (*type != TYPE_MARK) puterror(HSPERR_INVALID_FUNCPARAM);
	if (*val != ')') puterror(HSPERR_INVALID_FUNCPARAM);
	code_next();

	*type_res = HspVarBoost_Fixedpoint_typeid();		// 返値のタイプを指定する
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

	registvar(-1, HspVarBoost_Fixedpoint_Init);		// 新しい型の追加
}

/*------------------------------------------------------------*/

