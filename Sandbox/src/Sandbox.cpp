#include <R3D.h>
#include <R3D/Physics/Core/Colliders.h>
#include <R3D/Physics/Core/Topology.h>
#include <R3D/Utils/Debugger.h>
#include <R3D/Physics/Core/World.h>
#include <R3D/Physics/Systems/Integrator.h>

#define STACKING

namespace r3d
{
	template <class T>
	class NanChecker : public System
	{
	public:
		virtual void update(ArchetypeManager & am, double t, double dt) override
		{
			auto archetypes = am.matchAtLeastWithout(ComponentList::buildList<T>(), {});
			for (auto arch : archetypes)
			{
				T* t = get<T>(am, arch);
				size_t numEntities = getSize<T>(am, arch);
				const std::vector<Entity>& entities = getEntities(am, arch);
				for (size_t i = 0; i < numEntities; ++i)
				{
					if (std::isnan(t[i].vec.x))
					{
						R3D_CORE_CRITICAL("Nan. Entity {0}", entities[i].id);
					}
				}
			}
		}
	};

	class ContactPointEntityRemover : public System
	{
	public:
		virtual void update(r3d::ArchetypeManager& am, double t, double dt) override
		{
			auto archetypes = am.matchAtLeastWithout(ComponentList::buildList<ContactEntityTag>(), {});
			for (auto arch : archetypes)
			{
				auto& entities = getEntities(am, arch);
				size_t numEntities = getSize<ContactEntityTag>(am, arch);
				for (size_t i = 0; i < numEntities; ++i)
				{
					Entity e = entities[0];
					Application::getEntityManager()->destroy(e);
					am.removeEntity(e);
				}
			}
		}
	};

	class ContactPointEntityGenerator : public System
	{
	public:
		ContactPointEntityGenerator()
		{
			archetypeCollisionPoint = Archetype{ ComponentList::buildList<CirclePrimitive, Color, Transform, ContactEntityTag>() };
			archetypeCollisionNormal = Archetype{ ComponentList::buildList<SegmentPrimitive, Color, Transform, ContactEntityTag>() };
		}

		void setContacts(const CollisionData* collData) { m_collData = collData; }

		virtual void update(r3d::ArchetypeManager& am, double t, double dt) override
		{
			for (auto& arbiter : m_collData->arbiters)
			{
				for (size_t i = 0; i < arbiter.second.manifold.numContacts; ++i)
				{
					const Contact& contact = arbiter.second.manifold.contacts[i];
					createContactEntity(am, arbiter.second.manifold, i);
				}
			}
		}

	private: 

		void createContactEntity(r3d::ArchetypeManager& am, const Manifold& manifold, int contactIndex)
		{
			const real3& positionE1 = am.get<Transform>(manifold.e1)->position;
			const rquat& orientationE1 = am.get<Transform>(manifold.e1)->orientation;
			const real3& contactPositionWorld = manifold.contacts[contactIndex].position;
			 
			fquat q{ 1.0, 0.0, 0.0, 0.0 };

			float3 N1{ 1.0f,0.0f,0.0f };
			float3 N2 = (float3)manifold.normal;

			if (glm::length(glm::cross(N2, N1)) > R3D_EPSILON)
			{
				float3 M = glm::normalize(N1 + N2);
				float3 axis = glm::normalize(glm::cross(M, N2));

				float cos = glm::dot(M, N2);
				float sin = glm::length(glm::cross(M, N2));
				q.w = cos;
				q.x = sin * axis.x;
				q.y = sin * axis.y;
				q.z = sin * axis.z;
				q = glm::normalize(q);
			}

			float segLength = 0.333;
			float3 displacement{ 1.0, 0.0, 0.0 };
			float3 rotated_displacement = q * displacement;
			rotated_displacement = segLength * rotated_displacement / glm::length(rotated_displacement);

			auto entity_s = Application::getEntityManager()->create();
			am.setArchetype(entity_s, archetypeCollisionNormal);
			am.set<Color>(entity_s, Color{ float4{1.0, 0.0, 0.0, 1.0} });
			am.set<Transform>(entity_s, Transform{ (float3)contactPositionWorld + rotated_displacement, q, float3{0.3} });

			auto entity_c = Application::getEntityManager()->create();
			am.setArchetype(entity_c, archetypeCollisionPoint);
			am.set<Color>(entity_c, Color{ float4{1.0, 0.0, 0.0, 1.0} });
			am.set<Transform>(entity_s, Transform{ contactPositionWorld, q, float3{0.1} });
		}

	private:
		const CollisionData* m_collData;
		Archetype archetypeCollisionPoint;
		Archetype archetypeCollisionNormal;
	};

}

class WorldLayer : public r3d::Layer
{
	r3d::SphereSphereContactDetector sphereSphereContactDetector;
	r3d::SpherePlaneContactDetector spherePlaneContactDetector;
	r3d::BoxPlaneContactDetector boxPlaneContactDetector;
	r3d::BoxBoxContactDetector boxBoxContactDetector;

	r3d::ContactPointEntityGenerator contactEntityGenerator;
	r3d::ContactPointEntityRemover contactEntityRemover;
	r3d::CollisionData collisionData;

	r3d::ForceIntegrator forceIntegrator;
	r3d::VelocityIntegrator velocityIntegrator;

	r3d::Entity circleEnt;
	r3d::Entity segmentEntX;
	r3d::Entity segmentEntY;
	r3d::Entity segmentEntZ;
	r3d::Entity gridEnt;

	r3d::Entity firstBox;
	r3d::Entity newEntity;
	r3d::float3 newPosition;
	float newAngle;
	bool physicsOn;
	r3d::float3 newAngleAxis;
	r3d::float3 newSize;
	r3d::float3 newColor;

	bool show_demo_window;

	r3d::Camera camera;
	r3d::float3 deltaPosition;
	r3d::float3 centerPosition;
	r3d::float3 initialPosition;
	r3d::float3 up;

	r3d::float4x4 projectionMatrix;
	float right;
	float top;
	float fovy;
	float aspect;
	float nearPlane;
	float farPlane;
	enum class Projection
	{
		ORTHO,
		PERSP
	} projection;

	r3d::Shader linesShader;
	r3d::PrimitivesRenderer primitivesRenderer;

	int frameCounter;

private:
	r3d::Entity create_box(r3d::real3 position, r3d::real angle, r3d::real3 axis, r3d::real3 scale, r3d::real gravity, r3d::real invMass = 1.0, r3d::real3 velocity = r3d::real3{ 0.0 }, r3d::float4 color = r3d::float4{ 0.18, 0.4, 0.9, 1.0 })
	{
		using namespace r3d;
		
		auto boxEnt = Application::getEntityManager()->create();

		ArchetypeManager* am = getArchetypeManager();
		real cosSquareAngle = glm::cos(glm::radians(angle / 2.0));
		real sinSquareAngle = glm::sin(glm::radians(angle / 2.0));
		real3 squareAngleAxis = sinSquareAngle * axis;
		rquat squareOrientationQuat = glm::normalize(rquat{ cosSquareAngle, squareAngleAxis.x, squareAngleAxis.y, squareAngleAxis.z });
		
		real3x3 invI = (invMass < R3D_EPSILON) ? real3x3{0.0f} : glm::inverse(r3d::ColliderBox::computeInertiaTensor(scale, 1.0 / invMass)); // TODO: put back the inverse
		real3 force = real3{ 0.0, (invMass < R3D_EPSILON) ? 0.0 : gravity / invMass,0.0 };
		real3 torque = real3{ 0.0 };
		real3 angVelocity = real3{ 0.0 };
		real friction = 0.1;

		am->setArchetype < Transform, RigidBody, Color, ColliderBox, BoxPrimitive >(boxEnt);
		am->set<Transform>(boxEnt, Transform{position, squareOrientationQuat, scale});
		am->set<Color>(boxEnt, color);
		am->set<RigidBody>(boxEnt, RigidBody{invI, velocity, angVelocity, force, torque, invMass, friction});
		

		return boxEnt;
	}

public:
	WorldLayer() : r3d::Layer("World layer.")
	{
		using namespace r3d;
		 
		Application::getWindow()->setColor(0.02f, 0.02f, 0.1f);
		auto am = getArchetypeManager();

		circleEnt = Application::getEntityManager()->create();
		am->setArchetype < Transform, CirclePrimitive, Color> (circleEnt);
		am->set<Transform>(circleEnt, Transform{ {-3.75f, 2.5, 0.0}, {1.0, 0.0, 0.0, 0.0}, float3{1.0} });
		am->set<Color>(circleEnt, float4{ 0.5, 0.32, 0.0, 1.0 });

#ifdef STACKING
		physicsOn = true;
		real3 scale{ 1.0, 1.0, 1.0 };
		real3 position{1.0, scale.y, 0.0};
		int numBoxes = 10;
		for (int i = 0; i < numBoxes; ++i)
		{
			real3 addPos = { 0.0, 2.0 * i * scale.y + 0.05, 0.0 };
			real  scaleFactor = (1.0 - 0.9 * i / float(numBoxes));
			real3 scaleFactorVec = { scaleFactor, scaleFactor, scaleFactor };
			auto ent = create_box(position + addPos, 45 * i, real3{ 0.0, 1.0, 0.0 }, scale * scaleFactorVec, -0.6, 1.0, {0.0, 0.0, 0.0});
			if (i == numBoxes - 1)
			{
				newEntity = ent;
			}
			else if (i == 0)
			{
				firstBox = ent;
			};
		}
#else
		physicsOn = false;
		create_box(real3{ 0.0, 1.0, 0.0 }, 0.0, real3{ 1.0 ,0.0, 0.0 }, real3{ 1.0 }, 0.0, 0.00001, real3{ 0.0 }, float4{ 0.5, 0.5, 0.5, 1.0 });
		
		newAngle = 36.0f;
		newAngleAxis = float3{ 0.995f, 0.006f, 0.101f };
		newPosition = {1.077, 4.231, 0.0};
		newSize = { 1.0f };
		newColor = { 0.05f, 0.3f, 0.9f };
		newEntity = create_box(newPosition, newAngle, newAngleAxis, newSize, -2.0, 1.0, real3{ 0.0 }, { newColor.r, newColor.g, newColor.b, 1.0f });
#endif

		float cos45 = glm::cos(glm::radians(45.f));
		float sin45 = glm::sin(glm::radians(45.f));
		r3d::fquat quat1;
		quat1.w = 1.0; quat1.x = 0.0; quat1.y = 0.0; quat1.z = 0.0;
		segmentEntX = Application::getEntityManager()->create();
		am->setArchetype < Transform, SegmentPrimitive, Color >(segmentEntX);
		am->set<Transform>(segmentEntX, Transform{ {0.0, 0.01, 0.0}, quat1, real3{1.0} });
		am->set<Color>(segmentEntX, float4{ 1.0, 0.5, 0.5, 1.0 });

		r3d::fquat quatz;
		quatz.w = cos45; quatz.x = 0.0; quatz.y = sin45; quatz.z = 0.0;
		segmentEntZ = Application::getEntityManager()->create();
		am->setArchetype < Transform, SegmentPrimitive, Color >(segmentEntZ);
		am->set<Transform>(segmentEntZ, Transform{ {0.0, 0.01, 0.0}, quatz, real3{1.0} });
		am->set<Color>(segmentEntZ, float4{ 0.5, 0.5, 1.0, 1.0 });

		r3d::fquat quaty;
		quaty.w = cos45; quaty.x = 0.0; quaty.y = 0.0; quaty.z = sin45;
		segmentEntY = Application::getEntityManager()->create();
		am->setArchetype < Transform, SegmentPrimitive, Color >(segmentEntY);
		am->set<Transform>(segmentEntY, Transform{ {0.0, 0.01, 0.0}, quaty, real3{1.0} });
		am->set<Color>(segmentEntY, float4{ 0.5, 1.0, 0.5, 1.0 });

		
		ColliderPlane plane;
		plane.normal = { 0.0, 1.0, 0.0 };
		plane.offset = 0.0;
		gridEnt = Application::getEntityManager()->create();
		am->setArchetype < Transform, ColliderPlane, Color, RigidBody, PlanePrimitive >(gridEnt);
		am->set<Color>(gridEnt, float4{ 0.5, 0.0, 0.5, 1.0 });
		am->set<RigidBody>(gridEnt, RigidBody{ real3x3{0.0}, 0.0, 0.9, real3{0.0} });
		am->set<Transform>(gridEnt, Transform{ real3{plane.offset}, rquat{1.0, plane.normal.x, plane.normal.y, plane.normal.z}, real3{20.} });
		am->set<ColliderPlane>(gridEnt, plane);

#ifdef STACKING
		{
			real3 wallscale = { 5.0, 3.0, 0.5 };
			create_box(real3{ +0.0, wallscale.y, -5.0 }, 0, real3{ 0.0, 1.0, 0.0 }, wallscale, -0.6, 0.001, { 0.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0, 0.0 });
			create_box(real3{ -5.5, wallscale.y, 0.0 }, 90, real3{ 0.0, 1.0, 0.0 }, wallscale, -0.6, 0.001, { 0.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0, 0.0 });
			create_box(real3{ +5.5, wallscale.y, 0.0 }, 90, real3{ 0.0, 1.0, 0.0 }, wallscale, -0.6, 0.001, { 0.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0, 0.0 });
		}
		{
			real3 woodscale = { 0.2, 4.0, 1.0 };
			create_box(real3{ +3.0, woodscale.y + 0.0, 0.0 }, +5.0, real3{ 0.0, 0.0, 1.0 }, woodscale, -0.6, 1.0, { 0.0, 0.0, 0.0 }, { 1.0, 0.64, 0.0, 0.0 });
			create_box(real3{ -3.0, woodscale.y + 0.0, 0.0 }, -5.0, real3{ 0.0, 0.0, 1.0 }, woodscale, -0.6, 1.0, { 0.0, 0.0, 0.0 }, { 1.0, 0.64, 0.0, 0.0 });
		}
#endif

		// camera
		right = 5.0f;
		top = 5.0f;
		fovy = 90.0f;
		aspect = 1.0f;
		nearPlane = 2.0f;
		farPlane = 100.0f;
		projection = Projection::PERSP;

		if (projection == Projection::ORTHO)
		{
			deltaPosition = r3d::float3{ 0.0f };
			up = float3{ 0.0f, 1.0f, 0.0f };
			initialPosition = float3{ 0.0f, 0.0f, 5.0f };
			centerPosition = float3{ 0.0 };
			camera = Camera{ initialPosition, centerPosition, up };
			projectionMatrix = glm::ortho(-right, right, -top, top, nearPlane, farPlane);
		}
		else
		{
			projectionMatrix = glm::perspective(fovy, aspect, nearPlane, farPlane);
			deltaPosition = r3d::float3{ 0.0f };
			up = float3{ 0.0f, 1.0f, 0.0f };

#ifdef STACKING
			//initialPosition = float3{ -5.0f, 10.0f, 15.0f };
			initialPosition = float3{ -3.0f, 7.0f, 7.5f };
#else
			initialPosition = float3{ 0.0f, 2.0f, 4.0f };
#endif
			centerPosition = float3{ 0.0 };
			camera = Camera{ initialPosition, centerPosition, up };

		}

		// imgui
		show_demo_window = false;

		// shaders
		linesShader = std::move(Shader{ "C:/Users/matte/source/repos/OpenGL/Rendara3D/res/shaders/lines.shader" });
		primitivesRenderer.setShader(&linesShader);

		// Event listeners
		pushBackLayerListener(MouseButtonPressedEvent::getStaticType(), this);
		
		// physics stuff
		sphereSphereContactDetector.setCollisionData(&collisionData);
		spherePlaneContactDetector.setCollisionData(&collisionData);
		boxPlaneContactDetector.setCollisionData(&collisionData);
		boxBoxContactDetector.setCollisionData(&collisionData);

		Application::getDebugger()->setAm(getArchetypeManager());
		Application::getDebugger()->setEm(Application::getEntityManager());
		 
		frameCounter = 0;
	}



	virtual void onUpdate(r3d::Window* window) override
	{
		++frameCounter;
		double t = window->getLastTime();
		double dt = window->getLastFrameTime();
		auto am = getArchetypeManager();

		// reset contacts
		collisionData.totalContacts = 0;
		collisionData.contacts.clear();
		
		// remove contact entities
		contactEntityRemover.update(*am, t, dt);

		// detect contacts
		sphereSphereContactDetector.update(*am, t, dt);
		spherePlaneContactDetector.update(*am, t, dt);
		boxPlaneContactDetector.update(*am, t, dt);
		boxBoxContactDetector.update(*am, t, dt);
		
		// integrate forces
#ifndef STACKING	
		if(physicsOn)
		{
#endif

		forceIntegrator.update(*am, t, dt);
		
		for (auto arb = collisionData.arbiters.begin(); arb != collisionData.arbiters.end(); ++arb)
		{
			arb->second.preStep(am, 1.0 / dt);
		}
		
		const int iterations = 10;
		for (int i = 0; i < iterations; ++i)
		{
			for (auto arb = collisionData.arbiters.begin(); arb != collisionData.arbiters.end(); ++arb)
			{
				arb->second.applyImpulse(am);
			}
		}
		
		// integrate positions
		velocityIntegrator.update(*am, t, dt);

#if 0
		R3D_ERROR("frame: {0}", frameCounter);
		for (auto arb = collisionData.arbiters.begin(); arb != collisionData.arbiters.end(); ++arb)
		{
			arb->second.printInfo();
		}
		for (auto a : collisionData.arbiters)
		{
			if (a.second.manifold.numContacts > 0)
			{
				if (a.second.manifold.contacts->type != r3d::ContactType::BOXPLANE__POINT_FACE)
				{
					//a.second.printInfo();
					for (int i = 0; i < a.second.manifold.numContacts; ++i)
					{
						using namespace r3d;
						r3d::Contact* c = a.second.manifold.contacts + i;

						real3 impulse = a.second.manifold.normal * c->impulseNormal +
							a.second.manifold.tangent[0] * c->impulseTangent[0] +
							a.second.manifold.tangent[1] * c->impulseTangent[1];
						if (glm::dot(impulse, impulse) > R3D_EPSILON)
						{
							impulse = glm::normalize(impulse);
							r3d::Application::getDebugger()->draw_segment(c->position, impulse, { 0.0, 1.0, 0.0 });
						}

						if ((frameCounter % 120) == 0)
						{
							R3D_INFO("P = {0},{1},{2}", impulse.x, impulse.y, impulse.z);
							R3D_INFO("---------------");
						}


					}

				}
			}
		}

#endif

#ifndef STACKING	
		}
#endif		
#if defined(R3D_DEBUG) || defined(R3D_RELEASE)
		contactEntityGenerator.setContacts(&collisionData);
		contactEntityGenerator.update(*am, t, dt);
#endif
	}

	virtual void onRender(r3d::Window* window) override
	{
		double t = window->getCurrentTime();
		double dt = window->getLastFrameTime();
		r3d::ArchetypeManager* am = getArchetypeManager();
		linesShader.bind();
		linesShader.setUniformMatrix("view", camera.getViewMatrix(), false);
		linesShader.setUniformMatrix("projection", projectionMatrix, false);

		primitivesRenderer.setShader(&linesShader);
		primitivesRenderer.update(*am, t, dt);

	}

	virtual void onImGuiUpdate(r3d::Window* window) override
	{
		using namespace r3d;
		srand((int) window->getCurrentTime() * 1000);

		auto am = getArchetypeManager();
		static float f = 0.0f;
		float clear_color[3];
		clear_color[0] = window->getColorR();
		clear_color[1] = window->getColorG();
		clear_color[2] = window->getColorB();

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		ImGui::Begin("Create Primitives!");
		
		auto& vec0 = am->get<Transform>(newEntity)->position;
		ImGui::Text("Pos box0 : (%.8f, %.8f, %.8f)", vec0.x, vec0.y, vec0.z );

		ImGui::SliderFloat("pos   x", (float*) &newPosition.x, -5.0f, 5.0f);
		ImGui::SliderFloat("pos   y", (float*) &newPosition.y, -5.0f, 5.0f);
		ImGui::SliderFloat("pos   z", (float*) &newPosition.z, -5.0f, 5.0f);

		ImGui::SliderAngle("angle", (float*)  &newAngle, 0, 360);
		ImGui::SliderFloat("axis x", (float*) &newAngleAxis.x, -1, 1);
		ImGui::SliderFloat("axis y", (float*) &newAngleAxis.y, -1, 1);
		ImGui::SliderFloat("axis z", (float*) &newAngleAxis.z, -1, 1);
		ImGui::SliderFloat("size  x", (float*) &newSize.x, 0.1, 3.0);
		ImGui::SliderFloat("size  y", (float*) &newSize.y, 0.1, 3.0);
		ImGui::SliderFloat("size  z", (float*) &newSize.z, 0.1, 3.0);
		ImGui::ColorEdit3("color", (float*) &newColor);

#ifndef STACKING
		if (! physicsOn)
		{

			float cos = glm::cos(glm::radians(newAngle) / 2.0f);
			float sin = glm::sin(glm::radians(newAngle) / 2.0f);
			newAngleAxis = glm::normalize(newAngleAxis);

			am->get<Transform>(newEntity)->position = newPosition;
			auto& quat = am->get<Transform>(newEntity)->orientation;
			quat.w = cos;
			quat.x = sin * newAngleAxis.x;
			quat.y = sin * newAngleAxis.y;
			quat.z = sin * newAngleAxis.z;
			am->get<Transform>(newEntity)->scale = float3{ newSize };
			am->get<Color>(newEntity)->vec = float4{ newColor.r, newColor.g, newColor.b, 1.0f };

			if (ImGui::Button("Circle"))
			{
				if (am->has<Square>(newEntity)) am->remove<Square>(newEntity);
				if (am->has<Sphere>(newEntity)) am->remove<Sphere>(newEntity);
				if (am->has<Segment>(newEntity)) am->remove<Segment>(newEntity);
				if (am->has<Box>(newEntity)) am->remove<Box>(newEntity);
				if (!am->has<Circle>(newEntity))am->add<Circle>(newEntity, Circle{});
			}
			ImGui::SameLine();
			if (ImGui::Button("Quad"))
			{
				if (am->has<Circle>(newEntity)) am->remove<Circle>(newEntity);
				if (am->has<Sphere>(newEntity)) am->remove<Sphere>(newEntity);
				if (am->has<Segment>(newEntity)) am->remove<Segment>(newEntity);
				if (am->has<Box>(newEntity)) am->remove<Box>(newEntity);
				if (!am->has<Square>(newEntity))am->add<Square>(newEntity, Square{});
			}
			ImGui::SameLine();
			if (ImGui::Button("Segment"))
			{
				if (am->has<Circle>(newEntity)) am->remove<Circle>(newEntity);
				if (am->has<Sphere>(newEntity)) am->remove<Sphere>(newEntity);
				if (am->has<Square>(newEntity)) am->remove<Square>(newEntity);
				if (am->has<Box>(newEntity)) am->remove<Box>(newEntity);
				if (!am->has<Segment>(newEntity)) am->add<Segment>(newEntity, Segment{});
			}

			if (ImGui::Button("Sphere"))
			{
				if (am->has<Circle>(newEntity)) am->remove<Circle>(newEntity);
				if (am->has<Segment>(newEntity)) am->remove<Segment>(newEntity);
				if (am->has<Square>(newEntity)) am->remove<Square>(newEntity);
				if (am->has<Box>(newEntity)) am->remove<Box>(newEntity);
				if (!am->has<Sphere>(newEntity)) am->add<Sphere>(newEntity, Sphere{});
			}
			ImGui::SameLine();
			if (ImGui::Button("Box"))
			{
				if (am->has<Circle>(newEntity)) am->remove<Circle>(newEntity);
				if (am->has<Segment>(newEntity)) am->remove<Segment>(newEntity);
				if (am->has<Square>(newEntity)) am->remove<Square>(newEntity);
				if (am->has<Sphere>(newEntity)) am->remove<Sphere>(newEntity);
				if (!am->has<Box>(newEntity)) am->add<Box>(newEntity, Box{});
			}


			if (ImGui::Button("Create"))
			{
				Entity oldEntity = newEntity;
				newEntity = Application::getEntityManager()->create();
				am->setArchetype(newEntity, *am->getArchetype(oldEntity));
				am->copy(newEntity, oldEntity);
			}
		}
#endif

		ImGui::End();

		ImGui::Begin("Camera");
		ImGui::SliderFloat("px", &deltaPosition.x, -5.0f, 5.0f);
		ImGui::SliderFloat("py", &deltaPosition.y, -5.0f, 5.0f);
		ImGui::SliderFloat("pz", &deltaPosition.z, -5.0f, 5.0f);
		ImGui::SliderFloat("ux", &up.x, -1.0f, 1.0f);
		ImGui::SliderFloat("uy", &up.y, -1.0f, 1.0f);
		ImGui::SliderFloat("uz", &up.z, -1.0f, 1.0f);
		if (projection == Projection::ORTHO)
		{
			deltaPosition = { 0.0 };
			camera.getEye() = float3{0.0, 0.0, -5.0f} + deltaPosition;
			camera.getCenter() = deltaPosition;
		}
		else if (projection == Projection::PERSP)
		{
			ImGui::SliderFloat("cx", &centerPosition.x, -5.0f, 5.0f);
			ImGui::SliderFloat("cy", &centerPosition.y, -5.0f, 5.0f);
			ImGui::SliderFloat("cz", &centerPosition.z, -5.0f, 5.0f);
			camera.getEye() = initialPosition + deltaPosition;
			camera.getCenter() = centerPosition;
		}
		camera.getUp() = up;
		
		ImGui::Text("Projection");
		if (projection == Projection::ORTHO)
		{
			if (ImGui::Button("Ortho"))
			{
				projection = Projection::PERSP;
			}

			ImGui::SliderFloat("right", &right, -5.0f, 5.0f);
			ImGui::SliderFloat("top", &top, -5.0f, 5.0f);
			projectionMatrix = glm::ortho(-right, right, -top, top, nearPlane, farPlane);
		}
		else if (projection == Projection::PERSP)
		{
			if (ImGui::Button("Persp"))
			{
				projection = Projection::ORTHO;
			}

			//ImGui::SliderAngle("fovy", &fovy, 0.0, 180);
			////ImGui::SliderFloat("aspect", &aspect, 0.1f, 2.0f);
			//aspect = float(window->getWidth()) / float(window->getHeight());
			//projectionMatrix = glm::perspective(fovy, aspect, nearPlane, farPlane);
		}
		ImGui::End();

		ImGui::Begin("Canvas");
#ifndef STACKING
		if (ImGui::Button("Physics"))
		{
			if (physicsOn)
			{
				physicsOn = false;
				auto* rb = am->get<RigidBody>(newEntity);
				rb->angVelocity = real3{ 0.0 };
				rb->velocity = real3{ 0.0 };
			}
			else
			{
				physicsOn = true;
			}

		}
#endif 
#ifdef STACKING
		if (frameCounter % 1500 == 0)//ImGui::Button("Shoot"))
		{
			//float3 v{0.2*Random::randFloat(), 1.0 + 0.2*Random::randFloat(), 0.2*Random::randFloat()};
			float3 v{0.5, 0.5, 0.5};
			//float3 c{Random::randFloat(), Random::randFloat(), Random::randFloat()};
			float3 c{0.0, 1.0, 0.0};

			float angle = 30;
			create_box({ 2, 5, 13 }, angle, { 0.0, 1.0, -1.0 }, v, -0.6, 0.1, real3{0.0, 0.0, -6.0}, { c.r, c.g, c.b, 1.0 });
		}
#endif
		ImGui::Checkbox("Demo Window", &show_demo_window);
		ImGui::ColorEdit3("Bckg color", (float*)&clear_color);
		window->setColor(clear_color[0], clear_color[1], clear_color[2]);
		ImGui::Text("%.1f FPS (%.1f ms/frame)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);


		float plane_x = am->get<Transform>(gridEnt)->orientation.x;
		float plane_y = am->get<Transform>(gridEnt)->orientation.y;
		float plane_z = am->get<Transform>(gridEnt)->orientation.z;
		float plane_o = am->get<Transform>(gridEnt)->position.x;
		ImGui::SliderFloat("plane nx",  &plane_x, -1, 1);
		ImGui::SliderFloat("plane ny",  &plane_y, -1, 1);
		ImGui::SliderFloat("plane nz",  &plane_z, -1, 1);
		ImGui::SliderFloat("plane off", &plane_o, -5.0, 5.0);
		auto planeNormal = glm::normalize(float3{ plane_x, plane_y, plane_z });
		am->get<Transform>(gridEnt)->orientation = rquat{ 1.0, planeNormal.x, planeNormal.y, planeNormal.z };
		am->get<Transform>(gridEnt)->position.x = plane_o;

		ImGui::Text("Sphere Settings");
		int circleResolution = primitivesRenderer.getCircleResolution();
		int sphereResolution = primitivesRenderer.getSphereResolution();
		int sphereMeridians  = primitivesRenderer.getSphereMeridians();
		int sphereParallels  = primitivesRenderer.getSphereParallels();
		ImGui::SliderInt("circ. res", &circleResolution, 1, 50);
		ImGui::SliderInt("sph. res",  &sphereResolution, 1, 50);
		ImGui::SliderInt("sph. mer",  &sphereMeridians, 1, 50);
		ImGui::SliderInt("sph. par",  &sphereParallels, 1, 50);
		primitivesRenderer.setCircleResolution(circleResolution);
		primitivesRenderer.setSphereResolution(sphereResolution);
		primitivesRenderer.setSphereMeridians(sphereMeridians);
		primitivesRenderer.setSphereParallels(sphereParallels);


		ImGui::End();
	}

	virtual bool onEvent(r3d::Event& e) override
	{
		using namespace r3d;
		EventDispatcher dispatcher(e);
		//HANDLE_DISPATCH(dispatcher, MouseButtonPressedEvent, WorldLayer::onMouseButtonPressedEvent);
		return false;
	}
#if 0
	bool onMouseButtonPressedEvent(r3d::MouseButtonPressedEvent& e)
	{
		using namespace r3d;
		if (ImGui::IsAnyWindowHovered() == false)
		{
			auto am = getArchetypeManager();
			auto newEnt = Application::getEntityManager()->create();
			am->setArchetype < Transform, PrimitiveTag, Square, Color >(newEnt);
			am->set<Position>(newEnt, Position{ {rand()/RAND_MAX - 0.5, 0.0, rand() / RAND_MAX - 0.5} });
			am->set<Orientation>(newEnt, Orientation{ {0.0, newPrimitiveAngle, 0.0} });
			am->set<Scale>(newEnt, Scale{ {1.0, 1.0, 1.0} });
			am->set<Square>(newEnt, Square{ 1.0, 1.0 });
			float4 newColor4 = { newPrimitiveColor.r, newPrimitiveColor.g, newPrimitiveColor.b, 1.0 };
			am->set<Color>(newEnt, newColor4);
		}
		return false;
	}
#endif
};

class Sandbox : public r3d::Application
{
public:
	Sandbox() : Application{ "Sandbox app.", 800, 800 }
	{
#ifdef R3D_DEBUG_APP
		setDebugMode(false);
#endif
		pushBackLayer(new WorldLayer());
	}
	~Sandbox()
	{
	}
};

r3d::Application* r3d::createApplication()
{
	return new Sandbox();
}