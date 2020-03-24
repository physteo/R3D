#include <R3Dpch.h>
#include "Detection.h"
#include "Geometry.h"

#if defined(R3D_DEBUG)
#include <R3D/Core/Log.h>
#include <R3D/Core/Application.h>
#endif

namespace r3d
{

	const Topology BoxBox::s_BoxTopology = create_box_topology();

	const Topology& BoxBox::getTopology()
	{
		return BoxBox::s_BoxTopology;
	}

	struct FaceQuery
	{
		FaceQuery()
		{
			faceID = -1;
			penetration = std::numeric_limits<real>::infinity();
		}

		real penetration;
		int faceID;
	};

	struct PointQuery
	{
		int pointID;
		real3 position;
		real penetration;
	};

	struct EdgeQuery
	{
		int edgeID1;
		int edgeID2;
		real3 end1;
		real3 end2;
		real3 origin1;
		real3 origin2;
		real separation;
		real3 normal;
	};

	ColliderPlane get_plane_from_face(const Transform& box, const Facet& face)
	{
		real3 vertexInPlane = box.position + box.orientation * (box.scale * face.halfEdge->origin->position);

		ColliderPlane plane;
		plane.normal = box.orientation * face.computeNormal();
		plane.offset = glm::dot(plane.normal, vertexInPlane);

		return plane;
	}

	FaceQuery query_point_faces(const Transform& box1, const Transform& box2)
	{
		const Topology& top = BoxBox::getTopology();

		FaceQuery fq;

		for (int i = 0; i < top.faces.size(); ++i)
		{
			const Facet& face = top.faces[i];
			real distance;
			int pointID = get_deepest_point_id(top, box2, get_plane_from_face(box1, face), distance);

			if (pointID < 0)
			{
				// sat exists, early out
				fq.faceID = -1;
				fq.penetration = std::numeric_limits<real>::infinity();
				return fq;
			}
			if (abs(distance) < abs(fq.penetration) )
			{
				fq.penetration = -distance;
				fq.faceID = i;
			} 
		}

		return fq;
	}

	bool build_minkowsky_face(const real3& a, const real3& b, const real3& b_x_a, const real3& c, const real3& d, const real3& d_x_c)
	{
		real CBA = glm::dot(c, b_x_a);
		real DBA = glm::dot(d, b_x_a);
		real ADC = glm::dot(a, d_x_c);
		real BDC = glm::dot(b, d_x_c);
		return CBA * DBA < 0.0f && ADC * BDC < 0.0f && CBA * BDC > 0.0f;
	}

	real distance_edge_edge(const real3& edge1, const real3& point1, const real3& centroid1, const real3& edge2, const real3& point2, real3& normal)
	{
		real3 unnorm_normal = glm::cross(edge1, edge2);
		
		if (glm::dot(unnorm_normal, unnorm_normal) < R3D_EPSILON)
		{
			return -std::numeric_limits<real>::infinity();
		}

		normal = glm::normalize(unnorm_normal);
		if (glm::dot(normal, point1 - centroid1) < 0)
		{
			normal = -normal;
		}

		return glm::dot(normal, point2 - point1);
	}

	float get_separation(const real3& P1, const real3& E1, const real3& P2, const real3& E2, const real3& C1)
	{
		// Build search direction
		real3 E1_x_E2 = glm::cross(E1, E2);

		// Skip near parallel edges: |e1 x e2| = sin(alpha) * |e1| * |e2|
		const real kTolerance = 0.005f;

		real L = glm::length(E1_x_E2);
		if (L < kTolerance * glm::sqrt(glm::dot(E1,E1) * glm::dot(E2,E2)))
		{
			return -std::numeric_limits<real>::infinity();
		}

		// Assure consistent normal orientation (here: Hull1 -> Hull2)
		real3 N = E1_x_E2 / L;
		if (glm::dot(N, P1 - C1) < 0.0f)
		{
			N = -N;
		}

		// s = Dot(n, p2) - d = Dot(n, p2) - Dot(n, p1) = Dot(n, p2 - p1) 
		return glm::dot(N, P2 - P1);
	}

	EdgeQuery query_edges(const Transform& box1, const Transform& box2)
	{
		const Topology& top = BoxBox::getTopology();
		
		EdgeQuery eq;
		eq.separation = -std::numeric_limits<real>::infinity();

		for (int i = 0; i < top.halfEdges.size(); i+=2) 
		{
			real3 origin1 = box1.position + box1.orientation * (box1.scale * top.halfEdges[i].origin->position);
			real3 end1    = box1.position + box1.orientation * (box1.scale * top.halfEdges[i].next->origin->position);
			real3 edge1 = end1 - origin1;

			real3 normA = box1.orientation * top.halfEdges[i].left->computeNormal();
			real3 normB = box1.orientation * top.halfEdges[i].twin->left->computeNormal();

			for (int j = 0; j < top.halfEdges.size(); j+=2)
			{
				const real3& origin2 = box2.position + box2.orientation * (box2.scale * top.halfEdges[j].origin->position);
				const real3& end2    = box2.position + box2.orientation * (box2.scale * top.halfEdges[j].next->origin->position);
				const real3& edge2 = end2 - origin2;

				real3 normC = box2.orientation * top.halfEdges[j].left->computeNormal();
				real3 normD = box2.orientation * top.halfEdges[j].twin->left->computeNormal();

				if (build_minkowsky_face(normA, normB, -edge1, -normC, -normD, -edge2))
				{
					//real3 c1, c2;
					real separation = get_separation(origin1, edge1, origin2, edge2, box1.position);

					if (separation > eq.separation)
					{
						eq.separation = separation;
						eq.edgeID1 = i;
						eq.edgeID2 = j;
						eq.end1 = end1;
						eq.end2 = end2;
						eq.origin1 = origin1;
						eq.origin2 = origin2;
						eq.normal = glm::cross(edge1, edge2);
						if (glm::dot(eq.normal, origin1 - box1.position) < 0)
						{
							eq.normal = -eq.normal;
						}
					}
				}
			}
		}
		
		return eq;
	}

	std::pair<bool, real3> clip_segment_to_face(real3 po, real3 pe, const Facet& referenceFace, const Transform& boxFace)
	{
		ColliderPlane plane = get_plane_from_face(boxFace, referenceFace);
		real3 q;
		if (intersect_segment_plane(po, pe, plane, q))
		{
			return { true, q };
		}
		return { false, real3{0.0} };
	}

	int get_incident_face_id(const real3& referenceNormal, const Transform& boxPoint)
	{
		const Topology& top = BoxBox::getTopology();

		real dotmin = std::numeric_limits<real>::infinity();
		int idmin = -1;

		for (int i = 0; i < top.faces.size(); ++i)
		{
			real3 normal = boxPoint.orientation * top.faces[i].computeNormal();
			real dot = glm::dot(normal, referenceNormal);
			if (dot < dotmin)
			{
				dotmin = dot;
				idmin = i;
			}
		}

		return idmin;
	}

	bool is_point_below_face(const real3& point, const Transform& box, const ColliderPlane& plane, const Facet& face, real& dist, real3& intersection)
	{

		if (is_point_below_plane(point, plane, dist))
		{
			real3 facePoints[4];
			face.getFaceVertices(facePoints);
			for (int i = 0; i < 4; ++i)
			{
				facePoints[i] = box.position + box.orientation * (box.scale * facePoints[i]);
			}

			real3 p = point + real(2.) * dist * plane.normal;
			real3 q = point - real(2.) * dist * plane.normal;

			if (intersect_line_quad(q, p, facePoints[0], facePoints[1], facePoints[2], facePoints[3], 0.000001, intersection)) // TODO: control better tolerance 
			{
				if (glm::abs(dist) < R3D_EPSILON)
				{
					intersection = project_point_to_plane(point, plane);
				}
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	std::vector<PointQuery> clip_face(const Facet& incidentFace, const Transform& incidentBox, 
		const Facet& referenceFace, const Transform& referenceBox)
	{
		const ColliderPlane& refPlane = get_plane_from_face(referenceBox, referenceFace);

		std::vector<PointQuery> clippedPoints;
		// loop over side planes of refface, clip each segment of the incface.
		HalfEdge* begin = referenceFace.halfEdge;
		HalfEdge* half = begin;

		do
		{
			Facet* sideFace = half->twin->left;

			// debug draw 
#if defined(R3D_DEBUG)
			{
				real3 faceCenter = referenceBox.position + referenceBox.orientation * sideFace->computeCenter();
				real3 sideNormal = referenceBox.orientation * sideFace->computeNormal();
				Application::getDebugger()->draw_segment(faceCenter, sideNormal, { 1,1,0 });
			}
#endif
			// loop over segments of incface
			int segmentID = -1;
			HalfEdge* beginInc = incidentFace.halfEdge;
			HalfEdge* halfInc = beginInc;
			do
			{
				// clip this halfedge with sideface
				real3& po = incidentBox.position + incidentBox.orientation * (incidentBox.scale * halfInc->origin->position);
				real3& pe = incidentBox.position + incidentBox.orientation * (incidentBox.scale * halfInc->next->origin->position);

				auto result = clip_segment_to_face(po, pe, *sideFace, referenceBox);
				if (result.first)
				{
					// is below ref face?
					real dist;
					real3 intersection;
					if (is_point_below_face(result.second, referenceBox, refPlane, referenceFace, dist, intersection))
					{
						PointQuery p;
						p.penetration = -dist;
						p.pointID = segmentID;
						p.position = intersection;
						clippedPoints.push_back(p);
					}
				}
				--segmentID;
				halfInc = halfInc->next;
			} while (halfInc != beginInc);

			half = half->next;
		} while (half != begin);

		return clippedPoints;
	}

	std::vector<PointQuery> collect_penetrating_points(const Transform& boxPoint, const Transform& referenceBox, const ColliderPlane& refPlane, const Facet& referenceFace, bool project)
	{
		const Topology& top = BoxBox::getTopology();
		std::vector<PointQuery> penetratingPoints;
		for (int i = 0; i < top.vertices.size(); ++i)
		{
			real3 point = boxPoint.position + boxPoint.orientation * (boxPoint.scale * top.vertices[i].position);
			real dist;
			real3 intersection;
			if (is_point_below_face(point, referenceBox, refPlane, referenceFace, dist, intersection))
			{
				PointQuery p;
				p.penetration = -dist;
				p.pointID = i;
				p.position = project ? intersection : point;
				penetratingPoints.push_back(p);
			}
		}

		return penetratingPoints;
	}

	int create_point_face_contacts(FaceQuery fq, const Transform& referenceBox, Entity eRef, const Transform& incidentBox, Entity eInc, Manifold* manifold)
	{
		const Topology& top = BoxBox::getTopology();
		
		// get reference face
		const Facet& referenceFace = top.faces[fq.faceID];
		const real3& referenceNormal = glm::normalize(referenceBox.orientation * referenceFace.computeNormal());
		const ColliderPlane& refPlane = get_plane_from_face(referenceBox, referenceFace);

		// get incident face
		const Facet& incidentFace = top.faces[get_incident_face_id(referenceNormal, incidentBox)];
		const real3& incidentNormal = glm::normalize(incidentBox.orientation * incidentFace.computeNormal());
		const ColliderPlane& incPlane = get_plane_from_face(incidentBox, incidentFace);

		// which points of the incident face are just below the reference face?
		std::vector<PointQuery> penetratingPoints = collect_penetrating_points(incidentBox, referenceBox, refPlane, referenceFace, true);
		 
		// which points of the reference face are just below the incident face?
		std::vector<PointQuery> penetratingPoints2 = collect_penetrating_points(referenceBox, incidentBox, incPlane, incidentFace, false);

		// clip segments of the incident face against side planes
		std::vector<PointQuery> clippedPointsOfIncidentFace = clip_face(incidentFace, incidentBox, referenceFace, referenceBox);
		
#if defined(R3D_DEBUG)
		// debug draw
		{
			real3 faceCenter = referenceBox.position + referenceBox.orientation * referenceBox.scale * referenceFace.computeCenter();
			Application::getDebugger()->draw_segment(faceCenter, referenceNormal, { 1,0,0 });
		
			faceCenter = incidentBox.position + incidentBox.orientation * referenceBox.scale *  incidentFace.computeCenter();
			Application::getDebugger()->draw_segment(faceCenter, incidentNormal, { 0,1,0 });
		}
		// debug draw
		{
			for (auto v : clippedPointsOfIncidentFace)
				Application::getDebugger()->draw_circle(v.position, 0.15, { 1.0, 1.0, 0.0 });
		
			for (auto p : penetratingPoints)
				Application::getDebugger()->draw_circle(p.position, 0.18, { 1.0, 0.0, 1.0 });
		
			for (auto p : penetratingPoints2)
				Application::getDebugger()->draw_circle(p.position, 0.21, { 1.0, 0.5, 1.0 });
		
		}
#endif
		// *************** reduce
		manifold->e1 = eRef;
		manifold->e2 = eInc;
		manifold->normal = glm::normalize(referenceNormal);
		compute_basis(manifold->normal, manifold->tangent[0], manifold->tangent[1]);


		int numContacts = 0;
		for (auto& p : clippedPointsOfIncidentFace)
		{
			Contact* c = manifold->contacts + numContacts;
			c->type = ContactType::BOXBOX__POINT_FACE;
			c->fp.first = fq.faceID;
			c->fp.second = p.pointID;
			c->penetration = p.penetration;
			c->position = p.position;
			++numContacts;
		}
		
		for (auto& p : penetratingPoints)
		{
			Contact* c = manifold->contacts + numContacts;
			c->type = ContactType::BOXBOX__POINT_FACE;
			c->fp.first = fq.faceID;
			c->fp.second = p.pointID;
			c->penetration = p.penetration;
			c->position = p.position;
			++numContacts;
		}

		for (auto& p : penetratingPoints2)
		{
			Contact* c = manifold->contacts + numContacts;
			c->type = ContactType::BOXBOX__POINT_FACE;
			c->fp.first = fq.faceID;
			c->fp.second = p.pointID;
			c->penetration = p.penetration;
			c->position = p.position;
			++numContacts;
		}

#if defined(R3D_DEBUG)
		//Application::getDebugger()->draw_segment(manifoldCenter, manifold->normal, { 1.0, 1.0, 1.0 });
		
		if (numContacts > 8)
		{
			R3D_CORE_WARN("Creating manifold: {0}", numContacts);
		}
#endif

		manifold->numContacts = numContacts;
		return numContacts;
	}

	int create_edge_edge_contact(EdgeQuery eq, const Transform& referenceBox, Entity eRef, const Transform& incidentBox, Entity eInc, Manifold* manifold)
	{
		// find closest point between edges
		real3 c1, c2;
		closest_points_edges(eq.origin1, eq.end1, eq.origin2, eq.end2, c1, c2);

		manifold->numContacts = 1;
		manifold->e1 = eRef;
		manifold->e2 = eInc;
		manifold->normal = glm::normalize(eq.normal);
		compute_basis(manifold->normal, manifold->tangent[0], manifold->tangent[1]);

		Contact* c = manifold->contacts;
		c->type = ContactType::BOXBOX__EDGE_EDGE;
		c->fp.first = eq.edgeID1;
		c->fp.second = eq.edgeID2;
		c->penetration = -eq.separation;
		c->position = real(0.5) * (c1 + c2);

#if defined(R3D_DEBUG)
		Application::getDebugger()->draw_circle(c->position, 0.15, { 1.0, 0.64, 0.0 });
#endif
		return manifold->numContacts;
	}

	unsigned int BoxBox::detect(const Transform& box1, const Entity& e1, const Transform& box2, const Entity& e2, Manifold* manifold)
	{
		// query face-point contacts
 		FaceQuery fq1 = query_point_faces(box1, box2);  
		if (fq1.faceID < 0) return false;
		
		FaceQuery fq2 = query_point_faces(box2, box1);
		if (fq2.faceID < 0 || fq1.faceID < 0) return false;

		// query edge-edge contacts
		EdgeQuery eq = query_edges(box1, box2);
		if (eq.separation > 0) return false;
		
		// pick the minimum penetration
		const real bias = 0.0001;
		if (abs(eq.separation) + bias < abs(fq1.penetration) && abs(eq.separation) + bias < abs(fq2.penetration))
		{
			return create_edge_edge_contact(eq, box1, e1, box2, e2, manifold);
		}			
		else if (abs(fq1.penetration) <= abs(fq2.penetration) + bias )
		{
			return create_point_face_contacts(fq1, box1, e1, box2, e2, manifold);
		}
		else
		{
			return create_point_face_contacts(fq2, box2, e2, box1, e1, manifold);
		}
	} 
}