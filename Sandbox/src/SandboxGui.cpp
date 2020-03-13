#include "WorldLayer.h"
#include <R3D/Graphics/Components/BasicGraphicsComponents.h>

#include <filesystem>
#include <fstream>

namespace r3d
{

	struct ShaderEditor
	{
		ShaderProgramSource source;
		std::string filename;

		ShaderEditor() : source{}, filename{ "" } {}

		void saveToFile()
		{
			std::ofstream outFile;
			outFile.open(filename, std::ios::trunc);
			outFile << "#shader vertex\n";
			outFile << source.VertexSource;
			outFile << "#shader fragment\n";
			outFile << source.FragmentSource;
			if (source.GeometrySource != "")
			{
				outFile << "#shader geometry\n";
				outFile << source.GeometrySource;
			}
			if (source.ComputeShader != "")
			{
				outFile << "#shader compute\n";
				outFile << source.ComputeShader;
			}
			outFile.close();
		}
	};

	void create_dockspace(float fontscale);

	void WorldLayer::onImGuiUpdate(r3d::Window* window)
	{
		using namespace r3d;
		auto am = getArchetypeManager();
	
		create_dockspace(window->getFontscale());

		// main menu bar
		{
			std::string menuAction = "";
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("Help"))
				{
					if (ImGui::MenuItem("About..."))
					{
						menuAction = "about";
					}
					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();
			}

			if (menuAction == "about")
			{
				ImGui::OpenPopup("About");
			}

			ImVec2 windowSize = ImGui::GetWindowSize();
			ImGui::SetNextWindowSize(ImVec2(300, 200));
			if (ImGui::BeginPopupModal("About"))
			{
				ImGui::Spacing();
				ImGui::TextColored(ImVec4(1.0, 0.5, 0.0, 1.0), "****************************************");
				ImGui::TextColored(ImVec4(1.0, 0.5, 0.0, 1.0), "*              R3D Engine              *");
				ImGui::TextColored(ImVec4(1.0, 0.5, 0.0, 1.0), "****************************************");
				ImGui::Spacing();
				ImGui::TextColored(ImVec4(1.0, 0.5, 0.0, 1.0), "            by Matteo Campo");
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Text("GitHub:  ");
				ImGui::SameLine();
				ImGui::Text("https://github.com/physteo/R3D");
				ImGui::Text("Website: ");
				ImGui::SameLine();
				ImGui::Text("https://matteocampo.com");
				ImGui::Text("          ");
				ImGui::Spacing();
				if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}
		}

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

#ifndef STACKING
		ImGui::Begin("Create Primitives!", window->getFontscale());

		auto& vec0 = am->get<Transform>(newEntity)->position;
		ImGui::Text("Pos box0 : (%.8f, %.8f, %.8f)", vec0.x, vec0.y, vec0.z);

		ImGui::SliderFloat("pos   x", (float*)&newPosition.x, -10.0f, 10.0f);
		ImGui::SliderFloat("pos   y", (float*)&newPosition.y, -10.0f, 10.0f);
		ImGui::SliderFloat("pos   z", (float*)&newPosition.z, -10.0f, 10.0f);

		ImGui::SliderAngle("angle", (float*)&newAngle, 0, 360);
		ImGui::SliderFloat("axis x", (float*)&newAngleAxis.x, -1, 1);
		ImGui::SliderFloat("axis y", (float*)&newAngleAxis.y, -1, 1);
		ImGui::SliderFloat("axis z", (float*)&newAngleAxis.z, -1, 1);
		ImGui::SliderFloat("size  x", (float*)&newSize.x, 0.1, 3.0);
		ImGui::SliderFloat("size  y", (float*)&newSize.y, 0.1, 3.0);
		ImGui::SliderFloat("size  z", (float*)&newSize.z, 0.1, 3.0);
		ImGui::ColorEdit3("color", (float*)&newColor);

		if (!physicsOn)
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

		ImGui::End();
#endif
		// Camera
		{
			ImGui::Begin("Camera", window->getFontscale());
			ImGui::SliderFloat("sens", &mouseStatus.sensibility, 0.001f, 0.05f);
			ImGui::SliderFloat("px", &deltaPosition.x, -10.0f, 10.0f);
			ImGui::SliderFloat("py", &deltaPosition.y, -10.0f, 10.0f);
			ImGui::SliderFloat("pz", &deltaPosition.z, -10.0f, 10.0f);

			if (projection == Projection::ORTHO)
			{
				deltaPosition = { 0.0 };
				camera.getEye() = float3{ 0.0, 0.0, -5.0f } +deltaPosition;
				camera.getCenter() = deltaPosition;

			}

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

				ImGui::SliderAngle("fovy", &fovy, 0.0, 180);
				//ImGui::SliderFloat("aspect", &aspect, 0.1f, 2.0f);
				aspect = float(window->getWidth()) / float(window->getHeight());
				projectionMatrix = glm::perspective(fovy, aspect, nearPlane, farPlane);
			}
			real3& position = am->get<Transform>(gun)->position;
			position = camera.getEye() - (*am->get<float>(gun)) * camera.getCameraZ();
			position += 1.0f * (camera.getCameraX() - camera.getCameraY());
			am->get<Transform>(gun)->orientation = glm::toQuat(real3x3{ camera.getCameraX(), camera.getCameraY(), camera.getCameraZ() });
			ImGui::End();
		}

		// Canvas
		{
			ImGui::Begin("Canvas", window->getFontscale());
			ImGui::Text("Physics", window->getFontscale());

			if (physicsOn ? ImGui::Button("On") : ImGui::Button("Off"))
			{
				if (physicsOn)
				{
					physicsOn = false;

#ifndef STACKING
					auto* rb = am->get<RigidBody>(newEntity);
					rb->angVelocity = real3{ 0.0 };
					rb->velocity = real3{ 0.0 };
#endif 
				}
				else
				{
					physicsOn = true;
				}
			}

			ImGui::Text("Graphics");
			if (ImGui::Button("Wireframes"))
			{
				wireframesOn = !wireframesOn;
			}
			ImGui::SameLine();
			if (ImGui::Button("Solid"))
			{
				solidsOn = !solidsOn;
			}

#ifdef STACKING
			//if (physicsOn && frameCounter % 50 == 0)//ImGui::Button("Shoot"))
			//{
			//	//float3 v{0.2*Random::randFloat(), 1.0 + 0.2*Random::randFloat(), 0.2*Random::randFloat()};
			//	float3 v{ 0.2, 0.2, 0.2 };
			//	//float3 c{Random::randFloat(), Random::randFloat(), Random::randFloat()};
			//	float3 c = Palette::getInstance()->green;
			//
			//	float angle = 0;
			//	real3 bulletPos = am->get<Transform>(gun)->position;
			//	real3 bulletVel = -real(25.0 + Random::randFloat() * 2.0 - 1.0) * camera.getCameraZ();
			//	fquat bulletRot = glm::toQuat(real3x3{ camera.getCameraX(), camera.getCameraY(), camera.getCameraZ() });
			//
			//	createBox(bulletPos, bulletRot, v, -0.6, 1.0, bulletVel, { c.r, c.g, c.b, 1.0 }, 2.5);
			//}
#endif 

			ImGui::Text("SunLight");
			real3 sunPosition = solidRenderer.getSunPosition();
			ImGui::SliderFloat("sun x", &sunPosition.x, -10.0f, 10.0f);
			ImGui::SliderFloat("sun y", &sunPosition.y, -10.0f, 10.0f);
			ImGui::SliderFloat("sun z", &sunPosition.z, -10.0f, 10.0f);
			solidRenderer.setSunPosition(sunPosition);

			ImGui::Text("Ground");
			float plane_x = am->get<Transform>(gridEnt)->orientation.x;
			float plane_y = am->get<Transform>(gridEnt)->orientation.y;
			float plane_z = am->get<Transform>(gridEnt)->orientation.z;
			float plane_o = am->get<Transform>(gridEnt)->position.x;
			ImGui::SliderFloat("plane nx", &plane_x, -1, 1);
			ImGui::SliderFloat("plane ny", &plane_y, -1, 1);
			ImGui::SliderFloat("plane nz", &plane_z, -1, 1);
			ImGui::SliderFloat("plane off", &plane_o, -5.0, 5.0);
			auto planeNormal = glm::normalize(float3{ plane_x, plane_y, plane_z });
			am->get<Transform>(gridEnt)->orientation = rquat{ 1.0, planeNormal.x, planeNormal.y, planeNormal.z };
			am->get<Transform>(gridEnt)->position.x = plane_o;
			am->get<ColliderPlane>(gridEnt)->offset = plane_o;
			am->get<ColliderPlane>(gridEnt)->normal = planeNormal;

			ImGui::Text("Sphere Settings");
			int circleResolution = primitivesRenderer.getCircleResolution();
			int sphereResolution = primitivesRenderer.getSphereResolution();
			int sphereMeridians = primitivesRenderer.getSphereMeridians();
			int sphereParallels = primitivesRenderer.getSphereParallels();
			ImGui::SliderInt("circ. res", &circleResolution, 1, 50);
			ImGui::SliderInt("sph. res", &sphereResolution, 1, 50);
			ImGui::SliderInt("sph. mer", &sphereMeridians, 1, 50);
			ImGui::SliderInt("sph. par", &sphereParallels, 1, 50);
			primitivesRenderer.setCircleResolution(circleResolution);
			primitivesRenderer.setSphereResolution(sphereResolution);
			primitivesRenderer.setSphereMeridians(sphereMeridians);
			primitivesRenderer.setSphereParallels(sphereParallels);
			ImGui::End();
		}

		// Misc
		{
			ImGui::Begin("Misc", window->getFontscale());

			ImGui::Checkbox("Demo Window", &show_demo_window);
			
			float clear_color[3] = { window->getColorR(), window->getColorG(), window->getColorB() };
			ImGui::ColorEdit3("Bckg color", (float*)&clear_color);
			window->setColor(clear_color[0], clear_color[1], clear_color[2]);
			
			ImGui::Text("%.1f FPS (%.1f ms/frame)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
			double avg = boxBoxContactDetector.measuredTime / double(boxBoxContactDetector.trials);
			
			ImGui::Text("%.3f BoxBox", avg);
			
			ImGui::End();
		}

		// View
		{
			ImGui::Begin("View", window->getFontscale());

			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImVec2 windowSize = ImGui::GetWindowSize();
			ImGui::GetWindowDrawList()->AddImage(
				(void*)fbo.getTextureID(0),
				pos, ImVec2(pos.x + windowSize.x, pos.y + windowSize.y), 
				ImVec2(0, 1), 
				ImVec2(1, 0)
			);

			fboSize = { windowSize.x, windowSize.y };
			viewWindowCenter = { pos.x + windowSize.x / 2.0 , pos.y + windowSize.y / 2.0 };
			
			float currentTime = window->getCurrentTime();
			if (fboSize != fboSizePrev && currentTime - lastFboResize > 0.1)
			{
				onEvent(ViewWindowResizeEvent{ fboSize });
				fboSizePrev = fboSize;
				lastFboResize = currentTime;
			}

			ImGui::End();
		}

		// Shader editor
		{
			ImGui::Begin("Shader Editor", window->getFontscale(), 0, ImGuiWindowFlags_MenuBar);

			static ShaderEditor editor;
			static Shader* toEdit = nullptr;
			static std::string assignedTo = "Nothing";

			std::string menuAction = "";
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New"))
					{
						menuAction = "new"; 
					}

					if (ImGui::BeginMenu("Open"))
					{
						menuAction = "open";
						std::string path = "C:/dev/R3D/R3D/res/shaders/";
						for (const auto& entry : std::filesystem::directory_iterator(path))
						{
							auto filename = entry.path().generic_string().c_str();
							if (ImGui::MenuItem(filename))
							{
								editor.filename = filename;
								editor.source = ParseShader(filename);
							}
						}
						ImGui::EndMenu();
					}

					if (ImGui::MenuItem("Save", "Ctrl+S"))
					{
						menuAction = "save";
						if (editor.filename != "")
						{
							editor.saveToFile();
						}
					}

					if (ImGui::MenuItem("Save As.."))
					{
						menuAction = "saveAs";
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Assign to.."))
				{
					// TODO: loop over database of shaders
					if (ImGui::MenuItem("Solids"))
					{
						toEdit = &solidShader;
						assignedTo = "Solids";
					}
					if (ImGui::MenuItem("Wireframes"))
					{
						toEdit = &linesShader;
						assignedTo = "Wireframes";
					}
					if (ImGui::MenuItem("PostProcessing"))
					{
						toEdit = &fboQuad.shader;
						assignedTo = "PostProcessing";
					}
					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			// "New" popup window
			if (menuAction == "new")
			{
				ImGui::OpenPopup("New");
			}
			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImVec2 windowSize = ImGui::GetWindowSize();
			ImGui::SetNextWindowPos(pos);
			ImGui::SetNextWindowSize(ImVec2(windowSize.x / 1.6, windowSize.y / 6.5));
			if (ImGui::BeginPopupModal("New"))
			{
				editor.filename = "C:/dev/R3D/R3D/res/shaders/new.shader";
				ImGui::InputText("", &editor.filename);
				if (ImGui::Button("Create"))
				{
					editor.source = ShaderProgramSource{};
					editor.saveToFile();
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel"))
				{
					editor.filename = "";
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			// "Save As" popup window
			if (menuAction == "saveAs")
			{
				ImGui::OpenPopup("Save Shader As..");
			}
			ImGui::SetNextWindowPos(pos);
			ImGui::SetNextWindowSize(ImVec2(windowSize.x / 1.6, windowSize.y / 6.5));
			if (ImGui::BeginPopupModal("Save Shader As.."))
			{
				ImGui::InputText("", &editor.filename);
				if (ImGui::Button("Save"))
				{
					editor.saveToFile();
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}

			// edit shader
			if (editor.filename != "")
			{
				ImGui::Text(editor.filename.c_str());
				ImGui::Text("Assigned to: ");
				ImVec4 textColor = assignedTo == "Nothing" ? ImVec4(0.5, 0.5, 0.5, 1.0) : ImVec4(1.0, 0.5, 0.0, 1.0);
				ImGui::SameLine();
				ImGui::TextColored(textColor, "%s", assignedTo);

				editProgram(&editor);

				static std::string compilationResultMsg = "";
				static ImVec4 compilationResultCol(1.0, 1.0, 1.0, 1.0);
				if (ImGui::Button("Compile"))
				{
					unsigned int id = generateProgram(editor.source);
					if (id != 0)
					{
						if (toEdit != nullptr)
						{
							*toEdit = std::move(Shader{ id, editor.source, editor.filename });
						}
					}
					else
					{
						compilationResultMsg = "Failed to compile!";
						compilationResultCol = ImVec4(1.0, 0.0, 0.0, 1.0);
					}
				}
				ImGui::SameLine();
				ImGui::TextColored(compilationResultCol, compilationResultMsg.c_str());
			}

			ImGui::End();
		}
	}

	void edit_multiline(const std::string& name, std::string& shaderText)
	{
		if (ImGui::TreeNode(name.c_str()))
		{
			int linesOffset = 3;
			int numNewLines = linesOffset;
			for (auto c : shaderText)
			{
				numNewLines += (c == '\n');
			}
			static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
			ImGui::InputTextMultiline("##source", &shaderText, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * numNewLines), flags);
			ImGui::TreePop();
		}
	}

	void WorldLayer::editProgram(ShaderEditor* editor)
	{
		edit_multiline("Vertex shader", editor->source.VertexSource);
		edit_multiline("Fragment shader", editor->source.FragmentSource);
		edit_multiline("Geometry shader", editor->source.GeometrySource);
		edit_multiline("Compute shader", editor->source.ComputeShader);
	}

	// copy-pasted from ImGui demo
	void create_dockspace(float fontscale)
	{
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background 
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", fontscale, 0, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		ImGui::End();
	}

}