//---------------------------------------------------------------------------
//!	@file	InputPad.h
//! @brief	パッド入力管理
//---------------------------------------------------------------------------
#pragma once

//---------------------------------------------------------------------------
//! パッド種別
//---------------------------------------------------------------------------
//@{

enum class PAD_NO : u32 {
    PAD_NO1 = 0,
    PAD_NO2,
    PAD_NO3,
    PAD_NO4,

    PAD_NO_MAX
};

//@}
//---------------------------------------------------------------------------
//! ボタン種別
//---------------------------------------------------------------------------
//@{
enum class PAD_ID : u32 {    // XBOX   PS      SW
    PAD_UP = 0,
    PAD_DOWN,
    PAD_LEFT,
    PAD_RIGHT,

    PAD_1,     // A      □       B
    PAD_2,     // B      Ｘ      A
    PAD_3,     // X      ◯      Y
    PAD_4,     // Y      △      X
    PAD_5,     // LB     L2      L
    PAD_6,     // RB     R2      R
    PAD_7,     // Back   L2      ZL
    PAD_8,     // Start  R2      ZR
    PAD_9,     // L3     Share   -
    PAD_10,    // R3     Option  +
    PAD_11,    //        Pad     L3
    PAD_12,    //        PS      R3
    PAD_13,    //        Mute    Home
    PAD_14,    //                Capture
    PAD_15,    //
    PAD_16,    //

    PAD_ID_MAX
};

//@}
//===========================================================================
//!	@name	システム関数
//===========================================================================
//@{

//! 初期化
void InputPadInit();

//! 更新
void InputPadUpdate();

//! 終了
void InputPadExit();

//@}
//===========================================================================
//!	@name	入力関連関数
//===========================================================================
//@{

//! 指定パッドのボタンの1回だけ押下検証
//! @param	[in]	padID	パッドのボタン種別
//! @param	[in]	padNo	パッドの種別(指定がなければPAD_NO1)
//! @retval true	引数に指定されたパッドのボタンが押された
//! @retval false	引数に指定されたパッドのボタンが押されていない
bool IsPadOn(PAD_ID padID, int padType = 0, PAD_NO padNo = PAD_NO::PAD_NO1);

//! 指定パッドのボタンの1回だけ押下検証
//! @param	[in]	padID	パッドのボタン種別
//! @param	[in]	padNo	パッドの種別(指定がなければPAD_NO1)
//! @retval true	引数に指定されたパッドのボタンが離されている
//! @retval false	引数に指定されたパッドのボタンが押されている
bool IsPadRelease(PAD_ID padID, int padType = 0, PAD_NO padNo = PAD_NO::PAD_NO1);

//! 指定パッドのボタンの長押し押下検証
//! @param	[in]	padID	パッドのボタン種別
//! @param	[in]	padNo	パッドの種別(指定がなければPAD_NO1)
//! @retval true
//! 引数に指定されたパッドのボタンが長押し（2フレーム以上）されている
//! @retval false
//! 引数に指定されたパッドのボタンが押されていない（押されているフレーム数が1以下）
bool IsPadRepeat(PAD_ID padID, int padType = 0, PAD_NO padNo = PAD_NO::PAD_NO1, u32 frame = 2);

//@}

//===========================================================================
//!	@name	入力関連関数(Windowsポンプ命名)
//===========================================================================
//@{
//! 指定パッドのボタンの1回だけ押下検証
//! @param	[in]	padID	パッドのボタン種別
//! @param	[in]	padNo	パッドの種別(指定がなければPAD_NO1)
//! @retval true	引数に指定されたパッドのボタンが押された
//! @retval false	引数に指定されたパッドのボタンが押されていない
bool IsPadDown(PAD_ID padID, int padType = 0, PAD_NO padNo = PAD_NO::PAD_NO1);

//! 指定パッドのボタンの1回だけ押下検証
//! @param	[in]	padID	パッドのボタン種別
//! @param	[in]	padNo	パッドの種別(指定がなければPAD_NO1)
//! @retval true	引数に指定されたパッドのボタンが離されている
//! @retval false	引数に指定されたパッドのボタンが押されている
bool IsPadUp(PAD_ID padID, int padType = 0, PAD_NO padNo = PAD_NO::PAD_NO1);

//! 指定パッドのボタンの押下検証
//! @param	[in]	padID	パッドのボタン種別
//! @param	[in]	padNo	パッドの種別(指定がなければPAD_NO1)
//! @retval true	引数に指定されたパッドのボタンが押されている
//! @retval false	引数に指定されたパッドのボタンが押されていない
bool IsPad(PAD_ID padID, int padType = 0, PAD_NO padNo = PAD_NO::PAD_NO1);

//@}
