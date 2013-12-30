#ifndef  __BULLET_MANAGER_HPP__
# define __BULLET_MANAGER_HPP__

#include <Managers/BulletCollisionManager.hpp>

class BulletDynamicManager : public BulletCollisionManager
{
private:
	btSequentialImpulseConstraintSolver *_constraintSolver;
public:
	BulletDynamicManager()
		: BulletCollisionManager()
		, _constraintSolver(nullptr)
	{}

	virtual ~BulletDynamicManager()
	{
		uninit();
	}

	bool init()
	{
		if (!BulletCollisionManager::init(false))
			return false;
		_constraintSolver = new btSequentialImpulseConstraintSolver();
		if (!_constraintSolver)
			return false;
		_world = new btDiscreteDynamicsWorld(_dispatcher, _broadphase, _constraintSolver, &_defaultCollisionConfiguration);
		if (!_world)
			return false;
		static_cast<btDiscreteDynamicsWorld *>(_world)->setGravity(btVector3(0, -10, 0));
		return true;
	}

	virtual void uninit()
	{
		if (_constraintSolver)
			delete _constraintSolver;
	}
	inline btDynamicsWorld * getWorld() const { return static_cast<btDynamicsWorld *>(_world); }
};

#endif   //__BULLET_MANAGER_HPP__