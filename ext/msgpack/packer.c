/*
 * MessagePack for Ruby
 *
 * Copyright (C) 2008-2012 FURUHASHI Sadayuki
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "packer.h"

void msgpack_packer_init(msgpack_packer_t* pk)
{
    memset(pk, 0, sizeof(msgpack_packer_t));

    msgpack_buffer_init(PACKER_BUFFER_(pk));

    pk->io = Qnil;
}

void msgpack_packer_destroy(msgpack_packer_t* pk)
{
    msgpack_buffer_destroy(PACKER_BUFFER_(pk));
}

void msgpack_packer_mark(msgpack_packer_t* pk)
{
    msgpack_buffer_mark(PACKER_BUFFER_(pk));
    rb_gc_mark(pk->io);
}


void msgpack_buffer_flush_buffer_to_io(msgpack_packer_t* pk)
{
    // TODO write_to
}

void _msgpack_packer_write_string_to_io(msgpack_packer_t* pk, VALUE string)
{
    // TODO
}

void _msgpack_packer_allocate_writable_space(msgpack_packer_t* pk, size_t require)
{
    if(pk->io != Qnil) {
        msgpack_buffer_flush_buffer_to_io(pk);
    } else {
        msgpack_buffer_expand(PACKER_BUFFER_(pk), require);
    }
}

void msgpack_packer_write_array_value(msgpack_packer_t* pk, VALUE v)
{
    /* FIXME check len < uint32_t max */
    unsigned int len = (unsigned int)RARRAY_LEN(v);
    msgpack_packer_write_array_header(pk, len);

    unsigned int i;
    for(i=0; i < len; ++i) {
        VALUE e = rb_ary_entry(v, i);
        msgpack_packer_write_value(pk, e);
    }
}

int write_hash_foreach(VALUE key, VALUE value, VALUE pk_value)
{
    if (key == Qundef) {
        return ST_CONTINUE;
    }
    msgpack_packer_t* pk = (msgpack_packer_t*) pk_value;
    msgpack_packer_write_value(pk, key);
    msgpack_packer_write_value(pk, value);
    return ST_CONTINUE;
}

void msgpack_packer_write_hash_value(msgpack_packer_t* pk, VALUE v)
{
    /* FIXME check len < uint32_t max */
    unsigned int len = (unsigned int) RHASH_SIZE(v);
    msgpack_packer_write_map_header(pk, len);

    rb_hash_foreach(v, write_hash_foreach, (VALUE) pk);
}

static void _msgpack_packer_write_other_value(msgpack_packer_t* pk, VALUE v)
{
    // TODO
}

void msgpack_packer_write_value(msgpack_packer_t* pk, VALUE v)
{
    switch(rb_type(v)) {
    case RUBY_T_NIL:
        msgpack_packer_write_nil(pk);
        break;
    case RUBY_T_TRUE:
        msgpack_packer_write_true(pk);
        break;
    case RUBY_T_FALSE:
        msgpack_packer_write_false(pk);
        break;
    case RUBY_T_FIXNUM:
        msgpack_packere_write_fixnum_value(pk, v);
        break;
    case RUBY_T_SYMBOL:
        msgpack_packer_write_symbol_value(pk, v);
        break;
    case RUBY_T_STRING:
        msgpack_packer_write_string_value(pk, v);
        break;
    case RUBY_T_ARRAY:
        msgpack_packer_write_array_value(pk, v);
        break;
    case RUBY_T_HASH:
        msgpack_packer_write_hash_value(pk, v);
        break;
    case RUBY_T_BIGNUM:
        msgpack_packer_write_bignum_value(pk, v);
        break;
    case RUBY_T_FLOAT:
        msgpack_packer_write_float_value(pk, v);
        break;
    default:
        _msgpack_packer_write_other_value(pk, v);
    }
}
