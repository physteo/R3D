#pragma once

namespace r3d
{

	//! Container of contiguous slots in a ring geometry, of size "size". After all slots are written, it overwrites the oldest.
	/*!
		Contains data stored in contiguous slots, which can be traversed in a ring geometry (after the last slot,
		the first slot is visited: those two are not contiguous in memory).
		There are two ways of traversing a RingBuffer.
		1) Always advancing: this is useful for saving data as we advance, for example, frames in a simulation.
		   example:
		   while(true)
		   {
			 auto* currFrame = ring.getCurrent(); // retrieve current
			 modify(currFrame);					  // do something to curr
			 ring.saveAndAdvance(currFrame);		  // save current to active slot, advance to next slot
		   }
		2) Visiting forward or backward: useful for replaying the frames of a simulations.
		   example use:
		   // suppose we have advanced for a while, as show in 1) above.
		   visitBackward();
		   visitBackward(); // went back of two frames
		   auto* prevFrame = getCurrent();
		   visualize_for_debug(prevFrame);
		   saveAndAdvance(); // now, the two subsequent frames are deprecated
		   visitForward(); // does nothing: cannot visit deprecated frames.
	*/
	template <class T>
	class RingBuffer
	{
	public:
		RingBuffer(size_t size)
		{
			m_size = size;
			m_slots.resize(size);
			m_id.resize(size);
			m_deprecated.resize(size, true);
			m_index = 0;
			m_latestSlotId = 0;
		}

		//!< Saves current object to active slot in the ring, and advances to next slot.
		//!< If active_slot != latest_slot, all subsequent slots are set to "deprecated" and
		//!< cannot be traversed.
		void saveAndAdvance()
		{
			if (m_id[m_index] == m_latestSlotId)
			{
				m_deprecated[m_index] = false;
				m_index = nextIndex();
				m_deprecated[m_index] = false;
				m_slots[m_index] = m_current;
				m_id[m_index] = ++m_latestSlotId;
			}
			else
			{
				unsigned long long check = m_id[m_index];
				size_t cnt = m_index;
				while (check != m_latestSlotId)
				{
					m_deprecated[cnt] = true;
					check = m_id[cnt];
					cnt = nextIndex(cnt);
				}
				m_deprecated[m_index] = false;
				m_index = nextIndex();
				m_deprecated[m_index] = false;
				m_slots[m_index] = m_current;
				m_id[m_index] = ++m_latestSlotId;

			}
		}

		//!< Get pointer to current object
		T* getCurrent()
		{
			return &m_current;
		}

		//!< Get pointer to current object (const)
		const T* getCurrent() const
		{
			return &m_current;
		}

		//!< Overwrite the current object with the slot after the active one, which becomes the active slot. 
		void visitForward()
		{
			size_t next = nextIndex();
			if (m_deprecated[next] == false)
			{
				m_index = next;
				m_current = m_slots[m_index];
			}
			else
			{
				std::cout << "[RingBuffer warning]() : selected slot is deprecated." << std::endl;
			}
		}

		//!< Overwrite the current object with the slot before the active one, which becomes the active slot. 
		void visitBackward()
		{
			size_t next = prevIndex();
			if (m_deprecated[next] == false)
			{
				m_index = next;
				m_current = m_slots[m_index];
			}
			else
			{
				std::cout << "[RingBuffer warning]() : selected slot is deprecated." << std::endl;
			}
		}

		//!< Number of slots in the container.
		inline size_t size() const { return m_size; }
		//!< (for debug) Tells if slot "index" is deprecated (happens when doing saveAndAdvance() with active_slot != latest_slot)
		inline bool     isDeprecated(size_t index)  const { return !(m_deprecated[index]); }
		//!< (for debug) Returns the id of slot "index"
		inline size_t   getSlotId(size_t index)     const { return m_id[index]; }
		//!< (for debug) Returns the index of the active slot
		inline size_t   getActiveSlotIndex()        const { return m_index; }
		//!< (for debug) Returns const reference of the slot "index"
		inline const T& getSlot(size_t index)       const { return m_slots[index]; }
		//!< (for debug) Returns the ID of the latest slot
		inline unsigned long long getLatestSlotId() const { return m_latestSlotId; }

	private:
		T m_current;
		std::vector<T> m_slots;
		std::vector<bool> m_deprecated;
		std::vector<unsigned long long> m_id;
		unsigned long long m_latestSlotId;

		size_t m_size;
		size_t m_index;

		size_t nextIndex() const { return (m_index + 1) % m_size; }
		size_t prevIndex() const { return (m_index + m_size - 1) % m_size; }
		size_t nextIndex(size_t input) const { return (input + 1) % m_size; }
		size_t prevIndex(size_t input) const { return (input + m_size - 1) % m_size; }

	};

	void test_ringbuffer_1();

	struct RingBufferDataTest
	{
		int data;
	};

	class TestRingBuffer
	{
	public:
		RingBuffer<RingBufferDataTest> ring;

		TestRingBuffer() : ring{ 10 } {}
		void advanceFrame(int mod);
		void visitPrevFrame();
		void visitNextFrame();
		void print() const;

		bool compare(std::vector<size_t> data, std::vector<size_t> idx, std::vector<bool> avail,
			int currentData, unsigned long long latestSteppedId, size_t index);
	};

}