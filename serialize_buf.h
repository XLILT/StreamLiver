#pragma once

#include <vector>

class serialize_buf
{
public:
	serialize_buf(size_t capacity = 0)
	{
		if (capacity)
			vec.reserve(capacity);
	}

	~serialize_buf()
	{
	}

	void clear() { vec.clear(); }

	void reserve(size_t cap) { vec.reserve(cap); }

	inline serialize_buf &write_8b(uint8_t val)
	{
		vec.push_back(val);
		return *this;
	}

	inline serialize_buf &write_16b(uint16_t val)
	{
		vec.push_back(val >> 8);
		vec.push_back((uint8_t)val);
		return *this;
	}

	inline serialize_buf &write_24b(uint32_t val)
	{
		write_16b(val >> 8);
		vec.push_back(val);
		return *this;
	}

	inline serialize_buf &write_32b(uint32_t val)
	{
		return write_16b(val >> 16).write_16b(val);
	}

	inline serialize_buf &write(const uint8_t *data, size_t size)
	{
		vec.insert(vec.end(), data, data + size);
		return *this;
	}

	uint8_t *data() { return vec.data(); }

	const uint8_t *data() const { return vec.data(); }

	size_t size() const { return vec.size(); }

	serialize_buf & operator=(serialize_buf &&other)
	{
		vec.swap(other.vec);
		return *this;
	}

private:
	std::vector<uint8_t> vec;
};
