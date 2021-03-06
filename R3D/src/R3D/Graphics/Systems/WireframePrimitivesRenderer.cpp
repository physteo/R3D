#include <R3Dpch.h>

#include "WireframePrimitivesRenderer.h"
#include <R3D/Graphics/Components/BasicGraphicsComponents.h>
#include <R3D/Ecs/TransformComponent.h>
#include "../Core/VertexArray.h"
//#include <R3D/Physics/Core/Colliders.h>

namespace r3d
{

	WireframePrimitivesRenderer::WireframePrimitivesRenderer()
	{
		for (auto shapeId = m_shapesIds.begin(); shapeId != m_shapesIds.end(); ++shapeId)
		{
			m_shapesData.emplace_back();
			buildPrimitive(*shapeId);
		}
	}

	void WireframePrimitivesRenderer::buildPrimitive(size_t shapeId)
	{
		ShapeData& shapeData = m_shapesData[shapeId];

		shapeData.databuffer.generate();

		std::vector<float> positions;
		std::vector<unsigned int> indices;

		if (shapeId == CIRCLE) // build circle
		{
			buildCircle(positions, indices);
		}
		else if (shapeId == QUADRILATERAL) // build square
		{
			buildSquare(positions, indices);
		}
		else if (shapeId == SEGMENT) // build segment
		{
			buildSegment(positions, indices);
		}
		else if (shapeId == PLANE) // build segment
		{
			buildPlane(positions, indices);
		}
		else if (shapeId == SPHERE) // build segment
		{
			buildSphere(positions, indices);
		}
		else if (shapeId == BOX) // build segment
		{
			buildBox(positions, indices);
		}

		shapeData.vao = std::move(VertexArray{ { positions }, {3}, indices });
		shapeData.databuffer.bind();
		shapeData.vao.bind();

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(1 * sizeof(float4)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(2 * sizeof(float4)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(3 * sizeof(float4)));

		// color
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(4 * sizeof(float4)));

		glVertexAttribDivisor(1, 1);
		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		shapeData.vao.unbind();
		shapeData.databuffer.unbind();
		//shapeData.numInstances = 0;
	}

	void WireframePrimitivesRenderer::setCircleResolution(int circleResolution)
	{
		if (this->circleResolution != circleResolution)
		{
			this->circleResolution = circleResolution;
			buildPrimitive(CIRCLE);
		}
	}

	void WireframePrimitivesRenderer::setSphereResolution(int sphereResolution)
	{ 
		if (this->sphereResolution != sphereResolution)
		{
			this->sphereResolution = sphereResolution;
			buildPrimitive(SPHERE);
		}
	}

	void WireframePrimitivesRenderer::setSphereMeridians(int sphereMeridians)
	{ 
		if (this->sphereMeridians != sphereMeridians)
		{
			this->sphereMeridians = sphereMeridians;
			buildPrimitive(SPHERE);
		}
	}

	void WireframePrimitivesRenderer::setSphereParallels(int sphereParallels)
	{ 
		if (this->sphereParallels != sphereParallels)
		{
			this->sphereParallels = sphereParallels;
			buildPrimitive(SPHERE);
		}
	}

	void WireframePrimitivesRenderer::getInstancesData(ArchetypeManager& am, const ComponentList& componentList, std::vector<InstanceData>& outInstancesData)
	{
		auto archetypes = am.matchAtLeast(componentList, {});
		for (auto archIt = archetypes.begin(); archIt != archetypes.end(); ++archIt)
		{
			auto transform = get<Transform>(am, *archIt);
			auto color = get<Color>(am, *archIt);
			size_t numElements = getSize<Color>(am, *archIt);
			for (size_t j = 0; j < numElements; j++)
			{
				outInstancesData.emplace_back(compute_model_matrix(transform[j].position, transform[j].orientation, real(2.0) * transform[j].scale), color[j].vec);
			}
		}
	}

	void WireframePrimitivesRenderer::getInstancesDataPlane(ArchetypeManager& am, std::vector<InstanceData>& outInstancesData)
	{
		ComponentList componentList = ComponentList::buildList<Transform, PlanePrimitive, Color>();
		auto archetypes = am.matchAtLeast(componentList, {});

		float3 scale{ 10.0f, 10.0f, 0.0f };

		for (auto archIt = archetypes.begin(); archIt != archetypes.end(); ++archIt)
		{
			Transform* transform = get<Transform>(am, *archIt);
			PlanePrimitive* plane = get<PlanePrimitive>(am, *archIt);
			Color* color = get<Color>(am, *archIt);
			std::vector<Entity>* entities = getEntities(am, *archIt);

			if (!transform || !plane || !color || !entities)
			{
				R3D_CORE_WARN("Could not fetch data for archetype id = {0}. Components:", *archIt);
				return;
			}

			size_t numElements = getSize<PlanePrimitive>(am, *archIt);

			for (size_t j = 0; j < numElements; j++)
			{
				const float3& n = glm::normalize(float3{ transform[j].orientation.x, transform[j].orientation.y, transform[j].orientation.z });
				const float& off = transform[j].position.x;

				float3 tangent[2];
				compute_basis(n, tangent[0], tangent[1]);
				fquat q = glm::quat_cast(glm::transpose(real3x3{ n, tangent[0], tangent[1] }));

				if (am.has<Transform>((*entities)[j])) scale = am.get<Transform>((*entities)[j])->scale;

				outInstancesData.push_back(InstanceData{});
				outInstancesData.back().modelMatrix = std::move(compute_model_matrix((float3)(off * n), q, 2.0f * scale));
				outInstancesData.back().color = color[j].vec;
			}
		}
	}

	void WireframePrimitivesRenderer::prepareData(ArchetypeManager& am)
	{
		ComponentList requiredComponents;

		requiredComponents = ComponentList::buildList<Transform, Color, CirclePrimitive>();
		m_instancesData.push_back({});
		getInstancesData(am, requiredComponents, m_instancesData.back());

		requiredComponents = ComponentList::buildList<Transform, Color, SquarePrimitive>();
		m_instancesData.push_back({});
		getInstancesData(am, requiredComponents, m_instancesData.back());

		requiredComponents = ComponentList::buildList<Transform, Color, SegmentPrimitive>();
		m_instancesData.push_back({});
		getInstancesData(am, requiredComponents, m_instancesData.back());

		m_instancesData.push_back({});
		getInstancesDataPlane(am, m_instancesData.back());

		requiredComponents = ComponentList::buildList<Transform, Color, SpherePrimitive>();
		m_instancesData.push_back({});
		getInstancesData(am, requiredComponents, m_instancesData.back());

		requiredComponents = ComponentList::buildList<Transform, Color, BoxPrimitive>();
		m_instancesData.push_back({});
		getInstancesData(am, requiredComponents, m_instancesData.back());

	}

	void WireframePrimitivesRenderer::update(ArchetypeManager& am, double t, double dt)
	{
		prepareData(am);

		//bool depthTesting;
		//glGetBooleanv(GL_DEPTH_TEST, (GLboolean*)&depthTesting);
		//glDisable(GL_DEPTH_TEST);

		m_shader->bind();

		static unsigned long long cumulative = 0;
		static unsigned long long trials = 0;
		size_t totInstances = 0;
		auto start = std::chrono::high_resolution_clock::now();
		for (size_t group = 0; group < m_instancesData.size(); ++group)
		{
			totInstances += m_instancesData[group].size();
			InstancesRenderer::render<InstanceData>(m_instancesData[group], m_shapesData[group], GL_LINE_LOOP);
		}
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = end - start;
		cumulative += duration / std::chrono::microseconds(1);
		trials++;
		if (trials % 60 == 0)
		{
			R3D_CORE_INFO("instances({0}), duration instanced rend: {1}ns", totInstances, cumulative / double(trials) );

			cumulative = 0;
			trials = 0;
		}

		m_shader->unbind();
		m_instancesData.clear();

		//if (true == depthTesting)
		//{
		//	glEnable(GL_DEPTH_TEST);
		//}
	}

	void WireframePrimitivesRenderer::buildCircle(std::vector<float>& positions, std::vector<unsigned int>& indices)
	{
		positions.clear();
		indices.clear();
		for (size_t i = 0; i <= circleResolution; i++)
		{
			float theta = static_cast<float>(i) * (2 * glm::pi<float>()) / (float)circleResolution;
			positions.push_back(0.5f * cosf(theta)); // x1
			positions.push_back(0.5f * sinf(theta)); // z1
			positions.push_back(0.0f); // y1
			indices.push_back(static_cast<unsigned int>(i));
		}
		indices.push_back(static_cast<unsigned int>(indices.size()));
		positions.push_back(0.0f);
		positions.push_back(0.0f);
		positions.push_back(0.0f);
	}

	void WireframePrimitivesRenderer::buildSquare(std::vector<float>& positions, std::vector<unsigned int>& indices)
	{
		positions.clear();
		indices.clear();

		positions.push_back(-0.5f);
		positions.push_back(-0.5f);
		positions.push_back(+0.0f);

		positions.push_back(+0.5f);
		positions.push_back(-0.5f);
		positions.push_back(+0.0f);

		positions.push_back(+0.5f);
		positions.push_back(+0.5f);
		positions.push_back(+0.0f);

		positions.push_back(-0.5f);
		positions.push_back(+0.5f);
		positions.push_back(+0.0f);

		indices = { 0, 1, 2, 0, 2, 3 };
	}

	void WireframePrimitivesRenderer::buildSegment(std::vector<float>& positions, std::vector<unsigned int>& indices)
	{
		positions.clear();
		indices.clear();

		positions.push_back(-0.5f);
		positions.push_back(0.0f);
		positions.push_back(0.0f);

		positions.push_back(+0.5f);
		positions.push_back(0.0f);
		positions.push_back(0.0f);

		indices = { 0, 1 };
	}

	void WireframePrimitivesRenderer::buildPlane(std::vector<float>& positions, std::vector<unsigned int>& indices)
	{
		const int resolution = 20;

		positions.clear();
		indices.clear();

		unsigned int cnt = 0;
		float step = 1.0f / float(resolution);
		for (size_t i = 0; i < resolution; i += 2)
		{
			positions.push_back(-0.5f);
			positions.push_back(-0.5f + i * step);
			positions.push_back(0.0f);
			indices.push_back(cnt++);

			positions.push_back(+0.5f);
			positions.push_back(-0.5f + i * step);
			positions.push_back(0.0f);
			indices.push_back(cnt++);

			positions.push_back(+0.5f);
			positions.push_back(-0.5f + (i + 1) * step);
			positions.push_back(0.0f);
			indices.push_back(cnt++);

			positions.push_back(-0.5f);
			positions.push_back(-0.5f + (i + 1) * step);
			positions.push_back(0.0f);
			indices.push_back(cnt++);
		}

		size_t checkpoint = cnt;

		for (size_t i = 0; i < resolution; i += 2)
		{
			positions.push_back(-0.5f + i * step);
			positions.push_back(+0.5f);
			positions.push_back(0.0f);
			indices.push_back(cnt++);

			positions.push_back(-0.5f + i * step);
			positions.push_back(-0.5f);
			positions.push_back(0.0f);
			indices.push_back(cnt++);

			positions.push_back(-0.5f + (i + 1) * step);
			positions.push_back(-0.5f);
			positions.push_back(0.0f);
			indices.push_back(cnt++);

			positions.push_back(-0.5f + (i + 1) * step);
			positions.push_back(+0.5f);
			positions.push_back(0.0f);
			indices.push_back(cnt++);
		}

		positions.push_back(+0.5f);
		positions.push_back(+0.5f);
		positions.push_back(0.0f);
		indices.push_back(cnt);

		indices.push_back(static_cast<unsigned int>(checkpoint));
		indices.push_back(cnt);
		indices.push_back(1);
		indices.push_back(0);

	}

	void WireframePrimitivesRenderer::buildSphere(std::vector<float>& positions, std::vector<unsigned int>& indices)
	{
		positions.clear();
		indices.clear();

		unsigned int cnt = 0;
		for (size_t i = 0; i < sphereMeridians; ++i)
		{
			float phi = glm::radians(180.0f * i / float(sphereMeridians));
			float cos_phi = glm::cos(phi);
			float sin_phi = glm::sin(phi);

			for (int j = 0; j <= sphereResolution; j++)
			{
				float theta = glm::radians(360.0f * j / float(sphereResolution) + 90);
				float cos_theta = glm::cos(theta);
				float sin_theta = glm::sin(theta);
				positions.push_back(0.5f * cos_theta * cos_phi); // x1
				positions.push_back(0.5f * cos_theta * sin_phi); // z1
				positions.push_back(0.5f * sin_theta); // y1
				indices.push_back(cnt++);
			}
		}
		
		for (size_t i = 0; i < sphereParallels; ++i)
		{
			float phi = glm::radians(180.0f * i / float(sphereParallels) - 90.0f);
			float cos_phi = glm::cos(phi);
			float sin_phi = glm::sin(phi);

			for (int j = 0; j <= sphereResolution; j++)
			{
				float theta = glm::radians(360.0f * j / float(sphereResolution) + 90);
				float cos_theta = glm::cos(theta);
				float sin_theta = glm::sin(theta);
				positions.push_back(0.5f * cos_theta * cos_phi); // x1
				positions.push_back(0.5f * sin_theta * cos_phi); // z1
				positions.push_back(0.5f * sin_phi); // y1
				indices.push_back(cnt++);
			}
		}
	}

	void WireframePrimitivesRenderer::buildBox(std::vector<float>& positions, std::vector<unsigned int>& indices)
	{
		positions.clear();
		indices.clear();
		size_t cnt = 0;

		// z > 0
		positions.push_back(-0.5f);
		positions.push_back(-0.5f);
		positions.push_back(+0.5f);

		positions.push_back(+0.5f);
		positions.push_back(-0.5f);
		positions.push_back(+0.5f);

		positions.push_back(+0.5f);
		positions.push_back(+0.5f);
		positions.push_back(+0.5f);

		positions.push_back(-0.5f);
		positions.push_back(+0.5f);
		positions.push_back(+0.5f);

		// z < 0
		positions.push_back(-0.5f);
		positions.push_back(-0.5f);
		positions.push_back(-0.5f);

		positions.push_back(+0.5f);
		positions.push_back(-0.5f);
		positions.push_back(-0.5f);

		positions.push_back(+0.5f);
		positions.push_back(+0.5f);
		positions.push_back(-0.5f);

		positions.push_back(-0.5f);
		positions.push_back(+0.5f);
		positions.push_back(-0.5f);

		indices = { 0,1,5,6,1,2,0,3,4,6,2,3,7,4,5,0,3,6,7,4 };

	}

}
