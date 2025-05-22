// EndianUtils.h
#pragma once
#include <cstdint>
#include <type_traits>
#include <cstring>
#include <bit>

// 编译器特性检测
#if defined(_MSC_VER)
    #include <intrin.h>
    #define MIRROR_BSWAP16 _byteswap_ushort
    #define MIRROR_BSWAP32 _byteswap_ulong
    #define MIRROR_BSWAP64 _byteswap_uint64
#elif defined(__GNUC__) || defined(__clang__)
    #define MIRROR_BSWAP16 __builtin_bswap16
    #define MIRROR_BSWAP32 __builtin_bswap32
    #define MIRROR_BSWAP64 __builtin_bswap64
#else
    #error "Unsupported compiler"
#endif

namespace Mirror {
namespace Core {

    class EndianUtils {
    public:
        static constexpr bool IsLittleEndian() noexcept {
#if defined(_MSC_VER) || (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__))
            return true;
#else
            return false;
#endif
        }
        template<typename T>
        static T SwapBytes(T value) noexcept {

        static_assert(std::is_arithmetic_v<T>, 
                    "EndianUtils::SwapBytes only supports arithmetic types");
        static_assert(sizeof(T) <= 8,
                    "EndianUtils::SwapBytes type size exceeds 8 bytes");

        if constexpr (sizeof(T) == 1) {
            return value;
        } else if constexpr (std::is_integral_v<T>) {
            return SwapInteger(value);
        } else {
            return SwapFloatingPoint(value);
        }
    }

    // 小端序转换接口
    template<typename T>
    static T FromLittleEndian(T value) noexcept {
        return MaybeSwap<T, std::endian::little>(value);
    }

    template<typename T>
    static void WriteLittleEndian(uint8_t* dest, T value) noexcept {
        WriteMaybeSwap<T, std::endian::little>(dest, value);
    }

    template<typename T>
    static T ReadLittleEndian(const uint8_t* data) noexcept {
        return ReadMaybeSwap<T, std::endian::little>(data);
    }

    // 大端序转换接口
    template<typename T>
    static T FromBigEndian(T value) noexcept {
        return MaybeSwap<T, std::endian::big>(value);
    }

    template<typename T>
    static void WriteBigEndian(uint8_t* dest, T value) noexcept {
        WriteMaybeSwap<T, std::endian::big>(dest, value);
    }

    template<typename T>
    static T ReadBigEndian(const uint8_t* data) noexcept {
        return ReadMaybeSwap<T, std::endian::big>(data);
    }

private:
    // 整型交换核心实现
    template<typename T>
    static std::enable_if_t<sizeof(T) == 2, T> 
    SwapInteger(T value) noexcept {
        union { T val; uint16_t raw; } u{value};
        u.raw = MIRROR_BSWAP16(u.raw);
        return u.val;
    }

    template<typename T>
    static std::enable_if_t<sizeof(T) == 4, T> 
    SwapInteger(T value) noexcept {
        union { T val; uint32_t raw; } u{value};
        u.raw = MIRROR_BSWAP32(u.raw);
        return u.val;
    }

    template<typename T>
    static std::enable_if_t<sizeof(T) == 8, T> 
    SwapInteger(T value) noexcept {
        union { T val; uint64_t raw; } u{value};
        u.raw = MIRROR_BSWAP64(u.raw);
        return u.val;
    }

    // 浮点数处理
    template<typename T>
    static T SwapFloatingPoint(T value) noexcept {
        static_assert(sizeof(T) == 4 || sizeof(T) == 8, 
                     "EndianUtils: Only float/double supported");

        if constexpr (sizeof(T) == 4) {
            union { T f; uint32_t i; } u{value};
            u.i = MIRROR_BSWAP32(u.i);
            return u.f;
        } else {
            union { T f; uint64_t i; } u{value};
            u.i = MIRROR_BSWAP64(u.i);
            return u.f;
        }
    }

    // 编译期决策的字节序转换
    template<typename T, std::endian TargetEndian>
    static T MaybeSwap(T value) noexcept {
        if constexpr (std::endian::native == TargetEndian) {
            return value;
        } else {
            return SwapBytes(value);
        }
    }

    // 安全内存读取模板
    template<typename T, std::endian TargetEndian>
    static T ReadMaybeSwap(const uint8_t* data) noexcept {
        T value;
        #if defined(__GNUC__)
            __builtin_memcpy(&value, data, sizeof(T));
        #else
            std::memcpy(&value, data, sizeof(T));
        #endif
        return MaybeSwap<T, TargetEndian>(value);
    }

    // 安全内存写入模板
    template<typename T, std::endian TargetEndian>
    static void WriteMaybeSwap(uint8_t* dest, T value) noexcept {
        const T temp = MaybeSwap<T, TargetEndian>(value);
        #if defined(__GNUC__)
            __builtin_memcpy(dest, &temp, sizeof(T));
        #else
            std::memcpy(dest, &temp, sizeof(T));
        #endif
    }

    };

} // namespace Core
} // namespace Mirror

// 清理编译器特定宏
#undef MIRROR_BSWAP16
#undef MIRROR_BSWAP32
#undef MIRROR_BSWAP64
