#pragma once

#include <R3D/Ecs/System.h>
#include <R3D/Ecs/TransformComponent.h>

#include <R3D/Graphics/Core/VertexArray.h>
#include <R3D/Graphics/Core/InstancesRenderer.h>
#include <R3D/Graphics/Core/BasicMatrixOperations.h>
#include <R3D/Graphics/Core/Shader.h>
#include <R3D/Graphics/Components/BasicGraphicsComponents.h>

namespace r3d
{

	class PrimitivesRenderer : public System
	{
		int circleResolution{ 10 };
		int sphereResolution{ 15 };
		int sphereMeridians{ 10 };
		int sphereParallels{ 10 };

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
		PrimitivesRenderer();

		void buildPrimitive(size_t shapeId);
		void prepareData(ArchetypeManager& am);

		virtual void update(ArchetypeManager& am, double t, double dt) override;

		inline void setShader(Shader* shader) { m_shader = shader; }
		void setCircleResolution(int circleResolution);
		void setSphereResolution(int sphereResolution);
		void setSphereMeridians(int sphereMeridians);
		void setSphereParallels(int sphereParallels);
		inline int getCircleResolution() const { return circleResolution; }
		inline int getSphereResolution() const { return sphereResolution; }
		inline int getSphereMeridians() const { return sphereMeridians; }
		inline int getSphereParallels() const { return sphereParallels; }

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


