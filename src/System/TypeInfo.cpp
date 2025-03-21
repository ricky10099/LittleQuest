﻿//---------------------------------------------------------------------------
//! @file   TypeInfo.cpp
//! @brief  クラス型情報
//---------------------------------------------------------------------------
#include "TypeInfo.h"

//! ルートの型情報
TypeInfo TypeInfo::Root = TypeInfo("root", 0, nullptr);

//---------------------------------------------------------------------------
//! コンストラクタ
//---------------------------------------------------------------------------
// warning C26495: 変数 'Type::child_' が初期化されていません。
// 警告を抑制。グローバル変数領域の初期値nullptrを利用した起動時処理のため意図的に初期化していません。
// nullptrで初期化すると起動時初期化順序によっては上書きでツリー構造を破壊してしまいます。
// WinMain() 実行前に動作するためSTLコンテナなどアロケーションを必要とする実装も不可
#pragma warning(push)
#pragma warning(disable: 26495)

TypeInfo::TypeInfo(const char* class_name, size_t class_size, TypeInfo* parent_type, const char* desc_name) {
    // パラメーターの保存
    class_name_ = class_name;
    desc_name_  = desc_name;
    class_size_ = class_size;

    //----------------------------------------------------------
    // 継承ツリー構造の構築
    //----------------------------------------------------------
    if(parent_type == nullptr && strcmp(class_name, "root")) {    // "root"ではない
        parent_type = &TypeInfo::Root;                            // 基底クラスはルートに接続する
    }

    parent_ = parent_type;
    if(parent_type) {
        auto child = parent_type->child();
        // 親クラスの子があった場合は追加登録、子が一つもない場合は新規登録
        if(child) {
            siblings_ = child;
        }
        parent_type->child_ = this;
    }
}

#pragma warning(pop)

//---------------------------------------------------------------------------
//  インスタンスを作成(クラスをnewしてポインタを返す)
//---------------------------------------------------------------------------
void* TypeInfo::createInstance() const {
    return nullptr;
}

//---------------------------------------------------------------------------
//! クラス名を取得
//---------------------------------------------------------------------------
const char* TypeInfo::className() const {
    return class_name_;
}

//---------------------------------------------------------------------------
//! 説明文字列を取得
//---------------------------------------------------------------------------
const char* TypeInfo::descName() const {
    return desc_name_;
}

//---------------------------------------------------------------------------
//! クラスのサイズを取得
//---------------------------------------------------------------------------
size_t TypeInfo::classSize() const {
    return class_size_;
}

//---------------------------------------------------------------------------
//! 親ノードを取得
//---------------------------------------------------------------------------
const TypeInfo* TypeInfo::parent() const {
    return parent_;
}

//---------------------------------------------------------------------------
//! 子ノードを取得
//---------------------------------------------------------------------------
const TypeInfo* TypeInfo::child() const {
    return child_;
}

//---------------------------------------------------------------------------
//! 次の兄弟ノードを取得
//---------------------------------------------------------------------------
const TypeInfo* TypeInfo::siblings() const {
    return siblings_;
}

//===========================================================================
//  ユーティリティー
//===========================================================================

//---------------------------------------------------------------------------
//! 名前を指定して指定基底クラス型でnewする
//---------------------------------------------------------------------------
void* CreateInstanceFromName(std::string_view class_name, const TypeInfo& base_type) {
    const TypeInfo* p                  = base_type.child();
    bool            returnFromTraverse = false;
    const TypeInfo* next               = nullptr;

    //----------------------------------------------------------
    // 継承ツリー構造を探索
    // スタック再帰を使わない高速なツリー探索 (stackless tree traversal)
    //----------------------------------------------------------
    while(p != &base_type) {
        if(!returnFromTraverse) {
            // 名前チェックして一致したら作成
            if(p->className() == class_name) {
                return p->createInstance();
            }
        }

        if(p->child() && !returnFromTraverse) {
            // 子がある場合は子を先に調べる。(子から探索で戻ってきた場合は除外)
            next               = p->child();
            returnFromTraverse = false;
        } else if(p->siblings()) {
            // 兄弟がいる場合は兄弟を調べる
            next               = p->siblings();
            returnFromTraverse = false;
        } else {
            // 親へ戻る。
            next               = p->parent();
            returnFromTraverse = true;
        }
        p = next;
    }

    return nullptr;
}
