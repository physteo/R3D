#include <R3Dpch.h>

#include "Archetype.h"

namespace r3d
{

	void ComponentList::finish()
	{
		// sort the list
		std::sort(m_metatypes.begin(), m_metatypes.end(), [](auto left, auto right)
		{
			return left.getHash() < right.getHash();
		});

		// create ID
		std::string id;
		for (auto& elem : m_metatypes)
		{
			id.append(elem.getName());
			id.append("-");
		}
		m_id = StringHash::computeHash(id);
	}

	ArchetypeData::ArchetypeData() : m_archetype{}
	{
	}

	ArchetypeData::ArchetypeData(Archetype arch) : m_archetype(arch)
	{
		for (auto meta : arch.getComponentList().getMetatypes())
		{
			//auto compName = meta.getName();
			//componentDataMap.emplace(compName, RawPackedArray{ meta });
			auto compHash = meta.getHash();
			componentDataMap.emplace(compHash, RawPackedArray{ meta });
		}
	}

	void ArchetypeData::assignDefault(Entity entity)
	{
		for (auto& string_packedArray_pair : componentDataMap)
		{
			string_packedArray_pair.second.assign(entity, nullptr); // TODO: change to default constructor
			//componentDataMap.emplace(compName, RawPackedArray{ meta });
			//componentDataMap[compName] = std::move(RawPackedArray{ meta });
		}
	}

	void ArchetypeData::remove(Entity entity)
	{
		for (auto& string_packedArray_pair : componentDataMap)
		{
			string_packedArray_pair.second.remove(entity);
		}
	}

	const Archetype& ArchetypeData::getArchetype() const
	{
		return m_archetype;
	}

	const ComponentList& ArchetypeData::getComponentList() const
	{
		return m_archetype.getComponentList();
	}

}