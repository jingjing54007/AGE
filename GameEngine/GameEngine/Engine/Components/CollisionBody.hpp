#ifndef		__COLLISION_BODY_HPP__
#define		__COLLISION_BODY_HPP__

#include <btBulletDynamicsCommon.h>
#include <Components/Component.hh>
#include <Entities/EntityData.hh>
#include <Entities/Entity.hh>
#include <Core/Engine.hh>
#include <Managers/BulletCollisionManager.hpp>
#include "BulletCollision/CollisionShapes/btShapeHull.h"
#include <Utils/BtConversion.hpp>
#include <Utils/MatrixConversion.hpp>
#include <Core/AScene.hh>
#include <Components/CollisionLayers.hpp>
#include <MediaFiles/AssetsManager.hpp>


namespace Component
{
	ATTRIBUTE_ALIGNED16(struct) CollisionBody : public Component::ComponentBase<CollisionBody>
	{
		BT_DECLARE_ALIGNED_ALLOCATOR();
		typedef enum
		{
			SPHERE,
			BOX,
			MESH,
			UNDEFINED
		} CollisionShape;

		CollisionBody() :
			_manager(nullptr),
			shapeType(UNDEFINED),
			meshName(""),
			_collisionShape(nullptr),
			_body(nullptr)
		{
		}

		virtual void reset()
		{
			if (_body != nullptr)
			{
				_manager->getWorld()->removeCollisionObject(_body);
				delete _body;
				_body = nullptr;
			}
			if (_collisionShape != nullptr)
			{
				delete _collisionShape;
				_collisionShape = nullptr;
			}
			shapeType = UNDEFINED;
		}

		bool init()
		{
			_manager = _entity->getScene()->getInstance<BulletCollisionManager>();
			assert(_manager != nullptr);
			return true;
		}

		btCollisionShape &getShape()
		{
			assert(_collisionShape != nullptr && "Shape is NULL, RigidBody error. Tips : Have you setAcollisionShape to Component ?.");
			return *_collisionShape;
		}

		btCollisionObject &getBody()
		{
			assert(_body != nullptr && "Collision body is NULL. Tips : Have you setAcollisionShape to Component ?.");
			return *_body;
		}

		void setCollisionShape(CollisionShape c, const std::string &_meshName = "")
		{
			if (c == UNDEFINED)
				return;
			meshName = _meshName;
			reset();
			shapeType = c;
			btTransform transform;
			glm::vec3 position = posFromMat4(_entity->getLocalTransform());
			glm::vec3 scale = scaleFromMat4(_entity->getLocalTransform());
			std::cout << scale.x << " " << scale.y << " " << scale.z << std::endl;
			glm::vec3 rot = rotFromMat4(_entity->getLocalTransform(), true);
			transform.setIdentity();
			transform.setOrigin(convertGLMVectorToBullet(position));
			transform.setRotation(btQuaternion(rot.x, rot.y, rot.z));
			if (c == BOX)
			{
				_collisionShape = new btBoxShape(btVector3(0.5, 0.5, 0.5));//new btBoxShape(halfScale);
			}
			else if (c == SPHERE)
			{
				_collisionShape = new btSphereShape(1);//new btSphereShape(scale.x);
			}
			else if (c == MESH)
			{
				auto mesh = _entity->getScene()->getInstance<AssetsManager>()->get<ObjFile>(meshName);
				auto &geo = mesh->geometries[0]; // DIRTY HACK TEMPORARY
				// NEED TO REPLACE MESH BY MESH GROUP !
				btScalar *t = new btScalar[geo.vertices.size() * 3]();
				for (unsigned int i = 0; i < geo.vertices.size(); ++i)
				{
					t[i * 3] = geo.vertices[i].x;
					t[i * 3 + 1] = geo.vertices[i].y;
					t[i * 3 + 2] = geo.vertices[i].z;
				}
				btConvexHullShape *tmp = new btConvexHullShape(t, geo.vertices.size(), 3 * sizeof(btScalar));
				btShapeHull *hull = new btShapeHull(tmp);
				btScalar margin = tmp->getMargin();
				hull->buildHull(margin);
				tmp->setUserPointer(hull);
				btConvexHullShape *s = new btConvexHullShape();
				for (int i = 0; i < hull->numVertices(); ++i)
				{
					s->addPoint(hull->getVertexPointer()[i], false);
				}
				s->recalcLocalAabb();
				_collisionShape = s;
				delete[] t;
				delete hull;
				delete tmp;
			}
			_collisionShape->setLocalScaling(convertGLMVectorToBullet(scale));
			_body = new btCollisionObject();
			_body->setUserPointer(&_entity);
			_body->setCollisionShape(_collisionShape);
			_body->setWorldTransform(transform);
			_manager->getWorld()->addCollisionObject(_body, btCollisionObject::CF_STATIC_OBJECT, btCollisionObject::CF_STATIC_OBJECT);
		}

		virtual ~CollisionBody(void)
		{
			if (_body)
			{
				_manager->getWorld()->removeCollisionObject(_body);
				delete _body;
			}
			if (_collisionShape)
				delete _collisionShape;
		}


		//////
		////
		// Serialization

		template <typename Archive>
		Base *unserialize(Archive &ar, Entity e)
		{
			auto res = new CollisionBody();
			res->setEntity(e);
			ar(*res);
			return res;
		}

		template <typename Archive>
		void save(Archive &ar) const
		{
		}

		template <typename Archive>
		void load(Archive &ar)
		{
		}

		// !Serialization
		////
		//////

		CollisionShape shapeType;
		std::string meshName;
	private:
		std::shared_ptr<BulletCollisionManager> _manager;
		btCollisionShape *_collisionShape;
		btCollisionObject *_body;
		CollisionBody(CollisionBody const &);
		CollisionBody &operator=(CollisionBody const &);
	};

}

#endif //!__COLLISION_BODY_HPP__