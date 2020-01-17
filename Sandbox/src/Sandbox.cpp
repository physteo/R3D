
#include <r3d.h>

class Sandbox : public r3d::Application
{
public:
	Sandbox() : Application{} {}
	~Sandbox() {}
};


r3d::Application* r3d::createApplication()
{
	return new Sandbox();
}
 