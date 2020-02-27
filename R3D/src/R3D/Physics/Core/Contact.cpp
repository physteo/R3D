#include <R3Dpch.h>
#include "Contact.h"
#include <R3D/Ecs/ArchetypeManager.h>
#include <R3D/Ecs/TransformComponent.h>
#include <R3D/Physics/Components/BasicPhysicsComponents.h>

#include "Colliders.h"

namespace r3d
{

	void Arbiter::update(Manifold* newManifold)
	{
		Manifold mergedManifold;

		for (int i = 0; i < newManifold->numContacts; ++i)
		{
			Contact* cNew = newManifold->contacts + i;
			int k = -1;
			for (int j = 0; j < manifold.numContacts; ++j)
			{
				Contact* cOld = manifold.contacts + j;
				if (cNew->fp == cOld->fp && cNew->type == cOld->type)
				{
					k = j;
					break;
				}
			}

			if (k > -1)
			{
				Contact* c = mergedManifold.contacts + i;
				Contact* cOld = manifold.contacts + k;
				*c = *cNew;
				if (warmStart)
				{
					c->impulseNormal =  cOld->impulseNormal;
					c->impulseTangent[0] = cOld->impulseTangent[0];
					c->impulseTangent[1] = cOld->impulseTangent[1];
				}
				else
				{
					c->impulseNormal = 0.0;
					c->impulseTangent[0] = 0.0;
					c->impulseTangent[1] = 0.0;
				}
			}
			else
			{
				mergedManifold.contacts[i] = newManifold->contacts[i];
			}
		}

		for (int i = 0; i < newManifold->numContacts; ++i)
		{
			manifold.contacts[i] = mergedManifold.contacts[i];
		}

		manifold.numContacts = newManifold->numContacts;
		manifold.normal = newManifold->normal;
		manifold.tangent[0] = newManifold->tangent[0];
		manifold.tangent[1] = newManifold->tangent[1];
	}

	void Arbiter::preStep(ArchetypeManager* am, float inv_dt)
	{
		const float k_allowedPenetration = 0.01f;
		float k_biasFactor = positionCorrection ? 0.2f : 0.0f;

		Entity e1 = body1;
		Entity e2 = body2;
		if (e2 == manifold.e1)
		{
			std::swap(e1, e2);
		}

		Transform* tr1 = am->get<Transform>(e1);
		Transform* tr2 = am->get<Transform>(e2);
		RigidBody* rb1 = am->get<RigidBody>(e1);
		RigidBody* rb2 = am->get<RigidBody>(e2);

		real3& position1          = tr1->position;
		real3& position2          = tr2->position;
		const rquat& orientation1 = tr1->orientation;
		const rquat& orientation2 = tr2->orientation;
		real3& velocity1     = rb1->velocity;
		real3& velocity2     = rb2->velocity;
		real3& angVelocity1  = rb1->angVelocity;
		real3& angVelocity2  = rb2->angVelocity;
		const real& invMass1 = rb1->invMass;
		const real& invMass2 = rb2->invMass;
		const real3x3& invI1 = change_basis_of_matrix(orientation1, rb1->invI);
		const real3x3& invI2 = change_basis_of_matrix(orientation2, rb2->invI);

		const real3& n = manifold.normal;
		const real3 tangent[2] = { manifold.tangent[0], manifold.tangent[1] };
		
		for (int i = 0; i < manifold.numContacts; ++i)
		{
			Contact* c = manifold.contacts + i;

			if (c->type != ContactType::BOXPLANE__POINT_FACE && glm::dot(manifold.normal, position2 - position1) < 0)
			{
#if defined(R3D_DEBUG) || defined(R3D_RELEASE)
				//R3D_CORE_ERROR("direction wrong: {0},{1},{2}", manifold.normal.x, manifold.normal.y, manifold.normal.z);
#endif
				manifold.normal *= -1;
			}

			const real3& contactPointWorld = c->position;
			real3 r1 = contactPointWorld - position1;
			real3 r2 = contactPointWorld - position2;

			real3 rn1 = glm::cross(r1, n);
			real3 rn2 = glm::cross(r2, n);
			real kNormal = invMass1 + invMass2 + glm::dot(invI1 * rn1, rn1) + glm::dot(invI2 * rn2, rn2);
			c->massNormal = 1.0f / kNormal;

			for (int j = 0; j < 2; ++j)
			{
				real3 rt1 = glm::cross(r1, tangent[j]);
				real3 rt2 = glm::cross(r2, tangent[j]);
				real kTangent = invMass1 + invMass2 + glm::dot(invI1 * rt1, rt1) + glm::dot(invI2 * rt2, rt2);
				c->massTangent[j] = 1.0f / kTangent;
			}
			
			c->bias = -k_biasFactor * inv_dt * glm::min(real(0.0), -glm::abs(c->penetration) + k_allowedPenetration);

			if (accumulateImpulses)
			{
				// Apply normal + friction impulse
				real3 P = c->impulseNormal * n;
				P += c->impulseTangent[0] * tangent[0];
				P += c->impulseTangent[1] * tangent[1];

				velocity1 -= invMass1 * P;
				angVelocity1 -= invI1 * glm::cross(r1, P);

				velocity2 += invMass2 * P;
				angVelocity2 += invI2 * glm::cross(r2, P);
			}
		}
	}

	void Arbiter::applyImpulse(ArchetypeManager* am)
	{
		Entity e1 = body1;
		Entity e2 = body2;
		if (e2 == manifold.e1)
		{
			std::swap(e1, e2);
		}

		Transform* tr1 = am->get<Transform>(e1);
		Transform* tr2 = am->get<Transform>(e2);
		RigidBody* rb1 = am->get<RigidBody>(e1);
		RigidBody* rb2 = am->get<RigidBody>(e2);

		real3& position1          = tr1->position;
		real3& position2          = tr2->position;
		const rquat& orientation1 = tr1->orientation;
		const rquat& orientation2 = tr2->orientation;
		real3& velocity1     = rb1->velocity;
		real3& velocity2     = rb2->velocity;
		real3& angVelocity1  = rb1->angVelocity;
		real3& angVelocity2  = rb2->angVelocity;
		const real& invMass1 = rb1->invMass;
		const real& invMass2 = rb2->invMass;
		const real3x3& invI1 = change_basis_of_matrix(orientation1, rb1->invI);
		const real3x3& invI2 = change_basis_of_matrix(orientation2, rb2->invI);

		real3 v1 = velocity1;
		real3 w1 = angVelocity1;
		real3 v2 = velocity2;
		real3 w2 = angVelocity2;
		
		const real3& n = manifold.normal;
		const real3 tangent[2] = { manifold.tangent[0], manifold.tangent[1] };

		for (int i = 0; i < manifold.numContacts; ++i)
		{
			Contact* c = manifold.contacts + i;
			const real3& contactPointWorld = /* position2 + orientation2 * */ c->position;

			real3 r1 = contactPointWorld - position1;
			real3 r2 = contactPointWorld - position2;

			// Relative velocity at contact
			real3 dv = v2 + glm::cross(w2, r2) - v1 - glm::cross(w1, r1);

			for (int j = 0; j < 2; ++j)
			{
				real vt = glm::dot(dv, tangent[j]);
				real deltaImpulseTangent = c->massTangent[j] * (-vt);
				if (accumulateImpulses)
				{
					// Compute friction impulse
					real maxPt = friction * c->impulseNormal;

					// Clamp friction
					real oldImpulseTangent = c->impulseTangent[j];
					c->impulseTangent[j] = glm::clamp(oldImpulseTangent + deltaImpulseTangent, -maxPt, maxPt);
					deltaImpulseTangent = c->impulseTangent[j] - oldImpulseTangent;
				}
				real3 impulseTangent = deltaImpulseTangent * tangent[j];
				v1 -= invMass1 * impulseTangent;
				w1 -= invI1 * glm::cross(r1, impulseTangent);
				v2 += invMass2 * impulseTangent;
				w2 += invI2 * glm::cross(r2, impulseTangent);
			}

			// Compute normal impulse ******************************************************
			dv = dv = v2 + glm::cross(w2, r2) - v1 - glm::cross(w1, r1);

			real vn = glm::dot(dv, n);
			real deltaImpulseNormal = c->massNormal * (-vn + c->bias);

			if (accumulateImpulses)
			{
				// Clamp the accumulated impulse
				float Pn0 = c->impulseNormal;
				c->impulseNormal = glm::max(Pn0 + deltaImpulseNormal, real(0.0));
				deltaImpulseNormal = c->impulseNormal - Pn0;
			}
			else
			{
				deltaImpulseNormal = glm::max(deltaImpulseNormal, real(0.0));
			}

			// Apply contact impulse
			real3 impulseNormal = deltaImpulseNormal * n;

			v1 -= invMass1 * impulseNormal;
			w1 -= invI1 * glm::cross(r1, impulseNormal);
			v2 += invMass2 * impulseNormal;
			w2 += invI2 * glm::cross(r2, impulseNormal);

		}

		velocity1 = v1;
		angVelocity1 = w1;
		velocity2 = v2;
		angVelocity2 = w2;
	}

	void Arbiter::printInfo() const
	{
		R3D_INFO("Body 1:   {0}",this->body1.id);
		R3D_INFO("Body 2:   {0}",this->body2.id);
		R3D_INFO("friction: {0}",this->friction);
		R3D_INFO("Manifold: ");
		R3D_INFO("    e1: {0}", manifold.e1.id);
		R3D_INFO("    e2: {0}", manifold.e2.id);
		R3D_INFO("    n: {0},{1},{2}", manifold.normal.x, manifold.normal.y, manifold.normal.z);
		R3D_INFO("    t: {0},{1},{2}", manifold.tangent[0].x, manifold.tangent[0].y, manifold.tangent[0].z);
		R3D_INFO("    b: {0},{1},{2}", manifold.tangent[1].x, manifold.tangent[1].y, manifold.tangent[1].z);
		R3D_INFO("    num: {0}", manifold.numContacts);
		for (int i = 0; i < manifold.numContacts; i++)
		{
			const Contact* c = manifold.contacts + i;
			R3D_INFO("    C #{0} ************", i);
			R3D_INFO("    imp n: {0}", c->impulseNormal);
			R3D_INFO("    imp t: {0}", c->impulseTangent[0]);
			R3D_INFO("    imp b: {0}", c->impulseTangent[1]);
		}
	}

	

	Arbiter::Arbiter(Entity b1, Entity b2, Manifold manifold, real friction)
	{
		if (b1 < b2)
		{
			body1 = b1;
			body2 = b2;
		}
		else
		{
			body1 = b2;
			body2 = b1;
		}

		this->manifold = std::move(manifold);
		this->friction = friction;//sqrtf(body1->friction * body2->friction);
	}




}