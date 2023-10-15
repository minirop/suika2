/* -*- coding: utf-8; indent-tabs-mode: t; tab-width: 4; c-basic-offset: 4; -*- */

/*
 * Suika 2
 * Copyright (C) 2001-2023, TABATA Keiichi. All rights reserved.
 */

/*
 * Debugger (Suika2 Pro)
 *
 * [Changes]
 *  2022-06-11 作成
 *  2023-09-20 エクスポート機能強化
 *  2023-09-21 縦幅縮小
 */

#ifndef SUIKA_WINDEBUG_H
#define SUIKA_WINDEBUG_H

#include <windows.h>

/* メッセージボックスのタイトル */
#define MSGBOX_TITLE		L"Suika2 Pro"

/* バージョン文字列 */
#define VERSION_EN			L"Suika2 Pro 14\n" \
							L"A part of the Suika Studio Professional Development Suite\n" \
							L"This product is guaranteed to be available as an open source software in the future."
#define VERSION_JP			L"Suika2 Pro 14\n" \
							L"A part of the Suika Studio Professional Development Suite\n" \
							L"本製品は将来に渡ってオープンソースソフトウェアとして提供されることが保証されます。"

/* デバッガのサイズ */
#define DEBUGGER_WIDTH			(440)
#define DEBUGGER_MIN_HEIGHT		(700)

/* デバッガパネルを作成する */
BOOL InitDebuggerPanel(HWND hWndMain, HWND hWndGame, void *pWndProc);

/* デバッガウィンドウの位置を修正する */
VOID UpdateDebuggerWindowPosition(int nGameWidth, int nGameHeight);

/* スタートアップファイル/ラインを取得する */
BOOL GetStartupPosition(void);

/* デバッガのウィンドウハンドルであるかを返す */
BOOL IsDebuggerHWND(HWND hWnd);

/* デバッガのウィンドウプロシージャの処理 */
LRESULT CALLBACK WndProcDebugHook(HWND hWnd, UINT message, WPARAM wParam,
								  LPARAM lParam);

/* デバッガのWM_COMMANDハンドラ */
VOID OnCommandDebug(UINT nID, UINT nEvent);

const wchar_t *conv_utf8_to_utf16(const char *utf8_message);
const char *conv_utf16_to_utf8(const wchar_t *utf16_message);

#endif
