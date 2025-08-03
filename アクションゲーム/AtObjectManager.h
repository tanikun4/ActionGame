#pragma once
#include "Manager.h"
class AtObjectManager :
    public Manager<AtObjectManager>
{
public:
    friend class Manager<AtObjectManager>;
    Object* SetObject(int obj_id);
};

