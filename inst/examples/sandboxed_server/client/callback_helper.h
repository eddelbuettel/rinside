/*
 * Copyright (c) 2014 Christian Authmann
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


namespace callback_helper {
	// A recursive template sending the TYPEIDs of all template parameters over the stream
	template <typename... Params>
	struct send_pack;

	template <typename First, typename... Remaining>
	struct send_pack<First, Remaining...> {
		static void send(BinaryStream &stream) {
			auto type = TYPEID<First>();
			stream.write(type);
			send_pack<Remaining...>::send(stream);
		}
	};

	template <>
	struct send_pack<> {
		static void send(BinaryStream &stream) {
		}
	};

	// An exception when the wrong type is sent over the stream
	class type_mismatch_exception : std::exception {
	};

	// read a typeid from the stream, compare it to the expected type, then read the value
	template<typename T>
	T read_from_stream_with_typeid(BinaryStream &stream) {
		auto type = stream.read<int32_t>();
		if (type != TYPEID<T>()) {
			printf("trying to read type %d, got type %d\n", (int) TYPEID<T>(), (int) type);
			throw type_mismatch_exception();
		}
		return stream.read<T>();
	}

	// auto-generated functions for calling callbacks
	template<typename RESULT_TYPE>
	void call(const std::function<RESULT_TYPE()> &fun, BinaryStream &stream) {
		RESULT_TYPE result = fun();
		int32_t result_type = TYPEID<RESULT_TYPE>();
		stream.write(result_type);
		stream.write(result);
	}
	template<typename RESULT_TYPE, typename U0>
	void call(const std::function<RESULT_TYPE(U0)> &fun, BinaryStream &stream) {
		auto x0 = read_from_stream_with_typeid<typename std::decay<U0>::type>(stream);

		RESULT_TYPE result = fun(x0);
		int32_t result_type = TYPEID<RESULT_TYPE>();
		stream.write(result_type);
		stream.write(result);
	}
	template<typename RESULT_TYPE, typename U0, typename U1>
	void call(const std::function<RESULT_TYPE(U0, U1)> &fun, BinaryStream &stream) {
		auto x0 = read_from_stream_with_typeid<typename std::decay<U0>::type>(stream);
		auto x1 = read_from_stream_with_typeid<typename std::decay<U1>::type>(stream);

		RESULT_TYPE result = fun(x0, x1);
		int32_t result_type = TYPEID<RESULT_TYPE>();
		stream.write(result_type);
		stream.write(result);
	}
	template<typename RESULT_TYPE, typename U0, typename U1, typename U2>
	void call(const std::function<RESULT_TYPE(U0, U1, U2)> &fun, BinaryStream &stream) {
		auto x0 = read_from_stream_with_typeid<typename std::decay<U0>::type>(stream);
		auto x1 = read_from_stream_with_typeid<typename std::decay<U1>::type>(stream);
		auto x2 = read_from_stream_with_typeid<typename std::decay<U2>::type>(stream);

		RESULT_TYPE result = fun(x0, x1, x2);
		int32_t result_type = TYPEID<RESULT_TYPE>();
		stream.write(result_type);
		stream.write(result);
	}
	// TODO: more parameters
}
