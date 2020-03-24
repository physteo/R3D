#pragma once

namespace r3d
{

	class Metatype
	{
		using ConstructorFn = void(void*);
		using DestructorFn = void(void*);
		using CopyAssignmentFn = void(void*, void*);
		using MoveAssignmentFn = void(void*, void*);
		//using CompareFn = bool(void*, void*);

	public:
		Metatype() : m_size(0), m_name(""), m_hash(0), constructor([](void* p) {}), destructor([](void* p) {}), copy([](void*, void*) {}), move([](void*, void*) {}) {}

		template <class C>
		static Metatype buildMetaType()
		{
			Metatype meta;
			meta.m_size = sizeof(C);
			meta.m_name = typeid(C).name();
			meta.m_hash = typeid(C).hash_code();

			meta.constructor = [](void* p) {
				new(p) C{};
			};
			meta.destructor = [](void* p) {
				((C*)p)->~C();
			};
			meta.copy = [](void* destination, void* source) {
				*((C*)destination) = *((C*)source);
			};
			meta.move = [](void* destination, void* source) {
				*((C*)destination) = std::move(*((C*)source));
			};
			//meta.compare = [](void* lhs, void* rhs) {
			//	return *((C*)lhs) == *((C*)rhs);
			//};

			return meta;
		}

		inline size_t getSize() const { return m_size; }
		inline std::string getName() const { return m_name; }
		inline size_t getHash() const { return m_hash; }

		friend bool operator< (const Metatype &c1, const Metatype &c2)
		{
			return c1.getHash() < c2.getHash();
		}
		friend bool operator== (const Metatype &c1, const Metatype &c2)
		{
			return c1.getHash() == c2.getHash();
		}

		ConstructorFn* constructor;
		DestructorFn*  destructor;
		CopyAssignmentFn* copy;
		MoveAssignmentFn* move;
		//CompareFn* compare;

	private:
		size_t      m_size;
		std::string m_name;
		size_t      m_hash;

	};

#if 0

	struct TestingMetatype
	{
		bool test() const
		{
			std::vector<bool> results;

			Metatype intMeta = Metatype::buildMetaType<int>();

			results.push_back("int" == intMeta.getName());
			std::cout << "Name: " << results.back() << std::endl;

			results.push_back(sizeof(int) == intMeta.getSize());
			std::cout << "Size: " << results.back() << std::endl;

			results.push_back(typeid(int).hash_code() == intMeta.getHash());
			std::cout << "Hash: " << results.back() << std::endl;

			// putting a metatype of a simple type in some memory
			{
				void* pointer = malloc(intMeta.getSize());
				intMeta.constructor(pointer);
				*((int*)pointer) = 10;
				results.push_back(10 == *((int*)pointer));
				std::cout << "Simple construct on malloc'ed memory location: " << results.back() << std::endl;
				intMeta.destructor(pointer);
				free(pointer);
			}
			// putting a metatype of a complex type in some memory
			class Base {
			protected:
				int* reference;
			public:
				Base() { reference = new int(11); }
				~Base() { delete reference; }
			};
			class Foo : public Base
			{
			public:
				Foo() : Base() { value = *(this->reference); ciaone = 7.13f; lel = value > 10 ? true : false; }

				void printBaseReference() const
				{
					std::cout << "Foo::printBaseReference: " << *(this->reference) << std::endl;
				}

				float ciaone;
				bool lel;
				int value;
			};


			class Base2
			{
			protected:
				std::vector<int> values;
			public:
				Base2() {
					values.push_back(0); values.push_back(1); values.push_back(2);
				}
			};

			class Foo2 : public Base2
			{
			public:
				void print_at(size_t k) { std::cout << values.at(k) << std::endl; }
			};

			Metatype complex = Metatype::buildMetaType<Foo>();
			results.push_back("class `public: bool __thiscall ecsa::TestingMetatype::test(void)const '::`2'::Foo" == complex.getName());
			std::cout << "Name: " << results.back() << std::endl;

			results.push_back(sizeof(Foo) == complex.getSize());
			std::cout << "Size: " << results.back() << std::endl;

			results.push_back(typeid(Foo).hash_code() == complex.getHash());
			std::cout << "Hash: " << results.back() << std::endl;

			// putting a metatype of a simple type in some memory
			{
				void* pointer = malloc(complex.getSize());
				complex.constructor(pointer);

				results.push_back(true == ((Foo*)pointer)->lel);
				std::cout << "Complex->lel: " << results.back() << std::endl;
				results.push_back(7.13f == ((Foo*)pointer)->ciaone);
				std::cout << "Complex->ciaone: " << results.back() << std::endl;
				results.push_back(11 == ((Foo*)pointer)->value);
				std::cout << "Complex->value: " << results.back() << std::endl;

				std::cout << "before destructor" << std::endl;
				((Foo*)pointer)->printBaseReference();

				complex.destructor(pointer);

				//std::cout << "after destructor and free" << std::endl;
				//((Foo*)pointer)->printBaseReference();

				free(pointer);

			}


			// array 
			{
				Metatype complex2 = Metatype::buildMetaType<Foo2>();
				size_t max = 10;
				char* pointer = (char*)malloc(complex2.getSize() * max);
				for (size_t i = 0; i < max; i++)
				{
					complex2.constructor(pointer + i * complex2.getSize());
				}

				for (size_t i = 0; i < max; i++)
				{
					std::cout << "i: " << i << std::endl;
					((Foo2*)pointer)->print_at(0);
				}


				for (size_t i = 0; i < max; i++)
				{
					std::cout << "i: " << i << std::endl;
					((Foo2*)pointer)->print_at(0);
				}

				for (size_t i = 0; i < max; i++)
				{
					complex2.destructor(pointer + i * complex2.getSize());
				}


				free((void*)pointer);

			}



			bool fin = true;
			for (auto b : results)
				fin &= b;
			return fin;
		}




	};
#endif

}