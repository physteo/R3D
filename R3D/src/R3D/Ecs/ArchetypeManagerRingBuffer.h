#pragma once

#include "ArchetypeManager.h"

namespace r3d
{

	class ArchetypeManagerRingBuffer
	{
	public:
		ArchetypeManagerRingBuffer(size_t size)
		{
			m_size = size;
			m_managers.resize(size);
		}

		inline ArchetypeManager* getCurrent() { return &m_currentManager; }
		void saveCurrent()
		{
			increaseIndex();
			m_managers[m_index] = m_currentManager;
		}

	protected:
		void increaseIndex() { (m_index + 1) % m_size; }
		void decreaseIndex() { (m_index + m_size - 1) % m_size; }

	private:
		ArchetypeManager m_currentManager;
		std::vector<ArchetypeManager> m_managers;
		size_t m_size;
		size_t m_index;
	};

}
