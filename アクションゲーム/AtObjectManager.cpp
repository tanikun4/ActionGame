#include "AtObjectManager.h"
#include "Arrow.h"
Object* AtObjectManager::SetObject(int obj_id) {
	switch (obj_id) {
	case 0:
		return new Arrow(0);
	case 1:
		return new Arrow(1);
	default:
		return nullptr;
	}
	return nullptr;
}