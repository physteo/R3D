#pragma once
#include <R3D/Ecs/System.h>
#include "../Core/InstancesRenderer.h"

namespace r3d
{

	class Shader;

	class WireframePrimitivesRenderer : public System
	{
		size_t circleResolution{ 10 };
		size_t sphereResolution{ 15 };
		size_t sphereMeridians{ 10 };
		size_t sphereParallels{ 10 };

		enum Shape
		{
			CIRCLE = 0,
			QUADRILATERAL,
			SEGMENT,
			PLANE,
			SPHERE,
			BOX,
			NUM_SHAPES
		};
		std::array<size_t, NUM_SHAPES> m_shapesIds = { CIRCLE, QUADRILATERAL, SEGMENT, PLANE, SPHERE, BOX };

		struct InstanceData
		{
			InstanceData(const float4x4& modelMatrix = float4x4{ 1.0f }, const float4& color = float4{ 1.0f })
				: modelMatrix(modelMatrix), color(color) {}
			float4x4 modelMatrix;
			float4 color;
		};

	public:
		WireframePrimitivesRenderer();

		void buildPrimitive(size_t shapeId);
		void prepareData(ArchetypeManager& am);

		virtual void update(ArchetypeManager& am, double t, double dt) override;

		inline void setShader(Shader* shader) { m_shader = shader; }
		void setCircleResolution(int circleResolution);
		void setSphereResolution(int sphereResolution);
		void setSphereMeridians(int sphereMeridians);
		void setSphereParallels(int sphereParallels);
		inline size_t getCircleResolution() const { return circleResolution; }
		inline size_t getSphereResolution() const { return sphereResolution; }
		inline size_t getSphereMeridians() const { return sphereMeridians; }
		inline size_t getSphereParallels() const { return sphereParallels; }

	private:
		void buildCircle(std::vector<float>& positions, std::vector<unsigned int>& indices);
		void buildSquare(std::vector<float>& positions, std::vector<unsigned int>& indices);
		void buildSegment(std::vector<float>& positions, std::vector<unsigned int>& indices);
		void buildPlane(std::vector<float>& positions, std::vector<unsigned int>& indices);
		void buildSphere(std::vector<float>& positions, std::vector<unsigned int>& indices);
		void buildBox(std::vector<float>& positions, std::vector<unsigned int>& indices);

		void getInstancesData(ArchetypeManager& am, const ComponentList& componentList, std::vector<InstanceData>& outInstancesData);
		void getInstancesDataPlane(ArchetypeManager& am, std::vector<InstanceData>& outInstancesData);

	private:
		Shader* m_shader;
		std::vector< std::vector<InstanceData> > m_instancesData;
		std::vector<ShapeData> m_shapesData;
	};

}


