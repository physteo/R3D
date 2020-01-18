#include <R3Dpch.h>
#include "RawPackedArray.h"

namespace r3d
{

	RawPackedArray::RawPackedArray()
	{
		m_active = 0;
		//m_meta = meta;
		m_data = nullptr;
		m_dataEntities.resize(0);
	}

	RawPackedArray::RawPackedArray(Metatype meta)
	{
		m_active = 0;
		m_meta = meta;
		m_data = (char*)malloc(MAX_ELEMENTS * meta.getSize());
		for (size_t i = 0; i < MAX_ELEMENTS; i++)
		{
			meta.constructor(m_data + i * meta.getSize());
		}
		m_dataEntities.resize(MAX_ELEMENTS);
	}

	RawPackedArray::~RawPackedArray()
	{
		release();
	}

	RawPackedArray::RawPackedArray(const RawPackedArray& other)
	{

		m_map = other.m_map;
		m_dataEntities = other.m_dataEntities;
		m_active = other.m_active;
		m_meta = other.m_meta;

		// deep copy of data
		m_data = (char*)malloc(MAX_ELEMENTS * m_meta.getSize());
		for (size_t i = 0; i < MAX_ELEMENTS; i++)
		{
			void* destination = (void*)(m_data + i * m_meta.getSize());
			void* source = (void*)(other.m_data + i * m_meta.getSize());
			m_meta.constructor(destination);
			m_meta.copy(destination, source);
		}

	}

	RawPackedArray& RawPackedArray::operator=(const RawPackedArray& other)
	{
		if (m_meta == other.m_meta)
		{
			// no need to reallocate
			m_map = other.m_map;
			m_dataEntities = other.m_dataEntities;
			m_active = other.m_active;

			for (size_t i = 0; i < MAX_ELEMENTS; i++)
			{
				void* destination = (void*)(m_data + i * m_meta.getSize());
				void* source = (void*)(other.m_data + i * m_meta.getSize());
				m_meta.constructor(destination);
				m_meta.copy(destination, source);
			}
		}
		else
		{
			// need to deallocate and reallocate
			release();
			m_map = other.m_map;
			m_dataEntities = other.m_dataEntities;
			m_active = other.m_active;
			m_meta = other.m_meta;

			m_data = (char*)malloc(MAX_ELEMENTS * m_meta.getSize());
			for (size_t i = 0; i < MAX_ELEMENTS; i++)
			{
				void* destination = (void*)(m_data + i * m_meta.getSize());
				void* source = (void*)(other.m_data + i * m_meta.getSize());
				m_meta.constructor(destination);
				m_meta.copy(destination, source);
			}
		}
		return *this;
	}

	RawPackedArray::RawPackedArray(RawPackedArray&& other)
	{ 
		swapData(other);
	}

	RawPackedArray& RawPackedArray::operator=(RawPackedArray&& other)
	{
		// check for self-assignment.
		if (this != &other)
		{
			release();
			swapData(other);
		}
		return *this;
	}

	bool operator==(const RawPackedArray& lhs, const RawPackedArray& rhs)
	{
		bool firstCondition = (lhs.m_map == rhs.m_map) && (lhs.m_dataEntities == rhs.m_dataEntities)
			&& (lhs.m_active == rhs.m_active) && (lhs.m_meta == rhs.m_meta);
		if (firstCondition == false) return false;


		for (size_t i = 0; i < lhs.MAX_ELEMENTS; i++)
		{
			void* leftValue = (void*)(lhs.m_data + i * lhs.m_meta.getSize());
			void* rightValue = (void*)(rhs.m_data + i * rhs.m_meta.getSize());
			if (std::memcmp(leftValue, rightValue, lhs.m_meta.getSize()) == false) return false;
		}

		return true;
	}

	char* RawPackedArray::getComponent(Entity e)
	{
		return (m_data + (m_map.find(e)->second) * m_meta.getSize());
	}

	void RawPackedArray::copy(Entity e, char* data)
	{
		size_t entityIndex = m_map.find(e)->second;
		char* location = (m_data + entityIndex * m_meta.getSize());
		//memcpy(location, data, m_meta.getSize());
		m_meta.copy((void*)location, (void*)data);
	}

	void RawPackedArray::assign(Entity e, char* data)
	{
		char* location = (m_data + m_active * m_meta.getSize());
		m_meta.copy((void*)location, (void*)data);
		//memcpy(location, data, m_meta.getSize());

		m_dataEntities[m_active] = std::move(e);
		m_map.insert({ e, m_active });
		++m_active;
		if (m_active >= MAX_ELEMENTS)
		{
			std::cerr << "[ECS] Error: maximum number of components " << MAX_ELEMENTS << " exceeded." << std::endl;
			assert(false);
		}
	}

	void RawPackedArray::remove(Entity e)
	{
		assert(has(e));
		size_t i = m_map[e];

		char* sourceLocation = m_data + (m_active - 1) * m_meta.getSize();
		char* destinLocation = m_data + i * m_meta.getSize();
		//memcpy(destinLocation, sourceLocation, m_meta.getSize());
		m_meta.move((void*)destinLocation, (void*)sourceLocation);

		m_dataEntities[i] = std::move(m_dataEntities[m_active - 1]);
		m_dataEntities[m_active - 1] = Entity{ 29790, 0 };

		if (m_active > 1)
			m_map[m_dataEntities[i]] = i;

		m_map.erase(e);
		--m_active;
	}

	bool RawPackedArray::has(Entity e)
	{
		return m_map.find(e) != m_map.end();
	}

	void RawPackedArray::release()
	{
		m_map.clear();
		m_dataEntities.clear();
		if (m_data != nullptr)
		{
			for (size_t i = 0; i < MAX_ELEMENTS; i++)
			{
				m_meta.destructor(m_data + i * m_meta.getSize());
			}
		}
		m_active = 0;
		m_meta = Metatype{};
		free(m_data);
	}

	void RawPackedArray::swapData(RawPackedArray& other)
	{
		m_map = other.m_map;
		m_dataEntities = other.m_dataEntities;
		m_active = other.m_active;
		m_meta = other.m_meta;
		m_data = other.m_data;

		other.m_data = nullptr;
		other.m_active = 0;
		other.m_map.clear();
		other.m_dataEntities.clear();
		other.m_meta = Metatype{};
	}

	void TestingRawArray::testAll() const
	{
		std::cout << "Tests addition, deletion, usage of nonPod elements \n in the RawPackedArray: " << std::endl;
		bool first = testNonPod();
		std::cout << "Final result: " << (first ? "passed" : "failed") << std::endl;



	}

	// this performs addition, deletion, usage of nonPod elements into the RayArray
	bool TestingRawArray::testNonPod() const
	{

		Metatype complex2 = Metatype::buildMetaType<NonPodDerived>();
		RawPackedArray arr{ complex2 };

		Entity e0{ 0,0 };
		Entity e1{ 1,0 };
		Entity e2{ 2,0 };
		Entity e3{ 3,0 };
		Entity e4{ 4,0 };

		NonPodDerived toAssign0{ 1,2,3 };
		arr.assign(e0, (char*)&toAssign0);
		NonPodDerived toAssign1{ 10,20,30 };
		arr.assign(e1, (char*)&toAssign1);
		NonPodDerived toAssign2{ 100,200,300 };
		arr.assign(e2, (char*)&toAssign2);

		int sum0 = ((NonPodDerived*)arr.getComponent(e0))->sum_all();
		int sum1 = ((NonPodDerived*)arr.getComponent(e1))->sum_all();
		int sum2 = ((NonPodDerived*)arr.getComponent(e2))->sum_all();

		arr.remove(e0);
		arr.remove(e1);

		NonPodDerived toAssign3{ 1000,2000,3000 };
		arr.assign(e3, (char*)&toAssign3);
		NonPodDerived toAssign4{ 10000,20000,30000 };
		arr.assign(e4, (char*)&toAssign4);

		arr.remove(e3);

		int sum4 = ((NonPodDerived*)arr.getComponent(e4))->sum_all();
		int sum2b = ((NonPodDerived*)arr.getComponent(e2))->sum_all();

		bool final = true;

		if (sum0 == 6 && sum1 == 60 && sum2 == 600 && sum4 == 60000 && sum2b == sum2)
		{
			std::cout << "Addition, deletion, retrieval: passed." << std::endl;
		}
		else
		{
			std::cout << "Addition, deletion, retrieval: failed." << std::endl;
			final = false;
		}

		// test copy constructor
		RawPackedArray arr2(arr);
		if (arr2 == arr)
		{
			std::cout << "Copy constructor: passed." << std::endl;
		}
		else
		{
			std::cout << "Copy constructor: failed." << std::endl;
			final = false;
		}

		return final;

	}

}