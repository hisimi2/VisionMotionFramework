#pragma once
#include <vector>
#include <string>
#include <cstdint>

#include "VisionCommAPI.h"

namespace VisionComm
{
	// 장비/통신 관련 상태
	enum Status 
	{
		VisionOK = 0,
		VisionNotInitialized = -1,
		VisionTimeout = -2,
		VisionFailed = 1,
		VisionNotImplemented = 2,
		VisionNotSupported = 3,
		VisionConnectionFailed = 4
	};

	using ByteArray = std::vector<uint8_t>;

	// IVisionPacket: 프로토콜별 패킷 데이터 구조 추상화
	class IVisionPacket
    {
	public:
		virtual ~IVisionPacket() = default;

		// 직렬화 인터페이스
		virtual ByteArray ToByteArray() const = 0;
		virtual bool FromByteArray(const ByteArray& raw) = 0;

		// 프로토콜 식별자 (S, F 등)
		virtual int GetOpCode() const = 0; 
		virtual void SetOpCode(int code) = 0;
		virtual int GetSubCode() const = 0;
		virtual void SetSubCode(int code) = 0;

		// 매칭 식별자 (SystemByte 등)
		virtual uint32_t GetCorrelationId() const = 0;
		virtual void SetCorrelationId(uint32_t id) = 0;

		virtual const ByteArray& GetBody() const = 0;
		virtual void SetBody(const ByteArray& body) = 0;

		virtual size_t GetHeaderSize() const = 0;

		// [추가] 결과 파싱의 일반화: 패킷 본연의 로직으로 상태 및 결과 추출
		virtual bool ParseResponse(int& dataId, int& nRet, std::vector<std::string>& results) const = 0;
	};

} // namespace VisionCommm

