#pragma once

#include <sstream>
#include <iomanip>
#include <string>
#include <type_traits>
#include <cstdint>

namespace Trema::Utils
{
    template<typename T>
    std::enable_if_t<std::is_integral_v<T>, std::string> ToHex(T i)
    {
        using Unsigned = std::conditional_t<std::is_signed_v<T>, std::make_unsigned_t<T>, T>;
        auto value = static_cast<Unsigned>(i);
        std::stringstream stream;
        stream << "0x" << std::hex << std::setfill('0') << std::setw(sizeof(T) * 2)
               << static_cast<std::uint64_t>(value);
        return stream.str();
    }

    inline std::string ToHex(const std::string &s)
    {
        std::stringstream stream;
        stream << "0x";
        if(!s.empty())
        {
            try {
                std::uint64_t v = std::stoull(s);
                stream << std::hex << v;
            } catch(...) {
                stream << '0';
            }
        }
        else {
            stream << '0';
        }
        return stream.str();
    }
}
