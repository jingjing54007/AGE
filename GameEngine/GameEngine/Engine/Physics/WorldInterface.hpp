#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

#include "../Utils/Dependency.hpp"
#include "FilterGroup.hpp"
#include "ColliderType.hpp"
#include "MemoryPoolHelper.hpp"
#include "GenericData.hpp"

namespace AGE
{
	class NewRigidBody;
	class Collider;

	namespace Physics
	{
		class PhysicsInterface;
		class RigidBodyInterface;
		class MaterialInterface;
		class ColliderInterface;

		class WorldInterface : public Dependency < WorldInterface >
		{
			// Friendship
			friend NewRigidBody;

			friend Collider;

			friend PhysicsInterface;

			friend ColliderInterface;

		public:
			WorldInterface(void) = delete;

			// Constructors
			WorldInterface(PhysicsInterface *physics);

			WorldInterface(const WorldInterface &) = delete;

			// Assignment Operators
			WorldInterface &operator=(const WorldInterface &) = delete;

			// Methods
			PhysicsInterface *getPhysics(void);

			const PhysicsInterface *getPhysics(void) const;

			void setGravity(float x, float y, float z);

			void setTargetFPS(std::size_t target);

			std::size_t getTargetFPS(void) const;

			void update(float elapsedTime);

			void setFilterNameForFilterGroup(FilterGroup group, const std::string &name);

			const std::string &getFilterNameForFilterGroup(FilterGroup group) const;

			const FilterGroup getFilterGroupForFilterName(const std::string &name) const;

			void enableCollisionBetweenGroups(const std::string &group1, const std::string &group2);

			void disableCollisionBetweenGroups(const std::string &group1, const std::string &group2);

			// Virtual Methods
			virtual void setGravity(const glm::vec3 &gravity) = 0;

			virtual glm::vec3 getGravity(void) const = 0;

			virtual void enableCollisionBetweenGroups(FilterGroup group1, FilterGroup group2) = 0;

			virtual void disableCollisionBetweenGroups(FilterGroup group1, FilterGroup group2) = 0;

		protected:
			// Destructor
			virtual ~WorldInterface(void) = default;

		private:
			// Type Aliases
			using HashTable = std::unordered_map < std::string, FilterGroup > ;

			// Attributes
			PhysicsInterface *physics = nullptr;

			std::size_t targetFPS = 60;

			float accumulator = 0.0f;

			HashTable filterNameToFilterGroup;

			// Virtual Methods
			virtual RigidBodyInterface *createRigidBody(Private::GenericData *data) = 0;

			virtual void destroyRigidBody(RigidBodyInterface *rigidBody) = 0;

			virtual ColliderInterface *createCollider(ColliderType colliderType, Private::GenericData *data) = 0;

			virtual void destroyCollider(ColliderInterface *collider) = 0;

			virtual MaterialInterface *createMaterial(ColliderInterface *collider) = 0;

			virtual void destroyMaterial(MaterialInterface *material) = 0;

			virtual void simulate(float stepSize) = 0;
		};
	}
}

#include "WorldInterface.inl"