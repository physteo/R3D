#include <R3D/Physics/Core/Colliders.h>
#include "WorldLayer.h"

namespace r3d
{
	
	Entity WorldLayer::createSegment(r3d::float3 position, float angleDeg, r3d::float3 axis, float scale, r3d::float4 color)
	{
		using namespace r3d;

		r3d::fquat quat;
		axis = glm::dot(axis, axis) < R3D_EPSILON ? float3{ 0.0 } : glm::normalize(axis);
		float cos = glm::cos(glm::radians(angleDeg / 2.0));
		float sin = glm::sin(glm::radians(angleDeg / 2.0));
		quat.w = cos; quat.x = sin * axis.x; quat.y = sin * axis.y; quat.z = sin * axis.z;

		ArchetypeManager* am = getArchetypeManager();

		Entity segmentEnt = Application::getEntityManager()->create();
		am->setArchetype < Transform, SegmentPrimitive, Color >(segmentEnt);
		am->set<Transform>(segmentEnt, Transform{ position, quat, float3{scale} });
		am->set<Color>(segmentEnt, Color{ color });

		return segmentEnt;
	}

	Entity WorldLayer::createBox(r3d::real3 position, r3d::rquat quat, r3d::real3 scale, r3d::real gravity, r3d::real invMass, r3d::real3 velocity, r3d::float4 color, float timer)
	{
		using namespace r3d;

		auto boxEnt = Application::getEntityManager()->create();

		ArchetypeManager* am = getArchetypeManager();

		real3x3 invI = (invMass < R3D_EPSILON) ? real3x3{ 0.0f } : glm::inverse(r3d::ColliderBox::computeInertiaTensor(scale, 1.0 / invMass)); // TODO: put back the inverse
		real3 force = real3{ 0.0, (invMass < R3D_EPSILON) ? 0.0 : gravity / invMass,0.0 };
		real3 torque = real3{ 0.0 };
		real3 angVelocity = real3{ 0.0 };
		real friction = 0.1;

		am->setArchetype < Transform, RigidBody, Color, ColliderBox, BoxPrimitive >(boxEnt);
		am->set<Transform>(boxEnt, Transform{ position, quat, scale });
		am->set<Color>(boxEnt, color);
		am->set<RigidBody>(boxEnt, RigidBody{ invI, velocity, angVelocity, force, torque, invMass, friction });
		if (timer > 0)
		{
			SelfDestructionTimer sdt;
			sdt.birth = Application::getWindow()->getCurrentTime();
			sdt.lifespan = timer;
			am->add<SelfDestructionTimer>(boxEnt, std::move(sdt));
		}

		return boxEnt;
	}

	Entity WorldLayer::createBox(r3d::real3 position, r3d::real angle, r3d::real3 axis, r3d::real3 scale, r3d::real gravity, r3d::real invMass, r3d::real3 velocity, r3d::float4 color, float timer)
	{
		real cosSquareAngle = glm::cos(glm::radians(angle / 2.0));
		real sinSquareAngle = glm::sin(glm::radians(angle / 2.0));
		real3 squareAngleAxis = sinSquareAngle * axis;
		rquat squareOrientationQuat = glm::normalize(rquat{ cosSquareAngle, squareAngleAxis.x, squareAngleAxis.y, squareAngleAxis.z });
		
		return createBox(position, squareOrientationQuat, scale, gravity, invMass, velocity, color, timer);
	}
	

	WorldLayer::WorldLayer() : r3d::Layer("World layer.")
	{
		using namespace r3d;

		Application::getWindow()->setColor(0.02f, 0.02f, 0.1f);
		auto am = getArchetypeManager();

		// create axis
		createSegment(float3{ 0.02, 0.02, 0.02 }, 0.0, float3{ 0.0, 0.0, 0.0 }, 1.0, float4{ 1.0, 0.5, 0.5, 1.0 }); // x axis
		createSegment(float3{ 0.02, 0.02, 0.02 }, 90., float3{ 0.0, 1.0, 0.0 }, 1.0, float4{ 0.5, 0.5, 1.0, 1.0 }); // y axis
		createSegment(float3{ 0.02, 0.02, 0.02 }, 90., float3{ 0.0, 0.0, 1.0 }, 1.0, float4{ 0.5, 1.0, 0.5, 1.0 }); // z axis

		// create ground
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

		real3 scale{ 1.0, 1.0, 1.0 };
		real3 position{ 0.0, scale.y, 0.0 };
		int numBoxes = 10;
		for (int i = 0; i < numBoxes; ++i)
		{
			real3 addPos = { 0.0, 2.0 * i * scale.y + 0.05, 0.0 };
			real  scaleFactor = (1.0 - 0.9 * i / float(numBoxes));
			real3 scaleFactorVec = { scaleFactor, scaleFactor, scaleFactor };

			auto ent = createBox(position + addPos, 45 * i, real3{ 0.0, 1.0, 0.0 }, scale * scaleFactorVec, -0.6, 1.0, { 0.0, 0.0, 0.0 },
				Palette::getInstance().blue);

			if (i == numBoxes - 1)
			{
				newEntity = ent;
			}
			else if (i == 0)
			{
				firstBox = ent;
			};
		}

		real3 wallscale = { 5.0, 3.0, 0.5 };
		real3 wallscaleMini = { 5.0, 1.0, 0.5 };
		createBox(real3{ +0.0, wallscale.y, -5.0 }, 0, real3{ 0.0, 1.0, 0.0 }, wallscale, -0.6, 0.005, { 0.0, 0.0, 0.0 }, Palette::getInstance().red);
		createBox(real3{ -5.5, wallscale.y, 0.0 }, 90, real3{ 0.0, 1.0, 0.0 }, wallscale, -0.6, 0.005, { 0.0, 0.0, 0.0 }, Palette::getInstance().red);
		createBox(real3{ +5.5, wallscale.y, 0.0 }, 90, real3{ 0.0, 1.0, 0.0 }, wallscale, -0.6, 0.005, { 0.0, 0.0, 0.0 }, Palette::getInstance().red);
		createBox(real3{ +0.0, wallscaleMini.y, +5.5 }, 0, real3{ 0.0, 1.0, 0.0 }, wallscaleMini, -0.6, 0.005, { 0.0, 0.0, 0.0 }, Palette::getInstance().red);

		real3 woodscale = { 0.2, 4.0, 1.0 };
		createBox(real3{ +3.0, woodscale.y + 0.0, 0.0 }, +5.0, real3{ 0.0, 0.0, 1.0 }, woodscale, -0.6, 1.0, { 0.0, 0.0, 0.0 }, Palette::getInstance().orange);
		createBox(real3{ -3.0, woodscale.y + 0.0, 0.0 }, -5.0, real3{ 0.0, 0.0, 1.0 }, woodscale, -0.6, 1.0, { 0.0, 0.0, 0.0 }, Palette::getInstance().orange);

#else
		physicsOn = false;
		createBox(real3{ 0.0, 1.0, 0.0 }, 0.0, real3{ 1.0 ,0.0, 0.0 }, real3{ 1.0 }, 0.0, 0.00001, real3{ 0.0 }, float4{ 0.5, 0.5, 0.5, 1.0 });

		newAngle = 36.0f;
		newAngleAxis = float3{ 0.995f, 0.006f, 0.101f };
		newPosition = { 1.077, 4.231, 0.0 };
		newSize = { 1.0f };
		newColor = { 0.05f, 0.3f, 0.9f };
		newEntity = createBox(newPosition, newAngle, newAngleAxis, newSize, -2.0, 1.0, real3{ 0.0 }, { newColor.r, newColor.g, newColor.b, 1.0f });

#endif

		// camera
		right = 5.0f;
		top = 5.0f;
		fovy = glm::radians(90.0f);
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
			initialPosition = float3{ -3.0f, 7.0f, 15.5f };
#else
			initialPosition = float3{ 0.0f, 2.0f, 4.0f };
#endif
			centerPosition = float3{ 0.0 };
			camera = Camera{ initialPosition, centerPosition, up };

			gun = Application::getEntityManager()->create();
			real distanceFromCamera = 4.0;
			float3 scale{ 0.2, 0.2, 1.0 };
			am->setArchetype<Transform, Color, BoxPrimitive, float>(gun);
			am->set<Color>(gun, Color{ Palette::getInstance().orange });
			am->set<float>(gun, distanceFromCamera);
			am->set<Transform>(gun, Transform{ {}, {}, scale });
		}

		// options
		wireframesOn = true;
		solidsOn = true;
		physicsOn = true;
		show_demo_window = true;

		// shaders
		linesShader = Shader{ "C:/dev/R3D/R3D/res/shaders/lines.shader" };
		solidShader = Shader{ "C:/dev/R3D/R3D/res/shaders/solid.shader" };
		lampShader = Shader{ "C:/dev/R3D/R3D/res/shaders/lamp.shader" };

		primitivesRenderer.setShader(&linesShader);
		solidRenderer.setShader(&solidShader);
		solidRenderer.setLampShader(&lampShader);

		// Event listeners
		pushBackLayerListener(MouseButtonPressedEvent::getStaticType(), this);

		// physics stuff
		sphereSphereContactDetector.setCollisionData(&collisionData);
		spherePlaneContactDetector.setCollisionData(&collisionData);
		boxPlaneContactDetector.setCollisionData(&collisionData);
		boxBoxContactDetector.setCollisionData(&collisionData);

		frameCounter = 0;
		mouseStatus.lastPressedTime = 0.0f;
		mouseStatus.sensibility = 0.05f;
		cameraMode.fps = false;
		cameraMode.lastSwitchedTime = 0.0f;
		
		fboSize = {100, 100};
		fboSizePrev = fboSize;
		viewWindowCenter = { 0, 0 };
		lastFboResize = 0;
		fbo.attach2DTexture(GL_COLOR_ATTACHMENT0, fboSize.x, fboSize.y, TextureFormat::RGB, GL_UNSIGNED_BYTE);
		fbo.attachRenderBuffer(GL_DEPTH_COMPONENT, fboSize.x, fboSize.y);

		if (false == fbo.isComplete())
		{
			R3D_ASSERT(false, "Fbo not complete.");
		}

#if defined(R3D_DEBUG) || defined(R3D_RELEASE)
		// Debugger 
		Application::getDebugger()->setAm(getArchetypeManager());
		Application::getDebugger()->setEm(Application::getEntityManager());
#endif		 
	}

	void WorldLayer::onUpdate(r3d::Window* window)
	{
		++frameCounter;
		double t = window->getLastTime();
		double dt = window->getLastFrameTime();
		auto am = getArchetypeManager();
		auto em = r3d::Application::getEntityManager();

		if (physicsOn)
		{
			// integrate forces
			forceIntegrator.update(*am, t, dt);

			// detect contacts
			sphereSphereContactDetector.update(*am, t, dt);
			spherePlaneContactDetector.update(*am, t, dt);
			boxPlaneContactDetector.update(*am, t, dt);
			boxBoxContactDetector.update(*am, t, dt);

			// solve contacts
			collisionData.clearArbiters(em);
			collisionData.preStep(am, dt);
			collisionData.applyImpulses(am, 10);

			// update positions
			velocityIntegrator.update(*am, t, dt);

			// delete self destructing objects
			selfDestructionSystem.update(*am, t, dt);

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
		}

#if defined(R3D_DEBUG) || defined(R3D_RELEASE)
		// remove contact entities
		contactEntityRemover.update(*am, t, dt);
		// draw contacts
		contactEntityGenerator.setContacts(&collisionData);
		contactEntityGenerator.update(*am, t, dt);
#endif

		// inputs
		if (cameraMode.fps)
		{
			window->setCursorDisabled();
			std::pair<float, float> mousePos = Input::getInstance().getMousePosition(*window);
			float dx = mousePos.first - window->getWidth() / 2.0;
			float dy = mousePos.second - window->getHeight() / 2.0;
			float3 cameraDir = camera.getCenter() - camera.getEye();
			float theta = -mouseStatus.sensibility * dt * dy;
			float phi = -mouseStatus.sensibility * dt * dx;
			float3 axist = sin(theta) * camera.getCameraX();
			float3 axisp = sin(phi) * camera.getCameraY();
			fquat qt = fquat{cos(theta), axist.x, axist.y, axist.z };
			fquat qp = fquat{cos(phi), axisp.x, axisp.y, axisp.z};
			float3 rotatedDir = qp * qt * cameraDir;
			camera.setCenter(camera.getEye() + rotatedDir);
			
			// mouse pressed
			if (Input::getInstance().isMouseButtonPressed(GLFW_MOUSE_BUTTON_1, *window))
			{
				float current = window->getCurrentTime();
				if (current - mouseStatus.lastPressedTime > 0.1)
				{
					onMouseButtonPressedEvent(MouseButtonPressedEvent(GLFW_MOUSE_BUTTON_1, 0, mousePos));
					mouseStatus.lastPressedTime = current;
				}
			}

			window->setCursorPosition({ window->getWidth() / 2.0, window->getHeight() / 2.0 });

			float wasdSpeed = 0.1f;
			float3 delta{0.0f};
			if (Input::getInstance().isKeyPressed(GLFW_KEY_W, *window))
			{
				delta += -wasdSpeed * camera.getCameraZ();
			}
			if (Input::getInstance().isKeyPressed(GLFW_KEY_S, *window))
			{
				delta += +wasdSpeed * camera.getCameraZ();
			}
			if (Input::getInstance().isKeyPressed(GLFW_KEY_A, *window))
			{
				delta += -wasdSpeed * camera.getCameraX();
			}
			if (Input::getInstance().isKeyPressed(GLFW_KEY_D, *window))
			{
				delta += +wasdSpeed * camera.getCameraX();
			}

			camera.setCenter(camera.getCenter() + delta);
			camera.setEye(camera.getEye() + delta);
		}

		if (Input::getInstance().isKeyPressed(GLFW_KEY_TAB, *window))
		{
			float current = window->getCurrentTime();
			if (current - cameraMode.lastSwitchedTime > 0.5f) 
			{
				cameraMode.lastSwitchedTime = current;
				cameraMode.fps = !cameraMode.fps;
				if (cameraMode.fps)
				{
					window->setCursorDisabled();
					window->setCursorPosition({ window->getWidth() / 2.0, window->getHeight() / 2.0 });
				}
				else
				{
					window->setCursorVisible();
				}
			}
		}

		if (Input::getInstance().isKeyPressed(GLFW_KEY_ESCAPE, *window))
		{
			window->close();
		}
	}

	void WorldLayer::onRender(r3d::Window* window)
	{
		double t = window->getCurrentTime();
		double dt = window->getLastFrameTime();
		r3d::ArchetypeManager* am = getArchetypeManager();

		aspect = fboSize.x / fboSize.y;
		projectionMatrix = glm::perspective(fovy, aspect, nearPlane, farPlane);

		// pass uniforms to shaders
		linesShader.bind();
		linesShader.setUniformMatrix("view", camera.getViewMatrix(), false);
		linesShader.setUniformMatrix("projection", projectionMatrix, false);

		solidShader.bind();
		solidShader.setUniformMatrix("view", camera.getViewMatrix(), false);
		solidShader.setUniformMatrix("projection", projectionMatrix, false);
		solidShader.setUniformValue("cameraPos", camera.getEye());

		lampShader.bind();
		lampShader.setUniformMatrix("view", camera.getViewMatrix(), false);
		lampShader.setUniformMatrix("projection", projectionMatrix, false);

		window->setViewPort(fboSize.x, fboSize.y);
		fbo.bind();
		window->clearColorBufferBit();

		if (wireframesOn)
		{
			primitivesRenderer.setShader(&linesShader);
			primitivesRenderer.update(*am, t, dt);
		}

		if (solidsOn)
		{
			solidRenderer.setShader(&solidShader);
			solidRenderer.update(*am, t, dt);
		}
		
		fboQuad.draw(fbo.getTextureID(0));
		fbo.unbind();
		window->setViewPort(window->getWidth(), window->getHeight());
		
	}

	bool WorldLayer::onEvent(r3d::Event& e)
	{
		using namespace r3d;
		EventDispatcher dispatcher(e);
		HANDLE_DISPATCH(dispatcher, MouseButtonPressedEvent, WorldLayer::onMouseButtonPressedEvent);
		HANDLE_DISPATCH(dispatcher, ViewWindowResizeEvent, WorldLayer::onViewWindowResizeEvent);
		return false;
	}

#if 1

	bool WorldLayer::onViewWindowResizeEvent(r3d::ViewWindowResizeEvent& e)
	{
		fbo = std::move(FrameBuffer{});
		fbo.attach2DTexture(GL_COLOR_ATTACHMENT0, fboSize.x, fboSize.y, TextureFormat::RGB, GL_UNSIGNED_BYTE);
		fbo.attachRenderBuffer(GL_DEPTH_COMPONENT, fboSize.x, fboSize.y);
		return true;
	}

	bool WorldLayer::onMouseButtonPressedEvent(r3d::MouseButtonPressedEvent& e)
	{ 
		using namespace r3d;
		if (physicsOn)// && ImGui::IsAnyWindowHovered() == false)
		{
			auto am = getArchetypeManager();
			float3 v{ 0.2, 0.2, 0.2 };
			float3 c = Palette::getInstance().green;
			float angle = 0;
			real3 bulletPos = am->get<Transform>(gun)->position;
			real3 bulletVel = -real(25.0 + Random::randFloat() * 2.0 - 1.0) * camera.getCameraZ();
			fquat bulletRot = glm::toQuat(real3x3{ camera.getCameraX(), camera.getCameraY(), camera.getCameraZ() });
			createBox(bulletPos, bulletRot, v, -0.6, 10.0, bulletVel, { c.r, c.g, c.b, 1.0 }, 2.5);
		}

		return false;
	}
#endif


};
