#include <R3D.h>

namespace r3d
{

	struct ContactEntityTag { };

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
			archetypeCollisionPoint = Archetype{ ComponentList::buildList<Circle, Color, Position, Rotation, Scale, ContactEntityTag, PrimitiveTag>() };
			archetypeCollisionNormal = Archetype{ ComponentList::buildList<Segment, Color, Position, Rotation, Scale, ContactEntityTag, PrimitiveTag>() };
		}

		void setContacts(const CollisionData* collData) { m_collData = collData; }
		virtual void update(r3d::ArchetypeManager& am, double t, double dt) override
		{
			for (size_t i = 0; i < m_collData->contacts.size(); ++i)
			{
				float3 cross = glm::cross(float3{ 1.0, 0.0, 0.0 }, (float3) m_collData->contacts[i].contactNormal);
				float dot = glm::dot(float3{ 1.0, 0.0, 0.0 }, (float3)m_collData->contacts[i].contactNormal);
				float cos = dot;
				float sin_half = glm::sqrt((1 - cos) / 2.0f);
				float cos_half = glm::sqrt((1 + cos) / 2.0f);
				fquat quat;

				// normalize cross
				cross = glm::normalize(cross);
				quat.w = cos_half;
				quat.x = cross.x * sin_half;
				quat.y = cross.y * sin_half;
				quat.z = cross.z * sin_half;

				float segLength = 0.333;
				float3 displacement{ 1.0, 0.0, 0.0 };
				float3 rotated_displacement = quat * displacement;
				rotated_displacement = segLength * rotated_displacement / glm::length(rotated_displacement);

				auto entity_s = Application::getEntityManager()->create();
				am.setArchetype(entity_s, archetypeCollisionNormal);
				am.set<Segment>(entity_s, Segment{});
				am.set<Color>(entity_s, Color{ float4{1.0, 0.0, 0.0, 1.0} });
				am.set<Position>(entity_s, Position{ (float3) m_collData->contacts[i].contactPoint + rotated_displacement });
				am.set<Rotation>(entity_s, Rotation{ quat });
				am.set<Scale>(entity_s, Scale{ float3{0.2} });

				auto entity_c = Application::getEntityManager()->create();
				am.setArchetype(entity_c, archetypeCollisionPoint);
				am.set<Circle>(entity_c, Circle{});
				am.set<Color>(entity_c, Color{ float4{1.0, 0.0, 0.0, 1.0}  });
				am.set<Position>(entity_c, Position{ m_collData->contacts[i].contactPoint  });
				am.set<Rotation>(entity_c, Rotation{ quat });
				am.set<Scale>(entity_c, Scale{ float3{0.1} });

			}
		}

	private:
		const CollisionData* m_collData;
		Archetype archetypeCollisionPoint;
		Archetype archetypeCollisionNormal;
	};

}

class UpdatePositionSys : public r3d::System
{

public:
	UpdatePositionSys() : System() {}
	virtual void update(r3d::ArchetypeManager& am, double t, double dt) override
	{
		auto archetypes = am.matchAtLeastWithout(r3d::ComponentList::buildList<r3d::Position>(), {});
		for (auto arch = archetypes.begin(); arch != archetypes.end(); ++arch)
		{
			auto numEnts = getSize<r3d::Position>(am, *arch);
			auto positions = get<r3d::Position>(am, *arch);
			for (size_t i = 0; i < numEnts; ++i)
			{
				positions[i].vec.x = sin(t);
			}
		}
	}
};

class WorldLayer : public r3d::Layer
{
	r3d::SphereSphereContactDetector sphereSphereContactDetector;
	r3d::SpherePlaneContactDetector spherePlaneContactDetector;
	r3d::ContactPointEntityGenerator contactEntityGenerator;
	r3d::ContactPointEntityRemover contactEntityRemover;
	r3d::CollisionData collisionData;

	UpdatePositionSys updatePositionSys;
	r3d::Entity circleEnt;
	r3d::Entity squareEnt;
	r3d::Entity segmentEntX;
	r3d::Entity segmentEntY;
	r3d::Entity segmentEntZ;
	r3d::Entity gridEnt;

	r3d::Entity newEntity;
	r3d::float3 newPosition;
	float newAngle;
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
	enum class Projection
	{
		ORTHO,
		PERSP
	} projection;

	r3d::Shader linesShader;
	r3d::PrimitivesRenderer primitivesRenderer;

public:
	WorldLayer() : r3d::Layer("World layer.")
	{
		using namespace r3d;

		Profiler p;
		Profiler::startCollect();
		Profiler::stopCollect("Random stuff");

		Application::getWindow()->setColor(0.02f, 0.02f, 0.1f);

		circleEnt = Application::getEntityManager()->create();
		auto am = getArchetypeManager();
		am->setArchetype < Position, Rotation, Scale, PrimitiveTag, Color, Sphere> (circleEnt);
		am->set<Position>(circleEnt, Position{ {-0.75f, 2.5, 0.0} });
		am->set<Rotation>(circleEnt, Rotation{ {1.0, 0.0, 0.0, 0.0} });
		am->set<Scale>(circleEnt, Scale{ float3{1.0f} });
		am->set<Sphere>(circleEnt, Sphere{});
		am->set<Color>(circleEnt, float4{ 0.5, 0.32, 0.0, 1.0 });

		squareEnt = Application::getEntityManager()->create();
		am->setArchetype < Position, Rotation, Scale, PrimitiveTag, Square, Color >(squareEnt);
		am->set<Position>(squareEnt, Position{ {+0.75, 2.5, 0.0} });
		am->set<Rotation>(squareEnt, Rotation{ {1.0, 0.0, 0.0, 0.0} });
		am->set<Scale>(squareEnt, Scale{ float3{1.0} });
		am->set<Square>(squareEnt, Square{});
		am->set<Color>(squareEnt, float4{ 0.18, 0.4, 0.9, 1.0 });
		

		float cos45 = glm::cos(glm::radians(45.f));
		float sin45 = glm::sin(glm::radians(45.f));
		r3d::fquat quat1;
		quat1.w = 1.0; quat1.x = 0.0; quat1.y = 0.0; quat1.z = 0.0;
		segmentEntX = Application::getEntityManager()->create();
		am->setArchetype < Position, Rotation, Scale, PrimitiveTag, Segment, Color >(segmentEntX);
		am->set<Position>(segmentEntX, Position{ {0.0, 0.01, 0.0} });
		am->set<Rotation>(segmentEntX, Rotation{ quat1 });
		am->set<Scale>(segmentEntX, Scale{ {1.0, 1.0, 1.0} });
		am->set<Segment>(segmentEntX, Segment{});
		am->set<Color>(segmentEntX, float4{ 1.0, 0.5, 0.5, 1.0 });

		r3d::fquat quatz;
		quatz.w = cos45; quatz.x = 0.0; quatz.y = sin45; quatz.z = 0.0;
		segmentEntZ = Application::getEntityManager()->create();
		am->setArchetype < Position, Rotation, Scale, PrimitiveTag, Segment, Color >(segmentEntZ);
		am->set<Position>(segmentEntZ, Position{ {0.0, 0.01, 0.0} });
		am->set<Rotation>(segmentEntZ, Rotation{ quatz });
		am->set<Scale>(segmentEntZ, Scale{ {1.0, 1.0, 1.0} });
		am->set<Segment>(segmentEntZ, Segment{});
		am->set<Color>(segmentEntZ, float4{ 0.5, 0.5, 1.0, 1.0 });

		r3d::fquat quaty;
		quaty.w = cos45; quaty.x = 0.0; quaty.y = 0.0; quaty.z = sin45;
		segmentEntY = Application::getEntityManager()->create();
		am->setArchetype < Position, Rotation, Scale, PrimitiveTag, Segment, Color >(segmentEntY);
		am->set<Position>(segmentEntY, Position{ {0.0, 0.01, 0.0} });
		am->set<Rotation>(segmentEntY, Rotation{ quaty });
		am->set<Scale>(segmentEntY, Scale{ {1.0, 1.0, 1.0} });
		am->set<Segment>(segmentEntY, Segment{});
		am->set<Color>(segmentEntY, float4{ 0.5, 1.0, 0.5, 1.0 });

		r3d::fquat quatg;
		quatg.w = cos45; quatg.x = sin45; quatg.y = 0.0; quatg.z = 0.0;
		gridEnt = Application::getEntityManager()->create();
		am->setArchetype < Scale, PrimitiveTag, Plane, Color >(gridEnt);
		am->set<Scale>(gridEnt, Scale{ float3{20.0f} });
		am->set<Plane>(gridEnt, Plane{ float3{0.0f,1.0f,0.0f}, 0.0 });
		am->set<Color>(gridEnt, float4{ 0.2, 0.2, 0.2, 1.0 });

		newEntity = Application::getEntityManager()->create();
		am->setArchetype < Position, Rotation, Scale, PrimitiveTag, Color, Sphere >(newEntity);
		am->set<Position>(newEntity, Position{ {1.0, 0.0, 0.0} });
		am->set<Rotation>(newEntity, Rotation{ {1.0, 0.0, 0.0, 0.0} });
		am->set<Scale>(newEntity, Scale{ float3{0.5} });
		am->set<Sphere>(newEntity, Sphere{});
		am->set<Color>(newEntity, float4{ 1.0, 0.5, 0.5, 1.0 });

		show_demo_window = false;
		right = 5.0f;
		top = 5.0f;
		fovy = 90.0f;
		aspect = 1.0f;
		projection = Projection::ORTHO;
		projectionMatrix = glm::ortho(-right, right, -top, top, 0.1f, 20.f);
		//projectionMatrix = glm::perspective(fovy, aspect, 0.1f, 10.0f);

		linesShader = std::move(Shader{ "C:/Users/matte/source/repos/OpenGL/Rendara3D/res/shaders/lines.shader" });
		primitivesRenderer.setShader(&linesShader);

		newPosition = {0.0};
		newAngleAxis = { 1.0f, 0.0f, 0.0f };
		newAngle = 0.0f;
		newSize = { 1.0f };
		newColor = { 0.2f, 0.2f, 0.2f };

		deltaPosition = r3d::float3{ 0.0f };
		up = float3{ 0.0f, 1.0f, 0.0f };
		initialPosition = float3{ 0.0f, 0.0f, 5.0f };
		centerPosition = float3{ 0.0 };
		camera = Camera{ initialPosition, centerPosition, up };

		// Event listeners
		pushBackLayerListener(MouseButtonPressedEvent::getStaticType(), this);
		
		// physics stuff
		sphereSphereContactDetector.setCollisionData(&collisionData);
		spherePlaneContactDetector.setCollisionData(&collisionData);
	}



	virtual void onUpdate(r3d::Window* window) override
	{
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
		
		// create contact entities
		contactEntityGenerator.setContacts(&collisionData);
		contactEntityGenerator.update(*am, t, dt);
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

		float cos = glm::cos(newAngle / 2.0f);
		float sin = glm::sin(newAngle / 2.0f);
		newAngleAxis = glm::normalize(newAngleAxis);
		am->get<Position>(newEntity)->vec = newPosition;
		auto& quat = am->get<Rotation>(newEntity)->quat;
		quat.w = cos;
		quat.x = sin * newAngleAxis.x;
		quat.y = sin * newAngleAxis.y;
		quat.z = sin * newAngleAxis.z;
		am->get<Scale>(newEntity)->vec = float3{ newSize };
		am->get<Color>(newEntity)->vec = float4{ newColor.r, newColor.g, newColor.b, 1.0f };

		if (ImGui::Button("Circle"))
		{
			if(am->has<Square>(newEntity)) am->remove<Square>(newEntity);
			if (am->has<Sphere>(newEntity)) am->remove<Sphere>(newEntity);
			if(am->has<Segment>(newEntity)) am->remove<Segment>(newEntity);
			if(am->has<Box>(newEntity)) am->remove<Box>(newEntity);
			if(!am->has<Circle>(newEntity))am->add<Circle>(newEntity, Circle{});
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
			camera.getEye() = initialPosition + deltaPosition;
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
			projectionMatrix = glm::ortho(-right, right, -top, top, 0.1f, 20.f);
		}
		else if (projection == Projection::PERSP)
		{
			if (ImGui::Button("Persp"))
			{
				projection = Projection::ORTHO;
			}

			ImGui::SliderAngle("fovy", &fovy, 0.0, 180);
			//ImGui::SliderFloat("aspect", &aspect, 0.1f, 2.0f);
			aspect = float(window->getWidth()) / float(window->getHeight());
			projectionMatrix = glm::perspective(fovy, aspect, 0.1f, 20.0f);
		}
		ImGui::End();

		ImGui::Begin("Canvas");
		ImGui::Checkbox("Demo Window", &show_demo_window);
		ImGui::ColorEdit3("Bckg color", (float*)&clear_color);
		window->setColor(clear_color[0], clear_color[1], clear_color[2]);
		ImGui::Text("%.1f FPS (%.1f ms/frame)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);


		float plane_x = am->get<Plane>(gridEnt)->normal.x;
		float plane_y = am->get<Plane>(gridEnt)->normal.y;
		float plane_z = am->get<Plane>(gridEnt)->normal.z;
		float plane_o = am->get<Plane>(gridEnt)->offset;
		ImGui::SliderFloat("plane nx",  &plane_x, -1, 1);
		ImGui::SliderFloat("plane ny",  &plane_y, -1, 1);
		ImGui::SliderFloat("plane nz",  &plane_z, -1, 1);
		ImGui::SliderFloat("plane off", &plane_o, 0.1, 3.0);
		am->get<Plane>(gridEnt)->normal = glm::normalize(float3{ plane_x, plane_y, plane_z });
		am->get<Plane>(gridEnt)->offset = plane_o;

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
			am->setArchetype < Position, Rotation, Scale, PrimitiveTag, Square, Color >(newEnt);
			am->set<Position>(newEnt, Position{ {rand()/RAND_MAX - 0.5, 0.0, rand() / RAND_MAX - 0.5} });
			am->set<Rotation>(newEnt, Rotation{ {0.0, newPrimitiveAngle, 0.0} });
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