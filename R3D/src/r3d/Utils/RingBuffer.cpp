#include <R3Dpch.h>
#include "RingBuffer.h"

namespace r3d
{

	void TestRingBuffer::advanceFrame(int mod)
	{
		auto a = ring.getCurrent();
		a->data = mod;
		ring.saveAndAdvance();
	}

	void TestRingBuffer::visitPrevFrame()
	{
		ring.visitBackward();
	}

	void TestRingBuffer::visitNextFrame()
	{
		ring.visitForward();
	}

	void TestRingBuffer::print() const
	{
		std::cout << ring.getCurrent()->data << " - ";
		for (size_t i = 0; i < ring.size(); i++)
			std::cout << ring.getSlot(i).data << "(" << (ring.isDeprecated(i) ? "X" : "O") << ")" << ring.getSlotId(i) << ", ";
		std::cout << std::endl;
		std::cout << "latestSteppedId:" << ring.getLatestSlotId() << ", index" << ring.getActiveSlotIndex() << std::endl;
		std::cout << "---------------" << std::endl;
	}

	bool TestRingBuffer::compare(std::vector<size_t> data, std::vector<size_t> idx, std::vector<bool> deprec,
		int currentData, unsigned long long latestSteppedId, size_t index)
	{
		bool check = true;
		for (int i = 1; i < ring.size(); ++i)
		{
			check &= (ring.isDeprecated(i) == deprec.at(i));
			check &= (ring.getSlotId(i) == idx.at(i));
			check &= (ring.getSlot(i).data == data.at(i));
		}
		return check;
	}

	void test_ringbuffer_1()
	{
		TestRingBuffer test;

		for (int i = 1; i < 25; ++i)
		{
			test.advanceFrame(i);
			//test.print();
		}

		{
			std::vector<size_t> data = { 20, 21, 22, 23, 24, 15, 16, 17, 18, 19 };
			std::vector<size_t> idx = { 20, 21, 22, 23, 24, 15, 16, 17, 18, 19 };
			std::vector<bool> deprec = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
			bool check = test.compare(data, idx, deprec, 24, 24, 4);
			std::cout << "advance test 1: " << (check ? "pass" : "fail") << std::endl;
		}

		test.visitPrevFrame();
		{
			std::vector<size_t> data = { 20, 21, 22, 23, 24, 15, 16, 17, 18, 19 };
			std::vector<size_t> idx = { 20, 21, 22, 23, 24, 15, 16, 17, 18, 19 };
			std::vector<bool> deprec = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
			bool check = test.compare(data, idx, deprec, 23, 24, 3);
			std::cout << "visit prev test 1: " << (check ? "pass" : "fail") << std::endl;
		}

		test.visitPrevFrame();
		{
			std::vector<size_t> data = { 20, 21, 22, 23, 24, 15, 16, 17, 18, 19 };
			std::vector<size_t> idx = { 20, 21, 22, 23, 24, 15, 16, 17, 18, 19 };
			std::vector<bool> deprec = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
			bool check = test.compare(data, idx, deprec, 22, 24, 2);
			std::cout << "visit prev test 2: " << (check ? "pass" : "fail") << std::endl;
		}

		test.advanceFrame(100);
		{
			std::vector<size_t> data = { 20, 21, 22, 100, 24, 15, 16, 17, 18, 19 };
			std::vector<size_t> idx = { 20, 21, 22, 25, 24, 15, 16, 17, 18, 19 };
			std::vector<bool> deprec = { 1,   1,  1,  1,  0,  1,  1,  1,  1,  1 };
			bool check = test.compare(data, idx, deprec, 100, 25, 3);
			std::cout << "advance test 2: " << (check ? "pass" : "fail") << std::endl;
		}

		std::cout << "A warning is supposed to be printed here: " << std::endl;
		test.visitNextFrame();
		{
			std::vector<size_t> data = { 20, 21, 22, 100, 24, 15, 16, 17, 18, 19 };
			std::vector<size_t> idx = { 20, 21, 22, 25, 24, 15, 16, 17, 18, 19 };
			std::vector<bool> deprec = { 1,   1,  1,  1,  0,  1,  1,  1,  1,  1 };
			bool check = test.compare(data, idx, deprec, 100, 25, 3);
			std::cout << "visit next test 1: " << (check ? "pass" : "fail") << std::endl;
		}

	}

}