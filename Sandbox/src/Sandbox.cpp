#include <R3D.h>

class Sandbox : public r3d::Application
{
public:
	Sandbox() : Application{} {}
	~Sandbox() {}
};


r3d::Application* r3d::createApplication()
{
	r3d::EventManager eventMgr;
	r3d::EntityManager entityMgr;
	r3d::ArchetypeManager archetypeMgr;
	
	Entity a;
	
	r3d::Entity e = entityMgr.create();
	archetypeMgr.setArchetype<int>(e);

	return new Sandbox();
}
 