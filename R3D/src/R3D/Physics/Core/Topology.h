#pragma once

#include <R3D/Core/MathUtils.h>

namespace r3d
{
	struct VertexData;
	struct EdgeData;
	struct Facet;

	struct HalfEdge
	{
		HalfEdge* next;
		HalfEdge* twin;

		VertexData* origin;
		Facet* left;
	};

	struct VertexData
	{
		real3 position;
		HalfEdge* out;
	};

	struct Facet
	{
		HalfEdge* halfEdge;

		real3 computeNormal() const;
		real3 computeCenter() const;
		void getFaceVertices(real3* vertices) const;
	};

	std::vector<real3> get_positions_of_face(const Facet& facet);

	struct Topology
	{
		std::vector<VertexData> vertices;
		std::vector<HalfEdge> halfEdges;
		std::vector<Facet> faces;

		//TODO: add iterators
	};

	Topology create_box_topology();
	//static const Topology s_boxTopology;
	void test_box_topology();
	
}