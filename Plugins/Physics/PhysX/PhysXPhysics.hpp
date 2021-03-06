#pragma once

#include <PxPhysicsAPI.h>

#include <Physics/PhysicsInterface.hpp>

namespace AGE
{
	namespace Physics
	{
		class PhysXPhysics final : public PhysicsInterface
		{
		public:
			// Constructors
			PhysXPhysics(void) = default;

			PhysXPhysics(const PhysXPhysics &) = delete;

			// Assignment Operators
			PhysXPhysics &operator=(const PhysXPhysics &) = delete;

			// Destructor
			virtual ~PhysXPhysics(void) = default;

			// Methods
			physx::PxFoundation *getFoundation(void);

			physx::PxCooking *getCooking(void);

			physx::PxPhysics *getPhysics(void);

		private:
			// Attributes
			physx::PxDefaultErrorCallback defaultErrorCallback;
			
			physx::PxDefaultAllocator defaultAllocatorCallback;

			physx::PxTolerancesScale toleranceScale;
			
			static physx::PxFoundation *foundation;
			
			physx::PxCooking *cooking = nullptr;
			
			static physx::PxPhysics *physics;

			static bool extensions;

			static std::size_t counter;

			// Inherited Methods
			EngineType getPluginType(void) const override final;

			bool initialize(void) override final;

			void finalize(void) override final;

			WorldInterface *createWorld(void) override final;
		};
	}
}
