#include <R3Dpch.h>
#include "Topology.h"

namespace r3d 
{
	real3 Facet::computeNormal() const
	{
		real3 edge1 = (halfEdge->next->origin->position - halfEdge->origin->position);
		real3 edge2 = (halfEdge->next->next->origin->position - halfEdge->next->origin->position);
		return glm::normalize(glm::cross(edge1, edge2)); 
	}

	real3 Facet::computeCenter() const
	{
		real3 result{ 0.0f };
		int cnt = 0;
		if (halfEdge != 0)
		{
			HalfEdge* begin = halfEdge;
			HalfEdge* half = begin;
			do
			{
				result += half->origin->position;
				++cnt;
				half = half->next;
			} while (half != begin);
		}

		return result / real(cnt);
	}

	void Facet::getFaceVertices(real3* vertices) const
	{
		int found = 0; 
		if (this->halfEdge != 0)
		{
			HalfEdge* begin = halfEdge;
			HalfEdge* half = begin;
			do
			{
				real3& po = half->origin->position;
				vertices[found] = po;
				++found;
				half = half->next;
			} while (half != begin);
		}
	}

	static real3 boxVertex[8];

	int get_box_vertexID(real3& v)
	{
		for (int i = 0; i < 8; ++i)
		{
			if (glm::length(boxVertex[i] - v) < R3D_EPSILON)
			{
				return i;
			}
		}
		assert(false);
	}

	std::vector<real3> get_positions_of_face(const Facet& facet)
	{
		std::vector<real3> edges;

		if (facet.halfEdge != 0)
		{
			HalfEdge* begin = facet.halfEdge;
			HalfEdge* half = begin;
			do
			{
				real3& po = half->origin->position;
				real3& pe = half->next->origin->position;

				edges.push_back(po);
				half = half->next;
			} while (half != begin);
		}
		return edges;
	}

	void print_edges_of_face(const Facet& facet)
	{
		if (facet.halfEdge != 0)
		{
			HalfEdge* begin = facet.halfEdge;
			HalfEdge* half = begin;
			do
			{
				real3& po = half->origin->position;
				real3& pe = half->next->origin->position;

				std::cout << get_box_vertexID(po) << "," << get_box_vertexID(pe) << "\n";

				half = half->next;
			} while (half != begin);
		}
	}

	void print_edges_of_vertex(const VertexData& vertex)
	{
		if (vertex.out != nullptr)
		{
			HalfEdge* begin = vertex.out;
			HalfEdge* half = begin;
			do
			{
				// Do something.
				real3& po = half->origin->position;
				real3& pe = half->next->origin->position;

				std::cout << get_box_vertexID(po) << "," << get_box_vertexID(pe) << "\n";

				half = half->twin->next;
			} while (half != begin);
		}
	}

	void test_box_topology()
	{
		const Topology box = create_box_topology();

		for (const auto x : box.faces)
		{
			print_edges_of_face(x);
			printf("-----\n");
		}

		for (const auto x : box.vertices)
		{
			print_edges_of_vertex(x);
			printf("-----\n");
		}
	}
	
	Topology create_box_topology()
	{

		static real3 boxVertex[8] =
		{
			{ -1, -1, -1 },
			{ -1, -1, +1 },
			{ -1, +1, -1 },
			{ -1, +1, +1 },
			{ +1, -1, -1 },
			{ +1, -1, +1 },
			{ +1, +1, -1 },
			{ +1, +1, +1 }
		};

		Topology top;
		top.vertices.resize(8);
		top.halfEdges.resize(24);
		top.faces.resize(6);

		top.vertices[0].position = boxVertex[0];
		top.vertices[1].position = boxVertex[1];
		top.vertices[2].position = boxVertex[2];
		top.vertices[3].position = boxVertex[3];
		top.vertices[4].position = boxVertex[4];
		top.vertices[5].position = boxVertex[5];
		top.vertices[6].position = boxVertex[6];
		top.vertices[7].position = boxVertex[7];

		top.vertices[0].out = &top.halfEdges[0];
		top.vertices[1].out = &top.halfEdges[1];
		top.vertices[2].out = &top.halfEdges[5];
		top.vertices[3].out = &top.halfEdges[4];
		top.vertices[4].out = &top.halfEdges[8];
		top.vertices[5].out = &top.halfEdges[9];
		top.vertices[6].out = &top.halfEdges[13];
		top.vertices[7].out = &top.halfEdges[12];

		top.halfEdges[0].next = &top.halfEdges[2];
		top.halfEdges[1].next = &top.halfEdges[20];
		top.halfEdges[2].next = &top.halfEdges[4];
		top.halfEdges[3].next = &top.halfEdges[17];
		top.halfEdges[4].next = &top.halfEdges[6];
		top.halfEdges[5].next = &top.halfEdges[18];
		top.halfEdges[6].next = &top.halfEdges[0];
		top.halfEdges[7].next = &top.halfEdges[23];
		top.halfEdges[8].next = &top.halfEdges[16];
		top.halfEdges[9].next = &top.halfEdges[11];
		top.halfEdges[10].next = &top.halfEdges[21];
		top.halfEdges[11].next = &top.halfEdges[13];
		top.halfEdges[12].next = &top.halfEdges[22];
		top.halfEdges[13].next = &top.halfEdges[15];
		top.halfEdges[14].next = &top.halfEdges[19];
		top.halfEdges[15].next = &top.halfEdges[9];
		top.halfEdges[16].next = &top.halfEdges[1];
		top.halfEdges[17].next = &top.halfEdges[14];
		top.halfEdges[18].next = &top.halfEdges[12];
		top.halfEdges[19].next = &top.halfEdges[3];
		top.halfEdges[20].next = &top.halfEdges[8];
		top.halfEdges[21].next = &top.halfEdges[7];
		top.halfEdges[22].next = &top.halfEdges[5];
		top.halfEdges[23].next = &top.halfEdges[10];

		for (int i = 0; i < top.halfEdges.size(); i+=2)
		{
			top.halfEdges[i].twin = &top.halfEdges[(i+1) % top.halfEdges.size()];
			top.halfEdges[(i+1) % top.halfEdges.size()].twin = &top.halfEdges[i];
		}

		top.halfEdges[0].origin = &top.vertices[0];
		top.halfEdges[1].origin = &top.vertices[1];
		top.halfEdges[2].origin = &top.vertices[1];
		top.halfEdges[3].origin = &top.vertices[3];
		top.halfEdges[4].origin = &top.vertices[3];
		top.halfEdges[5].origin = &top.vertices[2];
		top.halfEdges[6].origin = &top.vertices[2];
		top.halfEdges[7].origin = &top.vertices[0];
		top.halfEdges[8].origin = &top.vertices[4];
		top.halfEdges[9].origin = &top.vertices[5];
		top.halfEdges[10].origin = &top.vertices[6];
		top.halfEdges[11].origin = &top.vertices[4];
		top.halfEdges[12].origin = &top.vertices[7];
		top.halfEdges[13].origin = &top.vertices[6];
		top.halfEdges[14].origin = &top.vertices[5];
		top.halfEdges[15].origin = &top.vertices[7];
		top.halfEdges[16].origin = &top.vertices[5];
		top.halfEdges[17].origin = &top.vertices[1];
		top.halfEdges[18].origin = &top.vertices[3];
		top.halfEdges[19].origin = &top.vertices[7];
		top.halfEdges[20].origin = &top.vertices[0];
		top.halfEdges[21].origin = &top.vertices[4];
		top.halfEdges[22].origin = &top.vertices[6];
		top.halfEdges[23].origin = &top.vertices[2];

		top.halfEdges[0].left = &top.faces[0];
		top.halfEdges[2].left = &top.faces[0];
		top.halfEdges[4].left = &top.faces[0];
		top.halfEdges[6].left = &top.faces[0];
		top.halfEdges[9].left = &top.faces[1];
		top.halfEdges[11].left = &top.faces[1];
		top.halfEdges[13].left = &top.faces[1];
		top.halfEdges[15].left = &top.faces[1];
		top.halfEdges[1].left = &top.faces[2];
		top.halfEdges[20].left = &top.faces[2];
		top.halfEdges[8].left = &top.faces[2];
		top.halfEdges[16].left = &top.faces[2];
		top.halfEdges[12].left = &top.faces[3];
		top.halfEdges[22].left = &top.faces[3];
		top.halfEdges[5].left = &top.faces[3];
		top.halfEdges[18].left = &top.faces[3];
		top.halfEdges[10].left = &top.faces[4];
		top.halfEdges[21].left = &top.faces[4];
		top.halfEdges[7].left = &top.faces[4];
		top.halfEdges[23].left = &top.faces[4];
		top.halfEdges[14].left = &top.faces[5];
		top.halfEdges[19].left = &top.faces[5];
		top.halfEdges[3].left = &top.faces[5];
		top.halfEdges[17].left = &top.faces[5];

		top.faces[0].halfEdge = &top.halfEdges[0];
		top.faces[1].halfEdge = &top.halfEdges[9];
		top.faces[2].halfEdge = &top.halfEdges[1];
		top.faces[3].halfEdge = &top.halfEdges[12];
		top.faces[4].halfEdge = &top.halfEdges[10];
		top.faces[5].halfEdge = &top.halfEdges[14];

		return top;
	}

}