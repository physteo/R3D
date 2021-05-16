
#include "WorldLayer.h"
#include <R3D/Physics/Core/Colliders.h>
#include <R3D/Physics/Core/Colliders.h>

//extern "C"
//{
//	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
//}

namespace r3d
{
	
	void SelfDestruction::update(ArchetypeManager& am, double t, double dt)
	{
		auto archetypes = am.matchAtLeast(ComponentList::buildList<SelfDestructionTimer>(), {});

		std::vector<Entity> toRemove;
		toRemove.reserve(10);

		for (auto arch : archetypes)
		{
			size_t size = getSize<SelfDestructionTimer>(am, arch);
			auto* selfDestr = get<SelfDestructionTimer>(am, arch);
			std::vector<Entity>* entities = getEntities(am, arch);

			for (size_t i = 0; i < size; ++i)
			{
				if (t - selfDestr[i].birth > selfDestr[i].lifespan)
				{
					toRemove.push_back((*entities)[i]);
				}
			}
		}

		// remove
		ManyEntitiesDestroyedEvent e{ std::move(toRemove) };
		Application::dispatchEventStatic(e);
	}

	Entity WorldLayer::createSegment(r3d::float3 position, float angleDeg, r3d::float3 axis, float scale, r3d::float4 color)
	{
		using namespace r3d;

		r3d::fquat quat;
		axis = glm::dot(axis, axis) < R3D_EPSILON ? float3{ 0.0 } : glm::normalize(axis);
		float cos = glm::cos(glm::radians(angleDeg / 2.0f));
		float sin = glm::sin(glm::radians(angleDeg / 2.0f));
		quat.w = cos; quat.x = sin * axis.x; quat.y = sin * axis.y; quat.z = sin * axis.z;

		ArchetypeManager* am = getArchetypeManager();

		Entity segmentEnt = Application::getEntityManager()->create();
		am->setArchetype < Transform, SegmentPrimitive, Color >(segmentEnt);
		am->set<Transform>(segmentEnt, Transform{ position, quat, float3{scale} });
		am->set<Color>(segmentEnt, Color{ color });

		return segmentEnt;
	}

	template<class Tag>
	Entity WorldLayer::createBox(r3d::real3 position, r3d::rquat quat, r3d::real3 scale, r3d::real gravity, r3d::real invMass, r3d::real3 velocity, r3d::float4 color, float timer, const std::string& materialName)
	{
		using namespace r3d;

		auto boxEnt = Application::getEntityManager()->create();

		ArchetypeManager* am = getArchetypeManager();

		real3x3 invI = (invMass < R3D_EPSILON) ? real3x3{ 0.0f } : glm::inverse(r3d::ColliderBox::computeInertiaTensor(scale, (real)1.0 / invMass)); // TODO: put back the inverse
		real3 force = real3{ 0.0, (invMass < R3D_EPSILON) ? 0.0 : gravity / invMass,0.0 };
		real3 torque = real3{ 0.0 };
		real3 angVelocity = real3{ 0.0 };
		real friction = (real)0.1;

		am->setArchetype < Transform, RigidBody, Color, ColliderBox, BoxPrimitive, Tag>(boxEnt);
		am->set<Transform>(boxEnt, Transform{ position, quat, scale });
		am->set<Color>(boxEnt, color);
		am->set<RigidBody>(boxEnt, RigidBody{ invI, velocity, angVelocity, force, torque, invMass, friction });
		if (timer > 0)
		{
			SelfDestructionTimer sdt;
			sdt.birth = (float)Application::getWindow()->getCurrentTime();
			sdt.lifespan = timer;
			am->add<SelfDestructionTimer>(boxEnt, std::move(sdt));
		}
		if (materialName != "")
		{
			am->add<MaterialComp>(boxEnt, MaterialComp{materialName});
			std::string meshName = "cube";// Random::randFloat() < 0.5 ? "cube" : "skydome";
			am->add<MeshID>(boxEnt, MeshID(meshName));
		}

		return boxEnt;
	}

	template<class Tag>
	Entity WorldLayer::createBox(r3d::real3 position, r3d::real angle, r3d::real3 axis, r3d::real3 scale, r3d::real gravity, r3d::real invMass, r3d::real3 velocity, r3d::float4 color, float timer, const std::string& materialName)
	{
		real cosSquareAngle = glm::cos(glm::radians(angle / 2.0f));
		real sinSquareAngle = glm::sin(glm::radians(angle / 2.0f));
		real3 squareAngleAxis = sinSquareAngle * axis;
		rquat squareOrientationQuat = glm::normalize(rquat{ cosSquareAngle, squareAngleAxis.x, squareAngleAxis.y, squareAngleAxis.z });
		
		return createBox<Tag>(position, squareOrientationQuat, scale, gravity, invMass, velocity, color, timer, materialName);
	}
	
	void WorldLayer::setUpFboBloom()
	{
		fboBloom = FrameBuffer{};
		fboBloom.attach2DTexture(GL_COLOR_ATTACHMENT0, fboSize.x, fboSize.y, TextureFormat::RGBA16F, TextureFormat::RGB, GL_FLOAT);
		fboBloom.attach2DTexture(GL_COLOR_ATTACHMENT1, fboSize.x, fboSize.y, TextureFormat::RGBA16F, TextureFormat::RGB, GL_FLOAT);
		fboBloom.setDrawBuffers();

		fboBloomMix = FrameBuffer{};
		fboBloomMix.attach2DTexture(GL_COLOR_ATTACHMENT0, fboSize.x, fboSize.y, TextureFormat::RGBA16F, TextureFormat::RGB, GL_FLOAT);
		fboBloomMix.setDrawBuffers();
	}

	void WorldLayer::setUpFboBlur()
	{
		for (int i = 0; i < 2; ++i)
		{
			fboBlur[i] = FrameBuffer{};
			fboBlur[i].attach2DTexture(GL_COLOR_ATTACHMENT0, fboSize.x, fboSize.y, TextureFormat::RGBA16F, TextureFormat::RGB, GL_FLOAT);
			fboBlur[i].setDrawBuffers();
		}
	}

	void WorldLayer::setUpFboDefault()
	{
		fboDefault = FrameBuffer{};
		fboDefault.attach2DTexture(GL_COLOR_ATTACHMENT0, fboSize.x, fboSize.y, TextureFormat::RGBA16F, TextureFormat::RGB, GL_FLOAT);
		fboDefault.attachRenderBuffer(GL_DEPTH_COMPONENT, fboSize.x, fboSize.y);
		fboDefault.setDrawBuffers();
	}

	void WorldLayer::setUpFboHDR()
	{
		fboHDR = FrameBuffer{};
		fboHDR.attach2DTexture(GL_COLOR_ATTACHMENT0, fboSize.x, fboSize.y, sRGB, TextureFormat::RGB, GL_UNSIGNED_BYTE);
		fboHDR.attachRenderBuffer(GL_DEPTH_COMPONENT, fboSize.x, fboSize.y);
		fboHDR.setDrawBuffers();
	}

	void WorldLayer::setUpFboShadow()
	{
		fboShadow = FrameBuffer{};
		fboShadow.attach2DTexture(GL_DEPTH_ATTACHMENT, fboShadowSize.x, fboShadowSize.y, TextureFormat::DEPTH, TextureFormat::DEPTH, GL_FLOAT, GL_NEAREST, GL_CLAMP_TO_BORDER);
		fboShadow.setDrawBuffers(false);
	}

	void WorldLayer::setUpFboShadowDebug()
	{
		fboShadowDebug = FrameBuffer{};
		fboShadowDebug.attach2DTexture(GL_COLOR_ATTACHMENT0, fboSize.x, fboSize.y, TextureFormat::RGBA16F, TextureFormat::RGB, GL_FLOAT);
		fboShadowDebug.setDrawBuffers();
	}

	void WorldLayer::setUpFbos()
	{
		fboBloom = FrameBuffer{};
		fboDefault = FrameBuffer{};
		fboHDR = FrameBuffer{};
		fboBlur[0] = FrameBuffer{};
		fboBlur[1] = FrameBuffer{};

		setUpFboBloom();
		setUpFboDefault();
		setUpFboHDR();
		setUpFboBlur();
		setUpFboShadowDebug();
	}

	void WorldLayer::setShadersSettings()
	{
		solidShader.bind();
		solidShader.setUniformValue("minShadowBias", minShadowBias);
		solidShader.setUniformValue("maxShadowBias", maxShadowBias);
		solidShader.setUniformValue("invBloomThreshold", bloomSettings.invThreshold[0], bloomSettings.invThreshold[1], bloomSettings.invThreshold[2]);
		solidShader.setUniformValue("cameraPos", camera.getEye());

		// pass lights
		solidShader.setUniformValue("pointLight.eye", solidRenderer.getPointLight().eye);
		solidShader.setUniformValue("pointLight.ambient", solidRenderer.getPointLight().ambient);
		solidShader.setUniformValue("pointLight.diffuse", solidRenderer.getPointLight().diffuse);
		solidShader.setUniformValue("pointLight.specular", solidRenderer.getPointLight().specular);
		solidShader.setUniformValue("pointLight.attenuation.constant", solidRenderer.getPointLight().attenuation.constant);
		solidShader.setUniformValue("pointLight.attenuation.linear", solidRenderer.getPointLight().attenuation.linear);
		solidShader.setUniformValue("pointLight.attenuation.quadratic", solidRenderer.getPointLight().attenuation.quadratic);

		solidShader.setUniformValue("spotLight.eye", solidRenderer.getSpotLight().eye);
		solidShader.setUniformValue("spotLight.direction", solidRenderer.getSpotLight().direction);
		solidShader.setUniformValue("spotLight.ambient", float(solidRenderer.isSpotLightOn()) * solidRenderer.getSpotLight().ambient);
		solidShader.setUniformValue("spotLight.diffuse", float(solidRenderer.isSpotLightOn()) * solidRenderer.getSpotLight().diffuse);
		solidShader.setUniformValue("spotLight.specular", float(solidRenderer.isSpotLightOn()) * solidRenderer.getSpotLight().specular);
		solidShader.setUniformValue("spotLight.cutOff", solidRenderer.getSpotLight().cutOff);
		solidShader.setUniformValue("spotLight.attenuation.constant", solidRenderer.getSpotLight().attenuation.constant);
		solidShader.setUniformValue("spotLight.attenuation.linear", solidRenderer.getSpotLight().attenuation.linear);
		solidShader.setUniformValue("spotLight.attenuation.quadratic", solidRenderer.getSpotLight().attenuation.quadratic);

		solidShader.setUniformValue("sunLight.eye", solidRenderer.getSunLight().eye);
		solidShader.setUniformValue("sunLight.center", solidRenderer.getSunLight().center);
		solidShader.setUniformValue("sunLight.ambient", solidRenderer.getSunLight().ambient);
		solidShader.setUniformValue("sunLight.diffuse", solidRenderer.getSunLight().diffuse);
		solidShader.setUniformValue("sunLight.specular", solidRenderer.getSunLight().specular);

		solidShader.unbind();


		hdrShader.bind();
		hdrShader.setUniformValue("exposure", hdrSettings.exposure);
		hdrShader.setUniformValue("gamma", hdrSettings.gamma);
		hdrShader.unbind();
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
		am->setArchetype < Transform, ColliderPlane, Color, RigidBody, PlanePrimitive, GroundTag, MaterialComp >(gridEnt);
		am->set<Color>(gridEnt, Palette::getInstance().grey);
		am->set<RigidBody>(gridEnt, RigidBody{ real3x3{0.0}, (real)0.0, (real)0.9, real3{0.0} });
		am->set<Transform>(gridEnt, Transform{ real3{plane.offset}, rquat{1.0, plane.normal.x, plane.normal.y, plane.normal.z}, real3{80.} });
		am->set<ColliderPlane>(gridEnt, plane);
		am->set<MaterialComp>(gridEnt, MaterialComp{ "cube" });

#ifdef STACKING

		real3 scale{ 1.0, 1.0, 1.0 };
		real3 position{ 0.0, scale.y, 0.0 };
		size_t numBoxes = 10;
		for (size_t i = 0; i < numBoxes; ++i)
		{
			int turningAngle = 0;
			real  scaleFactor = 1.0;// (1.0 - 0.9 * i / float(numBoxes));
			real3 scaleFactorVec = { scaleFactor, scaleFactor, scaleFactor };
			real3 addPos = { 0.0, 2.0 * scaleFactor * scale.y + static_cast<real>(i + (size_t)1) * 0.1, 0.0 };

			auto ent = createBox<BrickTag>(position, (real)turningAngle * i, real3{ 0.0, 1.0, 0.0 }, scale * scaleFactorVec, stdGravity, (real)1.0 / scaleFactor, { 0.0, 0.0, 0.0 },
				Palette::getInstance().blue, -1.0, "cube");

			position += addPos;

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
		createBox<WallTag>(real3{ +0.0, wallscale.y, -5.0 }, 0, real3{ 0.0, 1.0, 0.0 }, wallscale, stdGravity, (real)0.005, { 0.0, 0.0, 0.0 }, Palette::getInstance().red, -1.0, "brickwall");
		createBox<WallTag>(real3{ -5.5, wallscale.y, 0.0 }, 90, real3{ 0.0, 1.0, 0.0 }, wallscale, stdGravity, (real)0.005, { 0.0, 0.0, 0.0 }, Palette::getInstance().red, -1.0, "brickwall");
		createBox<WallTag>(real3{ +5.5, wallscale.y, 0.0 }, 90, real3{ 0.0, 1.0, 0.0 }, wallscale, stdGravity, (real)0.005, { 0.0, 0.0, 0.0 }, Palette::getInstance().red, -1.0, "brickwall");
		createBox<WallTag>(real3{ +0.0, wallscaleMini.y, +5.5 }, 0, real3{ 0.0, 1.0, 0.0 }, wallscaleMini, stdGravity, (real)0.005, { 0.0, 0.0, 0.0 }, Palette::getInstance().red, -1.0, "brickwall");
		
		real3 woodscale = { 0.2, 4.0, 1.0 };
		createBox<WoodTag>(real3{ +3.0, woodscale.y + 0.0, 0.0 }, +5.0, real3{ 0.0, 0.0, 1.0 }, woodscale, stdGravity, (real)1.0, { 0.0, 0.0, 0.0 }, Palette::getInstance().orange, -1.0, "brickwall");
		createBox<WoodTag>(real3{ -3.0, woodscale.y + 0.0, 0.0 }, -5.0, real3{ 0.0, 0.0, 1.0 }, woodscale, stdGravity, (real)1.0, { 0.0, 0.0, 0.0 }, Palette::getInstance().orange, -1.0, "brickwall");

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
		farPlane = 160.0f;
		projectionType = ProjectionType::PERSP;

		if (projectionType == ProjectionType::ORTHO)
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
			initialPosition = float3{ -6.0f, 9.0f, 13.5f };
#else
			initialPosition = float3{ 0.0f, 2.0f, 4.0f };
#endif
			centerPosition = float3{ 0.0f, 1.0f, 0.0f };
			camera = Camera{ initialPosition, centerPosition, up };

			gun = Application::getEntityManager()->create();
			real distanceFromCamera = 4.0;
			float3 scale{ 0.5, 0.5, 1 };
			am->setArchetype<Transform, Color, float>(gun);
			am->set<Color>(gun, Color{ Palette::getInstance().orange });
			am->set<float>(gun, distanceFromCamera);
			am->set<Transform>(gun, Transform{ {}, {}, scale });
		}

		// options
		wireframesOn = true;
		solidsOn = true;
		physicsOn = false;
		show_demo_window = true;

		// shaders
		linesShader       = Shader{ "C:/dev/R3D/R3D/res/shaders/lines.shader" };
		solidShader       = Shader{ "C:/dev/R3D/R3D/res/shaders/solid.shader" };
		sunLightShader    = Shader{ "C:/dev/R3D/R3D/res/shaders/lamp_sun.shader" };
		pointLightShader  = Shader{ "C:/dev/R3D/R3D/res/shaders/lamp_point.shader" };
		hdrShader         = Shader{ "C:/dev/R3D/R3D/res/shaders/hdr.shader" };
		blurShader        = Shader{ "C:/dev/R3D/R3D/res/shaders/blur.shader" };
		bloomBrightShader = Shader{ "C:/dev/R3D/R3D/res/shaders/bloom_select.shader" };
		bloomMixShader    = Shader{ "C:/dev/R3D/R3D/res/shaders/bloom_mix.shader" };
		shadowShader      = Shader{ "C:/dev/R3D/R3D/res/shaders/shadow.shader" };
		shadowDebugShader = Shader{ "C:/dev/R3D/R3D/res/shaders/shadow_debug.shader" };
		skyShader         = Shader{ "C:/dev/R3D/R3D/res/shaders/sky.shader" };
		contactsShader    = Shader{ "C:/dev/R3D/R3D/res/shaders/contacts.shader" };
		
		// Materials
		// Create texture array.
		// I load all textures and I put them into the texture array
		// I create a map: texture_name -> index in the array
		// I assign to objects the right index (using the map above)

		solidRenderer.materialsDatabase.insert({ "brickwall", Material{ "C:/dev/R3D/Sandbox/res/textures/diffuse_brickwall.jpg",
			"C:/dev/R3D/Sandbox/res/textures/specular_brickwall.jpg",
			"C:/dev/R3D/Sandbox/res/textures/normal_brickwall.jpg", 128 } });

		solidRenderer.materialsDatabase.insert({ "cube", Material{ "C:/dev/R3D/Sandbox/res/textures/diffuse_cube.png",
			"C:/dev/R3D/Sandbox/res/textures/specular_cube.png",
			"C:/dev/R3D/Sandbox/res/textures/normal_cube.png", 128 } });

		wireframesRenderer.setShader(&linesShader);
		solidRenderer.setShader(&solidShader);
		solidRenderer.setSunShader(&sunLightShader);
		solidRenderer.setPointShader(&pointLightShader);

		// Event listeners
		pushBackLayerListener(MouseButtonPressedEvent::getStaticType(), this);

		// Physics stuff
		sphereSphereContactDetector.setCollisionData(&collisionData);
		spherePlaneContactDetector.setCollisionData(&collisionData);
		boxPlaneContactDetector.setCollisionData(&collisionData);
		boxBoxContactDetector.setCollisionData(&collisionData);

		frameCounter = 0;
		
		fboSize = {Application::getWindow()->getWidth(), Application::getWindow()->getHeight() };
		fboSizePrev = fboSize;

		setUpFbos();
		setUpFboShadow();

		// shader's settings
		setShadersSettings();

		if (false == fboDefault.isComplete() || false == fboHDR.isComplete())
		{
			R3D_ASSERT(false, "Fbos not complete.");
		}

#if defined(R3D_DEBUG)
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
			collisionData.preStep(am, (real)dt);
			static const int iterations = 10;
			collisionData.applyImpulses(am, iterations);

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

		// inputs
		if (cameraMode.fps)
		{
			window->setCursorDisabled();
			std::pair<float, float> mousePos = Input::getInstance().getMousePosition(*window);
			float dx = mousePos.first - (float)window->getWidth() / 2.0f;
			float dy = mousePos.second - (float)window->getHeight() / 2.0f;
			float3 cameraDir = camera.getCenter() - camera.getEye();
			float theta = -mouseStatus.sensibility * (float)dt * dy;
			float phi = -mouseStatus.sensibility * (float)dt * dx;
			float3 axist = sin(theta) * camera.getCameraX();
			float3 axisp = sin(phi) * camera.getCameraY();
			fquat qt = fquat{cos(theta), axist.x, axist.y, axist.z };
			fquat qp = fquat{cos(phi), axisp.x, axisp.y, axisp.z};
			float3 rotatedDir = qp * qt * cameraDir;
			camera.setCenter(camera.getEye() + rotatedDir);
			
			// mouse pressed
			if (Input::getInstance().isMouseButtonPressed(GLFW_MOUSE_BUTTON_1, *window))
			{
				float current = (float)window->getCurrentTime();
				if (current - mouseStatus.lastPressedTime > 0.1f)
				{
					onMouseButtonPressedEvent(MouseButtonPressedEvent(GLFW_MOUSE_BUTTON_1, 0, mousePos));
					mouseStatus.lastPressedTime = current;
				}
			}

			std::pair<float, float> cursorPosition = { (float)window->getWidth() / 2.0f, (float)window->getHeight() / 2.0f };
			window->setCursorPosition(cursorPosition);

			float wasdSpeed = 0.5f;
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
			if (Input::getInstance().isKeyPressed(GLFW_KEY_F, *window))
			{
				if (t - lastSpotSwitchedOn > 0.5)
				{
					solidRenderer.switchSpotLight();
					lastSpotSwitchedOn = t;
				}
			}

			camera.setCenter(camera.getCenter() + delta);
			camera.setEye(camera.getEye() + delta);
		}

		if (Input::getInstance().isKeyPressed(GLFW_KEY_TAB, *window))
		{
			float current = (float)window->getCurrentTime();
			if (current - cameraMode.lastSwitchedTime > 0.5f) 
			{
				if (cameraMode.fps)
				{
					window->setCursorVisible();
					am->remove<BoxPrimitive>(gun);
				}
				else
				{
					window->setCursorDisabled();
					window->setCursorPosition({ (float)window->getWidth() / 2.0f, (float)window->getHeight() / 2.0f });
					am->add<BoxPrimitive>(gun, BoxPrimitive{});
				}
				cameraMode.lastSwitchedTime = current;
				cameraMode.fps = !cameraMode.fps;
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

		aspect = (float)fboSize.x / fboSize.y;
		projectionMatrix = glm::perspective(fovy, aspect, nearPlane, farPlane);

		setShadersSettings();

		// pass uniforms to shaders
		linesShader.bind();
		linesShader.setUniformMatrix("view", camera.getViewMatrix(), false);
		linesShader.setUniformMatrix("projection", projectionMatrix, false);

		solidShader.bind();
		solidShader.setUniformMatrix("view", camera.getViewMatrix(), false);
		solidShader.setUniformMatrix("projection", projectionMatrix, false);
		solidShader.setUniformValue("cameraPos", camera.getEye());
		solidShader.setUniformValue("invBloomThreshold", bloomSettings.invThreshold[0], bloomSettings.invThreshold[1], bloomSettings.invThreshold[2]);

		sunLightShader.bind();
		sunLightShader.setUniformMatrix("view", camera.getViewMatrix(), false);
		sunLightShader.setUniformMatrix("projection", projectionMatrix, false);

		pointLightShader.bind();
		pointLightShader.setUniformMatrix("view", camera.getViewMatrix(), false);
		pointLightShader.setUniformMatrix("projection", projectionMatrix, false);

		contactsShader.bind();
		contactsShader.setUniformMatrix("view", camera.getViewMatrix(), false);
		contactsShader.setUniformMatrix("projection", projectionMatrix, false);

		// 0. Shadow pass
		// light space matrix
		float4x4 lightSpaceMat;
		{
			float4x4 lightProj = glm::ortho(lightLRBT[0], lightLRBT[1], lightLRBT[2], lightLRBT[3], lighNearPlane, lighFarPlane);
			float4x4 lightView = glm::lookAt(solidRenderer.getSunLight().eye, solidRenderer.getSunLight().center, glm::vec3(0.0, 1.0, 0.0));
			lightSpaceMat = lightProj * lightView;
		}
		shadowShader.bind();
		shadowShader.setUniformMatrix("lightSpaceMat", lightSpaceMat, false);

		window->setViewPort(fboShadowSize.x, fboShadowSize.y);
		fboShadow.bind();
		window->clearColorBufferBit();
		glCullFace(GL_FRONT);
		if (solidsOn)
		{
			solidRenderer.setShader(&shadowShader);
			solidRenderer.update(*am, t, dt);
			solidRenderer.setShader(&solidShader);
		}
		glCullFace(GL_BACK);
		fboShadow.unbind();
		window->setViewPort(fboSize.x, fboSize.y);

		// 0b Shadow Debug
		fboShadowDebug.bind();
		window->clearColorBufferBit();
		shadowDebugShader.bind();
		shadowDebugShader.setTexture(GL_TEXTURE_2D, "screenTexture", fboShadow.getTextureID(0));
		Vaos::getInstance().quad.bind();
		Vaos::getInstance().quad.draw(GL_TRIANGLES);
		Vaos::getInstance().quad.unbind();
		shadowDebugShader.unbind();
		fboShadowDebug.unbind();

		// 1. Forward rendering to texture
		
		// pass shadows uniforms
		solidShader.bind();
		solidShader.setUniformMatrix("sunLightSpaceMat", lightSpaceMat, false);
		solidShader.setTexture(GL_TEXTURE_2D, "shadowMap", fboShadow.getTextureID(0));

		// render commands
		window->setViewPort(fboSize.x, fboSize.y);
		fboDefault.bind();
		window->clearColorBufferBit();

		// sky first
		glDisable(GL_CULL_FACE);
		glDepthMask(GL_FALSE);
		skyShader.bind();
		auto modelSky = compute_model_matrix(camera.getEye(), fquat{ 1.0, 0.0, 0.0, 0.0 }, float3{farPlane});
		skyShader.setUniformMatrix("projection", projectionMatrix, false);
		skyShader.setUniformMatrix("view", camera.getViewMatrix(), false);
		skyShader.setUniformMatrix("model", modelSky, false);
		skyShader.setUniformValue("sun_pos", solidRenderer.getSunLight().eye);
		Vaos::getInstance().skyDome.bind();
		Vaos::getInstance().skyDome.draw(GL_TRIANGLES);
		Vaos::getInstance().skyDome.unbind();
		skyShader.unbind();
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);

		// world
		if (wireframesOn)
		{
			wireframesRenderer.setShader(&linesShader);
			wireframesRenderer.update(*am, t, dt);
		}

		if (solidsOn)
		{
			solidRenderer.setShader(&solidShader);
			solidRenderer.update(*am, t, dt);
			solidRenderer.drawLights(camera.getEye());
		}
		
#if defined(R3D_DEBUG) || defined(R3D_RELEASE)
		// remove contact entities
		contactEntityRemover.update(*am, t, dt);
		// draw contacts
		contactEntityGenerator.setContacts(&collisionData);
		contactEntityGenerator.setShader(&contactsShader);
		contactEntityGenerator.setCamera(&camera);
		contactEntityGenerator.update(*am, t, dt);
#endif

		fboDefault.unbind();

		if (bloomSettings.on)
		{
			// 2. Bloom: a) Render bright colors
			fboBloom.bind();
			window->clearColorBufferBit();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, fboDefault.getTextureID(0));
			bloomBrightShader.bind();
			bloomBrightShader.setUniformValue("screenTexture", 0);
			bloomBrightShader.setUniformValue("invBloomThreshold", bloomSettings.invThreshold);
			Vaos::getInstance().quad.bind();
			Vaos::getInstance().quad.draw(GL_TRIANGLES);
			Vaos::getInstance().quad.unbind();
			bloomBrightShader.unbind();
			fboBloom.unbind();

			// 3. Bloom: b) Blur bright colors
			blurShader.bind();
			blurShader.setUniformValue("screenTexture", 0);

			int axis = 0;
			bool firstIteration = true;
			for (int i = 0; i < bloomSettings.blurPasses; ++i)
			{
				fboBlur[axis].bind();
				window->clearColorBufferBit();

				blurShader.setUniformValue("axis", axis);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, firstIteration ? fboBloom.getTextureID(1) : fboBlur[!axis].getTextureID(0));
				Vaos::getInstance().quad.bind();
				Vaos::getInstance().quad.draw(GL_TRIANGLES);
				Vaos::getInstance().quad.unbind();
				axis = !axis;
				firstIteration = false;
				fboBlur[axis].unbind();
			}
			blurShader.unbind();

			// 4. Bloom: c) Add blurred bright colors to original image
			fboBloom.bind();
			window->clearColorBufferBit();

			bloomMixShader.bind();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, fboDefault.getTextureID(0));
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, fboBlur[1].getTextureID(0));
			bloomMixShader.setUniformValue("screenTexture", 0);
			bloomMixShader.setUniformValue("brightColors", 1);
			bloomMixShader.setUniformValue("bloom", bloomSettings.intensity);
			Vaos::getInstance().quad.bind();
			Vaos::getInstance().quad.draw(GL_TRIANGLES);
			Vaos::getInstance().quad.unbind();
			bloomMixShader.unbind();

			fboBloom.unbind();
		}

		// 5. Gamma correction and tone mapping.
		fboHDR.bind();
		window->clearColorBufferBit();

		hdrShader.bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bloomSettings.on ? fboBloom.getTextureID(0) : fboDefault.getTextureID(0));
		hdrShader.setUniformValue("screenTexture", 0);
		Vaos::getInstance().quad.bind();
		Vaos::getInstance().quad.draw(GL_TRIANGLES);
		Vaos::getInstance().quad.unbind();
		hdrShader.unbind();

		fboHDR.unbind();

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
		setUpFbos();
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
			real3 bulletVel = -real(30.0 + Random::randFloat() * 2.0 - 1.0) * camera.getCameraZ();
			fquat bulletRot = glm::toQuat(real3x3{ camera.getCameraX(), camera.getCameraY(), camera.getCameraZ() });
			createBox<BulletTag>(bulletPos, bulletRot, v, stdGravity, 10.0, bulletVel, { c.r, c.g, c.b, 1.0 }, 1.0, "");
		}

		return false;
	}
#endif


};
