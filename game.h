#pragma once

namespace Game
{
#ifdef _DEBUG
	// ウインドウモード設定
	constexpr bool kWindowMode = true;
#else
	constexpr bool kWindowMode = false;
#endif
	// ウインドウ名
	const char* const kTitleText = "title";
	// ウインドウサイズ
	constexpr int kScreenWidth = 1280;
	constexpr int kScreenHeight = 720;
	// カラーモード
	constexpr int kColorDepth = 32;		// 32 or 16
};