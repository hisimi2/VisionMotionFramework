#pragma once

namespace VMF
{
	// Optional Class
	template<typename T>
	class VisionOptional
	{
	public:
		VisionOptional()
			: m_hasValue(false), m_value() {}
		VisionOptional(const T& val)
			: m_hasValue(true), m_value(val) {}

	public:
		bool has_value() const
		{
			return m_hasValue;
		}

		explicit operator bool() const
		{
			return m_hasValue;
		}

		const T& value() const
		{
			assert(m_hasValue && "VisionOptional : no value");

			return m_value;
		}

		T value_or(const T& defaultVal) const
		{
			return m_hasValue ? m_value : defaultVal;
		}

		VisionOptional& operator=(const T& val)
		{
			m_value = val;
			m_hasValue = true;

			return *this;
		}

		void reset()
		{
			m_hasValue = false;
		}

	private:

		bool m_hasValue;

		T m_value;
	};

}
