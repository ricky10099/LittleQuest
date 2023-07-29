﻿//---------------------------------------------------------------------------
//! @file   ProcTiming.h
//! @brief  処理タイミング
//---------------------------------------------------------------------------
#pragma once
#include "Priority.h"
#include <System/Signals.h>
#include <System/Cereal.h>
#include <functional>

class Object;
class Component;
USING_PTR(Object);
USING_PTR(Component);

using ProcTimingFunc = std::function<void()>;

//! タイミング
enum struct ProcTiming : u32
{
    PreUpdate = 0,
    Update,
    LateUpdate,
    PrePhysics,
    PostUpdate,

    PreDraw,
    Draw,
    LateDraw,
    PostDraw,

    Shadow,
    Gbuffer,
    Light,
    HDR,
    Filter,
    UI,

    NUM,
};

//! プライオリティ設定
constexpr int TIMING(ProcTiming p)
{
    return static_cast<int>(p);
}

//! プライオリティ設定(Macro)
#define OBJTIMING(p) TIMING(ProcTiming::##p)

std::string GetProcTimingName(ProcTiming proc);

ProcTimingFunc GetProcTimingFunc(ProcTiming proc);

class Callable
{
public:
    Callable() {}
    Callable(const std::string& name) { SetName(name); }
    virtual ~Callable() {}
    virtual void               Exec() = 0;
    virtual void               SetName(const std::string& str) { name_ = str; }
    virtual const std::string& GetName() { return name_; }

protected:
    std::string name_{};
};

#define ProcAddProc(name, x)                                                                                           \
    class Callable##name : public Callable                                                                             \
    {                                                                                                                  \
    public:                                                                                                            \
        Callable##name()                                                                                               \
            : Callable(#name)                                                                                          \
        {                                                                                                              \
        }                                                                                                              \
        void Exec x;                                                                                                   \
                                                                                                                       \
    private:                                                                                                           \
        CEREAL_SAVELOAD(arc, ver) { arc(CEREAL_NVP(name_)); }                                                          \
    };                                                                                                                 \
    std::shared_ptr<Callable##name> name = std::make_shared<Callable##name>();                                         \
    CEREAL_REGISTER_TYPE(Callable##name);                                                                              \
    CEREAL_REGISTER_POLYMORPHIC_RELATION(Callable, Callable##name)

//! スロット
struct SlotProc
{
    friend class Scene;
    friend class Object;
    friend class Component;

public:
    bool IsDirty() const { return dirty_; }

    void ResetDirty() { dirty_ = false; }

    const ProcTiming GetTiming() const { return timing_; }

    const Priority GetPriority() const { return priority_; }

    auto GetAddProc() { return func_; }

    const auto GetName() const { return name_; }

    const bool IsUpdate() const
    {
        if(static_cast<int>(timing_) < static_cast<int>(ProcTiming::PreDraw)) {
            return true;
        }
        return false;
    }

    const bool IsDraw() const
    {
        if(static_cast<int>(timing_) >= static_cast<int>(ProcTiming::PreDraw)) {
            return true;
        }
        return false;
    }

    void SetProc(std::string name, ProcTiming timing, Priority prio, ProcTimingFunc func)
    {
        name_     = name;
        dirty_    = true;
        timing_   = timing;
        priority_ = prio;
        proc_     = func;
    }

    ProcTimingFunc& GetProc() { return proc_; }

    void SetAddProc(std::shared_ptr<Callable> func, ProcTiming timing, Priority prio)
    {
        name_     = func->GetName();
        dirty_    = true;
        timing_   = timing;
        priority_ = prio;
        func_     = func;

        proc_ = nullptr;
    }

private:
    std::string               name_{};
    ProcTiming                timing_   = ProcTiming::Draw;
    Priority                  priority_ = Priority::NORMAL;
    sigslot::connection       connect_{};
    bool                      dirty_ = true;
    ProcTimingFunc            proc_;
    std::shared_ptr<Callable> func_;

private:
    //--------------------------------------------------------------------
    //! @name Cereal処理
    //--------------------------------------------------------------------
    //@{
    CEREAL_SAVELOAD(arc, ver)
    {
        dirty_ = true;           //セーブするときにdirty_つける
        arc(CEREAL_NVP(name_),   // name
            CEREAL_NVP(timing_),
            CEREAL_NVP(priority_),
            CEREAL_NVP(dirty_),
            CEREAL_NVP(func_));
        // connect_ は再構築させる
    }

    //@}
};

using SlotProcs = std::unordered_map<std::string, SlotProc>;
