#include "zone.hpp"

namespace msgpack {


void* zone::alloc()
{
	if(m_used >= m_pool.size()*MSGPACK_ZONE_CHUNK_SIZE) {
		m_pool.push_back(new chunk_t());
	}
	void* data = m_pool[m_used/MSGPACK_ZONE_CHUNK_SIZE]->cells[m_used%MSGPACK_ZONE_CHUNK_SIZE].data;
	++m_used;
	return data;
}

void zone::clear()
{
	if(!m_pool.empty()) {
		for(size_t b=0; b < m_used/MSGPACK_ZONE_CHUNK_SIZE; ++b) {
			cell_t* c(m_pool[b]->cells);
			for(size_t e=0; e < MSGPACK_ZONE_CHUNK_SIZE; ++e) {
				reinterpret_cast<object_class*>(c[e].data)->~object_class();
			}
		}
		cell_t* c(m_pool.back()->cells);
		for(size_t e=0; e < m_used%MSGPACK_ZONE_CHUNK_SIZE; ++e) {
			reinterpret_cast<object_class*>(c[e].data)->~object_class();
		}

		for(pool_t::iterator it(m_pool.begin()), it_end(m_pool.end());
				it != it_end;
				++it) {
			delete *it;
		}
		m_pool.clear();
	}
	m_used = 0;

	for(user_finalizer_t::reverse_iterator it(m_user_finalizer.rbegin()), it_end(m_user_finalizer.rend());
			it != it_end;
			++it) {
		it->call();
	}
	m_user_finalizer.clear();
}


}  // namespace msgpack
