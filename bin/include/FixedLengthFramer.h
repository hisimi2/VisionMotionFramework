#pragma once
#include "IFramer.h"
#include <vector>

namespace VisionCom
{
	class FixedLengthFramer : public IFramer
	{
	private:
		std::vector<uint8_t> m_buffer;
		size_t m_packetSize; // 생성 시 결정되는 패킷 크기

	public:
		// 생성자에서 패킷 크기를 주입받음 (기본값 설정 가능)
		explicit FixedLengthFramer(size_t packetSize = 664)
			: m_packetSize(packetSize) {}

		// C++14: 다형성 소멸자는 `= default`로 선언하는 것을 권장
		~FixedLengthFramer() override = default;

		void FeedBytes(const ByteVector& bytes) override {
			m_buffer.insert(m_buffer.end(), bytes.begin(), bytes.end());
		}

		bool NextFrame(ByteVector& frame) override {
			if (m_buffer.size() < m_packetSize) return false;

			frame.assign(m_buffer.begin(), m_buffer.begin() + m_packetSize);
			m_buffer.erase(m_buffer.begin(), m_buffer.begin() + m_packetSize);
			return true;
		}

		// 실행 중에도 크기를 바꿀 수 있는 인터페이스 (선택 사항)
		void SetPacketSize(size_t size) { m_packetSize = size; }
	};
}
